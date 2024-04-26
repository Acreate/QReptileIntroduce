#ifndef REQUESTTHREAD_H_H_HEAD__FILE__
#define REQUESTTHREAD_H_H_HEAD__FILE__
#pragma once
#include <QObject>
class RequetNetWorkThread;
class RequestConnect;
class Request;
#include "../../export/Tools_export.h"

class TOOLS_EXPORT RequestThread : public QObject {
	Q_OBJECT;
private:
	QUrl *urlRoot;
	Request *request;
	RequestConnect *requestConnect;
	RequetNetWorkThread *requetNetWorkThread;
public:
	RequestThread( QObject *parent );
	~RequestThread( ) override;
public:
	void setUrl( const QString &url );
	void setUrl( const QUrl &url );
	QUrl getUrl( ) const;
	void startThread( ) const;
	void waitThread( unsigned long ms = 20 );
	void stopThread( unsigned long ms = 20 );
Q_SIGNALS:
	void finish( const QUrl &url, const QString &html, Request *request, RequestConnect *requestConnect );
	void error( const QUrl &url );
	void start( );
	void stop( );
};

#endif // REQUESTTHREAD_H_H_HEAD__FILE__
