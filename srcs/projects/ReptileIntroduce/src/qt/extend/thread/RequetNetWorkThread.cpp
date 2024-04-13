#include "RequetNetWorkThread.h"
#include <QtMath>

#include "../netWork/Request.h"

RequetNetWorkThread::RequetNetWorkThread( const QUrl &url, Request *request, RequestConnect *requestConnect, QObject *parent )
: QThread( parent ), url( url ), request( request ), requestConnect( requestConnect ) {

}
RequetNetWorkThread::RequetNetWorkThread( QObject *parent ): QThread( parent ), request( nullptr ), requestConnect( nullptr ) {
}
RequetNetWorkThread::~RequetNetWorkThread( ) {
}
QUrl RequetNetWorkThread::getUrl( ) const {
	return url;
}
void RequetNetWorkThread::setUrl( const QUrl &url ) {
	this->url = url;
}
Request *RequetNetWorkThread::getRequest( ) const {
	return request;
}
void RequetNetWorkThread::setRequest( Request *const request ) {
	this->request = request;
}
RequestConnect *RequetNetWorkThread::getRequestConnect( ) const {
	return requestConnect;
}
void RequetNetWorkThread::setRequestConnect( RequestConnect *const requestConnect ) {
	this->requestConnect = requestConnect;
}
void RequetNetWorkThread::run( ) {
	if( isInterruptionRequested( ) )
		return;
	QNetworkReply *reply = request->netGetWork( url, requestConnect );
	emit netOver( url, request, requestConnect, reply );
}
void RequetNetWorkThread::waite( unsigned long sleepMS ) {
	while( isRunning( ) && !isFinished( ) )
		QThread::sleep( sleepMS );
}
void RequetNetWorkThread::waite( unsigned long sleepMS, std::function< int64_t( RequetNetWorkThread &currentThread ) > &&function ) {
	while( isRunning( ) && !isFinished( ) ) {
		auto funResut = function( *this );
		if( funResut < 0 ) {
			QThread::sleep( qAbs( funResut ) );
			return;
		} else if( funResut > 0 )
			QThread::sleep( funResut );
		else
			QThread::sleep( sleepMS );

	}
}
void RequetNetWorkThread::stop( unsigned long sleepMS ) {

	while( isRunning( ) && !isFinished( ) ) {
		requestInterruption( );
		QThread::sleep( sleepMS );
	}
}
void RequetNetWorkThread::stop( unsigned long sleepMS, std::function< int64_t( RequetNetWorkThread &currentThread ) > &&function ) {
	while( isRunning( ) && !isFinished( ) ) {
		requestInterruption( );
		auto funResut = function( *this );
		if( funResut < 0 ) {
			QThread::sleep( qAbs( funResut ) );
			return;
		} else if( funResut > 0 )
			QThread::sleep( funResut );
		else
			QThread::sleep( sleepMS );
	}
}
