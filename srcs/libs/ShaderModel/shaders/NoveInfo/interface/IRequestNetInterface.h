﻿#ifndef IREQUESTNETINTERFACE_H_H_HEAD__FILE__
#define IREQUESTNETINTERFACE_H_H_HEAD__FILE__
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../export/NoveInfo_export.h"
class INovelInfo;

/// <summary>
/// 这是一个回调接口，它总是处于被调用的状态
/// </summary>
class NOVEINFO_EXPORT IRequestNetInterface {
public:
	virtual ~IRequestNetInterface( ) {
	}
public:
	using INovelInfoSharedPtr = std::shared_ptr< INovelInfo >;
	using NovelPtrList = std::vector< INovelInfoSharedPtr >;
	using StdString = char *;
	using StdStringPtr = StdString *;
	using un_ordered_map = std::unordered_map< StdString, StdString >;
	/// <summary>
	/// 获取附加数据
	/// </summary>
	/// <returns>附加数据</returns>
	virtual void *getData( ) = 0;

	/// <summary>
	/// 获取 url
	/// </summary>
	/// <param name="outStr">输出的 url</param>
	/// <returns>该对象的 url 长度</returns>
	virtual size_t getUrl( std::string *outStr ) = 0;
	/// <summary>
	/// 设置网站地址
	/// </summary>
	/// <param name="url">网站的首地址</param>
	virtual void setUrl( const StdString &url ) = 0;
	/// <summary>
	/// 获取小说当中的类型与网址映射列表
	/// </summary>
	/// <param name="htmlText">解析页面</param>
	/// <returns>类型与目标地址</returns>
	virtual un_ordered_map *getTypeUrls( const StdString &htmlText ) = 0;

	/// <summary>
	/// 从页中返回解析到的小说
	/// </summary>
	/// <param name="htmlText">请求页</param>
	/// <param name="saveNovelInfos">已经存储的小说</param>
	/// <param name="appendDataPtr">附加的数据对象指针</param>
	/// <returns>解析到的小说列表</returns>
	virtual NovelPtrList getTypePageNovels( const StdString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) = 0;

	/// <summary>
	/// 从一个链接当中获取单个小说信息，这个行为不建议在 getTypePageNovels 中调用，而是作为被调用者隐式回调使用
	/// </summary>
	/// <param name="htmlText">小说所在的链接请求返回</param>
	/// <param name="saveNovelInfos">已经存储的小说列表</param>
	/// <param name="networkReplayNovel">当前获取的小说页面内容</param>
	/// <returns>小说信息对象指针</returns>
	virtual INovelInfoSharedPtr getUrlNovelInfo( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) = 0;
	/// <summary>
	/// 基于请求实现后进行下一次请求的判定
	/// 返回有效的链接对象表示继续请求，无效对象则退出请求
	/// </summary>
	/// <param name="htmlText">需要判断的网页内容</param>
	/// <param name="saveNovelInfos">已经保存的小说列表</param>
	/// <param name="lastNovelInfos">调用该成员函数之前已经存储的对象列表，与 saveNovelInfos 不同的是，它仅仅存储一页</param>
	/// <returns>下一页的地址</returns>
	virtual StdString getNext( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) = 0;
	/// <summary>
	/// 类型调用结束时候会被调用
	/// </summary>
	/// <param name="saveNovelInfos"></param>
	virtual void novelTypeEnd( const NovelPtrList &saveNovelInfos ) = 0;
	/// <summary>
	/// 结束该网站请求时被调用
	/// </summary>
	virtual void endHost( const NovelPtrList &saveNovelInfos ) = 0;
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
};

#endif // IREQUESTNETINTERFACE_H_H_HEAD__FILE__