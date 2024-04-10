#ifndef IREQUESTNETINTERFACEEXTEND_H_H_HEAD__FILE__
#define IREQUESTNETINTERFACEEXTEND_H_H_HEAD__FILE__

#pragma once

#include <QGenericPlugin>
#include <QNetworkReply>

#include "IRequestNetInterface.h"

class INovelInfo;

/// <summary>
/// 这是一个回调接口，它总是处于被调用的状态
/// </summary>
class NOVEINFO_EXPORT IRequestNetInterfaceExtend : public QObject, public IRequestNetInterface {
	Q_OBJECT;
public:
	IRequestNetInterfaceExtend( QObject *parent = nullptr );
	~IRequestNetInterfaceExtend( ) override;
	std::shared_ptr<void> getData( ) override;
	QMap<QString, QUrl> getTypeUrls( const QNetworkReply &networkReply ) override;
	NovelPtrList getTypePageNovels( const QNetworkReply &networkReply, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) override;
	INovelInfoSharedPtr getUrlNovelInfo( const QNetworkReply &networkReply, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) override;
	QUrl getNext( const QNetworkReply &networkReply, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) override;
	void novelTypeEnd( const NovelPtrList &saveNovelInfos ) override;
	void endHost( const NovelPtrList &saveNovelInfos ) override;
};

#endif // IREQUESTNETINTERFACEEXTEND_H_H_HEAD__FILE__
