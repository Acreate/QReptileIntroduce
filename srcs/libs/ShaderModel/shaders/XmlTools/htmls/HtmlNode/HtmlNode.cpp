#include "HtmlNode.h"


#include "../html/HtmlDoc.h"
#include "../../wstr/WStrTools.h"

using namespace XmlTools;

HtmlNode::HtmlNode( ) {
}
HtmlNode::~HtmlNode( ) {
}
std::shared_ptr< std::wstring > HtmlNode::getWSNode( ) {
	auto cwStr = cppstd_w_str_ptr->c_str( );
	auto c_w_str_star_ptr = cwStr + ptr_offset;
	std::shared_ptr< std::wstring > result( new std::wstring( c_w_str_star_ptr, ptr_c_str_len ) );
	return result;
}
std::shared_ptr< std::wstring > HtmlNode::getNodeWSName( ) {
	wchar_t currentChar = L'\0'; // 临时字符
	auto c_w_str = cppstd_w_str_ptr->c_str( ) + ptr_offset; // 字符串指向坐标
	size_t index = 0;
	for( ; index < ptr_c_str_len ; ++index ) { // 找到 < 后面的非空
		currentChar = c_w_str[ index ];
		if( currentChar == HtmlDoc::nodeStartChar || WStrTools::isJumpSpace( currentChar ) )
			continue;
		break;
	}
	c_w_str = c_w_str + index; // 指向第一个非空字符
	auto ptrCStrLen = ptr_c_str_len - index; // 缩减长度
	for( index = 0 ; index < ptrCStrLen ; ++index ) { // 找到第一个空或者 / > 等字符
		currentChar = c_w_str[ index ];
		if( WStrTools::isJumpSpace( currentChar ) || currentChar == HtmlDoc::nodeEndChar || currentChar == HtmlDoc::forwardSlash )
			break;
	}
	if( currentChar == HtmlDoc::forwardSlash ) { // 如果碰到斜杠 /(节点是尾节点)
		for( ; index < ptrCStrLen ; ++index ) { // 找到第一个非空或者 > 等字符
			currentChar = c_w_str[ index ];
			if( !WStrTools::isJumpSpace( currentChar ) ) {
				++index;
				c_w_str = c_w_str + index;
				for( index = 0 ; index < ptrCStrLen ; ++index ) { // 找到第一个空或者 > 等字符
					currentChar = c_w_str[ index ];
					if( WStrTools::isJumpSpace( currentChar ) || currentChar == HtmlDoc::nodeEndChar )
						break;
				}
				break;
			}
		}
	}
	std::shared_ptr< std::wstring > result( new std::wstring( c_w_str, index ) );
	return result;
}
