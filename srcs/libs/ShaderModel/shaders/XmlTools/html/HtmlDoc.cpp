#include "HtmlDoc.h"

#include <QDebug>
#include <clocale>
#include <qdir.h>

#include "../macro/cmake_to_c_cpp_header_env.h"
#include <path/Path.h>
HtmlDoc::RefWStr::RefWStr( const wchar_t *ptr, const size_t len ): ptr( ptr ), len( len ) {
}
HtmlDoc::RefWStr::~RefWStr( ) {

}
size_t HtmlDoc::RefWStr::converStdWstring( std::wstring *outStr ) {
	if( isRef( ) ) {
		*outStr = std::wstring( this->getPtr( ), this->len );
		return this->len;
	}
	return 0;
}
bool HtmlDoc::RefWStr::equeWStr( const wchar_t *c_w_str, const size_t c_w_str_len ) {
	if( c_w_str_len != len )
		return false;
	return equeWStr( c_w_str );
}
bool HtmlDoc::RefWStr::equeWStr( const wchar_t *c_w_str ) {
	for( size_t index = 0 ; index < len ; ++index )
		if( ptr[ index ] != c_w_str[ index ] )
			return false;
	return true;
}
wchar_t HtmlDoc::RefWStr::comp( const wchar_t *c_w_str, const size_t c_w_str_len ) {
	auto minLen = c_w_str_len > len;
	wchar_t discrepancy;
	if( minLen ) {
		for( size_t index = 0 ; index < len ; ++index ) {
			discrepancy = c_w_str[ index ] - this->ptr[ index ];
			if( discrepancy )
				return discrepancy;
		}
		return c_w_str[ len ];
	}
	for( size_t index = 0 ; index < c_w_str_len ; ++index ) {
		discrepancy = c_w_str[ index ] - this->ptr[ index ];
		if( discrepancy )
			return discrepancy;
	}
	// c_w_str[ c_w_str_len ] = 未知
	return 0 - this->ptr[ c_w_str_len ];
}
HtmlDoc::RefWStr HtmlDoc::RefWStr::generateRefWStr( const wchar_t *c_w_str_ptr, const wchar_t endChar ) {
	size_t index = 0;
	for( ; true ; ++index ) {
		wchar_t currentChar = c_w_str_ptr[ index ];
		if( currentChar == L'\0' )
			break;
		if( currentChar == endChar )
			return RefWStr{ c_w_str_ptr, index };
	}
	return { };
}
HtmlDoc::RefWStr HtmlDoc::RefWStr::generateRefWStr( const wchar_t *c_w_str_ptr, const wchar_t *c_w_str_end ) {
	size_t rightCWStrLen = 0, leftCWStrLen = 0;
	for( ; c_w_str_end[ rightCWStrLen ] != '\0' ; ++rightCWStrLen );
	for( ; c_w_str_ptr[ leftCWStrLen ] != '\0' ; ++leftCWStrLen );
	if( leftCWStrLen < rightCWStrLen )
		return { };
	bool currentChar = true;
	for( size_t index = 0 ; index < leftCWStrLen ; ++index )
		if( c_w_str_end[ index ] == c_w_str_ptr[ index ] ) {
			if( ( index + rightCWStrLen ) > leftCWStrLen ) // 超出比较范围
				break;
			for( size_t subCompIndex = 0 ; subCompIndex < rightCWStrLen ; ++subCompIndex )
				if( c_w_str_end[ index + subCompIndex ] != c_w_str_ptr[ index + subCompIndex ] )
					currentChar = false;
			if( currentChar ) // 匹配到的位置
				return { c_w_str_ptr, index + rightCWStrLen };
			currentChar = true;
		}
	return { };

}
HtmlDoc::RefWStr HtmlDoc::RefWStr::generateRefWStr( const wchar_t *c_w_str_ptr, const wchar_t *c_w_str_end, const size_t c_w_str_end_len ) {
	size_t leftCWStrLen = 0;
	for( ; c_w_str_ptr[ leftCWStrLen ] != '\0' ; ++leftCWStrLen );
	if( leftCWStrLen < c_w_str_end_len )
		return { };
	bool currentChar = true;
	for( size_t index = 0 ; index < leftCWStrLen ; ++index )
		if( c_w_str_end[ index ] == c_w_str_ptr[ index ] ) {
			if( ( index + c_w_str_end_len ) > leftCWStrLen ) // 超出比较范围
				break;
			for( size_t subCompIndex = 0 ; subCompIndex < c_w_str_end_len ; ++subCompIndex )
				if( c_w_str_end[ index + subCompIndex ] != c_w_str_ptr[ index + subCompIndex ] )
					currentChar = false;
			if( currentChar ) // 匹配到的位置
				return { c_w_str_ptr, index + c_w_str_end_len };
			currentChar = true;
		}
	return { };
}
HtmlDoc::RefWStr HtmlDoc::RefWStr::generateRefWStr( const wchar_t *c_w_str_ptr, const size_t c_w_str_ptr_len, const wchar_t *c_w_str_end, const size_t c_w_str_end_len ) {
	if( c_w_str_ptr_len < c_w_str_end_len )
		return { };
	bool currentChar = true;
	for( size_t index = 0 ; index < c_w_str_ptr_len ; ++index )
		if( c_w_str_end[ index ] == c_w_str_ptr[ index ] ) {
			if( ( index + c_w_str_end_len ) > c_w_str_ptr_len ) // 超出比较范围
				break;
			for( size_t subCompIndex = 0 ; subCompIndex < c_w_str_end_len ; ++subCompIndex )
				if( c_w_str_end[ index + subCompIndex ] != c_w_str_ptr[ index + subCompIndex ] )
					currentChar = false;
			if( currentChar ) // 匹配到的位置
				return { c_w_str_ptr, index + c_w_str_end_len };
			currentChar = true;
		}
	return { };
}
HtmlDoc::RefWStr HtmlDoc::RefWStr::generateRefWStr( const RefWStr src, const RefWStr des ) {
	auto c_w_str_ptr_len = src.getLen( );
	auto c_w_str_end_len = des.getLen( );
	auto c_w_str_end = des.getPtr( );
	auto c_w_str_ptr = src.getPtr( );
	if( src.getLen( ) < des.getLen( ) )
		return { };
	bool currentChar = true;
	for( size_t index = 0 ; index < c_w_str_ptr_len ; ++index )
		if( c_w_str_end[ index ] == c_w_str_ptr[ index ] ) {
			if( ( index + c_w_str_end_len ) > c_w_str_ptr_len ) // 超出比较范围
				break;
			for( size_t subCompIndex = 0 ; subCompIndex < c_w_str_end_len ; ++subCompIndex )
				if( c_w_str_end[ index + subCompIndex ] != c_w_str_ptr[ index + subCompIndex ] )
					currentChar = false;
			if( currentChar ) // 匹配到的位置
				return { c_w_str_ptr, index + c_w_str_end_len };
			currentChar = true;
		}
	return { };
}
HtmlDoc::RefWStr HtmlDoc::RefWStr::generateRefWStr( const wchar_t *c_w_str_ptr, const CheckFunction &callFunction ) {
	for( size_t index = 0 ; true ; ++index ) {
		wchar_t currentChar = c_w_str_ptr[ index ];
		size_t outCompNumber;
		size_t paramIndex = index;
		auto resultCompNumber = callFunction( currentChar, paramIndex, outCompNumber );
		if( resultCompNumber != 0 )
			if( resultCompNumber > 0 )
				return { c_w_str_ptr, index + outCompNumber };
			else
				return { c_w_str_ptr, index - outCompNumber };
		if( ( paramIndex != index && c_w_str_ptr[ paramIndex ] == L'\0' ) || currentChar == L'\0' )
			break;
		index = paramIndex;
	}
	return { };
}
HtmlDoc::RefWStr HtmlDoc::RefWStr::generateRefWStr( const RefWStr src, const CheckFunction &callFunction ) {
	if( src.isRef( ) ) {
		size_t len = src.getLen( );
		auto src_w_c_str = src.getPtr( );
		for( size_t index = 0 ; index < len ; ++index ) {
			size_t outCompNumber;
			wchar_t currentChar = src_w_c_str[ index ];
			size_t resultCompNumber = callFunction( currentChar, index, outCompNumber );
			if( resultCompNumber != 0 )
				if( resultCompNumber > 0 )
					return { src_w_c_str, index + outCompNumber };
				else
					return { src_w_c_str, index - outCompNumber };
			if( currentChar == L'\0' )
				break;
		}
	}
	return { };
}
HtmlDoc::HtmlNode::HtmlNode( ): cWStrPtr( nullptr ), cWStrEndPtr( nullptr ), htmlSize( 0 ) {
}
HtmlDoc::HtmlNode::HtmlNode( wchar_t *cWStrPtr, wchar_t *cWStrEndPtr, size_t htmlSize )
: cWStrPtr( cWStrPtr ), cWStrEndPtr( cWStrEndPtr ), htmlSize( htmlSize ) {

}
HtmlDoc::HtmlNode::~HtmlNode( ) {
}
HtmlDoc::HtmlDoc( ): html( nullptr ), htmlSize( 0 ) {
	normalNode = new SubNode;
}
bool HtmlDoc::isJumpSpace( wchar_t currentChar ) {
	return iswspace( currentChar ) || iswcntrl( currentChar ) || iswcntrl( currentChar );
}
int32_t HtmlDoc::init( ) {

	for( size_t index = 0 ; index < this->htmlSize ; ++index ) {
		wchar_t currentChar = this->html[ index ];
		if( isJumpSpace( currentChar ) ) {
			//qDebug( ) << u8"遭遇空格 [" << index << "]";
		} else if( currentChar == L'<' ) {
			currentChar = this->html[ index + 1 ];
			if( currentChar == L'!' ) {// 不可解析内容
				for( index = index + 1 ; index < this->htmlSize ; ++index )
					if( this->html[ index ] == L'>' )
						break;
				continue;
			}
			auto spaceJumpIndex = index + 1;
			// 过滤空格
			for( ; spaceJumpIndex < this->htmlSize ; ++spaceJumpIndex )
				if( !isJumpSpace( this->html[ spaceJumpIndex ] ) )
					break;
			auto nodeStartPtr = this->html + index;
			auto mixLen = this->htmlSize - index;
			// todo : 找到 / 之后的 >
			for( auto orgIndex = 0 ; orgIndex < mixLen ; ++orgIndex ) {
				currentChar = nodeStartPtr[ orgIndex ];
				if( currentChar == L'/' ) { // todo : 找到了 /
					for( auto orgSubIndex = orgIndex + 1 ; orgSubIndex < mixLen ; ++orgSubIndex ) {
						currentChar = nodeStartPtr[ orgSubIndex ];
						if( currentChar == L'>' ) { // todo : 找到了 >
							index = index + orgSubIndex;
							RefWStr refWStr = RefWStr( nodeStartPtr, orgSubIndex + 1 );
							QString filePathName = QString( u8"%1%2%3" ).arg( Project_Run_bin ).arg( QDir::separator( ) ).arg( u8"读取到的节点内容.txt" );
							QFile file( filePathName );
							if( Path::creatFilePath( filePathName ) )
								if( file.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text ) ) {
									std::wstring outWString;
									size_t converStdWstring = refWStr.converStdWstring( &outWString );
									QString fromStdWString = QString::fromStdWString( outWString );
									file.write( fromStdWString.toLocal8Bit( ) );
									file.close( );
								}
							break;
						}
					}
					break;
				}
			}
		} else if( currentChar == L'/' ) {
			qDebug( ) << u8"接触结束标识符 / [" << index << "]";
		} else if( currentChar == L'>' ) {
			qDebug( ) << u8"接触结束 > [" << index << "]";
		}
	}

	return 0;
}
HtmlDoc *HtmlDoc::newObj( ) {
	// todo : 使用 new 的方式创建的一个独立的内存。不再发生引用
	return nullptr;
}
HtmlDoc::~HtmlDoc( ) {
	if( this->html )
		delete[] this->html;
	delete normalNode;
}
std::shared_ptr< HtmlDoc > HtmlDoc::parse( wchar_t *c_str, const size_t c_str_len ) {
	auto result = new HtmlDoc( );
	result->html = c_str;
	result->htmlSize = c_str_len;
	if( !result->init( ) ) {
		result->html = new wchar_t [ c_str_len ];
		for( size_t index = 0 ; index < c_str_len ; ++index )
			result->html[ index ] = c_str[ index ];
		return std::shared_ptr< HtmlDoc >( result );
	}
	result->html = nullptr;
	return nullptr;
}
