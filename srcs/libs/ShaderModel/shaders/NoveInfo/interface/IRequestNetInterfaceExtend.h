#ifndef IREQUESTNETINTERFACEEXTEND_H_H_HEAD__FILE__
#define IREQUESTNETINTERFACEEXTEND_H_H_HEAD__FILE__

#pragma once

#include "IRequestNetInterface.h"

class INovelInfo;

/// <summary>
/// 这是一个回调接口，它总是处于被调用的状态
/// </summary>
class NOVEINFO_EXPORT IRequestNetInterfaceExtend : public IRequestNetInterface {
private:

public:
	IRequestNetInterfaceExtend( );
	~IRequestNetInterfaceExtend( ) override;
	un_ordered_map * getTypeUrls( const StdString &htmlText ) override;
	NovelPtrList getTypePageNovels( const StdString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) override;
	INovelInfoSharedPtr getUrlNovelInfo( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) override;
	StdString getNext( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) override;
	void novelTypeEnd( const NovelPtrList &saveNovelInfos ) override;
	void endHost( const NovelPtrList &saveNovelInfos ) override;
	bool setInterfaceParent( void *parent ) override;
	void deleteMember( ) override;
};

#endif // IREQUESTNETINTERFACEEXTEND_H_H_HEAD__FILE__
