#include "HtmlDoc.h"

#include <QDebug>
#include <clocale>
HtmlDoc::RefWStr::RefWStr( const wchar_t *ptr, const size_t len ): ptr( ptr ), len( len ) {
}
HtmlDoc::RefWStr::~RefWStr( ) {

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
HtmlDoc::RefWStr HtmlDoc::RefWStr::generateRefWStr( const wchar_t *c_w_str_ptr, const std::function< int64_t( const wchar_t &, size_t ) > &callFunction ) {
	for( size_t index = 0 ; true ; ++index ) {
		wchar_t currentChar = c_w_str_ptr[ index ];
		auto resultCompNumber = callFunction( currentChar, index );
		if( resultCompNumber != 0 )
			return { c_w_str_ptr, index + resultCompNumber };
		if( currentChar == L'\0' )
			break;
	}
	return { };
}
HtmlDoc::RefWStr HtmlDoc::RefWStr::generateRefWStr( const RefWStr src, const std::function< int64_t( const wchar_t & ) > &callFunction ) {
	if( src.isRef( ) ) {
		size_t len = src.getLen( );
		auto src_w_c_str = src.getPtr( );
		for( size_t index = 0 ; index < len ; ++index ) {
			auto resultCompNumber = callFunction( src_w_c_str[ index ] );
			if( resultCompNumber != 0 )
				return { src_w_c_str, index + resultCompNumber };
		}
	}
	return { };
}
HtmlDoc::HtmlDoc( ): html( nullptr ), htmlSize( 0 ) {

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
			qDebug( ) << u8"接触开始 < [" << index << "]";
			// 过滤空格
			auto spaceJumpIndex = index + 1;
			for( ; spaceJumpIndex < this->htmlSize ; ++spaceJumpIndex )
				if( !isJumpSpace( this->html[ spaceJumpIndex ] ) )
					break;
			auto cWStrPtr = this->html + spaceJumpIndex + 1;
			auto refWStr = RefWStr::generateRefWStr( cWStrPtr, [&]( const wchar_t checkCurrentChar, size_t currentIndex ) {
				if( isJumpSpace( checkCurrentChar ) || checkCurrentChar == L'/' || checkCurrentChar == L'>' ) {
					spaceJumpIndex = currentIndex;
					return -1;
				}
				return 0;
			} );
			bool isRef = refWStr.isRef( );
			if( isRef ) {
				refWStr.setLen( spaceJumpIndex );
				qDebug( ) << u8"检测字符串为 / (" << ( isRef ? u8"有效" : u8"无效" ) << ")";
				bool equeWStr = refWStr.equeWStr( L"DOCTYPE" );
				if( equeWStr )
					qDebug( ) << u8"节点为 html 标识  DOCTYPE";
			}
		} else if( currentChar == L'/' ) {
			qDebug( ) << u8"接触结束标识符 / [" << index << "]";
		} else if( currentChar == L'>' ) {
			qDebug( ) << u8"接触结束 > [" << index << "]";
		}
	}

	return 0;
}
HtmlDoc::~HtmlDoc( ) {
	if( this->html )
		delete[] this->html;
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
