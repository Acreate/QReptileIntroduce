#include "NovelNetJob.h"
#include <netWork/RequestConnect.h>
#include <netWork/Request.h>
#include <interface/IRequestNetInterfaceExtend.h>
#include <stream/IStream.h>

NovelNetJob::NovelNetJob( QObject *parent, IStream *i_stream, QObject *interface_obj_ptr, IRequestNetInterfaceExtend *interface_this_ptr ): QObject( parent ), interfaceObjPtr( interface_obj_ptr ), interfaceThisPtr( interface_this_ptr ), iStream( i_stream ) {

}

NovelNetJob::~NovelNetJob( ) {
	interfaceObjPtr->deleteLater( );
}
void NovelNetJob::start( ) {
	Request *request = new Request( this );
	RequestConnect *requestConnect = new RequestConnect( this );
	connect( requestConnect, &RequestConnect::networkReplyFinished, [=]( RequestConnect *request_connect ) {
		auto byteArray = request_connect->getNetworkReply( )->readAll( );
		if( byteArray.size( ) == 0 )
			return;
		std::string stdString = byteArray.toStdString( );
		*iStream << stdString << "\n";
		iStream->flush( );
	} );
	std::string url;
	interfaceThisPtr->getUrl( &url );
	*iStream << u8"正在请求 : " << url << "\n";
	iStream->flush( );
	request->netGetWork( QString::fromStdString( url ), requestConnect );
}
QString NovelNetJob::getUrl( ) const {
	std::string resultUrl;
	size_t size = interfaceThisPtr->getUrl( &resultUrl );
	if( size )
		return QString::fromStdString( resultUrl );
	return "";
}
