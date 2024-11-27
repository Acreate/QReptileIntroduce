#ifndef FUNCTION_H_H_HEAD__FILE__
#define FUNCTION_H_H_HEAD__FILE__
#pragma once
#include <iostream>
#include <qcoreapplication.h>
#include <QDateTime>
#include <QFileInfo>
#include <QMutexLocker>
#include <QString>
#include <QDir>

#include "argParser/ArgParser.h"
#include "htmls/htmlTools/HtmlWorkThread/HtmlWorkThread.h"
#include "interface/INovelInfo.h"
#include "nameSpace/cylHtmlTools.h"
#include "nameSpace/interfacePlugsType.h"
#include "path/Path.h"

/// <summary>
/// 根据源目标的文件内容删除目标文件的内容（关键字为一行删除）
/// </summary>
/// <param name="source_file_s">源列表</param>
/// <param name="des_file_s">目标列表</param>
bool removeFileSourceFilesKeys( const std::vector< std::string > &source_file_s, const std::vector< std::string > &des_file_s );

/// <summary>
/// 获取编译信息
/// </summary>
/// <returns>编译信息</returns>
QString getBuilderInfo( );
/// <summary>
/// std::string 转 std::wstring
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
std::wstring conver( const std::string &str );
/// <summary>
/// 从文件读取关键字
/// </summary>
/// <param name="paths">路径</param>
/// <param name="spls_list">切分列表</param>
/// <returns>关键字列表</returns>
std::vector< QString > readIngoreNameFiles( const std::vector< cylStd::ArgParser::String > &paths, const std::vector< QString > &spls_list );

inline QString & removeAllSpace( QString &str ) {
	size_t leftLen = str.length( );
	QString::value_type *buff = new QString::value_type [ leftLen ];
	size_t index = 0, buffIndex = 0;
	for( ; index < leftLen; ++index ) {
		auto value = str.at( index );
		if( value.isSpace( ) || !value.isPrint( ) )
			continue;
		buff[ buffIndex ] = value;
		++buffIndex;
	}
	str = QString( buff, buffIndex );
	delete[] buff;
	return str;
}
/// <summary>
/// 读取一个文件，一行为一个关键字
/// </summary>
/// <param name="path">文件路径</param>
/// <param name="spls_list">切分列表</param>
/// <returns>返回所有关键字</returns>
std::vector< QString > readIngoreNameFile( const QString &path, const std::vector< QString > &spls_list );
/// <summary>
/// 写入关键字，一行为一个关键字
/// </summary>
/// <param name="path">写入路径</param>
/// <param name="content">写入关键字列表</param>
/// <returns>写入数量</returns>
qsizetype writeIngoreNameFile( const QString &path, const std::vector< QString > &content );
/// <summary>
/// 调整关键字，存在子字符串则不需要源字符串(a -> aa / 其中只保留 a，不保留 aa)
/// </summary>
/// <param name="str_vector">匹配的字符串</param>
/// <returns>完成匹配的字符串</returns>
std::vector< QString > vectorStrAdjustSubStr( std::vector< QString > &str_vector );
/// <summary>
/// 调整关键字，存在子字符串则不需要源字符串(a -> aa / 其中只保留 a，不保留 aa)
/// </summary>
/// <param name="str_vector">匹配的字符串</param>
/// <returns>完成匹配的字符串</returns>
std::vector< std::wstring > vectorStrAdjustSubStr( std::vector< std::wstring > &str_vector );
/// <summary>
/// 去掉重复字符串
/// </summary>
/// <param name="str_vector">去除字符串</param>
/// <returns>唯一的元素</returns>
std::vector< QString > vectorStrduplicate( const std::vector< QString > &str_vector );
/// <summary>
/// 字符串列表排序-名称
/// </summary>
/// <param name="str_vector">排序列表</param>
/// <returns>排序结果</returns>
std::vector< QString > vectorStrSort( std::vector< QString > &str_vector );
/// <summary>
/// 字符串列表排序-长度
/// </summary>
/// <param name="str_vector">排序列表</param>
/// <returns>排序结果</returns>
std::vector< QString > vectorStrLenSort( std::vector< QString > &str_vector );
/// <summary>
/// 数组转换到以长度为 key 的映射列表 - QString 版本
/// </summary>
/// <param name="str_vector">转换数组</param>
/// <returns>长度 map</returns>
std::unordered_map< size_t, std::shared_ptr< std::vector< QString > > > vectorStrToLenKeyMap( const std::vector< QString > &str_vector );
/// <summary>
/// 数组转换到以长度为 key 的映射列表 - wstring 版本
/// </summary>
/// <param name="str_vector">转换数组</param>
/// <returns>长度 map</returns>
std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > > vectorStrToLenKeyMap( const std::vector< std::wstring > &str_vector );
/// <summary>
/// 字符串转换到宽字符
/// </summary>
/// <param name="str_vector">转换的字符串</param>
/// <returns>宽字符列表</returns>
std::vector< std::wstring > converToWString( std::vector< QString > &str_vector );

