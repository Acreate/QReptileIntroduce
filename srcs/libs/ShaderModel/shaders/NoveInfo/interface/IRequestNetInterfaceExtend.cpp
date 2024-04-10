#include "IRequestNetInterfaceExtend.h"
IRequestNetInterfaceExtend::IRequestNetInterfaceExtend( QObject *parent ) : QObject( parent ) {
}
IRequestNetInterfaceExtend::~IRequestNetInterfaceExtend( ) {
	qDebug( ) << u8"IRequestNetInterfaceExtend::~IRequestNetInterfaceExtend( )";
}
std::shared_ptr<void> IRequestNetInterfaceExtend::getData( ) {
	return { };
}
QMap<QString, QUrl> IRequestNetInterfaceExtend::getTypeUrls( const QNetworkReply &networkReply ) {
	return { };
}
IRequestNetInterface::NovelPtrList IRequestNetInterfaceExtend::getTypePageNovels( const QNetworkReply &networkReply, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) {
	return { };
}
IRequestNetInterface::INovelInfoSharedPtr IRequestNetInterfaceExtend::getUrlNovelInfo( const QNetworkReply &networkReply, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) {
	return { };
}
QUrl IRequestNetInterfaceExtend::getNext( const QNetworkReply &networkReply, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) {
	return { };
}
void IRequestNetInterfaceExtend::novelTypeEnd( const NovelPtrList &saveNovelInfos ) {
}
void IRequestNetInterfaceExtend::endHost( const NovelPtrList &saveNovelInfos ) {
}
