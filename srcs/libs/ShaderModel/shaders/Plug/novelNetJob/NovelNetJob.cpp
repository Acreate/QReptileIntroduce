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
#include "dateTime/DateTime.h"

#include "interface/INovelInfo.h"
#include "macro/cmake_to_c_cpp_header_env.h"
#include "path/Dir.h"
using namespace interfacePlugsType;

QString getNormalQString( const QString &q_string ) {
	QString result;
	qsizetype length = q_string.length( );
	QChar *buff = new QChar[ length ];
	qsizetype index = 0, buffIndex = 0;
	for( ; index < length; ++index ) {
		const QChar &&qchar = q_string[ index ];
		if( qchar.isSpace( ) || qchar.isNonCharacter( ) || qchar.isMark( ) || qchar.isPunct( ) || !qchar.isPrint( ) )
			continue;
		buff[ buffIndex ] = qchar;
		++buffIndex;
	}
	if( buffIndex != 0 )
		result = QString( buff, buffIndex );
	delete buff;
	return result;
}

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
	QSslConfiguration sslConfiguration = networkRequest->sslConfiguration( );
	sslConfiguration.setPeerVerifyMode( QSslSocket::VerifyNone );
	networkRequest->setSslConfiguration( sslConfiguration );
	typeCount = 0;
	QString ingFile( u8"%1%2%3%2%4%5" );
	ingFile = ingFile.arg( Project_Plug_bin ).arg( QDir::separator( ) ).arg( u8"ini" ).arg( u8"get_type" ).arg( u8".txt" );
	QFile qFile;
	qFile.setFileName( ingFile );
	if( qFile.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) ) {
		auto byteArray = qFile.readAll( );
		QString fileContent( byteArray );
		auto list = fileContent.split( "\n" );
		for( auto &str : list ) {
			auto normalQString = getNormalQString( str );
			if( normalQString.isEmpty( ) )
				continue;
			getTypeNamelist << normalQString;
		}
	}
	runStatus = 0;
}

