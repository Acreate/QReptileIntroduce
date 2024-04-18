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

bool HtmlDoc::findNextNodeEndChar( const wchar_t *c_str, const size_t c_str_len, size_t *start_Index ) {
	constexpr wchar_t singleQuotation = L'\''; // 单引号
	constexpr wchar_t doubleQuotation = L'\"'; // 双引号
	constexpr wchar_t exclamation = L'!'; // 感叹号。用于识别 DOCTYPE 节点或注释节点
	constexpr wchar_t nodeEndChar = L'>'; // 节点结束
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
	constexpr wchar_t singleQuotation = L'\''; // 单引号
	constexpr wchar_t doubleQuotation = L'\"'; // 双引号
	constexpr wchar_t exclamation = L'!'; // 感叹号。用于识别 DOCTYPE 节点或注释节点
	constexpr wchar_t nodeStartChar = L'<'; // 节点开始
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
	constexpr wchar_t singleQuotation = L'\''; // 单引号
	constexpr wchar_t doubleQuotation = L'\"'; // 双引号
	constexpr wchar_t exclamation = L'!'; // 感叹号。用于识别 DOCTYPE 节点或注释节点
	constexpr wchar_t forwardSlash = L'/'; // 斜杠路径符。节点类型判定(单元素节点/双元素节点)
	constexpr wchar_t nodeEndChar = L'>'; // 节点结束
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
	constexpr wchar_t nodeStartChar = L'<'; // 节点开始
	constexpr wchar_t nodeEndChar = L'>'; // 节点结束
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
	constexpr wchar_t singleQuotation = L'\''; // 单引号
	constexpr wchar_t doubleQuotation = L'\"'; // 双引号
	constexpr wchar_t nodeStartChar = L'<'; // 节点开始
	constexpr wchar_t nodeEndChar = L'>'; // 节点结束
	auto currentChar = c_str[ *start_Index ];
	if( currentChar != nodeStartChar )
		for( ++*start_Index ; *start_Index < *end_Index ; ++*start_Index ) {
			currentChar = c_str[ *start_Index ];
			if( currentChar == nodeStartChar )
				break;
		}

	// 碰到的第一个必须是 > 而不是 /
	for( ++*start_Index ; *start_Index <= *end_Index ; ++*start_Index ) {
		currentChar = c_str[ *start_Index ];
		if( WStrTools::isJumpSpace( currentChar ) )
			continue;
		if( currentChar == doubleQuotation ) {
			++*start_Index;
			for( ; *start_Index <= *end_Index ; ++*start_Index ) {
				currentChar = c_str[ *start_Index ];
				if( currentChar == doubleQuotation )
					break;
			}
		} else if( currentChar == singleQuotation ) {
			++*start_Index;
			for( ; *start_Index <= *end_Index ; ++*start_Index ) {
				currentChar = c_str[ *start_Index ];
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
	// 碰到的第一个必须是 / 而不是通用字符或者 >
	for( ; *start_Index <= *end_Index ; ++*start_Index ) {
		wchar_t currentChar = c_str[ *start_Index ];
		if( WStrTools::isJumpSpace( currentChar ) || currentChar == L'<' )
			continue;
		if( currentChar != L'/' )
			return true;
	}
	return false;
}
bool HtmlDoc::isAnnotation( const wchar_t *c_str, size_t *start_Index, size_t *end_Index ) {
	constexpr wchar_t exclamation = L'!'; // 感叹号。用于识别 DOCTYPE 节点或注释节点
	constexpr wchar_t nodeStartChar = L'<'; // 节点开始
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
HtmlDoc::HtmlDocSharedPtrVectorSharedPtr HtmlDoc::parse( const wchar_t *c_str, const size_t c_str_len, size_t *startIndex ) {

	std::shared_ptr< std::wstring > wstringPtr( new std::wstring( c_str, c_str_len ) );
	HtmlDocSharedPtrVectorSharedPtr result( new HtmlDocSharedPtrVector );

	constexpr wchar_t singleQuotation = L'\''; // 单引号
	constexpr wchar_t exclamation = L'!'; // 感叹号。用于识别 DOCTYPE 节点或注释节点
	constexpr wchar_t doubleQuotation = L'\"'; // 双引号
	constexpr wchar_t nodeStartChar = L'<'; // 节点开始
	constexpr wchar_t nodeEndChar = L'>'; // 节点结束
	constexpr wchar_t forwardSlash = L'/'; // 斜杠路径符。节点类型判定(单元素节点/双元素节点)
	constexpr wchar_t backSlash = L'\\'; // 反斜杠路径符
	wchar_t currentWChar; // 存储当前字符
	size_t notSpaceIndex; // 存储跳过空字符的下标
	std::wstring buffWString; // 存储从 HtmlDoc 获取的字符串内容
	auto stdWStr = wstringPtr.get( );
	auto htmlNodeCharPairs = parseHtmlNodeCharPair( stdWStr, c_str_len );
	size_t count = 0;
	QStringList singeNode, annno, start, end, none;
	for( auto htmlDocCharPair : *htmlNodeCharPairs.get( ) ) {
		auto left = htmlDocCharPair.get( )->left;
		auto right = htmlDocCharPair.get( )->len + left;
		bool nodeType = isAnnotation( stdWStr->c_str( ), &left, &right );
		if( nodeType && left < right ) {
			QString arg( u8"======""\n""\t""发现:""\n""\t""%1""\n""注释节点""\n""=======""\n" );
			annno.append( arg.arg( QString::fromStdWString( *htmlDocCharPair->getWString( ) ) ) );
		} else {
			left = htmlDocCharPair.get( )->left;
			right = htmlDocCharPair.get( )->len + left;
			nodeType = isSingelNode( stdWStr->c_str( ), &left, &right );
			if( nodeType ) {
				QString arg( u8"======""\n""\t""发现:""\n""\t""%1""\n""单节点""\n""=======""\n" );
				singeNode.append( arg.arg( QString::fromStdWString( *htmlDocCharPair->getWString( ) ) ) );
			} else {
				left = htmlDocCharPair.get( )->left;
				size_t endLeft = left;
				right = htmlDocCharPair.get( )->len + left;
				if( isStartNode( stdWStr->c_str( ), &left, &right ) ) {
					QString arg( u8"======""\n""\t""起始节点:""\n""\t""%1""\n""双节点""\n""=======""\n" );
					start.append( arg.arg( QString::fromStdWString( *htmlDocCharPair->getWString( ) ) ) );
				} else if( isEndNode( stdWStr->c_str( ), &endLeft, &right ) ) {
					QString arg( u8"======""\n""\t""结束节点:""\n""\t""%1""\n""双节点""\n""=======""\n" );
					end.append( arg.arg( QString::fromStdWString( *htmlDocCharPair->getWString( ) ) ) );
				} else {
					QString arg( u8"======""\n""\t""未知节点:""\n""\t""%1""\n""=======""\n" );
					none.append( arg.arg( QString::fromStdWString( *htmlDocCharPair->getWString( ) ) ) );
				}

			}
		}
		++count;
	}
	// singeNode, annno, start, end, none; 写入文件

	QString writeFilePathDir = QString( u8"%1%2%3%2%4" ).arg( Project_Run_bin ).arg( QDir::separator( ) ).arg( u8"write_test_cache" );
	QFile file( writeFilePathDir.arg( u8"singeNode.txt" ) );
	if( Path::creatFilePath( file.fileName( ) ) ) {
		if( file.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly ) ) {
			QString writeFileCont = singeNode.join( u8'\n' );
			file.write( writeFileCont.toLocal8Bit( ) );
			file.close( );
		}
	}
	file.setFileName( writeFilePathDir.arg( u8"annno.txt" ) );
	if( Path::creatFilePath( file.fileName( ) ) ) {
		if( file.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly ) ) {
			QString writeFileCont = annno.join( u8'\n' );
			file.write( writeFileCont.toLocal8Bit( ) );
			file.close( );
		}
	}
	file.setFileName( writeFilePathDir.arg( u8"start.txt" ) );
	if( Path::creatFilePath( file.fileName( ) ) ) {
		if( file.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly ) ) {
			QString writeFileCont = start.join( u8'\n' );
			file.write( writeFileCont.toLocal8Bit( ) );
			file.close( );
		}
	}
	file.setFileName( writeFilePathDir.arg( u8"end.txt" ) );
	if( Path::creatFilePath( file.fileName( ) ) ) {
		if( file.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly ) ) {
			QString writeFileCont = end.join( u8'\n' );
			file.write( writeFileCont.toLocal8Bit( ) );
			file.close( );
		}
	}
	file.setFileName( writeFilePathDir.arg( u8"none.txt" ) );
	if( Path::creatFilePath( file.fileName( ) ) ) {
		if( file.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly ) ) {
			QString writeFileCont = none.join( u8'\n' );
			file.write( writeFileCont.toLocal8Bit( ) );
			file.close( );
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
size_t HtmlDoc::getString( std::wstring *out_result_w_string ) {
	const wchar_t *cStr = html->c_str( );
	if( cStr == nullptr || this->nlen == 0 )
		return 0;
	*out_result_w_string = std::wstring( startPtr + cStr, this->nlen );
	return this->nlen;
}

HtmlDoc::HtmlDoc( )
: startPtr( 0 ), nlen( 0 ), parent( nullptr ) {
	children = new std::list< HtmlDoc * >;
}

HtmlDoc::HtmlDoc( const std::shared_ptr< std::wstring > &html, size_t startPtr, size_t nlen )
: html( html ), startPtr( startPtr ), nlen( nlen ), parent( nullptr ) {
	children = new std::list< HtmlDoc * >;
}

HtmlDoc::~HtmlDoc( ) {
	for( auto &child : *children )
		delete child;
	delete children;
}