/// <summary>
/// 检测元素是否存在列表当中
/// </summary>
/// <typeparam name="t_value_type">列表元素类型</typeparam>
/// <param name="vector">检测列表</param>
/// <param name="value">检测值</param>
/// <returns>存在返回 true</returns>
template< class t_value_type >
inline bool findVector( const std::vector< t_value_type > &vector, const t_value_type &value ) {
	auto begin = vector.begin( );
	auto end = vector.end( );
	for( ; begin != end; ++begin )
		if( value == *begin )
			return true;
	return false;
}

#define ErrorCout_MACRO( msg )  errorCout((msg),  __FILE__, __FUNCTION__, __LINE__)
inline void errorCout( const std::string &msg, const std::string &erro_file, const std::string &error_call, const size_t error_line ) {
	std::cerr << "\n===============\n\t错误文件: " << erro_file << u8"\n\t调用名称: " << error_call << u8"\n\t信息行数: " << error_line << u8"\n\t错误信息: " << msg << "\n===============" << std::endl;
}
inline void errorCout( const char* msg, const std::string &erro_file, const std::string &error_call, const size_t error_line ) {
	std::cerr << "\n===============\n\t错误文件: " << erro_file << u8"\n\t调用名称: " << error_call << u8"\n\t信息行数: " << error_line << u8"\n\t错误信息: " << msg << "\n===============" << std::endl;
}
inline void errorCout( const QString &msg, const QString &erro_file, const QString &error_call, const size_t error_line ) {
	errorCout( msg.toStdString( ), erro_file.toStdString( ), error_call.toStdString( ), error_line );
}
using LenMap = std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > >;
using FileLenMap = std::unordered_map< QString, LenMap >;
/// <summary>
/// 加载查找文件的配置
/// </summary>
/// <param name="find_key_option">关键字选项</param>
/// <param name="find_key_files_option">关键字文件选项</param>
/// <param name="app_name">app 名称，用于汇总命令行提供的查找关键字到该文件当中</param>
/// <param name="result_file_name_map">文件映射长度</param>
void loadFindKeyFiles( const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &find_key_option, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &find_key_files_option, QString &app_name, FileLenMap &result_file_name_map );
/// <summary>
/// 加载完全匹配忽略名称的配置
/// </summary>
/// <param name="ignore_equ_key_option">关键字选项</param>
/// <param name="ignore_equ_key_files_option">关键字文件选项</param>
/// <param name="result_map">返回线程工作完成被赋值的映射</param>
void loadingEquKeyFiles( const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &ignore_equ_key_option, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &ignore_equ_key_files_option, LenMap &result_map );

/// <summary>
/// 加载子字符串匹配忽略名称的配置
/// </summary>
/// <param name="ignore_sub_key_option">关键字选项</param>
/// <param name="ignore_sub_key_files_option">关键字文件选项</param>
/// <param name="result_map">返回线程工作完成被赋值的映射</param>
void loadingSubKeyFiles( const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &ignore_sub_key_option, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &ignore_sub_key_files_option, LenMap &result_map );

