#ifndef IREQUESTNETINTERFACE_H_H_HEAD__FILE__
#define IREQUESTNETINTERFACE_H_H_HEAD__FILE__
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <chrono>

#include "../nameSpace/interfacePlugsType.h"


class IStream;
class OStream;
namespace interfacePlugsType {
	/// <summary>
	/// 这是一个回调接口，它总是处于被调用的状态
	/// </summary>
	class NOVELINFO_EXPORT IRequestNetInterface {
	public:
		virtual ~IRequestNetInterface( ) {
		}
	public:
		/// <summary>
		/// 设置过期时间
		/// </summary>
		/// <param name="expir_day">过期时间</param>
		virtual void setExpireDay( int expir_day ) = 0;
		/// <summary>
		/// 获取请求过期时间
		/// </summary>
		/// <returns>过期时间</returns>
		virtual int getExpireDay( ) = 0;
		/// <summary>
		/// 设置写入路径
		/// </summary>
		/// <param name="path">路径</param>
		virtual void setOutPath( HtmlDocString *path ) = 0;
		/// <summary>
		/// 获取写入路径
		/// </summary>
		/// <param name="path">返回的路径</param>
		/// <returns>返回的大小</returns>
		virtual size_t getOutPath( HtmlDocString *path ) = 0;
		/// <summary>
		/// 获取建议请求间隔
		/// </summary>
		/// <returns>间隔-毫秒</returns>
		virtual size_t getRequestInterval( ) = 0;
		/// <summary>
		/// 初始化之前，被动调用
		/// 使用对象之前，应该调用该函数一次，并且它必须在 initBefore() 之前
		/// </summary>
		/// <returns>初始化之前</returns>
		virtual int initAfter( ) = 0;
		/// <summary>
		/// 初始化之后，被动调用
		/// 使用对象之前，应该调用该函数一次，并且它必须在 initAfter() 之后
		/// </summary>
		/// <returns>初始化直走</returns>
		virtual int initBefore( ) = 0;
		/// <summary>
		/// 获取附加数据
		/// </summary>
		/// <returns>附加数据</returns>
		virtual void getData( void *resultAnyPtr ) = 0;

		/// <summary>
		/// 获取 url
		/// </summary>
		/// <param name="outStr">输出的 url</param>
		/// <returns>该对象的 url 长度</returns>
		virtual size_t getRootUrl( HtmlDocString *outStr ) = 0;
		/// <summary>
		/// 设置网站地址
		/// </summary>
		/// <param name="url">网站的首地址</param>
		virtual void setRootUrl( const HtmlDocString &url ) = 0;

		/// <summary>
		/// 设置域
		/// </summary>
		/// <param name="host">域</param>
		virtual void setHost( const HtmlDocString &host ) = 0;
		/// <summary>
		/// 获取域
		/// </summary>
		/// <returns>域</returns>
		virtual size_t getHost( HtmlDocString *outHost ) = 0;
		/// <summary>
		/// 设置协议
		/// </summary>
		/// <param name="scheme">协议</param>
		virtual void setScheme( const HtmlDocString &scheme ) = 0;
		/// <summary>
		/// 获取协议
		/// </summary>
		/// <returns>协议</returns>
		virtual size_t getScheme( HtmlDocString *outScheme ) = 0;
		/// <summary>
		/// 获取小说当中的类型与网址映射列表
		/// </summary>
		/// <param name="request_url">解析页的 url</param>
		/// <param name="htmlText">解析页面</param>
		/// <returns>类型与目标地址</returns>
		virtual Map_HtmlStrK_HtmlStrV * formHtmlGetTypeTheUrls( const HtmlDocString &request_url, const HtmlDocString &htmlText ) = 0;

		/// <summary>
		/// 从页中返回解析到的小说
		/// </summary>
		/// <param name="type_name">页类型名称</param>
		/// <param name="request_url">请求页的 url</param>
		/// <param name="htmlText">请求页</param>
		/// <param name="saveNovelInfos">已经存储的小说</param>
		/// <param name="appendDataPtr">附加的数据对象指针</param>
		/// <returns>解析到的小说列表</returns>
		virtual Vector_INovelInfoSPtr formHtmlGetTypePageNovels( const HtmlDocString &type_name, const HtmlDocString &request_url, const HtmlDocString &htmlText, const Vector_INovelInfoSPtr &saveNovelInfos, void *appendDataPtr ) = 0;

		/// <summary>
		/// 从一个链接当中获取单个小说信息，这个行为不建议在 formHtmlGetTypePageNovels 中调用，而是作为被调用者隐式回调使用
		/// </summary>
		/// <param name="request_url">获取页的 url</param>
		/// <param name="htmlText">小说所在的链接请求返回</param>
		/// <param name="saveNovelInfos">已经存储的小说列表</param>
		/// <param name="networkReplayNovel">当前获取的小说页面内容</param>
		/// <returns>小说信息对象指针</returns>
		virtual INovelInfo_Shared formHtmlGetUrlNovelInfo( const HtmlDocString &request_url, const HtmlDocString &htmlText, const Vector_INovelInfoSPtr &saveNovelInfos, const INovelInfo_Shared &networkReplayNovel ) = 0;
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
		virtual HtmlDocString formHtmlGetNext( const HtmlDocString &type_name, const HtmlDocString &request_url, const HtmlDocString &htmlText, const Vector_INovelInfoSPtr &saveNovelInfos, const Vector_INovelInfoSPtr &lastNovelInfos ) = 0;
		/// <summary>
		/// 是否请求小说详情页面
		/// </summary>
		/// <param name="novel_info_ptr">查询的小说</param>
		/// <returns>如果需要获取，则返回 true，否则返回 true</returns>
		virtual bool isRequestNovelInfoUrl( const INovelInfoPtr &novel_info_ptr ) = 0;
		/// <summary>
		/// 类型调用结束时候会被调用
		/// </summary>
		/// <param name="root_url">根路径</param>
		/// <param name="type_name">请求类型名称</param>
		/// <param name="url">类型页面</param>
		/// <param name="saveNovelInfos"></param>
		virtual void novelTypeEnd( const HtmlDocString &root_url, const HtmlDocString &type_name, const HtmlDocString &url, const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos ) = 0;
		/// <summary>
		/// 结束该网站请求时被调用
		/// </summary>
		/// <param name="saveNovelInfos">请求到的所有小说信息</param>
		virtual void endHost( const Vector_INovelInfoSPtr &saveNovelInfos ) = 0;
		/// <summary>
		/// 设置父类
		/// </summary>
		/// <param name="parent">父节点</param>
		/// <returns>是否成功</returns>
		virtual bool setInterfaceParent( void *parent ) = 0;
		/// <summary>
		/// 删除成员
		/// </summary>
		virtual void deleteMember( ) = 0;

		/// <summary>
		/// 设置输出流
		/// </summary>
		/// <param name="o_stream">输出流对象</param>
		/// <returns>旧的流</returns>
		virtual OStream * setOStream( OStream *o_stream ) = 0;

		/// <summary>
		/// 设置输入流
		/// </summary>
		/// <param name="i_stream">输入流对象</param>
		/// <returns>旧的流</returns>
		virtual IStream * setIStream( IStream *i_stream ) = 0;
	};

}

#endif // IREQUESTNETINTERFACE_H_H_HEAD__FILE__
