#include "HtmlNode.h"

HtmlNode::HtmlNode( ): cWStrPtr( nullptr ), cWStrEndPtr( nullptr ), htmlSize( 0 ) {
}
HtmlNode::HtmlNode( wchar_t *cWStrPtr, wchar_t *cWStrEndPtr, size_t htmlSize )
: cWStrPtr( cWStrPtr ), cWStrEndPtr( cWStrEndPtr ), htmlSize( htmlSize ) {

}
HtmlNode::~HtmlNode( ) {
}
