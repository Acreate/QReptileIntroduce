#ifndef NOVELNETJOB_H_H_HEAD__FILE__
#define NOVELNETJOB_H_H_HEAD__FILE__
#pragma once
#include <QObject>

#include "../auto_generate_files/export/Plug_export.h"
#include <nameSpace/interfacePlugsType.h>
#include <nameSpace/httpNetWork.h>
#include <interface/IRequestNetInterface.h>
#include <QNetworkReply>
#include <memory>
#include <unordered_map>
#include <chrono>

#include "nameSpace/cylHtmlTools.h"

class PLUG_EXPORT NovelNetJob : public QObject {
	Q_OBJECT;
public:
	class PLUG_EXPORT NovelTypeNamePair {
		using Type_Name_String_Shared = std::shared_ptr< QString >;
		using Type_QUrl_Shared = std::shared_ptr< QUrl >;
		Type_Name_String_Shared typeName; // 首页名称
		Type_QUrl_Shared url; // 首页
	public:
		NovelTypeNamePair( const Type_Name_String_Shared &type_name, const Type_QUrl_Shared &url )
		: typeName( type_name )
		, url( url ) { }
		NovelTypeNamePair( const QString &type_name, const QUrl &url )
		: typeName( std::make_shared< QString >( type_name ) )
		, url( std::make_shared< QUrl >( url ) ) { }
		NovelTypeNamePair( const QString &type_name, const QString &url )
		: typeName( std::make_shared< QString >( type_name ) )
		, url( std::make_shared< QUrl >( url ) ) { }
		NovelTypeNamePair( ) : typeName( std::make_shared< QString >( ) )
		, url( std::make_shared< QUrl >( ) ) { }
		virtual ~NovelTypeNamePair( ) { }
	public: //- 属性
		void setTypeName( const QString &type_name ) { *typeName = type_name; }
		void setUrl( const QString &url ) { *this->url = url; }
		QString getTypeName( ) const {
			return *typeName;
		}
		QUrl getUrl( ) const {
			return *url;
		}
	public: //- 比较
		friend bool operator==( const NovelTypeNamePair &lhs, const NovelTypeNamePair &rhs ) {
			if( lhs.typeName == rhs.typeName
				&& lhs.url == rhs.url )
				return true;
			if( *lhs.typeName == *rhs.typeName
				&& *lhs.url == *rhs.url )
				return true;
			return false;
		}
		friend bool operator!=( const NovelTypeNamePair &lhs, const NovelTypeNamePair &rhs ) { return !( lhs == rhs ); }
	};
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
	using NovelTypeNamePair_Shared = std::shared_ptr< NovelTypeNamePair >;
	/// <summary>
	/// std::unordered_map< Url_Pair, Request_Pairt_Shared >
	/// </summary>
	using Unordered_Map_Pairt = std::unordered_map< NovelTypeNamePair_Shared, Request_Pairt_Shared >;
	using Unordered_Map_Pairt_Shared = std::shared_ptr< Unordered_Map_Pairt >;
private: // - 装饰指针
	QObject *interfaceObjPtr; // 小说基于 qt 框架的指针
	interfacePlugsType::IRequestNetInterface *interfaceThisPtr; // 小说的原始指针
private: // - 网络
	std::shared_ptr< cylHttpNetWork::NetworkAccessManager > networkAccessManager; // 请求管理对象
	std::shared_ptr< cylHttpNetWork::NetworkRequest > networkRequest; // 请求模型
	Request_Pairt_Shared root; // 首页的请求网络配对
	int32_t typeCount; // 小说类型的数量
private: // - 流
	OStream *oStream; // 输入流-程序输出到该流中，显示信息
private: // 配置
	QStringList getTypeNamelist; // 获取的小说类型列表
private: // 状态
	size_t runStatus; // 记录对象的运行状态
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
	std::unordered_map< QString, interfacePlugsType::Vector_INovelInfoSPtr_Shared > typeNovelsMap;
	std::unordered_map< QString, size_t > typeCountMap;
private:
	/// <summary>
	/// 存储小说到仓库
	/// </summary>
	/// <param name="requestedGetVectorINovelInfoSPtrShared">存储列表</param>
	/// <param name="saveMapNovelInfos">仓库已经保存的列表</param>
	/// <param name="novel">检测的小说</param>
	/// <param name="result_msg">如果返回为 nullptr，则该值保存错误消息</param>
	/// <returns>解析得到的小说</returns>
	interfacePlugsType::INovelInfo_Shared saveToStoreNovels(
		interfacePlugsType::Vector_INovelInfoSPtr_Shared &requestedGetVectorINovelInfoSPtrShared,
		interfacePlugsType::Vector_INovelInfoSPtr_Shared &saveMapNovelInfos,
		interfacePlugsType::INovelInfo_Shared &novel,
		QString *result_msg );
Q_SIGNALS: // - 获取被调用
	/// <summary>
	/// 请求一个根路径-获取被调用
	/// </summary>
	/// <param name="url">链接</param>
	/// <param name="request_connect">请求体</param>
	void requesting_get_root_page_signals( const QUrl &url, cylHttpNetWork::RequestConnect *request_connect );
	/// <summary>
	/// 请求一个 type 类型的页面-首次获取被调用
	/// </summary>
	/// <param name="root_url">根链接</param>
	/// <param name="type_name">小说类型</param>
	/// <param name="url">小说页面链接</param>
	/// <param name="html_string">网页内容</param>
	void requesting_get_type_page_url_signals( const QString &root_url, const QString &type_name, const QUrl &url, cylHtmlTools::HtmlString_Shared html_string );
	/// <summary>
	/// 请求一个 type 类型的页面-会在获取下一页时候被调用
	/// </summary>
	/// <param name="root_url">根链接</param>
	/// <param name="type">小说类型</param>
	/// <param name="old_url">上一个页面</param>
	/// <param name="url">小说页面链接</param>
	/// <param name="old_page_index">上一个页数</param>
	/// <param name="current_page_index">当前页数</param>
	/// <param name="saveMapNovelInfos">已经保存的小说列表</param>
	/// <param name="novel_s_ptr_shared">已经获取的小说</param>
	void requesting_get_next_type_page_url_signals( const QString &root_url, const QString &type, const QUrl &old_url, const QUrl &url, size_t old_page_index, size_t current_page_index, const interfacePlugsType::Vector_INovelInfoSPtr_Shared saveMapNovelInfos, const interfacePlugsType::Vector_INovelInfoSPtr_Shared novel_s_ptr_shared );
	/// <summary>
	/// 请求小说页面
	/// </summary>
	/// <param name="root_url">根链接</param>
	/// <param name="type">小说类型</param>
	/// <param name="type_page_url">小说页面链接</param>
	/// <param name="novelName">小说名称</param>
	/// <param name="url">小说url</param>
	/// <param name="novel_s_ptr_shared">已经获取到的小说列表</param>
	/// <param name="html_txt">获取到的小说文本</param>
	void requesting_get_novel_page_url_signals( const QString &root_url, const QString &type, const QString &type_page_url, const QString &novelName, const QUrl &url, const interfacePlugsType::Vector_INovelInfoSPtr_Shared novel_s_ptr_shared, const QString &html_txt );

