#include "RequestNetDefinition.h"

#include <qtextdocument.h>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextList>
IRequestNetInterface::un_ordered_map *RequestNet::formHtmlGetTypeTheUrls( const StdString &htmlText ) {

	return IRequestNetInterfaceExtend::formHtmlGetTypeTheUrls( htmlText );
}
IRequestNetInterface::NovelPtrList RequestNet::formHtmlGetTypePageNovels( const StdString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) {
	return IRequestNetInterfaceExtend::formHtmlGetTypePageNovels( htmlText, saveNovelInfos, appendDataPtr );
}
IRequestNetInterface::INovelInfoSharedPtr RequestNet::formHtmlGetUrlNovelInfo( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) {
	return IRequestNetInterfaceExtend::formHtmlGetUrlNovelInfo( htmlText, saveNovelInfos, networkReplayNovel );
}
IRequestNetInterface::StdString RequestNet::formHtmlGetNext( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) {
	return IRequestNetInterfaceExtend::formHtmlGetNext( htmlText, saveNovelInfos, lastNovelInfos );
}
