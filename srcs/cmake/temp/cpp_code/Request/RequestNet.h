#ifndef REQUESTNET_H_H_HEAD__FILE__
#define REQUESTNET_H_H_HEAD__FILE__
#pragma once
#include <QGenericPlugin>
#include <interface/IRequestNetInterfaceExtend.h>
#include "../macro/cmake_to_c_cpp_header_macro.h"
Q_DECLARE_INTERFACE( IRequestNetInterfaceExtend, IRequestNetInterfaceExtend_iid );
class RequestNet : public QObject, public IRequestNetInterfaceExtend {
	Q_OBJECT;
	Q_INTERFACES( IRequestNetInterfaceExtend )
private:
	QUrl url;
public:
	RequestNet( QObject *parent = nullptr );
	~RequestNet( ) override;
public: // 实现虚函数

	void getData( void *resultAnyPtr ) override;
	size_t getUrl( StdString *outStr ) override;
	void setUrl( const StdString &url ) override;
	void setHost( const StdString &host ) override;
	size_t getHost( StdString *outHost ) override;
	void setScheme( const StdString &scheme ) override;
	size_t getScheme( StdString *outScheme ) override;
	bool setInterfaceParent( void *parent ) override;
	void deleteMember( ) override;
public: // 实现解析
	un_ordered_map * formHtmlGetTypeTheUrls( const StdString &htmlText ) override;
	NovelPtrList formHtmlGetTypePageNovels( const StdString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) override;
	INovelInfoSharedPtr formHtmlGetUrlNovelInfo( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) override;
	StdString formHtmlGetNext( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) override;
public slots:
	IRequestNetInterfaceExtend *getRequestNetInterfaceExtendPatr( ) {
		return this;
	}
	
};

#endif // REQUESTNET_H_H_HEAD__FILE__
