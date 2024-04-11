#ifndef LISTSTRUCT_H_H_HEAD__FILE__
#define LISTSTRUCT_H_H_HEAD__FILE__
#pragma once
#include <cstring>
#include "../export/NoveInfo_export.h"

class NOVEINFO_EXPORT ArrayStruct {
private:
	ArrayStruct( );
	ArrayStruct( char *key, size_t keyLen, char *value, size_t valueLen );
public: // 列表工具
	static ArrayStruct **generateList( const size_t count );
	/// <summary>
	/// 获取指定下标的结构体
	/// 失败返回 nullptr
	/// </summary>
	/// <param name="listStructPtr">列表指针</param>
	/// <param name="listStructIndex">下标</param>
	/// <returns>返回数据</returns>
	static ArrayStruct *get( ArrayStruct **listStructPtr, const size_t listStructIndex );
	static size_t inster( ArrayStruct **listStructPtr, ArrayStruct *insterListStructPtr, const size_t listStructIndex );
	static size_t len( ArrayStruct **listStructPtr );
	/// <summary>
	/// 把一个列表数据拷贝到另外一个列表数据
	/// </summary>
	/// <param name="listStructPtrDes">拷贝目标</param>
	/// <param name="listStructPtrSrc">拷贝源</param>
	/// <param name="listStructPtrDesSize">拷贝源(<paramref name="listStructPtrDes"/> )大小</param>
	/// <returns>拷贝个数</returns>
	static size_t copy( ArrayStruct **listStructPtrDes, ArrayStruct **listStructPtrSrc, const size_t listStructPtrDesSize );
protected:
	char *key, *value;
	size_t keyLen, valueLen;
public:
	~ArrayStruct( );
	bool getKey( char *resultKey, size_t resultLen ) const;
	bool getValue( char *resultValue, size_t resultLen ) const;
	size_t getKeyLen( ) const;
	size_t getValueLen( ) const;
};

#endif // LISTSTRUCT_H_H_HEAD__FILE__
