#ifndef OSTREAM_H_H_HEAD__FILE__
#define OSTREAM_H_H_HEAD__FILE__
#pragma once
#include <qbytearray.h>
#include <QString>
#include "../auto_generate_files/export/IOStream_export.h"
struct QArrayData;
class IOSTREAM_EXPORT OStream {
public:
	/// <summary>
	/// 写入日志
	/// </summary>
	/// <param name="write_path">目录（非文件）</param>
	/// <param name="write_content">写入内容</param>
	/// <param name="file_base_name">基本名称</param>
	/// <param name="error_type">错误类型</param>
	/// <param name="dir_name">可能存在的中间目录</param>
	/// <param name="file_suffix">文件后缀</param>
	/// <returns>写入数量</returns>
	static qint64 outDebugLogFile( const QString &write_path, const QByteArray &write_content, QString file_base_name, QString error_type = u8"normal", QString dir_name = "", QString file_suffix = u8".log" );
	/// <summary>
	/// 同步流输出，使用指定 OStream 与 qdebu() 输出信息
	/// 如果 writePath 正确，那么将会写入内容。
	/// </summary>
	/// <param name="os">输出</param>
	/// <param name="msg">消息</param>
	/// <param name="file_name">产生错误的文件</param>
	/// <param name="line">产生错误的文件行数</param>
	/// <param name="call_fun_name">产生错误的调用函数</param>
	/// <param name="write_path">写入路径</param>
	/// <param name="write_content">写入内容</param>
	static QStringList anyDebugOut( OStream *os, const QString &msg, const QString &file_name, size_t line, const QString &call_fun_name, const QString &write_path, const QString &write_content );

	/// <summary>
	/// 同步流输出，使用指定 OStream 与 qdebu() 输出信息
	/// 如果 writePath 正确，那么将会写入内容。
	/// </summary>
	/// <param name="os">输出</param>
	/// <param name="msg">消息</param>
	/// <param name="file_name">产生错误的文件</param>
	/// <param name="line">产生错误的文件行数</param>
	/// <param name="call_fun_name">产生错误的调用函数</param>
	static QStringList anyDebugOut( OStream *os, const QString &msg, const QString &file_name, size_t line, const QString &call_fun_name );
	/// <summary>
	/// 使用指定 qdebug() 输出信息
	/// 如果 writePath 正确，那么将会写入内容。
	/// </summary>
	/// <param name="msg">消息</param>
	/// <param name="fileName">产生错误的文件</param>
	/// <param name="line">产生错误的文件行数</param>
	/// <param name="call_fun_name">产生错误的调用函数</param>
	/// <param name="write_path">写入路径</param>
	/// <param name="write_content">写入内容</param>
	static QStringList errorQDebugOut( const QString &msg, const QString &fileName, size_t line, const QString &call_fun_name, const QString &write_path, const QString &write_content );
	/// <summary>
	/// 使用指定流输出信息
	/// 当 os 不存在时，写入 qdebug()
	/// </summary>
	/// <param name="os">输出</param>
	/// <param name="msg">消息</param>
	/// <param name="fileName">产生错误的文件</param>
	/// <param name="line">产生错误的文件行数</param>
	/// <param name="call_fun_name">产生错误的调用函数</param>
	static QStringList errorQDebugOut( OStream *os, const QString &msg, const QString &fileName, size_t line, const QString &call_fun_name );
	/// <summary>
	/// 使用指定 qdebug() 输出信息
	/// 如果 writePath 正确，那么将会写入内容。
	/// </summary>
	/// <param name="msg">消息</param>
	/// <param name="fileName">产生错误的文件</param>
	/// <param name="line">产生错误的文件行数</param>
	/// <param name="call_fun_name">产生错误的调用函数</param>
	static QStringList errorQDebugOut( const QString &msg, const QString &fileName, size_t line, const QString &call_fun_name );

	/// <summary>
	/// 使用指定 os 输出信息，假设 os 为null，将会调用
	/// static void errorQDebugOut( const std::string &msg, const std::string &fileName, size_t line, const std::string &callFunName, const QString &writePath = QString( u8"" ), const QString &writeContent = QString( u8"" ) );
	/// 如果 writePath 正确，那么将会写入内容。
	/// </summary>
	/// <param name="os">输出</param>
	/// <param name="msg">消息</param>
	/// <param name="fileName">产生错误的文件</param>
	/// <param name="line">产生错误的文件行数</param>
	/// <param name="call_fun_name">产生错误的调用函数</param>
	/// <param name="write_path">写入路径</param>
	/// <param name="write_content">写入内容</param>
	static QStringList errorQDebugOut( OStream *os, const QString &msg, const QString &fileName, size_t line, const QString &call_fun_name, const QString &write_path, const QString &write_content );


	/// <summary>
	/// 使用指定 os 输出信息，假设 os 为null，将会调用
	/// static void errorQDebugOut( const std::string &msg );
	/// 如果 writePath 正确，那么将会写入内容。
	/// </summary>
	/// <param name="os">输出</param>
	/// <param name="msg">消息</param>
	static void anyDebugOut( OStream *os, const QString &msg );
	/// <summary>
	/// 使用指定 os 输出信息，假设 os 为null，将会调用
	/// static void errorQDebugOut( const std::string &msg );
	/// 如果 writePath 正确，那么将会写入内容。
	/// </summary>
	/// <param name="os">输出</param>
	/// <param name="msg">消息</param>
	static void errorQDebugOut( OStream *os, const QString &msg );
	/// <summary>
	/// 使用指定 os 输出信息，假设 os 为null，将会调用
	/// static void errorQDebugOut( const std::string &msg );
	/// 如果 writePath 正确，那么将会写入内容。
	/// </summary>
	/// <param name="msg">消息</param>
	static void errorQDebugOut( const QString &msg );
private: // - 类型
	template< class TKey, class TValue >
	using UN_Map_Iterator_Template = std::unordered_map< TKey, TValue >;
	using QString_Shared = std::shared_ptr< QString >;
	using OStream_Ptr = OStream *;
	using UN_Map_Iterator = UN_Map_Iterator_Template< QString_Shared, OStream_Ptr >::iterator;
private: // 静态成员
	static UN_Map_Iterator_Template< QString_Shared, OStream_Ptr > defaultOStreamMap;
public:
	static bool hasDefaultOStream( const QString &key ) {
		auto end = defaultOStreamMap.end( );
		auto first = defaultOStreamMap.begin( );
		for( ; first != end; ++first )
			if( *first->first == key )
				return true;
		return false;
	}
	static void setDefaultOStream( const QString &key, OStream *default_OStream ) {
		auto end = defaultOStreamMap.end( );
		auto first = defaultOStreamMap.begin( );
		for( ; first != end; ++first )
			if( *first->first == key ) {
				first->second = default_OStream;
				return;
			}
		defaultOStreamMap.emplace( std::make_shared< QString >( key ), default_OStream );
	}

	static OStream * getDefaultOStream( const QString &key ) {
		auto end = defaultOStreamMap.end( );
		auto first = defaultOStreamMap.begin( );
		for( ; first != end; ++first )
			if( *first->first == key )
				return first->second;
		return nullptr;
	}
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
