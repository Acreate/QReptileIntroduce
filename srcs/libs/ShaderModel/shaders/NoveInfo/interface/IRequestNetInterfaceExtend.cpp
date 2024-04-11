#include "IRequestNetInterfaceExtend.h"
IRequestNetInterfaceExtend::IRequestNetInterfaceExtend( ) {
}
IRequestNetInterfaceExtend::~IRequestNetInterfaceExtend( ) {

}
void *IRequestNetInterfaceExtend::getData( ) {
	return { };
}

void IRequestNetInterfaceExtend::novelTypeEnd( const NovelPtrList &saveNovelInfos ) {
}
void IRequestNetInterfaceExtend::endHost( const NovelPtrList &saveNovelInfos ) {
}
size_t IRequestNetInterfaceExtend::getUrl( std::string *outStr ) {
	return 0;
}
void IRequestNetInterfaceExtend::deleteMember( ) {
}

void IRequestNetInterfaceExtend::setUrl( const StdString &url ) {
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
	return nullptr;
}
bool IRequestNetInterfaceExtend::setInterfaceParent( void *parent ) {
	return false;
}
