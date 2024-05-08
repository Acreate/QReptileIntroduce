#ifndef REQUEST_H_H_HEAD__FILE__
#define REQUEST_H_H_HEAD__FILE__
#pragma once
#include <QObject>
#include <QSettings>
#include <QNetworkReply>

class NetworkRequest;
class NetworkAccessManager;
class RequestConnect;
#include "../export/Tools_export.h"

class TOOLS_EXPORT Request : public QObject {
	Q_OBJECT;
private:
	QSettings *setting = nullptr;
	NetworkAccessManager *networkAccessManager;
	RequestConnect *requestConnect;
public:
	Request( NetworkAccessManager *networkAccessManager, RequestConnect *requestConnect );
	Request( NetworkAccessManager *networkAccessManager, RequestConnect *requestConnect, QObject *parent );
	~Request( ) override;
public:
	QNetworkReply * netGetWork( const QString &url );
	QNetworkReply * netGetWork( const QUrl &url );
	QNetworkReply * netGetWork( const QString &url, NetworkRequest &network_request );
	QNetworkReply * netGetWork( const QUrl &url, NetworkRequest &network_request );
	QNetworkReply * netGetWork( NetworkRequest &network_request );

};

#endif // REQUEST_H_H_HEAD__FILE__
