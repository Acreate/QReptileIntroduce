#ifndef INSTANCE_FUNCTION_H_H_HEAD__FILE__
#define INSTANCE_FUNCTION_H_H_HEAD__FILE__
#pragma once

#include "../auto_generate_files/macro/cmake_to_c_cpp_header_env.h"

#include "stream/OStream.h"
#include <QDateTime>
#include <QDir>
#include <QUrl>

namespace instance_function {
	struct NovelNodeXPathInfo;

	/// @brief 获取基于 cmakefile 的根路径相对的文件路径
	/// @param builder_path 编译文件的路径
	/// @return 基于 cmake 根路径的相对路径
	static QString getCmakeRootPathBuilderFilePath( const QString &builder_path ) {
		return QDir( Cmake_Source_Dir ).relativeFilePath( builder_path );
	}
	/// @brief 获取基于 cmakefile 的根路径相对的文件路径
	/// @param builder_path 编译文件的路径
	/// @return 基于 cmake 根路径的相对路径
	static QString getCmakeRootPathBuilderFilePath( const char *builder_path ) {
		return QDir( Cmake_Source_Dir ).relativeFilePath( builder_path );
	}
	enum Novel_Xpath_Analysis_Error {
		None// 没有异常
		, Name_Error_Xpath// 名称 xpath 异常
		, Name_Error_None// 名称找不到异常
		, Url_Error_Xpath// url xpath 异常
		, Url_Error_None // url 找不到异常
		, DateTime_Error_Xpath// 时间 xpath 异常
		, DateTime_Error_None// 时间 找不到异常
		, DateTime_Error_Expire// 时间 超出限定
	};
	/// <summary>
	/// 获取错误内容
	/// </summary>
	/// <param name="quitMsg">错误代码</param>
	/// <param name="xpath">错误 xpath</param>
	/// <returns>错误内容</returns>
	inline QString get_error_info( instance_function::Novel_Xpath_Analysis_Error quitMsg, const QString &xpath ) {
		switch( quitMsg ) {
			case instance_function::Novel_Xpath_Analysis_Error::DateTime_Error_Expire :
				return u8" DateTime_Error_Expire 异常 ( 过期 )";
			case instance_function::Novel_Xpath_Analysis_Error::DateTime_Error_None :
				return u8" DateTime_Error_None 异常 ( 日期找不到 xpath : \"" + xpath + "\")";
			case instance_function::Novel_Xpath_Analysis_Error::DateTime_Error_Xpath :
				return u8" DateTime_Error_Xpath 异常 ( 日期 xpath 错误,xpath : \"" + xpath + "\" )";
			case instance_function::Novel_Xpath_Analysis_Error::Name_Error_None :
				return u8" Name_Error_None 异常 ( 名称错误, xpath : \"" + xpath + "\" )";
			case instance_function::Novel_Xpath_Analysis_Error::Name_Error_Xpath :
				return u8" Name_Error_Xpath 异常 ( 名称 xpath 错误, xpath : \"" + xpath + "\" )";
			case instance_function::Novel_Xpath_Analysis_Error::Url_Error_Xpath :
				return u8" Url_Error_Xpath 异常 ( url xpath 错误, xpath : \"" + xpath + "\" )";
			case instance_function::Novel_Xpath_Analysis_Error::Url_Error_None :
				return u8" Url_Error_Xpath 异常 ( url 错误, xpath : \"" + xpath + "\" )";
			case instance_function::Novel_Xpath_Analysis_Error::None :
				return "";
			default :
				return "none";
		}
	}
	/// <summary>
	/// 输出错误信息到指定文件
	/// </summary>
	/// <param name="os">流，可选为nullptr</param>
	/// <param name="url">错误的 url - 节选 host 为路径名</param>
	/// <param name="root_path">根路径</param>
	/// <param name="dir_name">存储目录-中间目录，写入 路径宏（Cache_Path_Dir）跟后</param>
	/// <param name="error_type">错误的类型 - 节点路径名</param>
	/// <param name="error_novel_type_name">错误的小说类型名称 - 节选路径名</param>
	/// <param name="error_file_suffix">文件路径后缀</param>
	/// <param name="error_call_path_file_name">错误诞生的文件</param>
	/// <param name="error_file_call_function_name">错误诞生的描述调用函数</param>
	/// <param name="error_file_call_function_line">错误诞生的描述行号</param>
	/// <param name="error_info_text">错误信息</param>
	/// <param name="error_write_info_content">错误内容</param>
	/// <returns>成功写入文件返回 true</returns>
	inline bool write_error_info_file( OStream *os, const QUrl &url, const QString &root_path, const QString &dir_name, const QString &error_type, const QString &error_novel_type_name, const QString &error_file_suffix, const QString &error_call_path_file_name, const QString &error_file_call_function_name, const size_t error_file_call_function_line, const QString &error_info_text, const QString &error_write_info_content ) {
		QDateTime currentDateTime = QDateTime::currentDateTime( );
		QString day = currentDateTime.toString( "yyyy_MM_dd" );
		QString time = currentDateTime.toString( "hh mm ss" );

		QString msg;
		msg.append( "\n<!--" )
			.append( "\n=========================		try : info" )
			.append( u8"\n\t当前时间 : " ).append( day + " " + time )
			.append( u8"\n\t错误文件 : " ).append( instance_function::getCmakeRootPathBuilderFilePath( error_call_path_file_name ) )
			.append( u8"\n\t信息位置 : " ).append( QString::number( error_file_call_function_line ) )
			.append( u8"\n\t信息函数 : " ).append( error_file_call_function_name )
			.append( "\n=========================		try : message" )
			.append( u8"\n\t类型 : " ).append( u8"请求页面" ).append( u8"(" ).append( url.toString( ) ).append( ")" )
			.append( "\n=========================		user : message" )
			.append( u8"\n\t自由信息 : " ).append( error_info_text )
			.append( "\n=========================" )
			.append( "\n-->" );
		OStream::anyStdCOut( msg, os );
		auto path = QString( root_path )
					.append( QDir::separator( ) )
					.append( day ).append( QDir::separator( ) )
					.append( dir_name ).append( QDir::separator( ) )
					.append( url.host( ) ).append( QDir::separator( ) )
					.append( error_type )
					.append( '-' )
					.append( time )
					.append( '-' )
					.append( error_novel_type_name ).append( error_file_suffix );
		QFileInfo fileInfo( path );
		auto dir = fileInfo.dir( );
		if( !dir.exists( ) )
			if( !dir.mkpath( dir.absolutePath( ) ) )
				return false;
		QFile writeHtmlFile( fileInfo.absoluteFilePath( ) );
		if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
			msg.append( '\n' );
			msg.append( error_write_info_content );
			writeHtmlFile.write( msg.toUtf8( ) );
			writeHtmlFile.close( );
		}
		return true;
	}
	namespace error {
		namespace root {
			namespace xpath {
				/// @brief 根页面请求返回为空内容
				/// @param request_url 请求链接
				/// @param outLogPath 输出路径
				/// @param html_text 解析的 html 文件内容
				/// @param call_code_file 错误文件路径名称
				/// @param call_function_name 调用函数名称
				/// @param line 调用所在行
				/// @param oStream 输出流-可选
				inline void write_error_info_file( const QString &request_url, const QString &outLogPath, const QString &html_text, const QString &call_code_file, const QString &call_function_name, decltype(__LINE__) line, OStream *oStream = nullptr ) {
					auto msg = QString( "%1 : %2" ).arg( request_url ).arg( QString( u8"xpath 异常，登出" ) );
					QUrl url( request_url );
					instance_function::write_error_info_file( oStream, url, outLogPath, "html_doc", "parse", u8"root", ".html", call_code_file, call_function_name, line, msg, html_text );
					OStream::anyStdCerr( msg, call_code_file, line, call_function_name, oStream );
				}
			}
			namespace htmlDoc {
				/// @brief 根页面请求返回为空内容
				/// @param request_url 请求链接
				/// @param outLogPath 输出路径
				/// @param call_code_file 错误文件路径名称
				/// @param call_function_name 调用函数名称
				/// @param line 调用所在行
				/// @param oStream 输出流-可选
				inline void write_error_info_file( const QString &request_url, const QString &outLogPath, const QString &call_code_file, const QString &call_function_name, decltype(__LINE__) line, OStream *oStream = nullptr ) {
					auto msg = QString( "%1 : %2" ).arg( request_url ).arg( QString( u8"获取页面为空，登出" ) );
					QUrl url( request_url );
					instance_function::write_error_info_file( oStream, url, outLogPath, "html_doc", "parse", u8"root", ".html", call_code_file, call_function_name, line, msg, "" );
					OStream::anyStdCerr( msg, call_code_file, line, call_function_name, oStream );
				}
			}
		}
		namespace type {
			namespace xpath {
				/// @brief 类型页面解析错误异常信息
				/// @param type_name 类型
				/// @param request_url 请求地址
				/// @param oStream 输出流-可选
				/// @param outLogPath 日志路径
				/// @param html_text html 文本
				/// @param call_code_file 调用函数文件路径名称
				/// @param call_function_name 调用函数名称
				/// @param line 调用行数
				inline void write_error_info_file( const QString &type_name, const QString &request_url, const QString &outLogPath, const QString &html_text, const QString &call_code_file, const QString &call_function_name, decltype(__LINE__) line, OStream *oStream = nullptr ) {
					auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( QString( u8"xpath 异常，登出" ) );
					QUrl url( request_url );
					instance_function::write_error_info_file( oStream, url, outLogPath, "xpath", "parse", type_name, ".html", call_code_file, call_function_name, line, msg, html_text );
				}
			}
			namespace htmlDoc {
				inline void write_error_info_file( const QString &type_name, const QString &request_url, const QString &outLogPath, const QString &html_text, const QString &call_code_file, const QString &call_function_name, decltype(__LINE__) line, OStream *oStream = nullptr ) {
					auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( QString( u8"文档为空" ) );
					QUrl url( request_url );
					instance_function::write_error_info_file( oStream, url, outLogPath, "html_doc", "parse", type_name, ".html", instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__, msg, "" );
				}
			}

		}


	}

}
#endif // INSTANCE_FUNCTION_H_H_HEAD__FILE__
