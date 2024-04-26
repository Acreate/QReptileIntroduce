#ifndef OSTREAM_H_H_HEAD__FILE__
#define OSTREAM_H_H_HEAD__FILE__
#pragma once
#include <qbytearray.h>

#include "../export/Tools_export.h"
struct QArrayData;
class TOOLS_EXPORT OStream {

public:
	virtual ~OStream( ) { }
public: // 重载 >>
	virtual OStream & operator >>( QChar &msg ) = 0;
	virtual OStream & operator >>( QArrayData &msg ) = 0;
	virtual OStream & operator >>( QByteArray &msg ) = 0;
	virtual OStream & operator >>( QString &msg ) = 0;
	virtual OStream & operator >>( std::string &msg ) = 0;
	virtual OStream & operator >>( std::wstring &msg ) = 0;

	virtual OStream & operator >>( int8_t &msg ) = 0;
	virtual OStream & operator >>( int16_t &msg ) = 0;
	virtual OStream & operator >>( int32_t &msg ) = 0;
	virtual OStream & operator >>( int64_t &msg ) = 0;

	virtual OStream & operator >>( uint8_t &msg ) = 0;
	virtual OStream & operator >>( uint16_t &msg ) = 0;
	virtual OStream & operator >>( uint32_t &msg ) = 0;
	virtual OStream & operator >>( uint64_t &msg ) = 0;

	virtual OStream & operator >>( float_t &msg ) = 0;
	virtual OStream & operator >>( double_t &msg ) = 0;
};


#endif // OSTREAM_H_H_HEAD__FILE__