void NovelNetJob::initConnect( ) {
	connect( root->first.get( ), &cylHttpNetWork::RequestConnect::networkReplyFinished, [this]( cylHttpNetWork::RequestConnect *request_connect ) {
		QNetworkReply *networkReply = request_connect->getNetworkReply( );
		emit requesting_get_root_page_signals( networkReply->url( ), request_connect );
	} );

	connect( this, &NovelNetJob::requesting_get_root_page_signals, this, &NovelNetJob::slots_requesting_get_root_page_signals );
	connect( this, &NovelNetJob::requesting_get_type_page_url_signals, this, &NovelNetJob::slots_requesting_get_type_page_url_signals );
	connect( this, &NovelNetJob::requesting_get_next_type_page_url_signals, this, &NovelNetJob::slots_requesting_get_next_type_page_url_signals );
	connect( this, &NovelNetJob::requested_get_type_page_url_end, this, &NovelNetJob::slots_requested_get_type_page_url_end );
	connect( this, &NovelNetJob::requested_get_web_page_signals_end, this, &NovelNetJob::slots_requested_get_web_page_signals_end );
}
bool NovelNetJob::start( ) {
	if( runStatus != 0 )
		return false;
	HtmlDocString resultUrl;
	size_t size = interfaceThisPtr->getRootUrl( &resultUrl );
	QString qUrl = QString::fromStdWString( resultUrl );
	cylHttpNetWork::NetworkRequest::setHostUrlRequestInterval( qUrl, 5000 );
	if( size == 0 )
		return false;
	qDebug( ) << networkAccessManager->supportedSchemes( );
	qDebug( ) << QSslSocket::sslLibraryBuildVersionString( );
	if( OStream::hasDefaultOStream( qUrl ) == false )
		OStream::setDefaultOStream( qUrl, oStream );
	interfaceThisPtr->initAfter( );
	interfaceThisPtr->initBefore( );
	runStatus = 1;
	root->second->netGetWork( qUrl, *networkRequest );
	return true;
}
QString NovelNetJob::getUrl( ) const {
	HtmlDocString resultUrl;
	size_t size = interfaceThisPtr->getRootUrl( &resultUrl );
	if( size )
		return QString::fromStdWString( resultUrl );
	return "";
}
void NovelNetJob::slots_requesting_get_root_page_signals( const QUrl &url, cylHttpNetWork::RequestConnect *request_connect ) {
	auto networkReply = request_connect->getNetworkReply( );
	QString rootUrl = url.host( );
	if( networkReply->error( ) != QNetworkReply::NoError ) {
		QString msg;
		msg.append( u8"首页异常 : " ).append( u8"\n\t" ).append( getErrorQStr( networkReply->error( ) ) ).append( "\n\t" ).append( u8"(" ).append( rootUrl ).append( ")" );
		OStream::anyDebugOut( oStream, msg.toStdString( ), __FILE__, __LINE__, __FUNCTION__ );
		return;
	}
	QString htmlText( networkReply->readAll( ) );
	if( htmlText.isEmpty( ) )
		return;
	auto mapHtmlStrKHtmlStrV = interfaceThisPtr->formHtmlGetTypeTheUrls( rootUrl.toStdWString( ), htmlText.toStdWString( ) );
	if( !mapHtmlStrKHtmlStrV ) {
		QString msg;
		msg.append( u8"首页异常 : " ).append( u8"(没有找到类型节点)结束" ).append( u8"(" ).append( rootUrl ).append( ")" );
		OStream::anyDebugOut( oStream, msg.toStdString( ), __FILE__, __LINE__, __FUNCTION__ );
		return;
	}

	auto iterator = mapHtmlStrKHtmlStrV->begin( );
	auto end = mapHtmlStrKHtmlStrV->end( );
	for( ; iterator != end; ++iterator ) {
		QString typeName = getNormalQString( QString::fromStdWString( iterator->first ) );
		auto getTypeNamelistIterator = getTypeNamelist.begin( );
		auto getTypeNamelistEnd = getTypeNamelist.end( );
		for( ; getTypeNamelistIterator != getTypeNamelistEnd; ++getTypeNamelistIterator )
			if( *getTypeNamelistIterator == typeName )
				break;
		if( getTypeNamelistIterator == getTypeNamelistEnd )
			continue;

		QString typeUrl = QString::fromStdWString( iterator->second );
		auto requestConnect = new cylHttpNetWork::RequestConnect;
		auto request = new cylHttpNetWork::Request( networkAccessManager.get( ), requestConnect );
		connect( requestConnect, &cylHttpNetWork::RequestConnect::networkReplyFinished, [requestConnect,request, typeName, typeUrl ,this]( cylHttpNetWork::RequestConnect *request_connect ) {
			auto networkReply = request_connect->getNetworkReply( );
			if( networkReply->error( ) != QNetworkReply::NoError ) {
				auto msg = getErrorQStr( networkReply->error( ) );
				msg.append( u8"\n类型 : " ).append( typeName ).append( u8"(" ).append( typeUrl ).append( ")" );
				OStream::anyDebugOut( oStream, msg.toStdString( ), __FILE__, __LINE__, __FUNCTION__ );
				return;
			}
			emit requesting_get_type_page_url_signals( getUrl( ), typeName, typeUrl, std::make_shared< cylHtmlTools::HtmlString >( QString( networkReply->readAll( ) ).toStdWString( ) ) );
			requestConnect->deleteLater( );
			request->deleteLater( );
		} );
		++typeCount;
		request->netGetWork( typeUrl, *networkRequest );
	}
}

