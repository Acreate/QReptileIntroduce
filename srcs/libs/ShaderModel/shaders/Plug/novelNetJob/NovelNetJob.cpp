#include "NovelNetJob.h"
#include <HttpNetWork/RequestConnect.h>
#include <HttpNetWork/Request.h>
#include <HttpNetWork/NetworkRequest.h>
#include <stream/OStream.h>

#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/htmlTools/XPath/XPath.h"
#include "htmls/HtmlNode/HtmlNode.h"
#include "thread/FileThread.h"
#include <HttpNetWork/NetworkAccessManager.h>
#include <qguiapplication.h>
#include <unordered_map>


#include "interface/INovelInfo.h"
using namespace interfacePlugsType;
NovelNetJob::NovelNetJob( OStream *o_stream, QObject *interface_obj_ptr, IRequestNetInterface *interface_this_ptr ): interfaceObjPtr( interface_obj_ptr )
, interfaceThisPtr( interface_this_ptr )
, oStream( o_stream ) {
	initObj( );
	initObjProperty( );
	initConnect( );
}

NovelNetJob::~NovelNetJob( ) {
	interfaceObjPtr->deleteLater( );
}
void NovelNetJob::initObj( ) {
	networkAccessManager = std::make_shared< cylHttpNetWork::NetworkAccessManager >( );
	networkRequest = std::make_shared< cylHttpNetWork::NetworkRequest >( );
	root = std::make_shared< std::pair< RequestConnect_Shared, Request_Shared > >( );
	root->first = std::make_shared< cylHttpNetWork::RequestConnect >( );
	root->second = std::make_shared< cylHttpNetWork::Request >( networkAccessManager.get( ), root->first.get( ) );

	typeRequestMap = std::make_shared< Unordered_Map_Pairt >( );
}
void NovelNetJob::initObjProperty( ) {
	interfaceThisPtr->setOStream( oStream );
	networkRequest->setHeader( QNetworkRequest::UserAgentHeader, networkRequest->getRandomUserAgentHeader( ) );
}

void NovelNetJob::initConnect( ) {
	connect( root->first.get( ), &cylHttpNetWork::RequestConnect::networkReplyFinished, [this]( cylHttpNetWork::RequestConnect *request_connect ) {
		QNetworkReply *networkReply = request_connect->getNetworkReply( );
		emit requesting_get_root_page_signals( networkReply->url( ), request_connect );
	} );

	connect( this, &NovelNetJob::requesting_get_root_page_signals, this, &NovelNetJob::slots_requesting_get_root_page_signals );
	connect( this, &NovelNetJob::requesting_get_type_page_url_signals, this, &NovelNetJob::slots_requesting_get_type_page_url_signals );
}
bool NovelNetJob::start( ) {
	HtmlDocString resultUrl;
	size_t size = interfaceThisPtr->getUrl( &resultUrl );
	if( size == 0 )
		return false;
	root->second->netGetWork( QString::fromStdWString( resultUrl ), *networkRequest );
	return true;
}
QString NovelNetJob::getUrl( ) const {
	HtmlDocString resultUrl;
	size_t size = interfaceThisPtr->getUrl( &resultUrl );
	if( size )
		return QString::fromStdWString( resultUrl );
	return "";
}
void NovelNetJob::slots_requesting_get_root_page_signals( const QUrl url, cylHttpNetWork::RequestConnect *request_connect ) {
	auto networkReply = request_connect->getNetworkReply( );
	QString htmlText( networkReply->readAll( ) );
	if( htmlText.isEmpty( ) )
		return;
	auto mapHtmlStrKHtmlStrV = interfaceThisPtr->formHtmlGetTypeTheUrls( htmlText.toStdWString( ) );

	if( !mapHtmlStrKHtmlStrV )
		return;
	typeRequestMap->clear( );
	do {
		auto iterator = mapHtmlStrKHtmlStrV->begin( );
		auto end = mapHtmlStrKHtmlStrV->end( );
		for( ; iterator != end; ++iterator ) {
			// 构建 url 映射
			NovelTypeNamePair_Shared urlPair( std::make_shared< NovelTypeNamePair >( ) );
			urlPair->setUrl( QString::fromStdWString( iterator->second ) );
			urlPair->setTypeName( QString::fromStdWString( iterator->first ) );
			Request_Pairt_Shared requestPairtShared = std::make_shared< Request_Pairt >( );
			requestPairtShared->first = std::make_shared< cylHttpNetWork::RequestConnect >( );
			requestPairtShared->second = std::make_shared< cylHttpNetWork::Request >( networkAccessManager.get( ), requestPairtShared->first.get( ) );
			typeRequestMap->emplace( urlPair, requestPairtShared );
			qDebug( ) << urlPair->getTypeName( ) << " : " << urlPair->getUrl( ).host( );
			qDebug( ) << "==============";
		}
	} while( false );
	do {
		auto iterator = typeRequestMap->begin( );
		auto end = typeRequestMap->end( );
		for( ; iterator != end; ++iterator ) {
			auto novelTypeNamePair = iterator->first;
			auto requestPairtShared = iterator->second;
			auto requestConnect = requestPairtShared->first.get( );
			connect( requestConnect, &cylHttpNetWork::RequestConnect::networkReplyFinished, [novelTypeNamePair,requestPairtShared ,this]( cylHttpNetWork::RequestConnect *request_connect ) {
				auto networkReply = request_connect->getNetworkReply( );
				if( networkReply->error( ) != QNetworkReply::NoError ) {
					qDebug( ) << __FILE__ << "\n\t: " << __LINE__ << "行\n\t: " << getErrorQStr( networkReply->error( ) ) << "\n=====";
					return;
				}
				emit requesting_get_type_page_url_signals( getUrl( ), novelTypeNamePair->getTypeName( ), novelTypeNamePair->getUrl( ), std::make_shared< cylHtmlTools::HtmlString >( QString( networkReply->readAll( ) ).toStdWString( ) ) );
			} );
			requestPairtShared->second->netGetWork( novelTypeNamePair->getUrl( ), *networkRequest );
		}
	} while( false );
}

