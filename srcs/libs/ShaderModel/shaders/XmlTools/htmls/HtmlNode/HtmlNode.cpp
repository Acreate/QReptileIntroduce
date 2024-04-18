#include "HtmlNode.h"

#include <cstdlib>

const HtmlNodeList htmlNodeParse( const wchar_t *htmlContent ) {
	
	size_t nLen = 0;
	while( htmlContent[ nLen ] != L'\0' )
		++nLen;
	nLen += 1;
	wchar_t *wcStr = ( wchar_t * )malloc( sizeof( wchar_t ) * nLen );
	for( size_t index = 0 ; index < nLen ; ++index )
		wcStr[ index ] = htmlContent[ index ];
	wcStr[ nLen - 1 ] = L'\0';

	return NULL;
}
size_t htmlNodeListSize( const HtmlNodeList list ) {

	size_t result = 0;
	if( list )
		do {
			if( ( list + result ) == NULL )
				return result;
			++result;
		} while( true );
	return 0;
}
size_t htmlNodeListAppend( HtmlNodeList *list, const HtmlNodePtr htmlNode ) {
	size_t oldSize = htmlNodeListSize( *list ); // 获取旧的长度
	size_t newSize = oldSize + 1; // 新的大小
	HtmlNodeList newList = ( HtmlNodeList )malloc( sizeof( HtmlNodeList ) * newSize ); // 申请内容
	for( size_t index = 0 ; index < oldSize ; ++index )
		newList[ index ] = ( *list )[ index ];
	newList[ oldSize ] = htmlNode;
	free( *list );
	*list = newList;
	return oldSize;
}
bool htmlNodeListFree( HtmlNodeList *list ) {
	size_t listSize = htmlNodeListSize( *list ); // 获取旧的长度
	for( size_t index = 0 ; index < listSize ; ++index ) {
		HtmlNode *htmlNode = ( *list )[ index ];
		HtmlNodePtr parentNode = htmlNodeGetRootNodePtr( htmlNode );
		if( parentNode ) {
			
		}
		free( htmlNode );
	}
	free( *list );
	*list = NULL;
	return true;
}
HtmlNodePtr htmlNodeGetRootNodePtr( const HtmlNodePtr currentHtmlNodePtr ) {
	HtmlNodePtr result = NULL;
	while( currentHtmlNodePtr->parent )
		result = currentHtmlNodePtr->parent;
	return result;
}
