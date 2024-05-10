#ifndef NOVELNETJOB_H_H_HEAD__FILE__
#define NOVELNETJOB_H_H_HEAD__FILE__
#pragma once
#include <QObject>

#include "../export/Plug_export.h"
#include <nameSpace/interfacePlugsType.h>
#include <nameSpace/httpNetWork.h>
#include <interface/IRequestNetInterface.h>
#include <QNetworkReply>

class PLUG_EXPORT NovelNetJob : public QObject {

	Q_OBJECT;
public:
	/// <summary>
	/// std::shared_ptr< cylHttpNetWork::RequestConnect >
	/// </summary>
	using RequestConnect_Shared = std::shared_ptr< cylHttpNetWork::RequestConnect >;
	/// <summary>
	/// std::shared_ptr< cylHttpNetWork::Request >
	/// </summary>
	using Request_Shared = std::shared_ptr< cylHttpNetWork::Request >;
	/// <summary>
	///  std::pair< std::shared_ptr< cylHttpNetWork::RequestConnect >, std::shared_ptr< cylHttpNetWork::Request > >
	/// </summary>
	using Request_Pairt = std::pair< RequestConnect_Shared, Request_Shared >;
	/// <summary>
	/// std::shared_ptr< std::pair< std::shared_ptr< cylHttpNetWork::RequestConnect > ,std::shared_ptr< cylHttpNetWork::Request > 
	///	>
	/// </summary>
	using Request_Pairt_Shared = std::shared_ptr< Request_Pairt >;
	/// <summary>
	///  std::pair<std::shared_ptr<QString>, std::shared_ptr<QUrl>>
	/// </summary>
	using Url_Pair = std::pair< std::shared_ptr< QString >, std::shared_ptr< QUrl > >;
	/// <summary>
	/// std::unordered_map< Url_Pair, Request_Pairt_Shared >
	/// </summary>
	using Unordered_Map_Pairt = std::unordered_map< Url_Pair, Request_Pairt_Shared >;
private: // - 装饰指针
	QObject *interfaceObjPtr; // 小说基于 qt 框架的指针
	interfacePlugsType::IRequestNetInterface *interfaceThisPtr; // 小说的原始指针
private: // - 网络
	std::shared_ptr< cylHttpNetWork::NetworkAccessManager > networkAccessManager; // 请求管理对象
	std::shared_ptr< cylHttpNetWork::NetworkRequest > networkRequest; // 请求模型
	Request_Pairt_Shared root; // 首页的请求网络配对
	std::shared_ptr< Unordered_Map_Pairt > typeRequestMap; // 小数类型请求网络配对列表
private: // - 流
	OStream *oStream; // 输入流-程序输出到该流中，显示信息
public:
	NovelNetJob( OStream *o_stream, QObject *interface_obj_ptr, interfacePlugsType::IRequestNetInterface *interface_this_ptr );
	~NovelNetJob( ) override;
private: // - 初始化
	void initObj( ); // 初始化对象
	void initObjProperty( );// 初始化对象属性
	void initConnect( ); // 初始信号
public: // 类的独有属性
	/// <summary>
	/// 爬虫开始工作
	/// </summary>
	/// <returns>实现链接信号操的对象指针</returns>
	bool start( );
	/// <summary>
	/// 获取 url
	/// </summary>
	/// <returns>url</returns>
	QString getUrl( ) const;
public: // - 小说网站的信息数据
	/// <summary>
	/// 类型与小说列表之间的映射
	/// </summary>
	std::unordered_map< QString, std::shared_ptr< interfacePlugsType::Vector_NovelSPtr > > typeNovelsMap;
private slots:
	void root_get_over( cylHttpNetWork::RequestConnect *request );
Q_SIGNALS: // - 获取被调用
	/// <summary>
	/// 请求一个根路径-获取被调用
	/// </summary>
	/// <param name="url">根路径</param>
	void requesting_get_root_page_signals( const QUrl &url );
	/// <summary>
	/// 请求一个 type 类型的页面-首次获取被调用
	/// </summary>
	/// <param name="root_url">根链接</param>
	/// <param name="type">小说类型</param>
	/// <param name="url">小说页面链接</param>
	void requesting_get_type_page_url_signals( const QString &root_url, const QString &type, const QUrl &url );
	/// <summary>
	/// 请求一个 type 类型的页面-会在获取下一页时候被调用
	/// </summary>
	/// <param name="root_url">根链接</param>
	/// <param name="type">小说类型</param>
	/// <param name="old_url">上一个页面</param>
	/// <param name="url">小说页面链接</param>
	/// <param name="old_page_index">上一个页数</param>
	/// <param name="current_page_index">当前页数</param>
	/// <param name="novel_s_ptr_shared">已经获取的小说</param>
	void requesting_get_next_type_page_url_signals( const QString &root_url, const QString &type, const QUrl &old_url, const QUrl &url, size_t old_page_index, size_t current_page_index, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared );
	/// <summary>
	/// 请求小说页面
	/// </summary>
	/// <param name="root_url">根链接</param>
	/// <param name="type">小说类型</param>
	/// <param name="type_page_url">小说页面链接</param>
	/// <param name="novelName">小说名称</param>
	/// <param name="url">小说url</param>
	/// <param name="novel_s_ptr_shared">已经获取到的小说列表</param>
	void requesting_get_novel_page_url_signals( const QString &root_url, const QString &type, const QString &type_page_url, const QString &novelName, const QUrl &url, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared );

