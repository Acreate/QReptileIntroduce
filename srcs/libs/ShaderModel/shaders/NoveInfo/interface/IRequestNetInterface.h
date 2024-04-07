#ifndef IREQUESTNETINTERFACE_H_H_HEAD__FILE__
#define IREQUESTNETINTERFACE_H_H_HEAD__FILE__

#pragma once

#include <QGenericPlugin>
#include <QNetworkReply>

#include "../export/NoveInfo_export.h"
class INovelInfo;

/// <summary>
/// 这是一个回调接口，它总是处于被调用的状态
/// </summary>
class NOVEINFO_EXPORT IRequestNetInterface : public QObject {
	Q_OBJECT;
public:
	IRequestNetInterface( QObject *parent = nullptr );
	~IRequestNetInterface( ) override;
public:
	/// <summary>
	/// 一个额外的信息指针数据
	/// </summary>
	/// <returns>指针对象</returns>
	virtual std::shared_ptr< void > getData( ) = 0;
	/// <summary>
	/// INovelInfo 共享指针
	/// </summary>
	using INovelInfoSharedPtr = std::shared_ptr< INovelInfo >;
	/// <summary>
	/// 小说类型的指针列表对象
	/// </summary>
	using NovelPtrList = std::vector< INovelInfoSharedPtr >;
	/// <summary>
	/// 获取顶级 host
	/// </summary>
	/// <returns>基于 host 实现的 QUrl 对象</returns>
	virtual QUrl getUrl( ) = 0;
	/// <summary>
	/// 从请求返回的信息中获取基于类型与链接信息的链接对象
	/// </summary>
	/// <param name="networkReply">完成请求之后的对象引用</param>
	/// <returns>基于小说类型与该类型相对应的网络地址</returns>
	virtual QMap< QString, QUrl > getTypeUrls( const QNetworkReply &&networkReply ) = 0;

	/// <summary>
	/// 从页中返回解析到的小说
	/// </summary>
	/// <param name="networkReply">请求页</param>
	/// <param name="saveNovelInfos">已经存储的小说</param>
	/// <param name="appendDataPtr">附加的数据对象指针</param>
	/// <returns>解析到的小说列表</returns>
	virtual NovelPtrList getTypePageNovels( const QNetworkReply &&networkReply, const NovelPtrList &&saveNovelInfos, void *appendDataPtr ) = 0;

	/// <summary>
	/// 从一个链接当中获取单个小说信息，这个行为不建议在 getTypePageNovels 中调用，而是作为被调用者隐式回调使用
	/// </summary>
	/// <param name="networkReply">小说所在的链接请求返回</param>
	/// <param name="saveNovelInfos">已经存储的小说列表</param>
	/// <param name="networkReplayNovel">当前获取的小说页面内容</param>
	/// <returns>小说信息对象指针</returns>
	virtual INovelInfoSharedPtr getUrlNovelInfo( const QNetworkReply &&networkReply, const NovelPtrList &&saveNovelInfos, const INovelInfoSharedPtr &&networkReplayNovel ) = 0;
	/// <summary>
	/// 基于请求实现后进行下一次请求的判定
	/// 返回有效的链接对象表示继续请求，无效对象则退出请求
	/// </summary>
	/// <param name="networkReply">需要判断的网页地址</param>
	/// <param name="saveNovelInfos">已经保存的小说列表</param>
	/// <param name="lastNovelInfos">调用该成员函数之前已经存储的对象列表，与 saveNovelInfos 不同的是，它仅仅存储一页</param>
	/// <returns>下一页的地址</returns>
	virtual QUrl getNext( const QNetworkReply &&networkReply, const NovelPtrList &&saveNovelInfos, const NovelPtrList &&lastNovelInfos ) = 0;
	/// <summary>
	/// 类型调用结束时候会被调用
	/// </summary>
	/// <param name="saveNovelInfos"></param>
	virtual void novelTypeEnd( const NovelPtrList &&saveNovelInfos ) = 0;
	/// <summary>
	/// 结束该网站请求时被调用
	/// </summary>
	virtual void endHost( const NovelPtrList &&saveNovelInfos ) = 0;
};

#endif // IREQUESTNETINTERFACE_H_H_HEAD__FILE__
