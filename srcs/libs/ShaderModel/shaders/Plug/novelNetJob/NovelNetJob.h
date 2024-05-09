#ifndef NOVELNETJOB_H_H_HEAD__FILE__
#define NOVELNETJOB_H_H_HEAD__FILE__
#pragma once
#include <QObject>

#include "../export/Plug_export.h"
#include <nameSpace/interfacePlugsType.h>
#include <interface/IRequestNetInterface.h>
#include <QNetworkReply>
class NetworkRequest;
class NetworkAccessManager;
class OStream;
class IStream;
class QNetworkReply;
class RequestConnect;
class PLUG_EXPORT NovelNetJob : public QObject {

	Q_OBJECT;
private:
	QObject *interfaceObjPtr; // 小说基于 qt 框架的指针
	interfacePlugsType::IRequestNetInterface *interfaceThisPtr; // 小说的原始指针
	OStream *oStream; // 输入流-程序输出到该流中，显示信息
	std::shared_ptr< NetworkAccessManager > networkAccessManagerSPtr; // html 请求网络管理
	std::shared_ptr< NetworkRequest > networkRequestSPtr; // html 的请求模型
public:
	NovelNetJob( QObject *parent, OStream *o_stream, QObject *interface_obj_ptr, interfacePlugsType::IRequestNetInterface *interface_this_ptr );
	~NovelNetJob( ) override;
public: // 类的独有属性
	/// <summary>
	/// 爬虫开始工作
	/// </summary>
	/// <returns>实现链接信号操的对象指针</returns>
	void start( );
	/// <summary>
	/// 获取 url
	/// </summary>
	/// <returns>url</returns>
	QString getUrl( ) const;
public: // - 小说网站的信息数据
	std::unordered_map< QString, std::shared_ptr< interfacePlugsType::Vector_NovelSPtr > > typeMap;
	std::unordered_map< RequestConnect*,  QNetworkReply::NetworkError > errorUrlMap;
private: //  - 类型请求
	/// <summary>
	/// 请求详情页面请求
	/// </summary>
	/// <param name="saveNovelInfos">对象所在列表</param>
	/// <param name="networkReplayNovel">小说对象指针</param>
	interfacePlugsType::INovelInfo_Shared get_request_novel_url( const interfacePlugsType::Vector_NovelSPtr &saveNovelInfos, const interfacePlugsType::INovelInfo_Shared &networkReplayNovel );
	/// <summary>
	/// 处理网页请求完成后的类型页面解析<br/>
	/// 主要用户返回页面当中的所有小说链接、查询是否需要请求小说的详情页面并且获取下页面的链接
	/// </summary>
	/// <param name="type">类型名称</param>
	/// <param name="url">请求的 url</param>
	/// <param name="request_connect">请求的完成体</param>
	void type_get( const QString &type, const QUrl &url, RequestConnect *request_connect );
	/// <summary>
	/// 处理网页请求完成后的首页页面解析
	/// 主要用户返回页面当中的所有小说类型链接
	/// </summary>
	/// <param name="request_connect">请求的完成体</param>
	void root_get( RequestConnect *request_connect );
	/// <summary>
	/// 响应 void get( const QString &type, const QString &url );
	/// </summary>
	/// <param name="type">类型页面</param>
	/// <param name="url">类型网络地址</param>
	void signal_get_type_page_url( const QString &type, const QString &url );
	/// <summary>
	/// 响应 void get( const QString &type, const QUrl &url );
	/// </summary>
	/// <param name="type">类型页面</param>
	/// <param name="url">类型网络地址</param>
	void signal_get_type_page_url( const QString &type, const QUrl &url );


Q_SIGNALS:
	/// <summary>
	/// 请求一个 type 类型的页面
	/// </summary>
	void get_type_page_url( const QString &type, const QString &url );
	/// <summary>
	/// 请求一个 type 类型的页面
	/// </summary>
	/// <param name="type"></param>
	/// <param name="url"></param>
	void get_type_page_url( const QString &type, const QUrl &url );
};


#endif // NOVELNETJOB_H_H_HEAD__FILE__
