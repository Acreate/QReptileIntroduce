#include "RequestConnect.h"
#include "Request.h"
#include <DebugInfo.h>
#include <QtMorc.h>
RequestConnect::~RequestConnect( ) {

}
void RequestConnect::setNetworkAccessManager( QNetworkAccessManager *const networkAccessManager ) {
	if( this->networkAccessManager.count( networkAccessManager ) != 0 )
		return;
	this->networkAccessManager.append( networkAccessManager );
	QT_CONNECT_AUTO_THIS( networkAccessManager, QNetworkAccessManager, authenticationRequired, RequestConnect, networkAccessManagerAuthenticationRequired );
	QT_CONNECT_AUTO_THIS( networkAccessManager, QNetworkAccessManager, encrypted, RequestConnect, networkAccessManagerEncrypted );
	QT_CONNECT_AUTO_THIS( networkAccessManager, QNetworkAccessManager, finished, RequestConnect, networkAccessManagerFinished );
	QT_CONNECT_AUTO_THIS( networkAccessManager, QNetworkAccessManager, preSharedKeyAuthenticationRequired, RequestConnect, networkAccessManagerPreSharedKeyAuthenticationRequired );
	QT_CONNECT_AUTO_THIS( networkAccessManager, QNetworkAccessManager, proxyAuthenticationRequired, RequestConnect, networkAccessManagerProxyAuthenticationRequired );
	QT_CONNECT_AUTO_THIS( networkAccessManager, QNetworkAccessManager, sslErrors, RequestConnect, networkAccessManagerSslErrors );
	connect( networkAccessManager, &QNetworkAccessManager::destroyed, [=]( ) {
		QObject *object = sender( );
		auto removeObj = qobject_cast< QNetworkAccessManager * >( object );
		if( removeObj ) {
			this->networkAccessManager.removeAll( removeObj );
		}
	} );
}
void RequestConnect::setNetworkReply( QNetworkReply *const networkReply ) {
	this->networkReply = networkReply;
	QT_CONNECT_AUTO_THIS( networkReply, QNetworkReply, socketStartedConnecting, RequestConnect, networkReplySocketStartedConnecting );
	QT_CONNECT_AUTO_THIS( networkReply, QNetworkReply, requestSent, RequestConnect, networkReplyRequestSent );
	QT_CONNECT_AUTO_THIS( networkReply, QNetworkReply, metaDataChanged, RequestConnect, networkReplyMetaDataChanged );
	connect( networkReply, &QNetworkReply::finished, [=]( ) {
		emit networkReplyFinished( this );
	} );
	QT_CONNECT_AUTO_THIS( networkReply, QNetworkReply, errorOccurred, RequestConnect, networkReplyErrorOccurred );
#if QT_CONFIG( ssl )
	QT_CONNECT_AUTO_THIS( networkReply, QNetworkReply, encrypted, RequestConnect, networkReplyEncrypted );
	QT_CONNECT_AUTO_THIS( networkReply, QNetworkReply, sslErrors, RequestConnect, networkReplySslErrors );
	QT_CONNECT_AUTO_THIS( networkReply, QNetworkReply, preSharedKeyAuthenticationRequired, RequestConnect, networkReplyPreSharedKeyAuthenticationRequired );
#endif
	QT_CONNECT_AUTO_THIS( networkReply, QNetworkReply, redirected, RequestConnect, networkReplyRedirected );
	QT_CONNECT_AUTO_THIS( networkReply, QNetworkReply, redirectAllowed, RequestConnect, networkReplyRedirectAllowed );
	QT_CONNECT_AUTO_THIS( networkReply, QNetworkReply, uploadProgress, RequestConnect, networkReplyUploadProgress );
	QT_CONNECT_AUTO_THIS( networkReply, QNetworkReply, downloadProgress, RequestConnect, networkReplyDownloadProgress );
}
