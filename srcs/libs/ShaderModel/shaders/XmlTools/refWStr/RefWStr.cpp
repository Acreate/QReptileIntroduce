#include "RefWStr.h"
RefWStr::RefWStr( const wchar_t *ptr, const size_t len ): ptr( ptr ), len( len ) {
}
RefWStr::~RefWStr( ) {

}
size_t RefWStr::converStdWstring( std::wstring *outStr ) {
	if( isRef( ) ) {
		*outStr = std::wstring( this->getPtr( ), this->len );
		return this->len;
	}
	return 0;
}
bool RefWStr::equeWStr( const wchar_t *c_w_str, const size_t c_w_str_len ) {
	if( c_w_str_len != len )
		return false;
	return equeWStr( c_w_str );
}
bool RefWStr::equeWStr( const wchar_t *c_w_str ) {
	for( size_t index = 0 ; index < len ; ++index )
		if( ptr[ index ] != c_w_str[ index ] )
			return false;
	return true;
}
wchar_t RefWStr::comp( const wchar_t *c_w_str, const size_t c_w_str_len ) {
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
RefWStr RefWStr::generateRefWStr( const wchar_t *c_w_str_ptr, const wchar_t endChar ) {
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
RefWStr RefWStr::generateRefWStr( const wchar_t *c_w_str_ptr, const wchar_t *c_w_str_end ) {
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
RefWStr RefWStr::generateRefWStr( const wchar_t *c_w_str_ptr, const wchar_t *c_w_str_end, const size_t c_w_str_end_len ) {
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
RefWStr RefWStr::generateRefWStr( const wchar_t *c_w_str_ptr, const size_t c_w_str_ptr_len, const wchar_t *c_w_str_end, const size_t c_w_str_end_len ) {
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
RefWStr RefWStr::generateRefWStr( const RefWStr src, const RefWStr des ) {
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
RefWStr RefWStr::generateRefWStr( const wchar_t *c_w_str_ptr, const CheckFunction &callFunction ) {
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
RefWStr RefWStr::generateRefWStr( const RefWStr src, const CheckFunction &callFunction ) {
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