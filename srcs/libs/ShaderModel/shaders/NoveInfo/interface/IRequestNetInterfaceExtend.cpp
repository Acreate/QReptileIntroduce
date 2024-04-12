#include "IRequestNetInterfaceExtend.h"
IRequestNetInterfaceExtend::IRequestNetInterfaceExtend( ) {
}
IRequestNetInterfaceExtend::~IRequestNetInterfaceExtend( ) {
}
IRequestNetInterface::un_ordered_map *IRequestNetInterfaceExtend::getTypeUrls( const StdString &htmlText ) {
	return nullptr;
}
IRequestNetInterface::NovelPtrList IRequestNetInterfaceExtend::getTypePageNovels( const StdString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) {
	return { };
}
IRequestNetInterface::INovelInfoSharedPtr IRequestNetInterfaceExtend::getUrlNovelInfo( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) {
	return { };
}
IRequestNetInterface::StdString IRequestNetInterfaceExtend::getNext( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) {
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
