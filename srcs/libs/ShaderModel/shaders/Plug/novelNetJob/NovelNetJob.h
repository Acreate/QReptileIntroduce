#ifndef NOVELNETJOB_H_H_HEAD__FILE__
#define NOVELNETJOB_H_H_HEAD__FILE__
#pragma once
#include <QObject>

#include "../auto_generate_files/export/Plug_export.h"
#include <nameSpace/interfacePlugsType.h>
#include <interface/IRequestNetInterface.h>
#include <QNetworkReply>
#include <memory>
#include <unordered_map>
#include <chrono>

#include "nameSpace/cylHtmlTools.h"


class QMutex;
class PLUG_EXPORT NovelNetJob : public QObject {
	Q_OBJECT;
public:
	using Duration = std::chrono::system_clock::time_point::duration;
	using TimePoint = std::chrono::time_point< std::chrono::system_clock >;
	using NetworkmanagerConnectFunction = std::function< bool( QNetworkReply *q_network_reply, const QString call_file_path_name, size_t call_line, const QString call_name ) >;
private: // - 装饰指针
	QObject *interfaceObjPtr; // 小说基于 qt 框架的指针
	interfacePlugsType::IRequestNetInterface *interfaceThisPtr; // 小说的原始指针
private: // - 网络
	QNetworkRequest networkRequest; // 请求模型
	QNetworkAccessManager *networkAccessManager; // 网络请求体
	int32_t typeCount; // 小说类型的数量
	size_t sepMs; // 等待查询间隔
	std::shared_ptr< QMutex > netMutex; // 锁
	TimePoint requestTime; // 请求时间
	size_t runStatus; // 记录对象的运行状态
	int requestMaxCount; // 记录最大请求次数
	int requestMaxMilliseconds; // 记录最大的请求时间
private: // - 流
	OStream *oStream; // 输入流-程序输出到该流中，显示信息
private: // 配置
	QStringList getTypeNamelist; // 获取的小说类型列表
	QString outPath; // 输出路径
	QString inPath; // 输入路径

private: //-静态成员
	static QStringList userAgentHeaderList; // 所有的浏览器 UserAgentHeader
public: //- 静态调用
	/// <summary>
	/// 等待 milliseconds 毫秒
	/// </summary>
	/// <param name="milliseconds">等待时间</param>
	/// <returns>等待时间</returns>
	static size_t waiteMilliseconds( const size_t &milliseconds );
	/// <summary>
	/// 获取一个随机的 UserAgentHeader
	/// </summary>
	/// <returns>UserAgentHeader 的字符串</returns>
	static const QString & getRandomUserAgentHeader( ) {
		qint64 qsizetype = userAgentHeaderList.size( );
		qint64 size = abs( qsizetype - 1 );
		if( size > 0 ) {
			size_t index = rand( ) % size + 0.5;
			if( index > qsizetype )
				return userAgentHeaderList.last( );
			return userAgentHeaderList[ index ];
		}
		return userAgentHeaderList[ 0 ];
	}
	/// <summary>
	/// 获取当前时间的时间戳
	/// </summary>
	/// <returns></returns>
	static Duration getCurrentTimeDuration( ) {
		return std::chrono::system_clock::now( ).time_since_epoch( );
	}
	/// <summary>
	/// 获取当前时间戳
	/// </summary>
	/// <returns>返回时间戳</returns>
	static std::chrono::time_point< std::chrono::system_clock > getCurrentTimePoint( ) {
		return std::chrono::system_clock::now( );
	}
	/// <summary>
	/// 时间戳转换到毫秒
	/// </summary>
	/// <param name="time_duration">时间戳</param>
	/// <returns>毫秒</returns>
	static long long getTimeDurationToMilliseconds( const Duration &time_duration ) {
		return std::chrono::duration_cast< std::chrono::milliseconds >( time_duration ).count( );
	}
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
	/// 获取请求间隔
	/// </summary>
	/// <returns>请求间隔-毫秒</returns>
	size_t getSepMs( ) const { return sepMs; }
	/// <summary>
	/// 设置请求间隔-毫秒
	/// </summary>
	/// <param name="sep_ms">间隔</param>
	void setSepMs( const size_t sep_ms ) { sepMs = sep_ms; }

	/// <summary>
	/// 获取 url
	/// </summary>
	/// <returns>url</returns>
	QString getUrl( ) const;

	const QString & getInPath( ) const { return inPath; }
	void setInPath( const QString &in_path ) { inPath = in_path; }

