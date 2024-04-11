#include "ArrayStruct.h"
ArrayStruct::ArrayStruct( ): key( nullptr ), value( nullptr ), keyLen( 0 ), valueLen( 0 ) {
}
ArrayStruct::ArrayStruct( char *key, size_t keyLen, char *value, size_t valueLen ): keyLen( keyLen ), valueLen( valueLen ) {
	this->key = new char[ this->keyLen ];
	memcpy( this->key, key, this->keyLen );
	this->key[ this->keyLen ] = 0;

	this->value = new char[ this->valueLen ];
	memcpy( this->value, value, this->valueLen );
	this->value[ this->valueLen ] = 0;
}
ArrayStruct **ArrayStruct::generateList( const size_t count ) {
	if( count == 0 )
		return nullptr;
	ArrayStruct **result = new ArrayStruct *[ count ];
	result[ count - 1 ] = nullptr;
	return result;
}
ArrayStruct *ArrayStruct::get( ArrayStruct **listStructPtr, const size_t listStructIndex ) {
	for( size_t index = 0 ; listStructPtr[ index ] != nullptr ; ++index )
		if( index == listStructIndex )
			return listStructPtr[ index ];
	return nullptr;
}
size_t ArrayStruct::inster( ArrayStruct **listStructPtr, ArrayStruct *insterListStructPtr, const size_t listStructIndex ) {
	size_t listSize = len( listStructPtr );
	ArrayStruct **result = new ArrayStruct *[ listSize ];
	size_t endIndex = listSize - 1;
	result[ endIndex ] = nullptr;
	size_t copyCount = listStructIndex - 1;
	copy( result, listStructPtr, copyCount );
	result[ listStructIndex ] = insterListStructPtr;
	copy( result + listStructIndex, listStructPtr + copyCount, endIndex );
	return listSize;

}
size_t ArrayStruct::len( ArrayStruct **listStructPtr ) {
	size_t index = 0;
	do {
		if( listStructPtr[ index ] == nullptr )
			return index;
		index++;
	} while( true );
}
size_t ArrayStruct::copy( ArrayStruct **listStructPtrDes, ArrayStruct **listStructPtrSrc, const size_t listStructPtrDesSize ) {
	size_t desListSize = len( listStructPtrSrc );
	size_t minSize = listStructPtrDesSize > desListSize ? desListSize : listStructPtrDesSize;
	size_t index = 0;
	for( ; index < minSize ; ++index )
		listStructPtrDes[ index ] = listStructPtrSrc[ index ];
	return minSize;
}
ArrayStruct::~ArrayStruct( ) {
	delete [] key;
	delete [] value;
	key = nullptr;
	value = nullptr;
}
bool ArrayStruct::getKey( char *resultKey, size_t resultLen ) const {
	if( resultLen < keyLen )
		return false;
	memcpy( resultKey, key, this->keyLen );
	return true;
}
bool ArrayStruct::getValue( char *resultValue, size_t resultLen ) const {
	if( resultLen < valueLen )
		return false;
	memcpy( resultValue, value, this->valueLen );
	return true;
}
size_t ArrayStruct::getKeyLen( ) const {
	return keyLen;
}
size_t ArrayStruct::getValueLen( ) const {
	return valueLen;
}