void NovelNetJob::slots_requesting_get_type_page_url_signals( const QString &root_url, const QString &type_name, const QUrl &type_url, cylHtmlTools::HtmlString_Shared html_string ) {

	size_t count = 0;
	if( this->typeCountMap.count( type_name ) )
		count = this->typeCountMap.at( type_name );

	Vector_INovelInfoSPtr_Shared requestedGetVectorINovelInfoSPtrShared( std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( ) );
	// 存储获取的小说
	auto iterator = this->typeNovelsMap.begin( );
	auto end = this->typeNovelsMap.end( );
	Vector_INovelInfoSPtr_Shared saveMapNovelInfos = nullptr;
	for( ; iterator != end; ++iterator )
		if( iterator->first == type_name ) { // 这是已经被调用了
			saveMapNovelInfos = iterator->second;
			break;
		}
	if( iterator == end ) { // 这是首次调用
		saveMapNovelInfos = std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( );
		this->typeNovelsMap.emplace( type_name, saveMapNovelInfos );
	}

	QString typePageUrl = type_url.toString( );
	auto novelInfos = interfaceThisPtr->formHtmlGetTypePageNovels( type_name.toStdWString( ), typePageUrl.toStdWString( ), *html_string, *saveMapNovelInfos, nullptr );
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
				auto request = new cylHttpNetWork::Request( networkAccessManager.get( ), requestConnect );
				request->netGetWork( novelUrl, *networkRequest );
				auto reply = requestConnect->getNetworkReply( );
				while( !reply->isRunning( ) ) // 等待运行
					request->sleep( 200 );
				while( !reply->isFinished( ) ) // 等待结束
					request->sleep( 200 );
				if( reply->error( ) != QNetworkReply::NoError ) { // 异常则跳过这次获取
					auto msg = getErrorQStr( reply->error( ) );
					msg.append( u8"\n小说地址:" ).append( novelUrl );
					OStream::anyDebugOut( oStream, msg.toStdString( ) );
					continue;
				}
				// 解析小说
				auto readAll = reply->readAll( );
				if( readAll.size( ) == 0 ) { // 异常则跳过这次获取
					QString msg = u8"没有找到正确的内容";
					msg.append( u8"\t小说地址:" ).append( novelUrl );
					OStream::anyDebugOut( oStream, msg.toStdString( ) );
					continue;
				}
				QString htmlText( readAll );
				request->deleteLater( ); // 释放
				requestConnect->deleteLater( ); // 释放
				INovelInfo_Shared formHtmlGetUrlNovelInfo = interfaceThisPtr->formHtmlGetUrlNovelInfo( novelUrl.toStdWString( ), htmlText.toStdWString( ), *saveMapNovelInfos, novel );
				if( formHtmlGetUrlNovelInfo )// 返回非 nullptr 则存储
					requestedGetVectorINovelInfoSPtrShared->emplace_back( novel ); // 存储已知小说
			} else
				requestedGetVectorINovelInfoSPtrShared->emplace_back( novel ); // 存储已知小说
		}
		auto requestedGetVectorIterator = requestedGetVectorINovelInfoSPtrShared->begin( );
		auto requestedGetVectorEnd = requestedGetVectorINovelInfoSPtrShared->end( );
		if( requestedGetVectorIterator != requestedGetVectorEnd ) {
			auto vectorPtr = saveMapNovelInfos.get( );
			for( ; requestedGetVectorIterator != requestedGetVectorEnd; ++requestedGetVectorIterator )
				vectorPtr->emplace_back( *requestedGetVectorIterator );
		}

		auto formHtmlGetNext = interfaceThisPtr->formHtmlGetNext( type_name.toStdWString( ), typePageUrl.toStdWString( ), *html_string, *saveMapNovelInfos, *requestedGetVectorINovelInfoSPtrShared );
		if( formHtmlGetNext.empty( ) )
			break;
		QUrl nextUrl( QString::fromStdWString( formHtmlGetNext ) );
		++count;
		this->typeCountMap.emplace( type_name, count );
		emit requesting_get_next_type_page_url_signals( root_url, type_name, type_url, nextUrl, count, count + 1, saveMapNovelInfos, requestedGetVectorINovelInfoSPtrShared );
		return;  // 正常执行完毕
	} while( false );

	// 没有使用 return 返回，则表示终止调用
	emit requested_get_type_page_url_end( getUrl( ), type_name, type_url, count, requestedGetVectorINovelInfoSPtrShared );
}
void NovelNetJob::slots_requesting_get_next_type_page_url_signals( const QString &root_url, const QString &type_name, const QUrl &old_url, const QUrl &url, size_t old_page_index, size_t current_page_index, const interfacePlugsType::Vector_INovelInfoSPtr_Shared saveMapNovelInfos, const interfacePlugsType::Vector_INovelInfoSPtr_Shared novel_s_ptr_shared ) {
	if( novel_s_ptr_shared->size( ) == 0 ) {
		emit requested_get_type_page_url_end( root_url, type_name, old_url, old_page_index, novel_s_ptr_shared );
		return;
	}
	auto requestConnect = new cylHttpNetWork::RequestConnect;
	auto request = new cylHttpNetWork::Request( networkAccessManager.get( ), requestConnect );
	connect( requestConnect, &cylHttpNetWork::RequestConnect::networkReplyFinished, [requestConnect,request, type_name, url ,this]( cylHttpNetWork::RequestConnect *request_connect ) {
		auto networkReply = request_connect->getNetworkReply( );
		if( networkReply->error( ) != QNetworkReply::NoError ) {
			auto msg = getErrorQStr( networkReply->error( ) );
			msg.append( u8"\n类型 : " ).append( type_name ).append( u8"(" ).append( url.toString( ) ).append( ")" );
			OStream::anyDebugOut( oStream, msg.toStdString( ), __FILE__, __LINE__, __FUNCTION__ );
			return;
		}
		emit requesting_get_type_page_url_signals( getUrl( ), type_name, url, std::make_shared< cylHtmlTools::HtmlString >( QString( networkReply->readAll( ) ).toStdWString( ) ) );
		requestConnect->deleteLater( );
		request->deleteLater( );
	} );
	++typeCount;
	request->netGetWork( url, *networkRequest );
}

