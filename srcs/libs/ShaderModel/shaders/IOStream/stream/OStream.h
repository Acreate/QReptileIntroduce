#ifndef OSTREAM_H_H_HEAD__FILE__
#define OSTREAM_H_H_HEAD__FILE__
#pragma once
#include <qbytearray.h>

#include "../export/IOStream_export.h"
struct QArrayData;
class IOSTREAM_EXPORT OStream {
public:
	static void errorQDebugOut( const std::string &msg, const std::string &fileName, size_t line );
public:
	virtual ~OStream( ) { }
public: // 重载 <<
	virtual OStream & operator <<( const QChar &msg ) = 0;
	virtual OStream & operator <<( const char &msg ) = 0;
	virtual OStream & operator <<( const wchar_t &msg ) = 0;
	virtual OStream & operator <<( const char16_t &msg ) = 0;
	virtual OStream & operator <<( const char32_t &msg ) = 0;
	virtual OStream & operator <<( const QArrayData &msg ) = 0;
	virtual OStream & operator <<( const QByteArray &msg ) = 0;
	virtual OStream & operator <<( const QString &msg ) = 0;
	virtual OStream & operator <<( const char *msg ) = 0;
	virtual OStream & operator <<( const std::string &msg ) = 0;
	virtual OStream & operator <<( const std::wstring &msg ) = 0;

	virtual OStream & operator <<( const int8_t &msg ) = 0;
	virtual OStream & operator <<( const int16_t &msg ) = 0;
	virtual OStream & operator <<( const int32_t &msg ) = 0;
	virtual OStream & operator <<( const int64_t &msg ) = 0;

	virtual OStream & operator <<( const uint8_t &msg ) = 0;
	virtual OStream & operator <<( const uint16_t &msg ) = 0;
	virtual OStream & operator <<( const uint32_t &msg ) = 0;
	virtual OStream & operator <<( const uint64_t &msg ) = 0;

	virtual OStream & operator <<( const float_t &msg ) = 0;
	virtual OStream & operator <<( const double_t &msg ) = 0;

	virtual OStream & operator <<( const void *msg ) = 0;
public:
	/// <summary>
	/// 刷新流
	/// </summary>
	virtual void flush( ) = 0;
};


#endif // OSTREAM_H_H_HEAD__FILE__
