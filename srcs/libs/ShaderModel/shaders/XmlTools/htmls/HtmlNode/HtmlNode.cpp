#include "HtmlNode.h"


#include "../html/HtmlDoc.h"
#include "../../wstr/WStrTools.h"

using namespace XmlTools;

HtmlNode::HtmlNode( ) {
}
HtmlNode::~HtmlNode( ) {
}
std::shared_ptr< std::wstring > HtmlNode::getWSNode( ) const {
	auto c_w_str_star_ptr = c_w_z_str->c_str( ) + ptr_offset;
	std::shared_ptr< std::wstring > result( new std::wstring( c_w_str_star_ptr, ptr_c_str_len ) );
	return result;
}
std::shared_ptr< std::wstring > HtmlNode::getNodeWSName( ) const {
	wchar_t currentChar = L'\0'; // 临时字符
	auto c_w_str = c_w_z_str->c_str( ) + ptr_offset; // 字符串指向坐标
	size_t index = 0;
	for( ; index < ptr_c_str_len; ++index ) { // 找到 < 后面的非空
		currentChar = c_w_str[ index ];
		if( currentChar == HtmlDoc::nodeStartChar || WStrTools::isJumpSpace( currentChar ) )
			continue;
		break;
	}
	c_w_str = c_w_str + index; // 指向第一个非空字符
	auto ptrCStrLen = ptr_c_str_len - index; // 缩减长度
	for( index = 0; index < ptrCStrLen; ++index ) { // 找到第一个空或者 / > 等字符
		currentChar = c_w_str[ index ];
		if( WStrTools::isJumpSpace( currentChar ) || currentChar == HtmlDoc::nodeEndChar || currentChar == HtmlDoc::forwardSlash )
			break;
	}
	if( currentChar == HtmlDoc::forwardSlash ) { // 如果碰到斜杠 /(节点是尾节点)
		for( ; index < ptrCStrLen; ++index ) { // 找到第一个非空或者 > 等字符
			currentChar = c_w_str[ index ];
			if( !WStrTools::isJumpSpace( currentChar ) ) {
				++index;
				c_w_str = c_w_str + index;
				for( index = 0; index < ptrCStrLen; ++index ) { // 找到第一个空或者 > 等字符
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
Vector_HtmlNodeSPtr_Shared HtmlNode::parseHtmlNodeCharPair( std::shared_ptr< std::wstring > std_c_w_str, size_t start_index, const size_t max_index, size_t &index_count ) {
	Vector_HtmlNodeSPtr_Shared result( new Vector_HtmlNodeSPtr );
	bool findCharResut = false;
	index_count = start_index;

	for( ; start_index < max_index; ++start_index ) {
		auto maxIndex = max_index;
		findCharResut = HtmlDoc::findNextNodeStartChar( std_c_w_str, maxIndex, start_index );
		if( !findCharResut )
			break;
		auto ptr = new HtmlNode;
		HtmlNode_Shared currentHtmlNodeCharPairSharedPtr( ptr );
		ptr->ptr_offset = start_index;
		findCharResut = HtmlDoc::findNextNodeEndChar( std_c_w_str, maxIndex, start_index );
		if( !findCharResut )
			break;
		ptr->ptr_c_str_len = start_index + 1 - ptr->ptr_offset;
		ptr->c_w_z_str = std_c_w_str;
		result->emplace_back( currentHtmlNodeCharPairSharedPtr );
	}
	index_count = start_index - index_count;
	return result;
}
