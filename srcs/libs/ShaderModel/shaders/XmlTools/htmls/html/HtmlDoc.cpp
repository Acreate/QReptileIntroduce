#include "HtmlDoc.h"

#include <QDebug>
#include <clocale>
#include <qdir.h>
#include "../../wstr/WStrTools.h"
#include "../../refWStr/RefWStr.h"
#include "../../macro/cmake_to_c_cpp_header_env.h"
#include <path/Path.h>

void print_console_msg( const RefWStr &msg ) {
	std::wstring outMsg;
	msg.converStdWstring( &outMsg );
	qDebug( ) << "void print_console_msg( const RefWStr &" << &msg << " )""\n""======""\n" << QString::fromStdWString( outMsg ).toLocal8Bit( ).toStdString( ).c_str( ) << "\n" "=======";
}
QString conver_console_QString_msg( const RefWStr &msg ) {
	std::wstring outMsg;
	msg.converStdWstring( &outMsg );
	return QString::fromStdWString( outMsg );
}

std::shared_ptr< std::wstring > HtmlDoc::HtmlNodeCharPair::getNodeWSName( ) {
	wchar_t currentChar = L'\0'; // 临时字符
	auto c_w_str = html->c_str( ) + left; // 字符串指向坐标
	size_t index = 0;
	for( ; index < len ; ++index ) { // 找到 < 后面的非空
		currentChar = c_w_str[ index ];
		if( currentChar == nodeStartChar || WStrTools::isJumpSpace( currentChar ) )
			continue;
		break;
	}
	c_w_str = c_w_str + index; // 指向第一个非空字符
	len = len - index; // 缩减长度
	for( index = 0 ; index < len ; ++index ) { // 找到第一个空或者 / > 等字符
		currentChar = c_w_str[ index ];
		if( WStrTools::isJumpSpace( currentChar ) || currentChar == nodeEndChar || currentChar == forwardSlash )
			break;
	}
	if( currentChar == forwardSlash ) { // 如果碰到斜杠 /(节点是尾节点)
		for( ; index < len ; ++index ) { // 找到第一个非空或者 > 等字符
			currentChar = c_w_str[ index ];
			if( !WStrTools::isJumpSpace( currentChar ) ) {
				++index;
				c_w_str = c_w_str + index;
				for( index = 0 ; index < len ; ++index ) { // 找到第一个空或者 > 等字符
					currentChar = c_w_str[ index ];
					if( WStrTools::isJumpSpace( currentChar ) || currentChar == nodeEndChar )
						break;
				}
				break;
			}
		}

	}
	len = index;
	std::shared_ptr< std::wstring > result( new std::wstring( c_w_str, len ) );
	return result;
}
bool HtmlDoc::findNextNodeEndChar( const wchar_t *c_str, const size_t c_str_len, size_t *start_Index ) {

	for( ; *start_Index < c_str_len ; ++*start_Index ) {
		auto currenChar = c_str[ *start_Index ];
		if( currenChar == singleQuotation ) {
			++*start_Index;
			for( ; *start_Index < c_str_len ; ++*start_Index ) {
				currenChar = c_str[ *start_Index ];
				if( currenChar == singleQuotation )
					break;
			}
		} else if( currenChar == doubleQuotation ) {
			++*start_Index;
			for( ; *start_Index < c_str_len ; ++*start_Index ) {
				currenChar = c_str[ *start_Index ];
				if( currenChar == doubleQuotation )
					break;
			}
		} else if( currenChar == nodeEndChar )
			return true;
	}
	return false;
}
bool HtmlDoc::findNextNodeStartChar( const wchar_t *c_str, const size_t c_str_len, size_t *start_Index ) {

	for( ; *start_Index < c_str_len ; ++*start_Index ) {
		auto currenChar = c_str[ *start_Index ];
		if( currenChar == singleQuotation ) {
			++*start_Index;
			for( ; *start_Index < c_str_len ; ++*start_Index ) {
				currenChar = c_str[ *start_Index ];
				if( currenChar == singleQuotation )
					break;
			}
		} else if( currenChar == doubleQuotation ) {
			++*start_Index;
			for( ; *start_Index < c_str_len ; ++*start_Index ) {
				currenChar = c_str[ *start_Index ];
				if( currenChar == doubleQuotation )
					break;
			}
		} else if( currenChar == exclamation ) {
			++*start_Index;
			for( ; *start_Index < c_str_len ; ++*start_Index ) {
				currenChar = c_str[ *start_Index ];
				if( currenChar == nodeStartChar )
					break;
			}
		} else if( currenChar == nodeStartChar )
			return true;
	}
	return false;
}
bool HtmlDoc::findNextNodeForwardSlash( const wchar_t *c_str, const size_t c_str_len, size_t *start_Index ) {

	for( ; *start_Index < c_str_len ; ++*start_Index ) {
		auto currenChar = c_str[ *start_Index ];
		if( currenChar == singleQuotation ) {
			++*start_Index;
			for( ; *start_Index < c_str_len ; ++*start_Index ) {
				currenChar = c_str[ *start_Index ];
				if( currenChar == singleQuotation )
					break;
			}
		} else if( currenChar == doubleQuotation ) {
			++*start_Index;
			for( ; *start_Index < c_str_len ; ++*start_Index ) {
				currenChar = c_str[ *start_Index ];
				if( currenChar == doubleQuotation )
					break;
			}
		} else if( currenChar == exclamation ) {
			++*start_Index;
			for( ; *start_Index < c_str_len ; ++*start_Index ) {
				currenChar = c_str[ *start_Index ];
				if( currenChar == nodeEndChar )
					break;
			}
		} else if( currenChar == forwardSlash )
			return true;
	}
	return false;
}
bool HtmlDoc::isSingelNode( const wchar_t *c_str, size_t *start_Index, size_t *end_Index ) {

	auto currentChar = c_str[ *start_Index ];
	if( currentChar != nodeStartChar )
		for( ++*start_Index ; *start_Index < *end_Index ; ++*start_Index ) {
			currentChar = c_str[ *start_Index ];
			if( currentChar == nodeStartChar )
				break;
		}
	auto forwardSlashIndex = *start_Index + 1;
	if( findNextNodeForwardSlash( c_str, *end_Index, &forwardSlashIndex ) ) {
		for( forwardSlashIndex += 1 ; forwardSlashIndex < *end_Index ; ++forwardSlashIndex ) {
			currentChar = c_str[ forwardSlashIndex ];
			if( WStrTools::isJumpSpace( currentChar ) )
				continue;
			if( currentChar != nodeEndChar )
				break;
			*end_Index = forwardSlashIndex;
			return true;
		}
	}
	return false;
}
bool HtmlDoc::isStartNode( const wchar_t *c_str, size_t *start_Index, size_t *end_Index ) {

	auto currentChar = c_str[ *start_Index ];
	if( currentChar != nodeStartChar )
		for( ++*start_Index ; *start_Index < *end_Index ; ++*start_Index ) {
			currentChar = c_str[ *start_Index ];
			if( currentChar == nodeStartChar )
				break;
		}

	// 碰到的第一个必须是 > 而不是 /
	for( auto index = *start_Index + 1 ; index <= *end_Index ; ++index ) {
		currentChar = c_str[ index ];
		if( WStrTools::isJumpSpace( currentChar ) )
			continue;
		if( currentChar == doubleQuotation ) {
			++index;
			for( ; index <= *end_Index ; ++index ) {
				currentChar = c_str[ index ];
				if( currentChar == doubleQuotation )
					break;
			}
		} else if( currentChar == singleQuotation ) {
			++index;
			for( ; index <= *end_Index ; ++index ) {
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
bool HtmlDoc::isEndNode( const wchar_t *c_str, size_t *start_Index, size_t *end_Index ) {
	wchar_t currentChar = 0;
	// 碰到的第一个必须是 / 而不是通用字符或者 >
	for( ; *start_Index <= *end_Index ; ++*start_Index ) {
		currentChar = c_str[ *start_Index ];
		if( currentChar == nodeStartChar )
			break;
	}
	for( auto index = *start_Index + 1 ; index <= *end_Index ; ++index ) {
		currentChar = c_str[ index ];
		if( WStrTools::isJumpSpace( currentChar ) )
			continue;
		if( currentChar != L'/' )
			return true;
	}
	return false;
}
bool HtmlDoc::isAnnotation( const wchar_t *c_str, size_t *start_Index, size_t *end_Index ) {
	auto currentChar = c_str[ *start_Index ];
	if( currentChar != nodeStartChar )
		for( ++*start_Index ; *start_Index < *end_Index ; ++*start_Index ) {
			currentChar = c_str[ *start_Index ];
			if( currentChar == nodeStartChar )
				break;
		}
	size_t endIndex = *start_Index + 1;

	for( ; endIndex < *end_Index ; ++endIndex ) {
		currentChar = c_str[ endIndex ];
		if( currentChar == exclamation ) {
			if( findNextNodeEndChar( c_str, *end_Index, &endIndex ) ) {
				*end_Index = endIndex;
				return true;
			}
			*start_Index = 1;
			*end_Index = 0;
			return false;
		}
	}
	return false;
}
HtmlDoc HtmlDoc::parse( const wchar_t *c_str, const size_t c_str_len, size_t *startIndex ) {

	HtmlDoc result;
	result.html_W_C_Str = std::make_shared< std::wstring >( c_str, c_str_len );

	auto stdWStr = result.html_W_C_Str.get( );
	auto resultHtml = parseHtmlNodeCharPair( stdWStr, c_str_len );
	auto htmlNodeCharPairs = resultHtml.get( );
	size_t maxSize = htmlNodeCharPairs->size( );
	for( size_t index = 0 ; index < maxSize ; ++index ) {
		auto htmlDocCharPair = htmlNodeCharPairs->at( index );
		auto left = htmlDocCharPair.get( )->left;
		auto right = htmlDocCharPair.get( )->len + left;
		bool nodeType = isAnnotation( stdWStr->c_str( ), &left, &right );
		if( nodeType && left < right ) {
			htmlDocCharPair->nodeType = HtmlNodeCharPair::AnnotationNode;
			result.htmlDocNode->emplace_back( htmlDocCharPair );
		} else {
			left = htmlDocCharPair.get( )->left;
			right = htmlDocCharPair.get( )->len + left;
			nodeType = isSingelNode( stdWStr->c_str( ), &left, &right );
			if( nodeType ) {
				htmlDocCharPair->nodeType = HtmlNodeCharPair::SingleNode;
				result.htmlDocNode->emplace_back( htmlDocCharPair );
			} else {
				left = htmlDocCharPair.get( )->left;
				size_t endLeft = left;
				right = htmlDocCharPair.get( )->len + left;
				if( isStartNode( stdWStr->c_str( ), &left, &right ) ) {
					auto nodeName = *htmlDocCharPair->getNodeWSName( );

					for( size_t lastNodeIndex = index + 1 ; lastNodeIndex < maxSize ; ++lastNodeIndex ) {
						auto endDocNodeCharPairs = htmlNodeCharPairs->at( lastNodeIndex );
						left = endDocNodeCharPairs.get( )->left;
						right = endDocNodeCharPairs.get( )->len + left;
						nodeType = isAnnotation( stdWStr->c_str( ), &left, &right );
						if( nodeType ) // 跳过注释节点
							continue;
						left = endDocNodeCharPairs.get( )->left;
						right = endDocNodeCharPairs.get( )->len + left;
						nodeType = isSingelNode( stdWStr->c_str( ), &left, &right );
						if( nodeType ) // 跳过单节点
							continue;
						left = endDocNodeCharPairs.get( )->left;
						endLeft = left;
						right = endDocNodeCharPairs.get( )->len + left;
						nodeType = isStartNode( stdWStr->c_str( ), &left, &right );
						if( nodeType ) // 跳过开头节点
							continue;
						nodeType = isEndNode( stdWStr->c_str( ), &endLeft, &right );
						if( !nodeType ) // 不是结束节点则跳过
							continue;
						auto endNodeName = *endDocNodeCharPairs->getNodeWSName( );
						if( nodeName == endNodeName ) { // 节点对象相等，则开始输出
							index = lastNodeIndex;
							htmlDocCharPair->nodeType = HtmlNodeCharPair::DoubleNode;
							result.htmlDocNode->emplace_back( htmlDocCharPair );
							break;
						}
					}
				} else if( isEndNode( stdWStr->c_str( ), &endLeft, &right ) )
					result.htmlDocNode->emplace_back( htmlDocCharPair );
				else
					result.htmlDocNode->emplace_back( htmlDocCharPair );
			}
		}
	}

	return result;
}
HtmlDoc::HtmlNodeCharPairSharedPtrVectorSharedPtr HtmlDoc::parseHtmlNodeCharPair( std::wstring *std_w_str, const size_t c_str_len ) {
	HtmlNodeCharPairSharedPtrVectorSharedPtr result( new HtmlNodeCharPairSharedPtrVector );
	bool findCharResut = false;
	auto c_str = std_w_str->c_str( );
	for( size_t index = 0 ; index < c_str_len ; ++index ) {
		findCharResut = findNextNodeStartChar( c_str, c_str_len, &index );
		if( !findCharResut )
			break;
		auto ptr = new HtmlNodeCharPair;
		HtmlNodeCharPairSharedPtr currentHtmlNodeCharPairSharedPtr( ptr );
		ptr->left = index;
		findCharResut = findNextNodeEndChar( c_str, c_str_len, &index );
		if( !findCharResut )
			break;
		ptr->len = index + 1 - ptr->left;
		ptr->html = std_w_str;
		result->emplace_back( currentHtmlNodeCharPairSharedPtr );
	}

	return result;
}

HtmlDoc::HtmlDoc( ): htmlDocNode( new HtmlNodeCharPairSharedPtrVector ) {

}
HtmlDoc::~HtmlDoc( ) {
}
