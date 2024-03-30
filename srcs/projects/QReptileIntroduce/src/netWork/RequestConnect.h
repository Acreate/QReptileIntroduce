﻿#ifndef REQUESTCONNECT_H_H_HEAD__FILE__
#define REQUESTCONNECT_H_H_HEAD__FILE__
#pragma once
#include <QNetworkReply>
#include <qtmetamacros.h>

class RequestConnect : public QObject {
	Q_OBJECT;
private:
	QNetworkAccessManager *networkAccessManager;
	QNetworkReply *networkReply;
public:
	RequestConnect( ) : networkAccessManager( nullptr ), networkReply( nullptr ) {
	}

	QNetworkAccessManager *getNetworkAccessManager( ) const {
		return networkAccessManager;
	}
	QNetworkReply *getNetworkReply( ) const {
		return networkReply;
	}

	void setNetworkAccessManager( QNetworkAccessManager *const networkAccessManager );
	void setNetworkReply( QNetworkReply *const networkReply );
Q_SIGNALS: //  QNetworkAccessManager 转发
	void networkAccessManagerAuthenticationRequired( QNetworkReply *reply, QAuthenticator *authenticator );
	void networkAccessManagerEncrypted( QNetworkReply *reply );
	void networkAccessManagerFinished( QNetworkReply *reply );
	void networkAccessManagerPreSharedKeyAuthenticationRequired( QNetworkReply *reply, QSslPreSharedKeyAuthenticator *authenticator );
	void networkAccessManagerProxyAuthenticationRequired( const QNetworkProxy &proxy, QAuthenticator *authenticator );
	void networkAccessManagerSslErrors( QNetworkReply *reply, const QList< QSslError > &errors );
Q_SIGNALS: //  QNetworkReply 转发
	void networkReplySocketStartedConnecting( );
	void networkReplyRequestSent( );
	void networkReplyMetaDataChanged( );
	void networkReplyFinished( );
	void networkReplyErrorOccurred( QNetworkReply::NetworkError );
#if QT_CONFIG( ssl )
	void networkReplyEncrypted( );
	void networkReplySslErrors( const QList< QSslError > &errors );
	void networkReplyPreSharedKeyAuthenticationRequired( QSslPreSharedKeyAuthenticator *authenticator );
#endif
	void networkReplyRedirected( const QUrl &url );
	void networkReplyRedirectAllowed( );

	void networkReplyUploadProgress( qint64 bytesSent, qint64 bytesTotal );
	void networkReplyDownloadProgress( qint64 bytesReceived, qint64 bytesTotal );
};

#endif // REQUESTCONNECT_H_H_HEAD__FILE__
