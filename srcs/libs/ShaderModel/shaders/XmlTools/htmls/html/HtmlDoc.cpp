#include "HtmlDoc.h"

#include "../../macro/cmake_to_c_cpp_header_env.h"
#include "../../wstr/WStrTools.h"
#include "../HtmlNode/HtmlNode.h"
#include "../enum/HtmlNodeType/Html_Node_Type.h"
#include <QDebug>
#include <clocale>
#include <memory>
#include <path/Path.h>
#include <qdir.h>
#include <string>
using namespace XmlTools;


bool HtmlDoc::findNextNodeEndChar( const std::shared_ptr< std::wstring > std_c_w_string, size_t &max_index, size_t &start_index ) {
	auto CWStrLen = std_c_w_string->length( );
	if( max_index > CWStrLen )
		max_index = CWStrLen;
	auto CWStr = std_c_w_string->c_str( );
	for( ; start_index < max_index; ++start_index ) {
		auto currenChar = CWStr[ start_index ];
		if( currenChar == singleQuotation ) {
			++start_index;
			for( ; start_index < max_index; ++start_index ) {
				currenChar = CWStr[ start_index ];
				if( currenChar == singleQuotation )
					break;
			}
		} else if( currenChar == doubleQuotation ) {
			++start_index;
			for( ; start_index < max_index; ++start_index ) {
				currenChar = CWStr[ start_index ];
				if( currenChar == doubleQuotation )
					break;
			}
		} else if( currenChar == nodeEndChar )
			return true;
	}
	return false;
}
bool HtmlDoc::findNextNodeStartChar( const std::shared_ptr< std::wstring > std_c_w_string, size_t &max_index, size_t &start_index ) {
	auto cwStrPtr = std_c_w_string->c_str( );
	auto cwStrLen = std_c_w_string->length( );
	if( max_index > cwStrLen )
		max_index = cwStrLen;
	for( ; start_index < max_index; ++start_index ) {
		auto currenChar = cwStrPtr[ start_index ];
		if( currenChar == singleQuotation ) {
			++start_index;
			for( ; start_index < max_index; ++start_index ) {
				currenChar = cwStrPtr[ start_index ];
				if( currenChar == singleQuotation )
					break;
			}
		} else if( currenChar == doubleQuotation ) {
			++start_index;
			for( ; start_index < max_index; ++start_index ) {
				currenChar = cwStrPtr[ start_index ];
				if( currenChar == doubleQuotation )
					break;
			}
		} else if( currenChar == exclamation ) {
			++start_index;
			for( ; start_index < max_index; ++start_index ) {
				currenChar = cwStrPtr[ start_index ];
				if( currenChar == nodeStartChar )
					break;
			}
		} else if( currenChar == nodeStartChar )
			return true;
	}
	return false;
}
bool HtmlDoc::findNextNodeForwardSlash( const std::shared_ptr< std::wstring > std_c_w_string, size_t &max_index, size_t &start_index ) {
	auto cwzStrPtr = std_c_w_string.get( )->c_str( );
	auto cwzStrLen = std_c_w_string->length( );
	if( max_index > cwzStrLen )
		max_index = cwzStrLen;
	for( ; start_index < max_index; ++start_index ) {
		auto currenChar = cwzStrPtr[ start_index ];
		if( currenChar == singleQuotation ) {
			++start_index;
			for( ; start_index < max_index; ++start_index ) {
				currenChar = cwzStrPtr[ start_index ];
				if( currenChar == singleQuotation )
					break;
			}
		} else if( currenChar == doubleQuotation ) {
			++start_index;
			for( ; start_index < max_index; ++start_index ) {
				currenChar = cwzStrPtr[ start_index ];
				if( currenChar == doubleQuotation )
					break;
			}
		} else if( currenChar == exclamation ) {
			++start_index;
			for( ; start_index < max_index; ++start_index ) {
				currenChar = cwzStrPtr[ start_index ];
				if( currenChar == nodeEndChar )
					break;
			}
		} else if( currenChar == forwardSlash )
			return true;
	}
	return false;
}
bool HtmlDoc::isSingelNode( const std::shared_ptr< std::wstring > std_c_w_string, size_t &start_index, size_t &end_index ) {
	auto c_str = std_c_w_string->c_str( );
	auto currentChar = c_str[ start_index ];
	if( currentChar != nodeStartChar )
		for( ++start_index; start_index < end_index; ++start_index ) {
			currentChar = c_str[ start_index ];
			if( currentChar == nodeStartChar )
				break;
		}
	auto forwardSlashIndex = start_index + 1;
	if( findNextNodeForwardSlash( std_c_w_string, end_index, forwardSlashIndex ) ) {
		for( forwardSlashIndex += 1; forwardSlashIndex < end_index; ++forwardSlashIndex ) {
			currentChar = c_str[ forwardSlashIndex ];
			if( WStrTools::isJumpSpace( currentChar ) )
				continue;
			if( currentChar != nodeEndChar )
				break;
			end_index = forwardSlashIndex;
			return true;
		}
	}
	return false;
}
bool HtmlDoc::isStartNode( const std::shared_ptr< std::wstring > std_c_w_string, size_t &start_index, size_t &end_index ) {
	auto c_str = std_c_w_string->c_str( );
	auto currentChar = c_str[ start_index ];
	if( currentChar != nodeStartChar )
		for( ++start_index; start_index < end_index; ++start_index ) {
			currentChar = c_str[ start_index ];
			if( currentChar == nodeStartChar )
				break;
		}

	// 碰到的第一个必须是 > 而不是 /
	for( auto index = start_index + 1; index <= end_index; ++index ) {
		currentChar = c_str[ index ];
		if( WStrTools::isJumpSpace( currentChar ) )
			continue;
		if( currentChar == doubleQuotation ) {
			++index;
			for( ; index <= end_index; ++index ) {
				currentChar = c_str[ index ];
				if( currentChar == doubleQuotation )
					break;
			}
		} else if( currentChar == singleQuotation ) {
			++index;
			for( ; index <= end_index; ++index ) {
				currentChar = c_str[ index ];
				if( currentChar == singleQuotation )
					break;
			}
		} else if( currentChar == L'/' )
			return false;
		else {
			return true;
		}
	}
	return false;
}
bool HtmlDoc::isEndNode( const std::shared_ptr< std::wstring > std_c_w_string, size_t &start_index, size_t &end_index ) {
	auto c_str = std_c_w_string->c_str( );
	wchar_t currentChar = 0;
	// 碰到的第一个必须是 / 而不是通用字符或者 >
	for( ; start_index <= end_index; ++start_index ) {
		currentChar = c_str[ start_index ];
		if( currentChar == nodeStartChar )
			break;
	}
	for( auto index = start_index + 1; index <= end_index; ++index ) {
		currentChar = c_str[ index ];
		if( WStrTools::isJumpSpace( currentChar ) )
			continue;
		if( currentChar != L'/' )
			return true;
	}
	return false;
}
bool HtmlDoc::isAnnotation( const std::shared_ptr< std::wstring > std_c_w_string, size_t &start_index, size_t &end_index ) {
	auto c_str = std_c_w_string->c_str( );
	auto currentChar = c_str[ start_index ];
	if( currentChar != nodeStartChar )
		for( ++start_index; start_index < end_index; ++start_index ) {
			currentChar = c_str[ start_index ];
			if( currentChar == nodeStartChar )
				break;
		}
	size_t endIndex = start_index + 1;

	for( ; endIndex < end_index; ++endIndex ) {
		currentChar = c_str[ endIndex ];
		if( currentChar == exclamation ) {
			if( findNextNodeEndChar( std_c_w_string, end_index, endIndex ) ) {
				end_index = endIndex;
				return true;
			}
			start_index = 1;
			end_index = 0;
			return false;
		}
	}
	return false;
}
HtmlDoc HtmlDoc::parse( const std::shared_ptr< std::wstring > std_c_w_string, size_t &end_index, size_t &start_index ) {
	HtmlDoc result;
	result.html_W_C_Str = std::make_shared< std::wstring >( std_c_w_string->c_str( ) + start_index, end_index - start_index );
	auto stdCWString = result.html_W_C_Str;
	size_t count;
	auto resultHtml = HtmlNode::parseHtmlNodeCharPair( stdCWString, 0, end_index, count );
	auto htmlNodeCharPairs = resultHtml.get( );
	size_t maxSize = htmlNodeCharPairs->size( );
	size_t index = start_index;
	start_index = 0;
	for( ; index < maxSize; ++index ) {
		auto htmlDocCharPair = htmlNodeCharPairs->at( index );
		auto left = htmlDocCharPair.get( )->ptr_offset;
		auto right = htmlDocCharPair.get( )->ptr_c_str_len + left;
		bool nodeType = isAnnotation( stdCWString, left, right );
		if( nodeType && left < right ) {
			htmlDocCharPair->nodeType = Html_Node_Type::AnnotationNode;
			result.htmlDocNode->emplace_back( htmlDocCharPair );
		} else {
			left = htmlDocCharPair.get( )->ptr_offset;
			right = htmlDocCharPair.get( )->ptr_c_str_len + left;
			nodeType = isSingelNode( stdCWString, left, right );
			if( nodeType ) {
				htmlDocCharPair->nodeType = Html_Node_Type::SingleNode;
				result.htmlDocNode->emplace_back( htmlDocCharPair );
			} else {
				left = htmlDocCharPair.get( )->ptr_offset;
				size_t endLeft = left;
				right = htmlDocCharPair.get( )->ptr_c_str_len + left;
				if( isStartNode( stdCWString, left, right ) ) {
					auto nodeName = *htmlDocCharPair->getNodeWSName( );

					for( size_t lastNodeIndex = index + 1; lastNodeIndex < maxSize; ++lastNodeIndex ) {
						auto endDocNodeCharPairs = htmlNodeCharPairs->at( lastNodeIndex );
						left = endDocNodeCharPairs.get( )->ptr_offset;
						right = endDocNodeCharPairs.get( )->ptr_c_str_len + left;
						nodeType = isAnnotation( stdCWString, left, right );
						if( nodeType ) // 跳过注释节点
							continue;
						left = endDocNodeCharPairs.get( )->ptr_offset;
						right = endDocNodeCharPairs.get( )->ptr_c_str_len + left;
						nodeType = isSingelNode( stdCWString, left, right );
						if( nodeType ) // 跳过单节点
							continue;
						left = endDocNodeCharPairs.get( )->ptr_offset;
						endLeft = left;
						right = endDocNodeCharPairs.get( )->ptr_c_str_len + left;
						nodeType = isStartNode( stdCWString, left, right );
						if( nodeType ) // 跳过开头节点
							continue;
						nodeType = isEndNode( stdCWString, endLeft, right );
						if( !nodeType ) // 不是结束节点则跳过
							continue;
						auto endNodeName = *endDocNodeCharPairs->getNodeWSName( );
						if( nodeName == endNodeName ) { // 节点对象相等，则开始输出
							index = lastNodeIndex;
							htmlDocCharPair->nodeType = Html_Node_Type::DoubleNode;
							result.htmlDocNode->emplace_back( htmlDocCharPair );
							break;
						}
					}
				} else if( isEndNode( stdCWString, endLeft, right ) )
					result.htmlDocNode->emplace_back( htmlDocCharPair );
				else
					result.htmlDocNode->emplace_back( htmlDocCharPair );
			}
		}
		if( index == 0 )
			start_index = htmlDocCharPair.get( )->ptr_offset;
	}
	size_t size = result.htmlDocNode->size( );
	return result;
}
HtmlNode_Shared HtmlDoc::getNodeFromName( const std::wstring &nodeName ) const {
	for( auto node : *htmlDocNode.get( ) ) {
		if( *node->getNodeWSName( ) == nodeName )
			return node;
	}
	return nullptr;
}
HtmlNode_Shared HtmlDoc::getNodeFromName( const std::function< bool( const std::wstring &nodeName, Html_Node_Type htmlNodeType ) > &callFun ) const {
	for( auto node : *htmlDocNode.get( ) ) {
		if( callFun( *node->getNodeWSName( ), node->nodeType ) )
			return node;
	}
	return nullptr;
}
HtmlDoc::HtmlDoc( ) : htmlDocNode( new Vector_HtmlNodeSPtr ) {

}
HtmlDoc::~HtmlDoc( ) {
}
