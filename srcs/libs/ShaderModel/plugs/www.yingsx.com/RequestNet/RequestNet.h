#ifndef REQUESTNET_H_H_HEAD__FILE__
#define REQUESTNET_H_H_HEAD__FILE__
#pragma once
#include <QGenericPlugin>
#include <QDateTime>
#include "../auto_generate_files/macro/cmake_to_c_cpp_header_macro.h"
#include "interface/IRequestNetInterface.h"
#include "nameSpace/cylHtmlTools.h"
using interfacePlugsType::IRequestNetInterface;
Q_DECLARE_INTERFACE( IRequestNetInterface, IRequestNetInterface_iid );
class RequestNet : public QObject, public IRequestNetInterface {
	Q_OBJECT;
	Q_INTERFACES( IRequestNetInterface )
private:
	static QString timeForm; // 时间格式
	/// <summary>
	/// 从原始(currentTime)格式获取字符串时候的格式化字符串
	/// <code>
	///		// currentTime 当前时间
	///		auto timeArgForm = currentTime.toString( orgCurrentFormToUpdateTimeForm );
	///		// .... *html_string_shared 作为小说页面获取到的小说字符串指针对象
	///		auto newTime = timeArgForm..arg( QString::fromStdWString( *html_string_shared );
	/// </code>
	/// </summary>
	static QString orgCurrentFormToUpdateTimeForm;
	static QDateTime currentTime; // 请求时间
	static QString currentTimeForm; // 当前请求时间的格式
private:
	QUrl rootUrl;
	OStream *thisOStream;
	QString outPath;
	
	int expireDay; // 过期-日
	std::shared_ptr< interfacePlugsType::Map_HtmlStrK_HtmlStrV > typeUrlMap;
public:
	RequestNet( QObject *parent = nullptr );
	~RequestNet( ) override;
public: // 实现虚函数
	
	/// <summary>
	/// 设置最大允许请求时间
	/// </summary>
	/// <param name="expir_day">请求时间</param>
	void setExpireDay( int expir_day ) override {
		expireDay =expir_day;
	}
	/// <summary>
	/// 获取最大允许请求时间
	/// </summary>
	/// <returns>请求时间</returns>
	int getExpireDay( ) override {
		return expireDay;
	}
	/// <summary>
	/// 设置写入路径
	/// </summary>
	/// <param name="path">路径</param>
	void setOutPath( interfacePlugsType::HtmlDocString *path ) override {
		outPath = QString::fromStdWString( *path );
	}
	/// <summary>
	/// 获取写入路径
	/// </summary>
	/// <param name="path">返回的路径</param>
	/// <returns>返回的大小</returns>
	size_t getOutPath( interfacePlugsType::HtmlDocString *path ) override {
		*path = outPath.toStdWString( );
		return path->length( );
	}

