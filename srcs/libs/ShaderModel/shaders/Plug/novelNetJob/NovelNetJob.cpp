#include "NovelNetJob.h"
#include <netWork/RequestConnect.h>
#include <netWork/Request.h>
#include <stream/OStream.h>

#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/HtmlNode/HtmlNode.h"
#include "thread/FileThread.h"
using namespace interfacePlugsType;
NovelNetJob::NovelNetJob( QObject *parent, OStream *o_stream, QObject *interface_obj_ptr, IRequestNetInterfaceExtend *interface_this_ptr ): QObject( parent ), interfaceObjPtr( interface_obj_ptr ), interfaceThisPtr( interface_this_ptr ), oStream( o_stream ) {
	interface_this_ptr->setOStream( oStream );
}

NovelNetJob::~NovelNetJob( ) {
	interfaceObjPtr->deleteLater( );
}
void NovelNetJob::start( ) {
	Request *request = new Request( this );
	RequestConnect *requestConnect = new RequestConnect( this );
	connect( requestConnect, &RequestConnect::networkReplyFinished, [=]( RequestConnect *request_connect )->void {
		QNetworkReply *networkReply = request_connect->getNetworkReply( );
		auto byteArray = networkReply->readAll( );
		if( byteArray.size( ) == 0 )
			return;
		*oStream << u8"请求完毕 : " << networkReply->url( ).host( ) << u8"(" << QThread::currentThread( )->currentThreadId( ) << u8")" << "\n";
		oStream->flush( );
		HtmlDocString htmlDocString = QString( byteArray ).toStdWString( );
		auto pairs = interfaceThisPtr->formHtmlGetTypeTheUrls( htmlDocString );
		if( pairs ) {
			auto iterator = pairs->begin( );
			auto end = pairs->end( );
			for( ; iterator != end; ++iterator ) {
				*oStream << iterator->first << u8":" << iterator->second << '\n';
			}
			oStream->flush( );
		}
		request->deleteLater( );
		requestConnect->deleteLater( );
	} );
	HtmlDocString url;
	interfaceThisPtr->getUrl( &url );
	*oStream << u8"正在请求 : " << url << u8"(" << QThread::currentThread( )->currentThreadId( ) << u8")" << "\n";
	oStream->flush( );
	request->netGetWork( QString::fromStdWString( url ), requestConnect );
}
QString NovelNetJob::getUrl( ) const {
	HtmlDocString resultUrl;
	size_t size = interfaceThisPtr->getUrl( &resultUrl );
	if( size )
		return QString::fromStdWString( resultUrl );
	return "";
}