void NovelNetJob::slots_requesting_get_type_page_url_signals( const QString &root_url, const QString &type, const QUrl &url, cylHtmlTools::HtmlString_Shared html_string ) {
	std::shared_ptr< interfacePlugsType::Vector_NovelSPtr > sharedPtrs;
	if( this->typeNovelsMap.count( type ) )
		sharedPtrs = this->typeNovelsMap.at( type );
	else {
		sharedPtrs = std::make_shared< interfacePlugsType::Vector_NovelSPtr >( );
		this->typeNovelsMap.emplace( type, sharedPtrs );
	}

	size_t count = 0;
	if( this->typeCountMap.count( type ) )
		count = this->typeCountMap.at( type );
	auto novelInfos = interfaceThisPtr->formHtmlGetTypePageNovels( *html_string, *sharedPtrs, nullptr );
	do {
		if( novelInfos.size( ) == 0 )
			break;
		// 没有小说则终止
		for( auto &novel : novelInfos ) {
			if( interfaceThisPtr->isRequestNovelInfoUrl( novel.get( ) ) ) { // 如果小说需要到详情页申请，则临时存储
				NovelTypeNamePair_Shared urlPair( std::make_shared< NovelTypeNamePair >( ) );
				HtmlDocString buffStr;
				novel->getNovelUrl( &buffStr );
				QString novelUrl = QString::fromStdWString( buffStr );
				novel->getNovelName( &buffStr );
				QString novelName = QString::fromStdWString( buffStr );
				auto requestConnect = new cylHttpNetWork::RequestConnect;
				auto Request = new cylHttpNetWork::Request( networkAccessManager.get( ), requestConnect );
				connect( requestConnect, &cylHttpNetWork::RequestConnect::networkReplyFinished, [this,=]( cylHttpNetWork::RequestConnect *request_connect ) {
					auto networkReply = request_connect->getNetworkReply( );
					if( networkReply->error( ) != QNetworkReply::NoError ) {
						qDebug( ) << __FILE__ << "\n\t: " << __LINE__ << "行\n\t: " << getErrorQStr( networkReply->error( ) ) << "\n=====";
						return;
					}
					auto byteArray = networkReply->readAll( );
					QString htmlTxt( byteArray );
					emit requesting_get_novel_page_url_signals( getUrl( ), type, url.host( ), novelName, novelUrl, sharedPtrs, htmlTxt );
					Request->deleteLater( );
					requestConnect->deleteLater( );
				} );
			} else
				sharedPtrs->emplace_back( novel ); // 存储已知小说
		}
		auto formHtmlGetNext = interfaceThisPtr->formHtmlGetNext( *html_string, *sharedPtrs, novelInfos );
		if( formHtmlGetNext.empty( ) ) 	// 返回空，则表示没有下一页
			break;
		auto requestConnect = new cylHttpNetWork::RequestConnect;
		auto Request = new cylHttpNetWork::Request( networkAccessManager.get( ), requestConnect );
		connect( requestConnect, &cylHttpNetWork::RequestConnect::networkReplyFinished, [=,this]( cylHttpNetWork::RequestConnect *request_connect ) {
			auto networkReply = request_connect->getNetworkReply( );
			if( networkReply->error( ) != QNetworkReply::NoError ) {
				qDebug( ) << __FILE__ << "\n\t: " << __LINE__ << "行\n\t: " << getErrorQStr( networkReply->error( ) ) << "\n=====";
				return;
			}
			emit requesting_get_type_page_url_signals( getUrl( ), type, url, std::make_shared< cylHtmlTools::HtmlString >( QString( networkReply->readAll( ) ).toStdWString( ) ) );
			Request->deleteLater( );
			requestConnect->deleteLater( );
		} );
		this->typeCountMap.emplace( type, count + 1 );
		// 正常执行完毕
		return;
	} while( false );
	// 没有使用 return 返回，则表示终止调用
	emit requested_get_type_page_url_end( getUrl( ), type, url, typeCountMap.at( type ), sharedPtrs );
}
void NovelNetJob::slots_requesting_get_next_type_page_url_signals( const QString &root_url, const QString &type, const QUrl &old_url, const QUrl &url, size_t old_page_index, size_t current_page_index, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared ) {
}
void NovelNetJob::slots_requesting_get_novel_page_url_signals( const QString &root_url, const QString &type, const QString &type_page_url, const QString &novelName, const QUrl &url, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared, const QString &html_txt ) {


}
void NovelNetJob::slots_requested_get_novel_page_url_signals( const QString &root_url, const QString &type, const QString &type_page_url, const QString &novelName, const QUrl &url, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared, const interfacePlugsType::INovelInfo_Shared novel_info_shared ) {
}
void NovelNetJob::slots_requested_get_type_page_url_end( const QString &root_url, const QString &type, const QUrl &url, size_t current_page_index, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared ) {
}
void NovelNetJob::slots_requested_get_web_page_signals_end( const QUrl &url ) {
}
