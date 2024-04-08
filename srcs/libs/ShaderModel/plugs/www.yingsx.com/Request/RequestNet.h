#ifndef REQUESTNET_H_H_HEAD__FILE__
#define REQUESTNET_H_H_HEAD__FILE__
#pragma once
#include <QGenericPlugin>
#include <interface/IRequestNetInterface.h>

#define IRequestNetInterface_iid "www.121ds.cc.json.IRequestNetInterface"
Q_DECLARE_INTERFACE( IRequestNetInterface, IRequestNetInterface_iid );

class RequestNet : public IRequestNetInterface {
	Q_OBJECT;
	Q_INTERFACES( IRequestNetInterface )
public:
	RequestNet( QObject *parent = nullptr ): IRequestNetInterface( parent ) {
	}
	~RequestNet( ) override;
public: // 实现虚函数
	std::shared_ptr< void > getData( ) override;
	QUrl getUrl( ) override;
	QMap< QString, QUrl > getTypeUrls( const QNetworkReply &&networkReply ) override;
	NovelPtrList getTypePageNovels( const QNetworkReply &&networkReply, const NovelPtrList &&saveNovelInfos, void *appendDataPtr ) override;
	INovelInfoSharedPtr getUrlNovelInfo( const QNetworkReply &&networkReply, const NovelPtrList &&saveNovelInfos, const INovelInfoSharedPtr &&networkReplayNovel ) override;
	QUrl getNext( const QNetworkReply &&networkReply, const NovelPtrList &&saveNovelInfos, const NovelPtrList &&lastNovelInfos ) override;
	void novelTypeEnd( const NovelPtrList &&saveNovelInfos ) override;
	void endHost( const NovelPtrList &&saveNovelInfos ) override;
};

#endif // REQUESTNET_H_H_HEAD__FILE__