	/// <summary>
	/// 页面请求结束
	/// </summary>
	/// <param name="root_url">根链接</param>
	/// <param name="type">类型名称</param>
	/// <param name="url">链接</param>
	/// <param name="current_page_index">结束的当前下标</param>
	/// <param name="novel_s_ptr_shared">获取到的小说列表</param>
	void requested_get_type_page_url_end( const QString &root_url, const QString &type, const QUrl &url, size_t current_page_index, const interfacePlugsType::Vector_INovelInfoSPtr_Shared novel_s_ptr_shared );
	/// <summary>
	/// 小说网站请求结束
	/// </summary>
	/// <param name="url">小说的网站</param>
	void requested_get_web_page_signals_end( const QUrl &url );
private : // 信号处理

	/// <summary>
	/// 请求一个根路径-获取被调用
	/// </summary>
	/// <param name="url">链接</param>
	/// <param name="request_connect">请求体</param>
	void slots_requesting_get_root_page_signals( const QUrl &url, cylHttpNetWork::RequestConnect *request_connect );

	/// <summary>
	/// 请求一个 type 类型的页面-首次获取被调用
	/// </summary>
	/// <param name="root_url">根链接</param>
	/// <param name="type_name">小说类型</param>
	/// <param name="type_url">小说页面链接</param>
	/// <param name="html_string">网页内容</param>
	void slots_requesting_get_type_page_url_signals( const QString &root_url, const QString &type_name, const QUrl &type_url, cylHtmlTools::HtmlString_Shared &html_string );
	/// <summary>
	/// 请求下一页时调用该链接
	/// </summary>
	/// <param name="root_url">网站首页</param>
	/// <param name="type_name">类型名称</param>
	/// <param name="old_url">上一页链接</param>
	/// <param name="url">当前也链接</param>
	/// <param name="old_page_index">上一页下标</param>
	/// <param name="current_page_index">当前页下标</param>
	/// <param name="saveMapNovelInfos">已经保存的小说列表</param>
	/// <param name="novel_s_ptr_shared">小说列表</param>
	void slots_requesting_get_next_type_page_url_signals( const QString &root_url, const QString &type_name, const QUrl &old_url, const QUrl &url, size_t old_page_index, size_t current_page_index, const interfacePlugsType::Vector_INovelInfoSPtr_Shared &saveMapNovelInfos, const interfacePlugsType::Vector_INovelInfoSPtr_Shared &novel_s_ptr_shared );

	/// <summary>
	/// 小说类型结束会调用该函数
	/// </summary>
	/// <param name="root_url">根链接</param>
	/// <param name="type_name">类型名称</param>
	/// <param name="url">类型链接</param>
	/// <param name="current_page_index">当前下标</param>
	/// <param name="novel_s_ptr_shared">小说列表</param>
	void slots_requested_get_type_page_url_end( const QString &root_url, const QString &type_name, const QUrl &url, size_t current_page_index, const interfacePlugsType::Vector_INovelInfoSPtr_Shared &novel_s_ptr_shared );
	/// <summary>
	/// 网站结束会调用该链接
	/// </summary>
	/// <param name="url">网站链接</param>
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
