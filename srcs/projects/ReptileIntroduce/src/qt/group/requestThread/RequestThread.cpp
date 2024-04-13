#include "RequestThread.h"
#include "../../extend/netWork/RequestConnect.h"
#include "../../extend/thread/RequetNetWorkThread.h"
#include "../../extend/netWork/Request.h"
#include <QUrl>
RequestThread::RequestThread( QObject *parent ): QObject( parent ) {
	urlRoot = nullptr;
	request = new Request( this );
	requestConnect = new RequestConnect( request );
	requetNetWorkThread = new RequetNetWorkThread( this );
}
RequestThread::~RequestThread( ) {
	if( urlRoot )
		delete urlRoot;
}
void RequestThread::setUrl( const QString &url ) {
	if( urlRoot ) {
		if( urlRoot->url( ) == url )
			return;
		delete urlRoot;
	}
	urlRoot = new QUrl( url );
}
void RequestThread::setUrl( const QUrl &url ) {
	if( urlRoot ) {
		if( urlRoot->url( ) == url.url( ) )
			return;
		delete urlRoot;
	}
	urlRoot = new QUrl( url );
}
QUrl RequestThread::getUrl( ) const {
	return *urlRoot;
}
void RequestThread::startThread( ) const {
	requetNetWorkThread->wait( );
	requetNetWorkThread->setRequest( request );
	requetNetWorkThread->setRequestConnect( requestConnect );
	requetNetWorkThread->start( );
}
void RequestThread::waitThread( unsigned long ms ) {
	requetNetWorkThread->waite( ms );
}
void RequestThread::stopThread( unsigned long ms ) {
	requetNetWorkThread->stop( ms );
}