void NovelNetJob::slots_requested_get_type_page_url_end( const QString &root_url, const QString &type_name, const QUrl &url, size_t current_page_index, const interfacePlugsType::Vector_INovelInfoSPtr_Shared novel_s_ptr_shared ) {
	qDebug( ) << u8"-----------------------";
	qDebug( ) << type_name << "(" << url.toString( ) << ") 请求结束";
	qDebug( ) << u8"-----------------------";
	auto novelInfos = typeNovelsMap.at( type_name );
	interfaceThisPtr->novelTypeEnd( root_url.toStdWString( ), type_name.toStdWString( ), url.toString( ).toStdWString( ), *novelInfos );
	--typeCount;
	if( typeCount == 0 )
		emit requested_get_web_page_signals_end( root_url );
}
void NovelNetJob::slots_requested_get_web_page_signals_end( const QUrl &url ) {

	Vector_INovelInfoSPtr novelInfoSPtr;
	auto mapIterator = typeNovelsMap.begin( );
	auto mapEnd = typeNovelsMap.end( );
	for( ; mapIterator != mapEnd; ++mapIterator ) {
		for( auto &novel : *mapIterator->second ) {
			auto saveIterator = novelInfoSPtr.begin( );
			auto saveEnd = novelInfoSPtr.end( );
			for( ; saveIterator != saveEnd; ++saveIterator ) {
				auto nsaveNovel = *saveIterator;
				HtmlDocString leftUrl;
				HtmlDocString rightUrl;
				if( nsaveNovel->getNovelUrl( &leftUrl ) == novel->getNovelUrl( &rightUrl ) && leftUrl == rightUrl )
					break;
			}
			if( saveIterator != saveEnd )
				continue;
			novelInfoSPtr.emplace_back( novel );
		}
	}
	interfaceThisPtr->endHost( novelInfoSPtr );
	runStatus = 0; // 可以重新运行
	qDebug( ) << "请求 : " << url.host( ) << " 结束";
}
