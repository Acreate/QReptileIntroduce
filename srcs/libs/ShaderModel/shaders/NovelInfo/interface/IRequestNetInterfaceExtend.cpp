#include "IRequestNetInterfaceExtend.h"
using namespace interfacePlugsType;
IRequestNetInterfaceExtend::IRequestNetInterfaceExtend( ) {
}
IRequestNetInterfaceExtend::~IRequestNetInterfaceExtend( ) {
}
un_ordered_map * IRequestNetInterfaceExtend::formHtmlGetTypeTheUrls( const HtmlDocString &htmlText ) {
	

	return nullptr;
}
NovelPtrList IRequestNetInterfaceExtend::formHtmlGetTypePageNovels( const HtmlDocString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) {
	return { };
}
INovelInfoSharedPtr IRequestNetInterfaceExtend::formHtmlGetUrlNovelInfo( const HtmlDocString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) {
	return { };
}
HtmlDocString IRequestNetInterfaceExtend::formHtmlGetNext( const HtmlDocString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) {
	return { };
}
void IRequestNetInterfaceExtend::novelTypeEnd( const NovelPtrList &saveNovelInfos ) {
}
void IRequestNetInterfaceExtend::endHost( const NovelPtrList &saveNovelInfos ) {
}
bool IRequestNetInterfaceExtend::setInterfaceParent( void *parent ) {
	return false;
}
void IRequestNetInterfaceExtend::deleteMember( ) {
}
