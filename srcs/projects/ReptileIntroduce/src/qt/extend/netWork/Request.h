#ifndef REQUEST_H_H_HEAD__FILE__
#define REQUEST_H_H_HEAD__FILE__
#pragma once
#include <QObject>
#include <QSettings>
#include <QNetworkReply>
class RequestConnect;
class Request : public QObject {
	Q_OBJECT;
private:
	QSettings *setting = nullptr;
	QNetworkAccessManager *networkAccessManager;
public:
	Request( QObject *parent = nullptr );
	~Request( ) override;
public:
	QNetworkAccessManager *getNetworkAccessManager( ) const {
		return networkAccessManager;
	}
	void setRequestSettingFilePath( const QString &filePath );
	QNetworkReply *netGetWork( const QString &url, RequestConnect *requestConnect );
	QNetworkReply *netGetWork( const QUrl &url, RequestConnect *requestConnect );

};

#endif // REQUEST_H_H_HEAD__FILE__