	/// <summary>
	/// 请求小说页面-完成
	/// </summary>
	/// <param name="root_url">根链接</param>
	/// <param name="type">小说类型</param>
	/// <param name="type_page_url">小说页面链接</param>
	/// <param name="novelName">小说名称</param>
	/// <param name="url">小说url</param>
	/// <param name="novel_s_ptr_shared">已经获取到的小说列表</param>
	/// <param name="novel_info_shared">获取到的小说</param>
	void requested_get_novel_page_url_signals( const QString &root_url, const QString &type, const QString &type_page_url, const QString &novelName, const QUrl &url, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared, const interfacePlugsType::INovelInfo_Shared novel_info_shared );
	/// <summary>
	/// 页面请求结束
	/// </summary>
	/// <param name="root_url">根链接</param>
	/// <param name="type">类型名称</param>
	/// <param name="url">链接</param>
	/// <param name="current_page_index">结束的当前下标</param>
	/// <param name="novel_s_ptr_shared">获取到的小说列表</param>
	void requested_get_type_page_url_end( const QString &root_url, const QString &type, const QUrl &url, size_t current_page_index, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared );
	/// <summary>
	/// 小说网站请求结束
	/// </summary>
	/// <param name="url">小说的网站</param>
	void requested_get_web_page_signals_end( const QUrl &url );
private slots: // 信号处理
	void slots_requesting_get_root_page_signals( const QUrl &url );
	void slots_requesting_get_type_page_url_signals( const QString &root_url, const QString &type, const QUrl &url );
	void slots_requesting_get_next_type_page_url_signals( const QString &root_url, const QString &type, const QUrl &old_url, const QUrl &url, size_t old_page_index, size_t current_page_index, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared );
	void slots_requesting_get_novel_page_url_signals( const QString &root_url, const QString &type, const QString &type_page_url, const QString &novelName, const QUrl &url, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared );
	void slots_requested_get_novel_page_url_signals( const QString &root_url, const QString &type, const QString &type_page_url, const QString &novelName, const QUrl &url, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared, const interfacePlugsType::INovelInfo_Shared novel_info_shared );
	void slots_requested_get_type_page_url_end( const QString &root_url, const QString &type, const QUrl &url, size_t current_page_index, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared );
	void slots_requested_get_web_page_signals_end( const QUrl &url );
};


