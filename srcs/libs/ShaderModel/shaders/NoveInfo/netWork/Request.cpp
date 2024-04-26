#include "Request.h"

#include <QFileDialog>
#include <QNetworkReply>

#include <QtMorc.h>
#include <DebugInfo.h>
#include <QNetworkAccessManager>
#include <qguiapplication.h>

#include "RequestConnect.h"

Request::Request( QObject *parent ) : QObject( parent ) {
	networkAccessManager = new QNetworkAccessManager( this );
}
Request::~Request( ) {
	networkAccessManager->deleteLater( );
	if( setting ) {
		setting->sync( );
		delete setting;
	}
}
void Request::setRequestSettingFilePath( const QString &filePath ) {
	if( setting )
		setting->setPath( QSettings::IniFormat, QSettings::UserScope, filePath );
	else
		setting = new QSettings( filePath, QSettings::IniFormat );
	QVariant value = setting->value( tr( u8"homeUrl" ) );

}

QNetworkReply *Request::netGetWork( const QString &url, RequestConnect *requestConnect ) {
	QUrl qUrl( url );

	return Request::netGetWork( qUrl, requestConnect );
}
QNetworkReply *Request::netGetWork( const QUrl &url, RequestConnect *requestConnect ) {
	QNetworkRequest request;
	request.setHeader( QNetworkRequest::UserAgentHeader, tr( u8"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.5735.289 Safari/537.36 QIHU 360SE" ) );
	request.setUrl( url );
	requestConnect->setNetworkAccessManager( this );
	QNetworkReply *networkReply = networkAccessManager->get( request );
	requestConnect->setNetworkReply( networkReply );
	return networkReply;
}
