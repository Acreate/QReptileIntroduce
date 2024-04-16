#include "RequestNetDefinition.h"

#include <qtextdocument.h>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextList>
IRequestNetInterface::un_ordered_map *RequestNet::formHtmlGetTypeTheUrls( const StdString &htmlText ) {

	QTextDocument document;

	document.setHtml( QString::fromLocal8Bit( htmlText.c_str( ) ) );

	qDebug( ) << "=======================";
	qDebug( ) << document.toHtml( );
	qDebug( ) << "=======================";
	QTextCursor cursor( &document );
	while( !cursor.atBlockEnd( ) ) {

		QTextFrame *currentFrame = cursor.currentFrame( );
		QTextList *currentList = cursor.currentList( );
		QTextTable *currentTable = cursor.currentTable( );

		QTextBlock block = cursor.block( );

		QString blockText = block.text( );
		QTextBlockFormat blockFormat = block.blockFormat( );
		QList< QTextLayout::FormatRange > formatRanges = block.textFormats( );

		// 输出这个文本块
		qDebug( ) << "blockText:""\n\t" << blockText;
		// 移动到下一个位置
		if( !cursor.movePosition( QTextCursor::NextBlock ) )
			break;
	}

	qDebug( ) << "=======================";

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
