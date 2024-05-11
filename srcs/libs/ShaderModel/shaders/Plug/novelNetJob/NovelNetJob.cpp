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
	networkAccessManager = std::make_shared< cylHttpNetWork::NetworkAccessManager >( this );
	networkRequest = std::make_shared< cylHttpNetWork::NetworkRequest >( );
	root = std::make_shared< std::pair< RequestConnect_Shared, Request_Shared > >( );
	root->first = std::make_shared< cylHttpNetWork::RequestConnect >( );
	root->second = std::make_shared< cylHttpNetWork::Request >( networkAccessManager.get( ), root->first.get( ) );
}
void NovelNetJob::initObjProperty( ) {
	interfaceThisPtr->setOStream( oStream );
	QString agentHeader = networkRequest->getRandomUserAgentHeader( );
	networkRequest->setHeader( QNetworkRequest::UserAgentHeader, agentHeader );
	QSslConfiguration sslConfiguration;
	sslConfiguration.setPeerVerifyMode( QSslSocket::VerifyNone );
	networkRequest->setSslConfiguration( sslConfiguration );
	networkRequest->setAttribute( QNetworkRequest::ConnectionEncryptedAttribute, true );
	networkRequest->setTransferTimeout( std::chrono::milliseconds( 25000 ) );
	microseconds = std::chrono::microseconds( 1500 );
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
	qDebug( ) << networkAccessManager->supportedSchemes( );
	qDebug( ) << QSslSocket::sslLibraryBuildVersionString( );
	root->second->netGetWork( QString::fromStdWString( resultUrl ), *networkRequest, microseconds );
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
	if( networkReply->error( ) != QNetworkReply::NoError ) {
		OStream::errorQDebugOut( getErrorQStr( networkReply->error( ) ).toStdString( ), __FILE__, __LINE__ );
		return;
	}
	QString htmlText( networkReply->readAll( ) );
	if( htmlText.isEmpty( ) )
		return;
	auto mapHtmlStrKHtmlStrV = interfaceThisPtr->formHtmlGetTypeTheUrls( htmlText.toStdWString( ) );
	if( !mapHtmlStrKHtmlStrV ) {
		OStream::errorQDebugOut( "(没有找到类型节点)结束", __FILE__, __LINE__ );
		return;
	}
	auto iterator = mapHtmlStrKHtmlStrV->begin( );
	auto end = mapHtmlStrKHtmlStrV->end( );
	for( ; iterator != end; ++iterator ) {
		QUrl typeUrl = QString::fromStdWString( iterator->second );
		QString typeName = QString::fromStdWString( iterator->first );
		auto requestConnect = new cylHttpNetWork::RequestConnect;
		auto request = new cylHttpNetWork::Request( networkAccessManager.get( ), requestConnect );

		connect( requestConnect, &cylHttpNetWork::RequestConnect::networkReplyFinished, [requestConnect,request, typeName, typeUrl ,this]( cylHttpNetWork::RequestConnect *request_connect ) {
			auto networkReply = request_connect->getNetworkReply( );
			if( networkReply->error( ) != QNetworkReply::NoError ) {
				OStream::errorQDebugOut( getErrorQStr( networkReply->error( ) ).toStdString( ), __FILE__, __LINE__ );
				return;
			}
			emit requesting_get_type_page_url_signals( getUrl( ), typeName, typeUrl, std::make_shared< cylHtmlTools::HtmlString >( QString( networkReply->readAll( ) ).toStdWString( ) ) );
			requestConnect->deleteLater( );
			request->deleteLater( );
		} );
		request->netGetWork( typeUrl, *networkRequest, microseconds );
	}
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
				connect( requestConnect, &cylHttpNetWork::RequestConnect::networkReplyFinished, [Request,requestConnect,type,url,novelName,novelUrl, sharedPtrs,this]( cylHttpNetWork::RequestConnect *request_connect ) {
					auto networkReply = request_connect->getNetworkReply( );
					if( networkReply->error( ) != QNetworkReply::NoError ) {
						OStream::errorQDebugOut( getErrorQStr( networkReply->error( ) ).toStdString( ), __FILE__, __LINE__ );
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
		//if( formHtmlGetNext.empty( ) ) 	// 返回空，则表示没有下一页
		//	break;
		//auto requestConnect = new cylHttpNetWork::RequestConnect;
		//auto Request = new cylHttpNetWork::Request( networkAccessManager.get( ), requestConnect );
		//connect( requestConnect, &cylHttpNetWork::RequestConnect::networkReplyFinished, [this, type, url,Request,requestConnect]( cylHttpNetWork::RequestConnect *request_connect ) {
		//	auto networkReply = request_connect->getNetworkReply( );
		//	if( networkReply->error( ) != QNetworkReply::NoError ) {
		//		errorQDebugOut( getErrorQStr( networkReply->error( ) ).toStdString( ), __FILE__, __LINE__ );
		//		return;
		//	}
		//	emit requesting_get_type_page_url_signals( getUrl( ), type, url, std::make_shared< cylHtmlTools::HtmlString >( QString( networkReply->readAll( ) ).toStdWString( ) ) );
		//	Request->deleteLater( );
		//	requestConnect->deleteLater( );
		//} );

		//this->typeCountMap.emplace( type, count + 1 );

		//return;  // 正常执行完毕
	} while( false );
	// 没有使用 return 返回，则表示终止调用
	emit requested_get_type_page_url_end( getUrl( ), type, url, count, sharedPtrs );
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
