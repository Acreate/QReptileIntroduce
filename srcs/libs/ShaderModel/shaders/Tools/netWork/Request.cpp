#include "Request.h"

#include <QFileDialog>
#include <QNetworkReply>

#include <QtMorc.h>
#include <DebugInfo.h>
#include "NetworkAccessManager.h"
#include <qguiapplication.h>

#include "RequestConnect.h"
#include "NetworkRequest.h"


Request::Request( NetworkAccessManager *networkAccessManager, RequestConnect *requestConnect, QObject *parent ) : networkAccessManager( networkAccessManager ), QObject( parent ), requestConnect( requestConnect ) {
	requestConnect->setNetworkAccessManager( networkAccessManager );
}

Request::Request( NetworkAccessManager *networkAccessManager, RequestConnect *requestConnect ): Request( networkAccessManager, requestConnect, nullptr ) {
}
Request::~Request( ) {
}


QNetworkReply * Request::netGetWork( const QString &url ) {
	QUrl qUrl( url );
	return Request::netGetWork( qUrl );
}
QNetworkReply * Request::netGetWork( const QUrl &url ) {
	NetworkRequest request;
	request.setUrl( url );
	return netGetWork( request );
}
QNetworkReply * Request::netGetWork( const QString &url, NetworkRequest &network_request ) {
	QUrl qUrl( url );
	NetworkRequest networkRequest = network_request;
	networkRequest.setUrl( qUrl );
	return netGetWork( network_request );

}
QNetworkReply * Request::netGetWork( const QUrl &url, NetworkRequest &network_request ) {
	NetworkRequest networkRequest = network_request;
	networkRequest.setUrl( url );
	return netGetWork( network_request );
}
QNetworkReply * Request::netGetWork( NetworkRequest &network_request ) {
	requestConnect->setNetworkAccessManager( networkAccessManager );
	QNetworkReply *networkReply = networkAccessManager->get( network_request );
	requestConnect->setNetworkReply( networkReply );
	return networkReply;
}
