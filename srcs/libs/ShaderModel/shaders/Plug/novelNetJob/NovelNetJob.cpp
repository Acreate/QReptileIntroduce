#include "NovelNetJob.h"
#include <netWork/RequestConnect.h>
#include <netWork/Request.h>
#include <interface/IRequestNetInterfaceExtend.h>
NovelNetJob::NovelNetJob( QObject *parent, QObject *interface_obj_ptr, IRequestNetInterfaceExtend *interface_this_ptr ): QObject( parent ), interfaceObjPtr( interface_obj_ptr ), interfaceThisPtr( interface_this_ptr ) {

}

NovelNetJob::~NovelNetJob( ) {
	interfaceObjPtr->deleteLater( );
}
void NovelNetJob::start( ) {
	Request *request = new Request( this );
	RequestConnect *request_connect = new RequestConnect( this );
	std::string url;
	interfaceThisPtr->getUrl( &url );
	request->netGetWork( QString::fromStdString( url ), request_connect );
	connect( request_connect, &RequestConnect::networkReplyFinished, this, &NovelNetJob::networkReplyFinished );
}
QString NovelNetJob::getUrl( ) const {
	std::string resultUrl;
	size_t size = interfaceThisPtr->getUrl( &resultUrl );
	if( size )
		return QString::fromStdString( resultUrl );
	return "";
}
void NovelNetJob::networkReplyFinished( RequestConnect *requestConnect ) {

}
