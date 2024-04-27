#ifndef REQUESTNET_H_H_HEAD__FILE__
#define REQUESTNET_H_H_HEAD__FILE__
#pragma once
#include <QGenericPlugin>
#include <interface/IRequestNetInterfaceExtend.h>
#include "../macro/cmake_to_c_cpp_header_macro.h"
using interfacePlugsType::IRequestNetInterfaceExtend;
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
	size_t getUrl( interfacePlugsType::HtmlDocString *outStr ) override;
	void setUrl( const interfacePlugsType::HtmlDocString &url ) override;
	void setHost( const interfacePlugsType::HtmlDocString &host ) override;
	size_t getHost( interfacePlugsType::HtmlDocString *outHost ) override;
	void setScheme( const interfacePlugsType::HtmlDocString &scheme ) override;
	size_t getScheme( interfacePlugsType::HtmlDocString *outScheme ) override;
	bool setInterfaceParent( void *parent ) override;
	void deleteMember( ) override;
public: // 实现解析
	interfacePlugsType::un_ordered_map * formHtmlGetTypeTheUrls( const interfacePlugsType::HtmlDocString &htmlText ) override;
	interfacePlugsType::NovelPtrList formHtmlGetTypePageNovels( const interfacePlugsType::HtmlDocString &htmlText, const interfacePlugsType::NovelPtrList &saveNovelInfos, void *appendDataPtr ) override;
	interfacePlugsType::INovelInfoSharedPtr formHtmlGetUrlNovelInfo( const interfacePlugsType::HtmlDocString &htmlText, const interfacePlugsType::NovelPtrList &saveNovelInfos, const interfacePlugsType::INovelInfoSharedPtr &networkReplayNovel ) override;
	interfacePlugsType::HtmlDocString formHtmlGetNext( const interfacePlugsType::HtmlDocString &htmlText, const interfacePlugsType::NovelPtrList &saveNovelInfos, const interfacePlugsType::NovelPtrList &lastNovelInfos ) override;
	OStream * setOStream( OStream *o_stream ) override;
	IStream * setIStream( IStream *i_stream ) override;
private:
	OStream *oStream;
	IStream *iStream;
public slots:
	IRequestNetInterfaceExtend * getRequestNetInterfaceExtendPatr( ) {
		return this;
	}

};

#endif // REQUESTNET_H_H_HEAD__FILE__