	/// <summary>
	/// 设置写入路径
	/// </summary>
	/// <param name="path">写入路径</param>
	void setPath( const QString &path ) {
		outPath = path;
	}
	/// <summary>
	/// 设置写入路径
	/// </summary>
	/// <param name="type_s">请求类型列表</param>
	void setRequestTypeNames( const std::vector< QString > &type_s ) {
		for( auto &typeName : type_s )
			getTypeNamelist.append( typeName );
	}
	/// <summary>
	/// 设置写入路径
	/// </summary>
	/// <param name="type_s">请求类型列表</param>
	void setRequestTypeNames( const QVector< QString > &type_s ) {
		getTypeNamelist.append( type_s );
	}
	/// <summary>
	/// 获取写入路径
	/// </summary>
	/// <returns></returns>
	QString getPath( ) const {
		return outPath;
	}
public: // - 小说网站的信息数据
	/// <summary>
	/// 类型与小说列表之间的映射
	/// </summary>
	std::unordered_map< QString, interfacePlugsType::Vector_INovelInfoSPtr_Shared > typeNovelsMap;
private: //- 网络
	/// <summary>
	/// 请求一个 url 网络
	/// </summary>
	/// <param name="result">网络对象，如果为 nullptr，则重新创建</param>
	/// <param name="network_request">参考请求体</param>
	/// <param name="url">请求链接</param>
	/// <param name="last_request_time_point">上次一的请求时间</param>
	/// <param name="sep_ms">距离上一次调用允许下次调用的时间间隔</param>
	/// <param name="callFunction">完成请求后调用函数</param>
	/// <param name="call_finle_path_name">调用函数的文件路径名称</param>
	/// <param name="call_function_name">调用函数的函数名称</param>
	/// <param name="call_line">调用函数的行数</param>
	/// <returns>应答体</returns>
	QNetworkReply * requestUrl( QNetworkAccessManager *&result, const QNetworkRequest &network_request, const QUrl &url, TimePoint &last_request_time_point, size_t &sep_ms, const NetworkmanagerConnectFunction &callFunction, const QString &call_finle_path_name, const QString &call_function_name, const size_t call_line );
	/// <summary>
	/// 请求一个 url 网络
	/// </summary>
	/// <param name="result">网络对象，如果为 nullptr，则重新创建</param>
	/// <param name="network_request">参考请求体</param>
	/// <param name="url">请求链接</param>
	/// <param name="last_request_time_point">上次一的请求时间</param>
	/// <param name="sep_ms">距离上一次调用允许下次调用的时间间隔</param>
	/// <returns>应答体</returns>
	QNetworkReply * requestUrl( QNetworkAccessManager *&result, const QNetworkRequest &network_request, const QUrl &url, TimePoint &last_request_time_point, size_t &sep_ms );
	/// <summary>
	/// 发送 get 请求
	/// </summary>
	/// <param name="url">请求的 url</param>
	/// <param name="requestMaxCount">最大请求次数，如果等于 0，则忽略次数</param>
	/// <param name="requestMaxMs">最大请求时间，如果等于 0，则忽略时间</param>
	/// <param name="error_msg">产生错误时候，写入的错误信息</param>
	/// <param name="error_file_append_base_name">产生错误的时候，写入的文件的追加文件名</param>
	/// <param name="call_function">回调函数</param>
	/// <param name="call_finle_path_name">应该填入调用该函数的文件路径( __FILE__ )</param>
	/// <param name="call_function_name">应该填入调用该函数的函数名( __FUNCTION__ )</param>
	/// <param name="call_line">应该填入调用该函数的行数( __LINE__ )</param>
	/// <param name="old_url">上一页 url</param>
	/// <returns>应答体</returns>
	QNetworkReply * requestGet( const QUrl &url, const size_t requestMaxCount, const size_t requestMaxMs, const QString &error_msg, const QString &error_file_append_base_name, const NetworkmanagerConnectFunction &call_function, const QString &call_finle_path_name, const QString &call_function_name, const size_t call_line, const QString &old_url );
	/// <summary>
	/// 发送 get 请求
	/// </summary>
	/// <param name="url">请求的 url</param>
	/// <param name="requestMaxCount">最大请求次数，如果等于 0，则忽略次数</param>
	/// <param name="requestMaxMs">最大请求时间，如果等于 0，则忽略时间</param>
	/// <param name="error_msg">产生错误时候，写入的错误信息</param>
	/// <param name="error_file_append_base_name">产生错误的时候，写入的文件的追加文件名</param>
	/// <param name="call_finle_path_name">应该填入调用该函数的文件路径( __FILE__ )</param>
	/// <param name="call_function_name">应该填入调用该函数的函数名( __FUNCTION__ )</param>
	/// <param name="call_line">应该填入调用该函数的行数( __LINE__ )</param>
	/// <param name="old_url">上次请求网址</param>
	/// <returns>应答体</returns>
	QNetworkReply * requestGet( const QUrl &url, const size_t requestMaxCount, const size_t requestMaxMs, const QString &error_msg, const QString &error_file_append_base_name, const QString &call_finle_path_name, const QString &call_function_name, const size_t call_line, const QString &old_url );
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
	/// <summary>
	/// 请求页面信息，并且返回下一页。
	/// </summary>
	/// <param name="type_name">页面类型名称</param>
	/// <param name="type_url">页面类型 url</param>
	/// <param name="html_string">页面文本</param>
	/// <returns>下一页的链接，失败返回空</returns>
	QString getPageInfo( const QString &type_name, const QUrl &type_url, cylHtmlTools::HtmlString_Shared html_string );
	/// <summary>
	/// 类型页面小说请求终止
	/// </summary>
	/// <param name="type_name">页面类型</param>
	/// <param name="url">终止 url</param>
	/// <param name="current_page_index">终止下标</param>
	void novelPageInfoRequestEnd( const QString &type_name, const QUrl &url, size_t current_page_index );
Q_SIGNALS: // - 获取被调用
	/// <summary>
	/// 请求一个根路径-获取被调用
	/// </summary>
	/// <param name="url">链接</param>
	/// <param name="qnetwork_reply">请求体</param>
	void requesting_get_root_page_signals( const QUrl &url, QNetworkReply *qnetwork_reply );
	/// <summary>
	/// 小说网站请求结束
	/// </summary>
	/// <param name="url">小说的网站</param>
	void requested_get_web_page_signals_end( const QUrl &url );
	/// <summary>
	/// 任务结束
	/// </summary>
	void endJob( );
private slots: // 信号处理

	/// <summary>
	/// 请求一个根路径-获取被调用
	/// </summary>
	/// <param name="url">链接</param>
	/// <param name="qnetwork_reply">请求体</param>
	void slots_requesting_get_root_page_signals( const QUrl &url, QNetworkReply *qnetwork_reply );
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
