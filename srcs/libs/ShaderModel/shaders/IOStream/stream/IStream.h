#ifndef ISTREAM_H_H_HEAD__FILE__
#define ISTREAM_H_H_HEAD__FILE__
#pragma once
#include <QString>

#include "../export/IOStream_export.h"
class IOSTREAM_EXPORT Flush;
class IOSTREAM_EXPORT IStream {
public:
	virtual ~IStream( ) { }
public: // 重载 >>
	virtual IStream & operator >>( QChar &msg ) = 0;
	virtual IStream & operator >>( QArrayData &msg ) = 0;
	virtual IStream & operator >>( QByteArray &msg ) = 0;
	virtual IStream & operator >>( QString &msg ) = 0;
	virtual IStream & operator >>( std::string &msg ) = 0;
	virtual IStream & operator >>( std::wstring &msg ) = 0;

	virtual IStream & operator >>( int8_t &msg ) = 0;
	virtual IStream & operator >>( int16_t &msg ) = 0;
	virtual IStream & operator >>( int32_t &msg ) = 0;
	virtual IStream & operator >>( int64_t &msg ) = 0;

	virtual IStream & operator >>( uint8_t &msg ) = 0;
	virtual IStream & operator >>( uint16_t &msg ) = 0;
	virtual IStream & operator >>( uint32_t &msg ) = 0;
	virtual IStream & operator >>( uint64_t &msg ) = 0;

	virtual IStream & operator >>( float_t &msg ) = 0;
	virtual IStream & operator >>( double_t &msg ) = 0;


};

#endif // ISTREAM_H_H_HEAD__FILE__
