#ifndef NOVELNETJOB_H_H_HEAD__FILE__
#define NOVELNETJOB_H_H_HEAD__FILE__
#pragma once
#include <QObject>

#include "../export/Plug_export.h"
#include "interface/IRequestNetInterface.h"

class QNetworkReply;
class RequestConnect;
class PLUG_EXPORT NovelNetJob : public QObject, public IRequestNetInterface {

	Q_OBJECT;
private:
	QObject *interfaceObjPtr;
	IRequestNetInterface *interfaceThisPtr;
public:
	NovelNetJob( QObject *parent, QObject *interface_obj_ptr, IRequestNetInterface *interface_this_ptr );
	~NovelNetJob( ) override;
public: // 属性
	void getData( void *resultAnyPtr ) override;
	size_t getUrl( StdString *outStr ) override;
	size_t getHost( StdString *outHost ) override;
	size_t getScheme( StdString *outScheme ) override;

	void setUrl( const StdString &url ) override;
	void setHost( const StdString &host ) override;
	void setScheme( const StdString &scheme ) override;
public: // 解析
	un_ordered_map * formHtmlGetTypeTheUrls( const StdString &htmlText ) override;
	NovelPtrList formHtmlGetTypePageNovels( const StdString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) override;
	INovelInfoSharedPtr formHtmlGetUrlNovelInfo( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) override;
	StdString formHtmlGetNext( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) override;
public: // 结束
	void novelTypeEnd( const NovelPtrList &saveNovelInfos ) override;
	void endHost( const NovelPtrList &saveNovelInfos ) override;
public: // 节点
	bool setInterfaceParent( void *parent ) override;
	void deleteMember( ) override;
public: // 类的独有属性
	QNetworkReply * start( RequestConnect *request_connect );
};


#endif // NOVELNETJOB_H_H_HEAD__FILE__
