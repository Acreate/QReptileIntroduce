#include "NovelNetJob.h"
#include <netWork/RequestConnect.h>
#include <netWork/Request.h>
#include <netWork/NetworkRequest.h>
#include <stream/OStream.h>

#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/HtmlNode/HtmlNode.h"
#include "thread/FileThread.h"
#include <netWork/NetworkAccessManager.h>
#include <qguiapplication.h>

#include "interface/INovelInfo.h"
using namespace interfacePlugsType;
NovelNetJob::NovelNetJob( QObject *parent, OStream *o_stream, QObject *interface_obj_ptr, IRequestNetInterface *interface_this_ptr ): QObject( parent ), interfaceObjPtr( interface_obj_ptr ), interfaceThisPtr( interface_this_ptr ), oStream( o_stream ), networkAccessManagerSPtr( std::make_shared< NetworkAccessManager >( ) ), networkRequestSPtr( std::make_shared< NetworkRequest >( ) ) {
	networkRequestSPtr->setHeader( QNetworkRequest::UserAgentHeader, networkRequestSPtr->getRandomUserAgentHeader( ) );
	interface_this_ptr->setOStream( oStream );
	auto getSUrl = qOverload< const QString &, const QString & >( &NovelNetJob::get_type_page_url );
	auto getsignal_get = qOverload< const QString &, const QString & >( &NovelNetJob::signal_get_type_page_url );
	connect( this, getSUrl, this, getsignal_get );
	auto getQUrl = qOverload< const QString &, const QUrl & >( &NovelNetJob::get_type_page_url );
	auto getQsignal_get = qOverload< const QString &, const QUrl & >( &NovelNetJob::signal_get_type_page_url );
	connect( this, getQUrl, this, getQsignal_get );
}

NovelNetJob::~NovelNetJob( ) {
	interfaceObjPtr->deleteLater( );
}
void NovelNetJob::start( ) {
	RequestConnect *requestConnect = new RequestConnect( this );
	Request *request = new Request( networkAccessManagerSPtr.get( ), requestConnect );
	connect( requestConnect, &RequestConnect::networkReplyFinished, [=]( RequestConnect *request_connect )->void {
		thread( )->currentThread( )->sleep( 5 );
		if( errorUrlMap.count( request_connect ) == 0 )
			root_get( request_connect );
		else
			qDebug( ) << "错误: " << errorUrlMap.at( request_connect );
		request->deleteLater( );
		requestConnect->deleteLater( );
	} );
	connect( requestConnect, &RequestConnect::networkReplyErrorOccurred,
		[=]( QNetworkReply::NetworkError network_error )->void {
			errorUrlMap.emplace( requestConnect, network_error );
		} );
	HtmlDocString url;
	interfaceThisPtr->getUrl( &url );
	*oStream << u8"start 正在请求 : " << url << u8"(" << QThread::currentThread( )->currentThreadId( ) << u8")" << "\n";
	oStream->flush( );

	request->netGetWork( QString::fromStdWString( url ), *networkRequestSPtr );
}
QString NovelNetJob::getUrl( ) const {
	HtmlDocString resultUrl;
	size_t size = interfaceThisPtr->getUrl( &resultUrl );
	if( size )
		return QString::fromStdWString( resultUrl );
	return "";
}