/// <summary>
/// 等待工作线程完成，线程需要在准备状态，而不是在工作状态
/// </summary>
/// <param name="runCount">线程运行数量</param>
/// <param name="threads">线程队列</param>
/// <param name="qMutex">线程锁</param>
/// <param name="call_function">每轮查询都会调用该函数一次</param>
inline void waitThreadOverJob( size_t runCount, std::vector< cylHtmlTools::HtmlWorkThread * > &threads, QMutex *qMutex, const std::function< void( ) > &call_function ) {
	auto currentTime = std::chrono::system_clock::now( );
	size_t count = 0;
	if( runCount == 0 )
		runCount = 8;
	auto minRunCount = runCount - 1;
	do {
		size_t size = threads.size( );
		auto begin = threads.begin( );
		auto end = threads.end( );
		if( begin == end )
			return;
		while( count < runCount && size > minRunCount ) {
			cylHtmlTools::HtmlWorkThread *writeThread = *begin;
			if( writeThread->isInit( ) ) {
				writeThread->start( );
				++count;
			}
			++begin;
		}
		do {
			if( begin == end )
				break;
			cylHtmlTools::HtmlWorkThread *writeThread = *begin;
			if( writeThread->isFinish( ) ) {
				threads.erase( begin );
				--count;
				delete writeThread;
				break;
			}
			if( call_function )
				call_function( );
			QMutexLocker lock( qMutex );
			auto cur = std::chrono::system_clock::now( );
			auto sep = cur - currentTime;
			auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
			++begin;
			if( second < 5 )
				continue;
			std::cout << u8"正在进行查找 => 剩余数量[" << size << ']' << std::endl;
			currentTime = cur;
		} while( true );
	} while( true );
}
/// <summary>
/// 运行一个子进程
/// </summary>
/// <param name="app_path">主进程路径</param>
/// <param name="qt_mutex">计数访问锁</param>
/// <param name="ref_count">计数器</param>
/// <param name="run_plug_path">运行插件路径</param>
/// <param name="ref_arg_parser_obj_sptr">arg 参数对象</param>
/// <param name="requet_type_name_options">获取类型选项</param>
/// <param name="out_path_values">输出路径值</param>
/// <param name="novel_plug_paths">小说路径值</param>
void runSubProcess( const QString &app_path, QMutex &qt_mutex, size_t &ref_count, const std::vector< QString > &run_plug_path, const std::shared_ptr< cylStd::ArgParser > &ref_arg_parser_obj_sptr, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &requet_type_name_options, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &out_path_values, const std::vector< QString > &novel_plug_paths );

/// <summary>
/// 输出插件的 url ，并且运行一个子进程
/// </summary>
/// <param name="app_path">主进程路径</param>
/// <param name="qt_mutex">计数访问锁</param>
/// <param name="ref_count">计数器</param>
/// <param name="run_plug_path">运行插件路径</param>
/// <param name="ref_arg_parser_obj_sptr">arg 参数对象</param>
/// <param name="requet_type_name_options">获取类型选项</param>
/// <param name="out_path_values">输出路径值</param>
/// <param name="novel_plug_paths">小说路径值</param>
void runPrintfUrlSubProcess( const QString &app_path, QMutex &qt_mutex, size_t &ref_count, const std::vector< QString > &run_plug_path, const std::shared_ptr< cylStd::ArgParser > &ref_arg_parser_obj_sptr, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &requet_type_name_options, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &out_path_values, const std::vector< QString > &novel_plug_paths );
/// <summary>
/// 在该进程运行最后一个插件，并且输出该插件的 url
/// </summary>
/// <param name="qt_mutex">计数锁</param>
/// <param name="ref_count">计数器</param>
/// <param name="path">输出路径</param>
/// <param name="run_plug_path">运行的插件路径表</param>
/// <param name="requet_type_name_options">获取的类型选项表</param>
/// <param name="requesTypeNameVector">获取的类型列表</param>
/// <param name="novel_plug_paths">指向的所有插件路径</param>
/// <param name="call_function_file_path">调用的文件调用</param>
/// <param name="call_function_name">调用的函数名称</param>
/// <param name="call_function_line">调用的行</param>
void loadPrintfUrlLastPlugOnThisProcess( QMutex &qt_mutex, size_t &ref_count, const QString &path, const std::vector< QString > &run_plug_path, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &requet_type_name_options, const std::vector< QString > &requesTypeNameVector, const std::vector< QString > &novel_plug_paths, const char *call_function_file_path, const char *call_function_name, const size_t &call_function_line );
/// <summary>
/// 在该进程运行最后一个插件
/// </summary>
/// <param name="qt_mutex">计数锁</param>
/// <param name="ref_count">计数器</param>
/// <param name="path">输出路径</param>
/// <param name="run_plug_path">运行的插件路径表</param>
/// <param name="requet_type_name_options">获取的类型选项表</param>
/// <param name="requesTypeNameVector">获取的类型列表</param>
/// <param name="novel_plug_paths">指向的所有插件路径</param>
/// <param name="call_function_file_path">调用的文件调用</param>
/// <param name="call_function_name">调用的函数名称</param>
/// <param name="call_function_line">调用的行</param>
void loadLastPlugOnThisProcess( QMutex &qt_mutex, size_t &ref_count, const QString &path, const std::vector< QString > &run_plug_path, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &requet_type_name_options, const std::vector< QString > &requesTypeNameVector, const std::vector< QString > &novel_plug_paths, const char *call_function_file_path, const char *call_function_name, const size_t &call_function_line );

