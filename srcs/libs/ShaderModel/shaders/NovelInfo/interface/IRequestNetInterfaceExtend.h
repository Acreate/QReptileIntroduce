#ifndef IREQUESTNETINTERFACEEXTEND_H_H_HEAD__FILE__
#define IREQUESTNETINTERFACEEXTEND_H_H_HEAD__FILE__

#pragma once

#include "IRequestNetInterface.h"
#include "../nameSpace/interfacePlugsType.h"
#include "stream/IStream.h"
#include "stream/OStream.h"

namespace interfacePlugsType {
	/// <summary>
	/// 这是一个回调接口，它总是处于被调用的状态
	/// </summary>
	class NOVELINFO_EXPORT IRequestNetInterfaceExtend : public IRequestNetInterface {
	private:

	public:
		IRequestNetInterfaceExtend( );
		~IRequestNetInterfaceExtend( ) override;
		un_ordered_map * formHtmlGetTypeTheUrls( const HtmlDocString &htmlText ) override;
		NovelPtrList formHtmlGetTypePageNovels( const HtmlDocString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) override;
		INovelInfoSharedPtr formHtmlGetUrlNovelInfo( const HtmlDocString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) override;
		HtmlDocString formHtmlGetNext( const HtmlDocString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) override;
		void novelTypeEnd( const NovelPtrList &saveNovelInfos ) override;
		void endHost( const NovelPtrList &saveNovelInfos ) override;
		bool setInterfaceParent( void *parent ) override;
		void deleteMember( ) override;
		virtual OStream * setOStream( OStream *o_stream ) = 0;
		virtual IStream * setIStream( IStream *i_stream ) = 0;
	};

}

#endif // IREQUESTNETINTERFACEEXTEND_H_H_HEAD__FILE__
