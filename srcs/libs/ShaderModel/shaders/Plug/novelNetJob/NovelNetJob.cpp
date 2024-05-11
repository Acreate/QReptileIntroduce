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
	connect( root->first.get( ), &cylHttpNetWork::RequestConnect::networkReplyFinished, this, &NovelNetJob::root_get_over );
	typeRequestMap = std::make_shared< Unordered_Map_Pairt >( );
}
void NovelNetJob::initObjProperty( ) {
	interfaceThisPtr->setOStream( oStream );
	networkRequest->setHeader( QNetworkRequest::UserAgentHeader, networkRequest->getRandomUserAgentHeader( ) );
}

void NovelNetJob::initConnect( ) {

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
void NovelNetJob::root_get_over( cylHttpNetWork::RequestConnect *request ) {
	auto networkReply = request->getNetworkReply( );
	QString htmlText( networkReply->readAll( ) );
	if( htmlText.isEmpty( ) )
		return;
	auto mapHtmlStrKHtmlStrV = interfaceThisPtr->formHtmlGetTypeTheUrls( htmlText.toStdWString( ) );

	if( !mapHtmlStrKHtmlStrV )
		return;
	typeRequestMap->clear( );
	auto iterator = mapHtmlStrKHtmlStrV->begin( );
	auto end = mapHtmlStrKHtmlStrV->end( );
	for( ; iterator != end; ++iterator ) {
		// 构建 url 映射
		NovelTypeNamePair_Shared urlPair(std::make_shared<NovelTypeNamePair>(  ));
		urlPair->setUrl( QString::fromStdWString( iterator->second ) );
		urlPair->setTypeName( QString::fromStdWString( iterator->first ) );
		Request_Pairt_Shared requestPairtShared = std::make_shared< Request_Pairt >( );
		requestPairtShared->first = std::make_shared< cylHttpNetWork::RequestConnect >( );
		requestPairtShared->second = std::make_shared< cylHttpNetWork::Request >( networkAccessManager.get( ), requestPairtShared->first.get( ) );
		typeRequestMap->emplace( urlPair, requestPairtShared );
		qDebug( ) << urlPair->getTypeName( ) << " : " << urlPair->getUrl( ).host( );
		qDebug( ) << "==============";

		// todo : 开始请求

	}
}
void NovelNetJob::slots_requesting_get_root_page_signals( const QUrl &url ) {
	qDebug( ) << u8"slots_requesting_get_root_page_signals" << url;
}
void NovelNetJob::slots_requesting_get_type_page_url_signals( const QString &root_url, const QString &type, const QUrl &url ) {
}
void NovelNetJob::slots_requesting_get_next_type_page_url_signals( const QString &root_url, const QString &type, const QUrl &old_url, const QUrl &url, size_t old_page_index, size_t current_page_index, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared ) {
}
void NovelNetJob::slots_requesting_get_novel_page_url_signals( const QString &root_url, const QString &type, const QString &type_page_url, const QString &novelName, const QUrl &url, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared ) {
}
void NovelNetJob::slots_requested_get_novel_page_url_signals( const QString &root_url, const QString &type, const QString &type_page_url, const QString &novelName, const QUrl &url, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared, const interfacePlugsType::INovelInfo_Shared novel_info_shared ) {
}
void NovelNetJob::slots_requested_get_type_page_url_end( const QString &root_url, const QString &type, const QUrl &url, size_t current_page_index, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared ) {
}
void NovelNetJob::slots_requested_get_web_page_signals_end( const QUrl &url ) {
}