	/// <summary>
	/// 获取建议请求间隔
	/// </summary>
	/// <returns>间隔-毫秒</returns>
	size_t getRequestInterval( ) override;
	/// <summary>
	/// 初始化之前，被动调用
	/// 使用对象之前，应该调用该函数一次，并且它必须在 initBefore() 之前
	/// </summary>
	/// <returns>初始化之前</returns>
	int initAfter( ) override;
	/// <summary>
	/// 初始化之后，被动调用
	/// 使用对象之前，应该调用该函数一次，并且它必须在 initAfter() 之后
	/// </summary>
	/// <returns>初始化直走</returns>
	int initBefore( ) override;
	/// <summary>
	/// 获取附加数据
	/// </summary>
	/// <returns>附加数据</returns>
	void getData( void *resultAnyPtr ) override;
	/// <summary>
	/// 获取根链接
	/// </summary>
	/// <param name="outStr">输出的 url</param>
	/// <returns>该对象的 url 长度</returns>
	size_t getRootUrl( interfacePlugsType::HtmlDocString *outStr ) override;
	/// <summary>
	/// 设置根链接
	/// </summary>
	/// <param name="url">网站的首地址</param>
	void setRootUrl( const interfacePlugsType::HtmlDocString &url ) override;
	/// <summary>
	/// 设置域
	/// </summary>
	/// <param name="host">域</param>
	void setHost( const interfacePlugsType::HtmlDocString &host ) override;
	/// <summary>
	/// 获取域
	/// </summary>
	/// <returns>域</returns>
	size_t getHost( interfacePlugsType::HtmlDocString *outHost ) override;
	/// <summary>
	/// 设置协议
	/// </summary>
	/// <param name="scheme">协议</param>
	void setScheme( const interfacePlugsType::HtmlDocString &scheme ) override;
	/// <summary>
	/// 获取协议
	/// </summary>
	/// <returns>协议</returns>
	size_t getScheme( interfacePlugsType::HtmlDocString *outScheme ) override;
	/// <summary>
	/// 设置父类
	/// </summary>
	/// <param name="parent">父节点</param>
	/// <returns>是否成功</returns>
	bool setInterfaceParent( void *parent ) override;
	/// <summary>
	/// 删除成员
	/// </summary>
	void deleteMember( ) override;
public: // 实现解析
	/// <summary>
	/// 获取小说当中的类型与网址映射列表
	/// </summary>
	/// <param name="url">解析页的 url</param>
	/// <param name="htmlText">解析页面</param>
	/// <returns>类型与目标地址</returns>
	interfacePlugsType::Map_HtmlStrK_HtmlStrV * formHtmlGetTypeTheUrls( const interfacePlugsType::HtmlDocString &url, const interfacePlugsType::HtmlDocString &htmlText ) override;
	/// <summary>
	/// 从页中返回解析到的小说
	/// </summary>
	/// <param name="type_name">页类型名称</param>
	/// <param name="request_url">请求页的url</param>
	/// <param name="htmlText">请求页</param>
	/// <param name="saveNovelInfos">已经存储的小说</param>
	/// <param name="appendDataPtr">附加的数据对象指针</param>
	/// <returns>解析到的小说列表</returns>
	interfacePlugsType::Vector_INovelInfoSPtr formHtmlGetTypePageNovels( const interfacePlugsType::HtmlDocString &type_name, const interfacePlugsType::HtmlDocString &request_url, const interfacePlugsType::HtmlDocString &htmlText, const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos, void *appendDataPtr ) override;
	/// <summary>
	/// 从一个链接当中获取单个小说信息，这个行为不建议在 formHtmlGetTypePageNovels 中调用，而是作为被调用者隐式回调使用
	/// </summary>
	/// <param name="request_url">获取页的 url</param>
	/// <param name="htmlText">小说所在的链接请求返回</param>
	/// <param name="saveNovelInfos">已经存储的小说列表</param>
	/// <param name="networkReplayNovel">当前获取的小说页面内容</param>
	/// <returns>小说信息对象指针</returns>
	interfacePlugsType::INovelInfo_Shared formHtmlGetUrlNovelInfo( const interfacePlugsType::HtmlDocString &request_url, const interfacePlugsType::HtmlDocString &htmlText, const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos, const interfacePlugsType::INovelInfo_Shared &networkReplayNovel ) override;
	/// <summary>
	/// 基于请求实现后进行下一次请求的判定
	/// 返回有效的链接对象表示继续请求，无效对象则退出请求
	/// </summary>
	/// <param name="type_name">页类型名称</param>
	/// <param name="request_url">解析下一页的 url</param>
	/// <param name="htmlText">需要判断的网页内容</param>
	/// <param name="saveNovelInfos">已经保存的小说列表</param>
	/// <param name="lastNovelInfos">调用该成员函数之前已经存储的对象列表，与 saveNovelInfos 不同的是，它仅仅存储一页</param>
	/// <returns>下一页的地址</returns>
	interfacePlugsType::HtmlDocString formHtmlGetNext( const interfacePlugsType::HtmlDocString &type_name, const interfacePlugsType::HtmlDocString &request_url, const interfacePlugsType::HtmlDocString &htmlText, const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos, const interfacePlugsType::Vector_INovelInfoSPtr &lastNovelInfos ) override;
	// 实现判定
	/// <summary>
	/// 是否请求小说详情页面
	/// </summary>
	/// <param name="novel_info_ptr">查询的小说</param>
	/// <returns>如果需要获取，则返回 true，否则返回 true</returns>
	bool isRequestNovelInfoUrl( const interfacePlugsType::INovelInfoPtr &novel_info_ptr ) override;
	/// <summary>
	/// 类型调用结束时候会被调用
	/// </summary>
	/// <param name="root_url">根路径</param>
	/// <param name="type_name">请求类型名称</param>
	/// <param name="url">类型页面</param>
	/// <param name="saveNovelInfos"></param>
	void novelTypeEnd( const interfacePlugsType::HtmlDocString &root_url, const interfacePlugsType::HtmlDocString &type_name, const interfacePlugsType::HtmlDocString &url, const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos ) override;
	/// <summary>
	/// 结束该网站请求时被调用
	/// </summary>
	/// <param name="saveNovelInfos">请求到的所有小说信息</param>
	void endHost( const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos ) override;
private:
	OStream *oStream;
	IStream *iStream;
public: // 实现流相关
	OStream * setOStream( OStream *o_stream ) override;
	IStream * setIStream( IStream *i_stream ) override;
public slots:
	IRequestNetInterface * getRequestNetInterfacePatr( ) {
		return this;
	}

};

#endif // REQUESTNET_H_H_HEAD__FILE__