/// <summary>
/// 空文件夹管理器
/// </summary>
class QPathManage {
	std::vector< QString > managePaths;
	std::vector< QString > ingPaths;
public:
	explicit QPathManage( const std::vector< QString > &paths )
		: managePaths( paths ) { }
	QPathManage( ) { }
	inline std::vector< QString > getSubstrateDir( const QString &path ) {
		std::vector< QString > result;

		QFileInfo info( path );
		if( info.isFile( ) )
			return result;
		QDir dirInfo( path );
		auto entryInfoList = dirInfo.entryInfoList( QDir::AllDirs | QDir::NoDotAndDotDot );
		QString filePath = info.absoluteFilePath( );
		if( entryInfoList.size( ) == 0 )
			return { filePath };
		for( auto &fileInfo : entryInfoList )
			if( fileInfo.isDir( ) ) {
				QString absoluteFilePath = fileInfo.absoluteFilePath( );
				auto substrateDir = getSubstrateDir( absoluteFilePath );
				if( substrateDir.size( ) > 0 )
					result.insert( result.begin( ), substrateDir.begin( ), substrateDir.end( ) );
			}
		return result;
	}
	/// <summary>
	/// 加入管理
	/// </summary>
	/// <param name="path">加入管理的路径</param>
	void appendPath( const QString &path ) {
		QFileInfo info( path );
		managePaths.emplace_back( info.absoluteFilePath( ) );
	}
	/// <summary>
	/// 忽略管理
	/// </summary>
	/// <param name="path">忽略管理的路径</param>
	void removePath( const QString &path ) {
		QFileInfo info( path );
		ingPaths.emplace_back( info.absoluteFilePath( ) );
	}
	/// <summary>
	/// 移除空路径（一般指空文件夹）
	/// </summary>
	/// <returns>被移除的空路径</returns>
	std::vector< QString > removeEmptyPtah( ) {
		std::vector< QString > removePath;
		std::vector< QString > checkPaths;
		for( auto &absPath : managePaths ) {
			auto substrateDir = getSubstrateDir( absPath );
			for( auto &appendAbsDirPath : substrateDir ) {
				auto iterator = checkPaths.begin( );
				auto end = checkPaths.end( );
				if( std::find_if( iterator
					, end
					, [&]( const QString &checkPath ) {
						if( checkPath == appendAbsDirPath )
							return true;
						return false;
					} ) == end )
					checkPaths.emplace_back( appendAbsDirPath );
			}
		}
		qsizetype manageSize = checkPaths.size( ) - 1;
		std::sort( checkPaths.begin( ), checkPaths.end( ) );
		while( manageSize > 0 ) {
			auto managePath = checkPaths[ manageSize ];
			--manageSize;

			QDir info( managePath );
			auto entryInfoList = info.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot );
			auto iterator = entryInfoList.begin( );
			auto end = entryInfoList.end( );

			auto ingPathVectorBegin = ingPaths.begin( );
			auto ingPathVectorEnd = ingPaths.end( );
			auto removePathVectorBegin = removePath.begin( );
			auto removePathVectorEnd = removePath.end( );
			bool isDeleteDir = true;
			for( ; iterator != end; ++iterator ) {
				QString absoluteFilePath = iterator->absoluteFilePath( );
				if( std::find_if( ingPathVectorBegin
					, ingPathVectorEnd
					, [&]( const QString &file_abs_path ) {
						if( absoluteFilePath == file_abs_path )
							return true;
						return false;
					} ) != ingPathVectorEnd ) {
					isDeleteDir = false;
					continue;
				}

				if( std::find_if( removePathVectorBegin
					, removePathVectorEnd
					, [&]( const QString &file_abs_path ) {
						if( absoluteFilePath == file_abs_path )
							return true;
						return false;
					} ) != removePathVectorEnd ) {
					isDeleteDir = false;
					continue;
				}

				Path::removePath( absoluteFilePath );
				removePath.emplace_back( absoluteFilePath );
			}

			if( isDeleteDir ) {
				info.removeRecursively( );
				removePath.emplace_back( managePath );
			}
		}
		return removePath;
	}
};

#endif // FUNCTION_H_H_HEAD__FILE__
