#ifndef INOVELINFO_H_H_HEAD__FILE__
#define INOVELINFO_H_H_HEAD__FILE__
#pragma once
#include <QString>
#include <qdatetime.h>

#include "../export/NoveInfo_export.h"
class NOVEINFO_EXPORT INovelInfo : public QObject {
	Q_OBJECT;
public:
	INovelInfo( QObject *parent = nullptr );
	~INovelInfo( ) override;
public:
	/// <summary>
	/// 获取小说名称
	/// </summary>
	/// <returns>小说名称</returns>
	virtual QString getName( ) = 0;
	/// <summary>
	/// 获取小说所在链接
	/// </summary>
	/// <returns>链接</returns>
	virtual QUrl getNovelUrl( ) = 0;
	/// <summary>
	/// 获取当时获取小说信息的所在页
	/// </summary>
	/// <returns>所在页的链接</returns>
	virtual QUrl getPageUrl( ) = 0;
	/// <summary>
	/// 获取小说所在类型的链接
	/// </summary>
	/// <returns>小说类型的链接</returns>
	virtual QUrl getTypeUrl( ) = 0;
	/// <summary>
	/// 获取小说类型名称
	/// </summary>
	/// <returns>类型名称</returns>
	virtual QString getTypeName( ) = 0;
	/// <summary>
	/// 获取小说站点
	/// </summary>
	/// <returns>站点 host</returns>
	virtual QUrl getHost( ) = 0;
	/// <summary>
	/// 获取小说的介绍
	/// </summary>
	/// <returns>介绍内容</returns>
	virtual QString getInfo( ) = 0;
	/// <summary>
	/// 获取小说作者
	/// </summary>
	/// <returns>作者的名称</returns>
	virtual QString getAuthor( ) = 0;
	/// <summary>
	/// 获取小说最后更新项
	/// </summary>
	/// <returns>最后更新的章节名称</returns>
	virtual QString getNovelLastUpdateItem( ) = 0;
	/// <summary>
	/// 获取小说最后更新章节的链接
	/// </summary>
	/// <returns>最后章节的链接地址</returns>
	virtual QUrl getNovelLastUpdatItemUrl( ) = 0;
	/// <summary>
	/// 获取小说所在站点最后更新的时间
	/// </summary>
	/// <returns>小说更新时间</returns>
	virtual QDateTime getNovelUpdateTime( ) = 0;
	/// <summary>
	/// 获取该进程获取站点时解析小说的时间点
	/// </summary>
	/// <returns>小说解析时间点</returns>
	virtual QDateTime getNovelRequestTime( ) = 0;
};

#endif // INOVELINFO_H_H_HEAD__FILE__