inline QString getErrorQStr( QNetworkReply::NetworkError error_code ) {
	switch( error_code ) {
		case QNetworkReply::AuthenticationRequiredError :
			return QObject::tr( u8"the proxy requires authentication in order to honour the request but did not accept any credentials offered (if any)" );
		case QNetworkReply::BackgroundRequestNotAllowedError :
			return QObject::tr( u8"the background request is not currently allowed due to platform policy." );
		case QNetworkReply::ConnectionRefusedError :
			return QObject::tr( u8"the connection to the proxy server was refused (the proxy server is not accepting requests)" );
		case QNetworkReply::ContentAccessDenied :
			return QObject::tr( u8"the access to the remote content was denied (similar to HTTP error 403)" );
		case QNetworkReply::ContentConflictError :
			return QObject::tr( u8"the request could not be completed due to a conflict with the current state of the resource." );
		case QNetworkReply::ContentGoneError :
			return QObject::tr( u8"the requested resource is no longer available at the server." );
		case QNetworkReply::ContentNotFoundError :
			return QObject::tr( u8"the remote content was not found at the server (similar to HTTP error 404)" );
		case QNetworkReply::ContentOperationNotPermittedError :
			return QObject::tr( u8"the operation requested on the remote content is not permitted" );
		case QNetworkReply::ContentReSendError :
			return QObject::tr( u8"the request needed to be sent again, but this failed for example because the upload data could not be read a second time." );
		case QNetworkReply::HostNotFoundError :
			return QObject::tr( u8"the remote host name was not found (invalid hostname)" );
		case QNetworkReply::InsecureRedirectError :
			return QObject::tr( u8"while following redirects, the network access API detected a redirect from a encrypted protocol (https) to an unencrypted one (http). (This value was introduced in 5.6.)" );
		case QNetworkReply::InternalServerError :
			return QObject::tr( u8"the server encountered an unexpected condition which prevented it from fulfilling the request." );
		case QNetworkReply::NetworkSessionFailedError :
			return QObject::tr( u8"the connection was broken due to disconnection from the network or failure to start the network." );
		case QNetworkReply::OperationCanceledError :
			return QObject::tr( u8"the operation was canceled via calls to abort() or close() before it was finished." );
		case QNetworkReply::OperationNotImplementedError :
			return QObject::tr( u8"the server does not support the functionality required to fulfill the request." );
		case QNetworkReply::ProtocolFailure :
			return QObject::tr( u8"a breakdown in protocol was detected (parsing error, invalid or unexpected responses, etc.)" );
		case QNetworkReply::ProtocolInvalidOperationError :
			return QObject::tr( u8"the requested operation is invalid for this protocol" );
		case QNetworkReply::ProtocolUnknownError :
			return QObject::tr( u8"the Network Access API cannot honor the request because the protocol is not known" );
		case QNetworkReply::ProxyAuthenticationRequiredError :
			return QObject::tr( u8"the proxy requires authentication in order to honour the request but did not accept any credentials offered (if any)" );
		case QNetworkReply::ProxyConnectionClosedError :
			return QObject::tr( u8"the proxy server closed the connection prematurely, before the entire reply was received and processed" );
		case QNetworkReply::ProxyConnectionRefusedError :
			return QObject::tr( u8"the connection to the proxy server was refused (the proxy server is not accepting requests)" );
		case QNetworkReply::ProxyNotFoundError :
			return QObject::tr( u8"the proxy host name was not found (invalid proxy hostname)" );
		case QNetworkReply::ProxyTimeoutError :
			return QObject::tr( u8"the connection to the proxy timed out or the proxy did not reply in time to the request sent" );
		case QNetworkReply::RemoteHostClosedError :
			return QObject::tr( u8"the remote server closed the connection prematurely, before the entire reply was received and processed" );
		case QNetworkReply::ServiceUnavailableError :
			return QObject::tr( u8"the server is unable to handle the request at this time." );
		case QNetworkReply::SslHandshakeFailedError :
			return QObject::tr( u8"the SSL/TLS handshake failed and the encrypted channel could not be established. The sslErrors() signal should have been emitted." );
		case QNetworkReply::TemporaryNetworkFailureError :
			return QObject::tr( u8"the connection was broken due to disconnection from the network, however the system has initiated roaming to another access point. The request should be resubmitted and will be processed as soon as the connection is re-established." );
		case QNetworkReply::TimeoutError :
			return QObject::tr( u8"the connection to the proxy timed out or the proxy did not reply in time to the request sent" );
		case QNetworkReply::TooManyRedirectsError :
			return QObject::tr( u8"while following redirects, the maximum limit was reached. The limit is by default set to 50 or as set by QNetworkRequest::setMaxRedirectsAllowed(). (This value was introduced in 5.6.)" );
		case QNetworkReply::UnknownContentError :
			return QObject::tr( u8"an unknown error related to the remote content was detected" );
		case QNetworkReply::UnknownNetworkError :
			return QObject::tr( u8"an unknown network-related error was detected" );
		case QNetworkReply::UnknownProxyError :
			return QObject::tr( u8"an unknown proxy-related error was detected" );
		case QNetworkReply::UnknownServerError :
			return QObject::tr( u8"an unknown error related to the server response was detected" );
		case QNetworkReply::NoError :
			return QObject::tr( u8"no error condition" );
		default :
			return QObject::tr( u8"none error" );
	}
}
#endif // NOVELNETJOB_H_H_HEAD__FILE__
