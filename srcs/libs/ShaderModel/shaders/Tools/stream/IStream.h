#ifndef ISTREAM_H_H_HEAD__FILE__
#define ISTREAM_H_H_HEAD__FILE__
#pragma once
#include <QString>

#include "../export/Tools_export.h"
class TOOLS_EXPORT IStream {
public:
	virtual ~IStream( ) { }
public: // 重载 <<
	virtual IStream & operator <<( const QChar &msg ) = 0;
	virtual IStream & operator <<( const QArrayData &msg ) = 0;
	virtual IStream & operator <<( const QByteArray &msg ) = 0;
	virtual IStream & operator <<( const QString &msg ) = 0;
	virtual IStream & operator <<( const std::string &msg ) = 0;
	virtual IStream & operator <<( const std::wstring &msg ) = 0;
	virtual IStream & operator <<( const char *msg ) = 0;

	virtual IStream & operator <<( int8_t msg ) = 0;
	virtual IStream & operator <<( int16_t msg ) = 0;
	virtual IStream & operator <<( int32_t msg ) = 0;
	virtual IStream & operator <<( int64_t msg ) = 0;

	virtual IStream & operator <<( uint8_t msg ) = 0;
	virtual IStream & operator <<( uint16_t msg ) = 0;
	virtual IStream & operator <<( uint32_t msg ) = 0;
	virtual IStream & operator <<( uint64_t msg ) = 0;

	virtual IStream & operator <<( float_t msg ) = 0;
	virtual IStream & operator <<( double_t msg ) = 0;
public:
	/// <summary>
	/// 刷新流
	/// </summary>
	virtual void flush( ) = 0;
};


#endif // ISTREAM_H_H_HEAD__FILE__
