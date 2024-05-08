﻿#include "RequestConnect.h"
#include "Request.h"
#include <DebugInfo.h>
#include <QtMorc.h>
#include "NetworkAccessManager.h"
RequestConnect::RequestConnect( QObject *parent ): QObject( parent ), networkAccessManager( nullptr ), networkReply( nullptr ) {
}
RequestConnect::~RequestConnect( ) {

}
void RequestConnect::setNetworkAccessManager( NetworkAccessManager *networkAccessManager ) {
	if( this->networkAccessManager == networkAccessManager )
		return;
	this->networkAccessManager = networkAccessManager;
	QT_CONNECT_AUTO_THIS( networkAccessManager, QNetworkAccessManager, authenticationRequired, RequestConnect, networkAccessManagerAuthenticationRequired );
	QT_CONNECT_AUTO_THIS( networkAccessManager, QNetworkAccessManager, encrypted, RequestConnect, networkAccessManagerEncrypted );
	QT_CONNECT_AUTO_THIS( networkAccessManager, QNetworkAccessManager, finished, RequestConnect, networkAccessManagerFinished );
	QT_CONNECT_AUTO_THIS( networkAccessManager, QNetworkAccessManager, preSharedKeyAuthenticationRequired, RequestConnect, networkAccessManagerPreSharedKeyAuthenticationRequired );
	QT_CONNECT_AUTO_THIS( networkAccessManager, QNetworkAccessManager, proxyAuthenticationRequired, RequestConnect, networkAccessManagerProxyAuthenticationRequired );
	QT_CONNECT_AUTO_THIS( networkAccessManager, QNetworkAccessManager, sslErrors, RequestConnect, networkAccessManagerSslErrors );

	auto overload = QOverload< QObject * >::of( &QObject::destroyed );
	// 对象被删除
	connect( networkAccessManager, overload, [=]( QObject *obj ) {
		if( networkAccessManager == obj )
			this->networkAccessManager = nullptr;
		qDebug( ) << __FILE__ << " : " << __LINE__ << "\n\t""networkAccessManager, overload, [=]( QObject *" << obj << " )";
	} );
}
void RequestConnect::setNetworkReply( QNetworkReply *const networkReply ) {
	if( this->networkReply == networkReply )
		return;
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

	auto overload = QOverload< QObject * >::of( &QObject::destroyed );
	// 对象被删除
	connect( networkReply, overload, [=]( QObject *obj ) {
		qDebug( ) << __FILE__ << " : " << __LINE__ << "\n\t" "networkReply, overload, [=]( QObject *" << obj << " )";
	} );
}
