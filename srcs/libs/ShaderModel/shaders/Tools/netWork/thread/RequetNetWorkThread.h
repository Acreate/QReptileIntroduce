#ifndef REQUETNETWORKTHREAD_H_H_HEAD__FILE__
#define REQUETNETWORKTHREAD_H_H_HEAD__FILE__
#pragma once

#include <QThread>
#include <QUrl>
#include <qnetworkreply.h>
class Request;
class RequestConnect;
#include "../../export/Tools_export.h"

class TOOLS_EXPORT RequetNetWorkThread : public QThread {
	Q_OBJECT;
private: //  它们不应该由该类控制释放
	/// <summary>
	/// 地址
	/// </summary>
	QUrl url;
	/// <summary>
	/// 请求对象向
	/// </summary>
	Request *request;
	/// <summary>
	/// 请求对象响应槽
	/// </summary>
	RequestConnect *requestConnect;
public:
	RequetNetWorkThread( const QUrl &url, Request *request, RequestConnect *requestConnect, QObject *parent );
	RequetNetWorkThread( QObject *parent );
	~RequetNetWorkThread( ) override;
public:
	QUrl getUrl( ) const;
	void setUrl( const QUrl &url );
	Request * getRequest( ) const;
	void setRequest( Request *const request );
	RequestConnect * getRequestConnect( ) const;
	void setRequestConnect( RequestConnect *const requestConnect );
protected:
	void run( ) override;
	/// <summary>
	/// 等待终止 <br/>
	/// 若线程一直不停止，该函数将会用无休止等待
	/// </summary>
	/// <param name="sleepMS">每次睡眠时间</param>
public:
	void waite( unsigned long sleepMS );
	/// <summary>
	/// 等待终止 <br/>
	/// 存在参数 function 函数，该函数校验该对象继续方式<br/>
	/// function 函数 <br/>
	///		返回 0 表示默认方式 <br/>
	///		返回 > 0 表示等待时间 <br/>
	///		返回 < 0 表示只等待一次即将返回 <br/>
	/// </summary>
	/// <param name="sleepMS">每次睡眠时间时间</param>
	/// <param name="function">校验函数</param>
	void waite( unsigned long sleepMS, std::function< int64_t( RequetNetWorkThread &currentThread ) > &&function );
	/// <summary>
	/// 终止线程 <br/>
	/// 该函数会发送终止信号到线程，线程中的 isInterruptionRequested( ) 会返回这个信号。 <br/>
	/// </summary>
	/// <param name="sleepMS">每次睡眠时间</param>
	void stop( unsigned long sleepMS );
	/// <summary>
	/// 终止线程 <br/>
	/// 该函数会发送终止信号到线程，线程中的 isInterruptionRequested( ) 会返回这个信号。 <br/>
	/// 存在参数 function 函数，该函数校验该对象继续方式<br/>
	/// function 函数 <br/>
	///		返回 0 表示默认方式 <br/>
	///		返回 > 0 表示等待时间 <br/>
	///		返回 < 0 表示只等待一次即将返回 <br/>
	/// </summary>
	/// <param name="sleepMS">每次睡眠时间时间</param>
	/// <param name="function">校验函数</param>
	void stop( unsigned long sleepMS, std::function< int64_t( RequetNetWorkThread &currentThread ) > &&function );
Q_SIGNALS:
	void netOver( const QUrl &url, const Request *request, const RequestConnect *request_connect, const QNetworkReply *reply );
};

#endif // REQUETNETWORKTHREAD_H_H_HEAD__FILE__
