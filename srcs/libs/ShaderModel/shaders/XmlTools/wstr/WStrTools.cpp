#include "WStrTools.h"

#include <cwctype>

bool WStrTools::isJumpSpace( wchar_t currentChar ) {
	return iswspace( currentChar ) || iswcntrl( currentChar ) || iswcntrl( currentChar );
}

size_t WStrTools::findNextWCharPotion( const wchar_t *w_c_ptr, size_t startIndex, const wchar_t find_w_c_char, size_t maxIndex ) {
	size_t index = startIndex;
	for( ; index < maxIndex ; ++index )
		if( w_c_ptr[ index ] == find_w_c_char )
			break;
	return index - startIndex;
}