void NovelNetJob::root_get( RequestConnect *request_connect ) {
	QNetworkReply *networkReply = request_connect->getNetworkReply( );
	auto byteArray = networkReply->readAll( );
	if( byteArray.size( ) == 0 ) {
		*oStream << u8"数据异常 : " << networkReply->url( ).host( ) << u8"(" << QThread::currentThread( )->currentThreadId( ) << u8")" << "\n";
		oStream->flush( );
		return;
	}
	*oStream << u8"root_get 请求完毕 : " << networkReply->url( ).host( ) << u8"(" << QThread::currentThread( )->currentThreadId( ) << u8")" << "\n";
	oStream->flush( );
	HtmlDocString htmlDocString = QString( byteArray ).toStdWString( );
	auto pairs = interfaceThisPtr->formHtmlGetTypeTheUrls( htmlDocString );
	if( pairs ) {
		auto iterator = pairs->begin( );
		auto end = pairs->end( );
		for( ; iterator != end; ++iterator ) {
			*oStream << iterator->first << u8"(" << iterator->second << u8")" u8"\n";
			emit get_type_page_url( QString::fromStdWString( iterator->first ), QString::fromStdWString( iterator->second ) );
		}
		oStream->flush( );
	}
}
interfacePlugsType::INovelInfo_Shared NovelNetJob::get_request_novel_url( const Vector_NovelSPtr &saveNovelInfos, const INovelInfo_Shared &networkReplayNovel ) {
	RequestConnect *requestConnect = new RequestConnect( this );
	Request *request = new Request( networkAccessManagerSPtr.get( ), requestConnect );
	INovelInfo_Shared inovelPtr = nullptr;
	bool isEnd = false;
	connect( requestConnect, &RequestConnect::networkReplyFinished, [&]( RequestConnect *request_connect )->void {
		// 处理小说
		QNetworkReply *networkReply = request_connect->getNetworkReply( );
		auto byteArray = networkReply->readAll( );
		if( byteArray.size( ) != 0 ) {
			HtmlDocString htmlDocString = QString( byteArray ).toStdWString( );
			inovelPtr = interfaceThisPtr->formHtmlGetUrlNovelInfo( htmlDocString, saveNovelInfos, networkReplayNovel );
		}
		*oStream << u8"connect( requestConnect )请求完毕 : " << networkReply->url( ).host( ) << u8"(" << QThread::currentThread( )->currentThreadId( ) << u8")" << "\n";
		request->deleteLater( );
		requestConnect->deleteLater( );
		isEnd = true;
		oStream->flush( );
	} );
	HtmlDocString url;
	if( networkReplayNovel->getNovelUrl( &url ) > 0 ) {
		request->netGetWork( QString::fromStdWString( url ), *networkRequestSPtr );
		do {
			if( isEnd )
				break;
			qApp->processEvents( ); // 没有结束，则处理其他消息，然后继续返回该出
		} while( true );
	}
	*oStream << u8"get_request_novel_url 请求完毕 : " << url << u8"(" << QThread::currentThread( )->currentThreadId( ) << u8")" << "\n";
	oStream->flush( );
	return inovelPtr;
}
void NovelNetJob::type_get( const QString &type, const QUrl &url, RequestConnect *request_connect ) {

	QNetworkReply *networkReply = request_connect->getNetworkReply( );
	auto byteArray = networkReply->readAll( );
	if( byteArray.size( ) == 0 )
		return;
	*oStream << u8"请求完毕 : " << networkReply->url( ).host( ) << u8"(" << QThread::currentThread( )->currentThreadId( ) << u8")" << "\n";
	oStream->flush( );
	HtmlDocString htmlDocString = QString( byteArray ).toStdWString( );

	auto novelInfos = typeMap.at( type );
	auto novelPtrList = interfaceThisPtr->formHtmlGetTypePageNovels( htmlDocString, *novelInfos, nullptr );
	if( novelPtrList.size( ) > 0 )
		for( auto &novelSPtr : novelPtrList ) {
			if( interfaceThisPtr->isRequestNovelInfoUrl( novelSPtr.get( ) ) ) {
				thread( )->currentThread( )->usleep( 500 );
				novelInfos->emplace_back( get_request_novel_url( *novelInfos, novelSPtr ) );
			} else
				novelInfos->emplace_back( novelSPtr );
		}

}
void NovelNetJob::signal_get_type_page_url( const QString &type, const QString &url ) {
	NovelNetJob::signal_get_type_page_url( type, QUrl( url ) );
}
void NovelNetJob::signal_get_type_page_url( const QString &type, const QUrl &url ) {
	typeMap.emplace( type, std::make_shared< Vector_NovelSPtr >( ) );
	RequestConnect *requestConnect = new RequestConnect( this );
	Request *request = new Request( networkAccessManagerSPtr.get( ), requestConnect );
	connect( requestConnect, &RequestConnect::networkReplyFinished, [=]( RequestConnect *request_connect )->void {
		type_get( type, url, request_connect );
		request->deleteLater( );
		requestConnect->deleteLater( );
	} );
	thread( )->currentThread( )->usleep( 500 );
	request->netGetWork( url, *networkRequestSPtr );
}
