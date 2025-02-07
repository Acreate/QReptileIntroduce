﻿#ifndef FUNCTION_H_H_HEAD__FILE__
#define FUNCTION_H_H_HEAD__FILE__
#pragma once
#include "auto_generate_files/macro/cmake_to_c_cpp_header_env.h"
#include "argParser/ArgParser.h"
#include "novelNetJob/NovelNetJob.h"
#include "path/Dir.h"
#include "plug/LoadPlug.h"
#include <iostream>
#include <QProcess>
#include <ioFile/IOFile.h>
#include <QMutex>
#include <htmls/htmlTools/HtmlWorkThread/HtmlWorkThreadPool.h>
#include <htmls/htmlTools/HtmlWorkThread/HtmlWorkThread.h>
#include <qguiapplication.h>

#include "DB/DBTools.h"
#include "htmls/htmlDoc/Tools/HtmlDocTools.h"
#include "htmlString/HtmlStringTools.h"
#include "novelNetJob/NovelDBJob.h"
#include "path/Path.h"
#include <sstream>

#include "interface/instance_function.h"

#include "dateTime/DateTime.h"
/// @brief 路径管理对象
class PathManage {
	std::filesystem::path currentPath;
	std::vector< std::filesystem::path > updateFileVector;
	std::mutex objMutex;
public:
	PathManage( const std::filesystem::path &current_path )
		: currentPath( current_path ) { }
	PathManage( const PathManage &other )
		: currentPath( other.currentPath ),
		updateFileVector( other.updateFileVector ) { }
	PathManage( PathManage &&other ) noexcept
		: currentPath( std::move( other.currentPath ) ),
		updateFileVector( std::move( other.updateFileVector ) ) { }
	PathManage & operator=( const PathManage &other ) {
		if( this == &other )
			return *this;
		currentPath = other.currentPath;
		updateFileVector = other.updateFileVector;
		return *this;
	}
	PathManage & operator=( PathManage &&other ) noexcept {
		if( this == &other )
			return *this;
		currentPath = std::move( other.currentPath );
		updateFileVector = std::move( other.updateFileVector );
		return *this;
	}
	std::filesystem::path getCurrentPath( ) const { return currentPath; }
	/// @brief 路径是否存在，不存在则返回 false
	/// @return 存在返回 true
	bool exists( ) const {
		return std::filesystem::exists( currentPath );
	}
	/// @brief 对象路径下是否不存在文件，不存在返回 true
	/// @return 不存在文件或者路径不存在则返回 true
	bool isEmptyDir( ) const {
		if( !std::filesystem::is_directory( currentPath ) )
			return false; // 是文件，或者不存在，返回 false
		if( !std::filesystem::exists( currentPath ) )
			return true;
		std::filesystem::recursive_directory_iterator directoryIterator( currentPath );
		std::filesystem::recursive_directory_iterator endIterator;
		for( ; directoryIterator != endIterator; ++directoryIterator )
			if( !directoryIterator->is_directory( ) )
				return false;
		return true;
	}
	/// @brief 添加更新路径-不存在或者是文件夹，则添加失败
	/// @param update_path 更新的路径
	/// @result 失败返回 false
	bool updateFilePath( const std::filesystem::path &update_path );
	/// @brief 返回旧文件
	/// @return 旧文件列表 
	std::vector< std::filesystem::path > getOldPath( );
	/// @brief 获取空文件夹-该路径下只存在文件夹，不存在文件的文件夹
	/// @return 空文件夹列表
	std::vector< std::filesystem::path > getEmptyDir( );
	std::vector< std::filesystem::path > getUpdateFileVector( );
	virtual ~PathManage( ) {
	}
};

class Time {
private:
	using Ratio = std::ratio< 1, 10000000 >;
	using Duration = std::chrono::duration< long long, Ratio >;
	using Clock = std::chrono::system_clock;
	using TimePoint = std::chrono::time_point< Clock >;
	TimePoint current_time;
public:
	Time( ) : Time( Clock::now( ) ) {
	}
	explicit Time( const TimePoint &current_time )
		: current_time( current_time ) { }

	template< class Conver_Duration_Type >
	Conver_Duration_Type toDuration( ) const {
		return std::chrono::duration_cast< Conver_Duration_Type >( current_time.time_since_epoch( ) );
	}
	/// @brief 格式化输出-utc 时间 \n
	/// %Y - xxxx 年 \n
	/// %m - xx 月 \n
	/// %d - xx 日 \n
	/// %H - xx 小时 \n
	/// %M - xx 分钟 \n
	/// %S - xx 秒	 \n
	/// @see std::put_time
	/// @param formatting_string 格式化字符串
	/// @return 格式化后的字符串
	std::string toFormattingUtcString( const std::string &formatting_string ) {
		std::time_t tt = Clock::to_time_t( current_time );
		std::tm tm = *std::gmtime( &tt ); //GMT (UTC)
		std::stringstream ss;
		ss << std::put_time( &tm, formatting_string.c_str( ) );
		return ss.str( );
	}
	/// @brief 格式化输出-本地时间 \n
	/// %Y - xxxx 年 \n
	/// %m - xx 月 \n
	/// %d - xx 日 \n
	/// %H - xx 小时 \n
	/// %M - xx 分钟 \n
	/// %S - xx 秒	 \n
	/// @see std::put_time
	/// @param formatting_string 格式化字符串
	/// @return 格式化后的字符串
	std::string toFormattingLocalString( const std::string &formatting_string ) {
		std::time_t tt = Clock::to_time_t( current_time );
		std::tm tm = *std::localtime( &tt ); //Locale time-zone, usually UTC by default.
		std::stringstream ss;
		ss << std::put_time( &tm, formatting_string.c_str( ) );
		return ss.str( );
	}

	/// @brief 获取时间点
	/// @return 时间点
	TimePoint getCurrentTime( ) const { return current_time; }
	/// @brief 设置新的时间点
	/// @param current_time 新的时间点
	void setCurrentTime( const TimePoint &current_time ) { this->current_time = current_time; }
	/// @brief 更新当前时间
	void updateTimeDate( ) {
		current_time = Clock::now( );
	}
	Time operator-( const Time &right ) {
		Duration duration = this->current_time - right.current_time;
		auto ms = std::chrono::duration_cast< std::chrono::milliseconds >( duration );
		return Time( TimePoint( ms ) );
	}
};

using NovelInfo_Shared = interfacePlugsType::INovelInfo_Shared; // 小说信息指针
using NovelInfoKeyPair = std::pair< NovelInfo_Shared, QStringList >; // 小说关键字映射
using NovelInfoKeyPairVector = std::vector< NovelInfoKeyPair >; // 多小说映射
using PathNovelPair = std::pair< QString, NovelInfoKeyPairVector >;
using PathWriteNovelInfoUnMap = std::vector< PathNovelPair >; // 多路径小说映射

extern const QString exportFindDirMidName; // 查找的中间名称 (-fkf 选项)

extern const QString exportAllDirMidName; // 全部导出的中间名称 (-edb 选项)

/// <summary>
/// 文件的 cmake 项目相对路径
/// </summary>
extern std::string callFileName;
/// <summary>
/// 输出间隔
/// </summary>
extern std::chrono::seconds duration;
extern std::string applicationPid; // app id
extern Time runTime; // app 起始运行时间
/// <summary>
/// 获取编译信息
/// </summary>
/// <returns>编译信息</returns>
QString getBuilderInfo( );
/// @brief 获取基于运行时间后的秒数
/// @return 秒
std::chrono::seconds getRunSepSec( );
/// @brief 获取基于运行时间后的毫秒
/// @return 毫秒
std::chrono::milliseconds getRunSepMill( );
/// @brief 获取基于运行时间后的分钟
/// @return 毫秒
std::chrono::minutes getRunSepMin( );
/// @brief 获取基于运行时间的偏移秒钟与毫秒
/// @param sec 返回的秒
/// @param mill 返回的毫秒
/// @return 失败返回 false
bool getRunSepSecAndMill( size_t &sec, size_t &mill );
/// @brief 获取分钟、秒钟与毫秒
/// @param min 分钟
/// @param sec 秒钟
/// @param mill 毫秒
/// @return 失败返回 false
bool getRunSepSecAndMillAndMin( size_t &min, size_t &sec, size_t &mill );
/// @brief 获取基于运行时间的偏移时、分、秒以及毫秒
/// @param hour 时
/// @param min 分
/// @param sec 秒
/// @param mill 毫秒
/// @return 失败返回false
bool getRunSepSecAndMillAndMinAndHour( size_t &hour, size_t &min, size_t &sec, size_t &mill );
/// @brief 获取基于运行时间便宜时间的字符串
/// @return 字符串
std::string getRunSepSecAndMillAndMinAndHourToStdString( );
/// @brief 删除导出目录（-w 指定生成的 exis_legitimate_out_dir_path 路径对象）
/// @param exis_legitimate_out_dir_path 起始路径列表
/// @param stdCoutMutex 输出锁
/// @param callFunctionName 调用函数名称
/// @param line 调用行号
void rmoveExportPath( const std::vector< QString > &exis_legitimate_out_dir_path, QMutex &stdCoutMutex, const std::string &callFunctionName, const size_t &line );

/// <summary>
/// 数据库操作
/// </summary>
/// <param name="arg_parser">参数列表解析对象</param>
void dbReadWriteChanger( const std::shared_ptr< cylStd::ArgParser > &arg_parser );

/// <summary>
/// 调整关键字，存在子字符串则不需要源字符串(a -> aa / 其中只保留 a，不保留 aa)
/// </summary>
/// <param name="str_vector">匹配的字符串</param>
/// <returns>完成匹配的字符串</returns>
std::vector< QString > vectorStrAdjustSubStr( const std::vector< QString > &str_vector );
/// <summary>
/// 去掉重复字符串
/// </summary>
/// <param name="str_vector">去除字符串</param>
/// <returns>唯一的元素</returns>
std::vector< QString > vectorStrDuplicate( const std::vector< QString > str_vector );
/// <summary>
/// 字符串列表排序-名称
/// </summary>
/// <param name="str_vector">排序列表</param>
/// <param name="isGreater">是否字符定义末尾到字符定义起始</param>
/// <returns>排序结果</returns>
std::vector< QString > & vectorStrSort( std::vector< QString > &str_vector, bool isGreater = false );
/// <summary>
/// 字符串列表排序-长度
/// </summary>
/// <param name="str_vector">排序列表</param>
/// <param name="isGreater">是否长度最长到最短</param>
/// <returns>排序结果</returns>
std::vector< QString > & vectorStrLenSort( std::vector< QString > &str_vector, bool isGreater = false );

/// <summary>
/// 写入拼接迭代器后的内容到文件
/// </summary>
/// <param name="write_file_path">写入文件的路径</param>
/// <param name="qt_str_vector">被拼接的迭代器</param>
/// <param name="jion">拼接字符串</param>
/// <returns>写入内容</returns>
QString writeJionStringVector( const QString &write_file_path, const std::vector< QString > &qt_str_vector, const QString &jion );

/// <summary>
/// 显示 app 名称
/// </summary>
/// <param name="args">选项解析对象</param>
void showAppName( std::shared_ptr< cylStd::ArgParser > &args );
/// <summary>
/// 显示版本与命令行参数信息
/// </summary>
/// <param name="args">选项解析对象</param>
void showVersion( std::shared_ptr< cylStd::ArgParser > &args );
/// <summary>
/// 显示帮助信息
/// </summary>
void showHelp( );
/// <summary>
/// 输出帮助信息
/// </summary>
/// <param name="args">选项解析对象</param>
void showHelp( std::shared_ptr< cylStd::ArgParser > &args );

/// <summary>
/// 获取路径选项的全部文件
/// </summary>
/// <param name="args">选项对象</param>
/// <param name="option">选项名称</param>
/// <returns>存在的文件列表</returns>
std::vector< QString > getFilePathsOptionPaths( const std::shared_ptr< cylStd::ArgParser > &args, const cylStd::ArgParser::String &option );
/// <summary>
/// 获取首个非文件地址。（路径不存在，则返回该地址）
/// </summary>
/// <param name="args">选项对象</param>
/// <param name="option">选项名称</param>
/// <returns>首个文件夹地址</returns>
QString getFirstDirPathsOptionPath( std::shared_ptr< cylStd::ArgParser > &args, const cylStd::ArgParser::String &option );

/// <summary>
/// 获取文本文件
/// </summary>
/// <param name="filePaths">获取文件列表</param>
/// <returns>路径与文本文件内容的映射</returns>
std::unordered_map< QString, QString > getFileText( const std::vector< QString > &filePaths );

/// <summary>
/// 对一个字符串进行多次切分
/// </summary>
/// <param name="split_str_obj">切分对象</param>
/// <param name="split_str_key_s">切分的匹配字符串</param>
/// <returns>切分后的字符串列表</returns>
std::vector< QString > splitMultipleStrList( const QString &split_str_obj, const std::vector< QString > &split_str_key_s );

/// <summary>
/// 输出一个工作任务信息
/// </summary>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="call_function_name">调用函数名称</param>
/// <param name="work_msg">输出消息</param>
/// <param name="file_name">调用文件名</param>
/// <param name="line">调用行号</param>
void outStdCountStreamMsg( QMutex &std_cout_mutex, const std::string &call_function_name, const std::string &work_msg, const std::string &file_name, const size_t line );

/// <summary>
/// 获取文本文件，并且切分到列表
/// </summary>
/// <param name="file_paths">获取文件列表</param>
/// <param name="split_vector">字符串切分关键字</param>
/// <param name="is_remove_space">是否删除关键字空格</param>
/// <returns>路径与文本文件内容的映射</returns>
std::vector< QString > getFileTextSplit( const std::vector< QString > &file_paths, const std::vector< QString > &split_vector, bool is_remove_space = false );

/// <summary>
/// 获取文件的基本名称
/// </summary>
/// <param name="filePathInfo">文件路径信息</param>
/// <returns>文件的基本名称</returns>
QString getFileBaseName( const QFileInfo &filePathInfo );

/// <summary>
/// 在子进程运行一个爬虫插件
/// </summary>
/// <param name="app_path">子进程文件路径</param>
/// <param name="qt_mutex">进程锁</param>
/// <param name="ref_count">引用计数</param>
/// <param name="novel_plug_paths">插件路径</param>
/// <param name="request_type_name_options">请求名称选项参数</param>
/// <param name="is_nsts_option">是否使用输出开始运行时间选项 - false 表示不输出</param>
/// <param name="is_nste_option">是否使用输出结束运行时间选项 - false 表示不输出</param>
/// <param name="is_nsnstr_option">是否使用输出运行市场 - false 表示不输出</param>
/// <param name="call_function_file_path">调用该函数的文件路径-错误时使用</param>
/// <param name="call_function_name">调用该函数的函数名称-错误时使用</param>
/// <param name="call_function_line">调用该函数的行数-错误时使用</param>
/// <param name="out_path_values">请求完成路径选项</param>
std::vector< QProcess * > runPluginToSubProcess( const QString &app_path, QMutex &qt_mutex, size_t &ref_count, const std::vector< QString > &novel_plug_paths, const QString &request_type_name_options, bool is_nsts_option, bool is_nste_option, bool is_nsnstr_option, const QString &out_path_values, const std::string &call_function_file_path, const char *call_function_name, const size_t &call_function_line );
/// <summary>
/// 在当前进程中运行爬虫
/// </summary>
/// <param name="qt_mutex">进程锁</param>
/// <param name="ref_count">引用计数</param>
/// <param name="out_path">输出路径</param>
/// <param name="reques_type_names_vector">请求类型名称列表</param>
/// <param name="novel_plug_path">小说爬虫插件文件路径</param>
/// <param name="call_function_file_path">调用该函数的文件路径-错误时使用</param>
/// <param name="call_function_name">调用该函数的函数名称-错误时使用</param>
/// <param name="call_function_line">调用该函数的行数-错误时使用</param>
/// <returns>返回插件对象</returns>
NovelNetJob * runPluginToThisProcess( QMutex &qt_mutex, size_t &ref_count, const QString &out_path, const std::vector< QString > &reques_type_names_vector, const QString &novel_plug_path, const std::string &call_function_file_path, const char *call_function_name, const size_t &call_function_line );

/// <summary>
/// 运行小说请求插件
/// </summary>
/// <param name="args">选项解析对象</param>
void runRequestDownloadPlugs( std::shared_ptr< cylStd::ArgParser > &args );

/// <summary>
/// 内容写入文件
/// </summary>
/// <param name="file_path">文件路径</param>
/// <param name="open_mode">打开模式</param>
/// <param name="write_content_vector">写入内容</param>
/// <param name="jion_str">拼接字符串</param>
/// <returns>写入个数</returns>
qint64 writeFile( const QString &file_path, const QFileDevice::OpenMode open_mode, const std::vector< QString > &write_content_vector, const QString &jion_str );

/// <summary>
/// 内容写入文件
/// </summary>
/// <param name="file_path">文件路径</param>
/// <param name="write_content_vector">写入内容</param>
/// <param name="jion_str">拼接字符串</param>
/// <returns>写入个数</returns>
inline qint64 writeFile( const QString &file_path, const std::vector< QString > &write_content_vector, const QString &jion_str ) {
	return writeFile( file_path, QIODeviceBase::ReadWrite | QIODeviceBase::Truncate, write_content_vector, jion_str );
}

/// @brief 移除字符串所有空字符
/// @param str 移除的字符串对象
/// @return 返回移除后的字符串对象
QString & removeAllSpace( QString &str );

/// <summary>
/// 对文件映射当中的文件内容进行切分，并且整理到新的映射当中 <br/>
/// 旧映射 => <文件路径,文件内容> <br/>
/// 新映射 => <文件路径,切分后的内容> 
/// </summary>
/// <param name="map">文件映射</param>
/// <param name="split_str_key">切分关键字</param>
/// <param name="isAdjustSubStr">是否需要取出子字符串</param>
/// <returns>新映射</returns>
std::unordered_map< QString, std::vector< QString > > getPairsKey( const std::unordered_map< QString, QString > &map, const QString &split_str_key, const bool isAdjustSubStr );

/// <summary>
/// 删除在映射当中相等匹配的关键字
/// </summary>
/// <param name="dest_file_text_map_keys">映射</param>
/// <param name="source_equ_keys">相等匹配的关键字</param>
void unmapRemoveIfEquKeys( std::unordered_map< QString, std::vector< QString > > &dest_file_text_map_keys, const std::vector< QString > &source_equ_keys );
/// <summary>
/// 删除在映射当中段内匹配的关键字
/// </summary>
/// <param name="dest_file_text_map_keys">映射</param>
/// <param name="source_sub_keys">段内匹配的关键字</param>
void unmapRemoveIfSubKeys( std::unordered_map< QString, std::vector< QString > > &dest_file_text_map_keys, const std::vector< QString > &source_sub_keys );
/// @brief 用于检查关键字文件，并且处理完善查找关键字文件内容
/// @param arg_parser 参数列表解析对象
void checkFindKeYFiles( const std::shared_ptr< cylStd::ArgParser > &arg_parser );
/// <summary>
/// 用于处理查找配置关键字文件内容
/// </summary>
/// <param name="args">参数列表解析对象</param>
void checkKeyFile( std::shared_ptr< cylStd::ArgParser > &args );
/// <summary>
/// 获取存在的文件路径
/// </summary>
/// <param name="arg_parser">参数列表解析对象</param>
/// <param name="option">选项</param>
/// <param name="end_str">文件的后缀名称</param>
/// <returns>存在的文件路径</returns>
std::vector< QString > getOptionExisFilePath( const std::shared_ptr< cylStd::ArgParser > &arg_parser, const cylStd::ArgParser::String &option, QString end_str = "" );
/// <summary>
/// 检查合法的目录
/// </summary>
/// <param name="arg_parser">参数解析对象</param>
/// <param name="option">支持选项</param>
/// <returns>可使用的目录路径</returns>
std::vector< QString > getOptionLegitimateOutDirPath( const std::shared_ptr< cylStd::ArgParser > &arg_parser, const cylStd::ArgParser::String &option );

using Str_Vector = std::vector< interfacePlugsType::HtmlDocString >; // 字符串序列
using Str_Vector_S = std::shared_ptr< Str_Vector >; // 字符串序列对象指针
using Len_Pair = std::pair< size_t, Str_Vector_S >;
using Len_Map_Str_Vector_S = std::vector< Len_Pair >; // 长度匹配的字符串序列映射对象
using Len_Map_Str_Vector_S_Shared = std::shared_ptr< Len_Map_Str_Vector_S >; // 长度匹配的字符串序列映射对象指针

/// @brief 查找长度映射对象当中是否存在指定字符串
/// @param inster_map_obj_shared_ptr 查找的长度匹配映射对象
/// @param comp_str 查找的字符串
/// @return 存在返回 true
bool lenMapHasEquStr( const Len_Map_Str_Vector_S &inster_map_obj_shared_ptr, const interfacePlugsType::HtmlDocString &comp_str );

/// @brief comp_str 是否存在子字符串
/// @param inster_map_obj_shared_ptr 查找的长度匹配映射对象
/// @param comp_str 查找的字符串
/// @return 存在返回 true
bool lenMapHasSubStr( const Len_Map_Str_Vector_S &inster_map_obj_shared_ptr, const interfacePlugsType::HtmlDocString &comp_str );

/// @brief 序列字符串容器对象转换到长度匹配字符串容器映射
/// @param inster_map_obj 需要存储到的映射对象
/// @param vector 转换序列
/// @return 返回非空对象，但是存在 size() 为 0 的情况
void vectorConverToLenMap( Len_Map_Str_Vector_S &inster_map_obj, const std::vector< interfacePlugsType::HtmlDocString > &vector );

/// <summary>
/// 执行数据库读取功能
/// </summary>
/// <param name="db_paths">数据库路径-多个数据库</param>
/// <param name="std_cout_mutex">输出锁-使用标准输出时将会锁定</param>
/// <param name="inster_novel_vector_map_mutex">插入锁-进行小说插入 inster_novel_vector_map_obj 时将会锁定</param>
/// <param name="inster_novel_vector_map_obj">小说保存对象</param>
/// <param name="novel_inster_count">返回小说个数</param>
/// <param name="db_novel_count">数据库小说数量总统计</param>
/// <param name="expire">过期选项</param>
/// <param name="has_ex_option">是否存在过期功能</param>
/// <param name="jump_equ_name_len_map">跳过完全匹配的标题容器</param>
/// <param name="jump_sub_name_len_map">跳过段内匹配的标题容器</param>
/// <param name="is_db_path_ative">如果 db_paths 中存在有效的数据库，该值为 true</param>
/// <returns>正在执行任务的线程池</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getDBNovelsInfo( const std::vector< QString > &db_paths, QMutex &std_cout_mutex, QMutex &inster_novel_vector_map_mutex, NovelDBJob::NovelTypePairVector_Shared &inster_novel_vector_map_obj, size_t &novel_inster_count, size_t &db_novel_count, const size_t expire, bool has_ex_option, const Len_Map_Str_Vector_S &jump_equ_name_len_map, const Len_Map_Str_Vector_S &jump_sub_name_len_map, bool &is_db_path_ative );

/// <summary>
/// 执行获取关键字文件的任务
/// </summary>
/// <param name="arg_parser">命令行解析列表对象</param>
/// <param name="option">查找任务的命令行选项</param>
/// <param name="save_obj">关键字信息保存对象<文件路径, 关键字列表></param>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="inster_read_find_key_map_mutex">文件关键字读取锁</param>
/// <returns>任务线程池对象指针</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getFindKeyFileKeyToMap( const std::shared_ptr< cylStd::ArgParser > &arg_parser, const cylStd::ArgParser::String &option, std::unordered_map< QString, std::vector< interfacePlugsType::HtmlDocString > > &save_obj, QMutex &std_cout_mutex, QMutex &inster_read_find_key_map_mutex );
/// <summary>
/// 执行获取关键字文件的任务
/// </summary>
/// <param name="arg_parser">命令行解析列表对象</param>
/// <param name="option">查找任务的命令行选项</param>
/// <param name="save_obj">关键字信息保存对象-<关键字列表></param>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="inster_read_find_key_map_mutex">对象插入更改锁</param>
/// <returns>任务线程池对象指针</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getFindKeyFileKeyToVector( const std::shared_ptr< cylStd::ArgParser > &arg_parser, const cylStd::ArgParser::String &option, Len_Map_Str_Vector_S_Shared &save_obj, QMutex &std_cout_mutex, QMutex &inster_read_find_key_map_mutex );

/// <summary>
/// 存储一个写入对象
/// </summary>
/// <param name="write_path">写入路径</param>
/// <param name="write_novel_shared_ptr">写入对象</param>
/// <param name="novel_infos_write_map">写入映射列表<路径，写入对象列表></param>
/// <returns>成功返回 true</returns>
bool emplaceWriteMap( const QString &write_path, const interfacePlugsType::INovelInfo_Shared &write_novel_shared_ptr, NovelDBJob::NovelTypePairVector &novel_infos_write_map );
/// <summary>
/// 激活全导出功能线程池
/// </summary>
/// <param name="vector_db_novelinfo_pairt">小说列表映射序列</param>
/// <param name="novel_infos_write_map">匹配完成的写入列表映射</param>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="inster_novel_vectos_mutex">插入锁</param>
/// <returns>正在工作的线程池</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getDBFilterNovelInfo( const NovelDBJob::NovelTypePairVector_Shared &vector_db_novelinfo_pairt, NovelDBJob::NovelTypePairVector_Shared novel_infos_write_map, QMutex &std_cout_mutex, QMutex &inster_novel_vectos_mutex );
/// <summary>
/// 插入一个小说key
/// </summary>
/// <param name="save_path">小说保存路径</param>
/// <param name="novel_info">小说信息对象</param>
/// <param name="key">关键字</param>
/// <param name="merge_find_keys_novel_infos_map">插入对象</param>
void emplaceNovelKeyMap( const QString &save_path, const std::shared_ptr< interfacePlugsType::INovelInfo > &novel_info, const QString &key, PathWriteNovelInfoUnMap &merge_find_keys_novel_infos_map );

/// @brief 匹配一个key，成功返回该 key
/// @param novel_info 匹配的小说信息
/// @param find_key_map 查找的长度匹配字符串数组映射
/// @param key 返回的匹配key
/// @return 成功返回 true，并且 key 设置为 key 内容
bool lenMapFindNovelKey( const NovelInfo &novel_info, const Len_Map_Str_Vector_S &find_key_map, interfacePlugsType::HtmlDocString *key );

/// <summary>
/// 激活查找功能
/// </summary>
/// <param name="novel_type_pair_vector_shared">小说列表序列</param>
/// <param name="exis_find_file_path_map_key">查找关键字映射</param>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="merge_find_keys_types_novel_infos_map_mutex">操作 type 文件的关键字映射的锁</param>
/// <param name="merge_find_keys_types_novel_infos_map">存储 type 文件的关键字映射</param>
/// <param name="merge_find_keys_dbs_novel_infos_map_mutex">操作 db 文件的关键字映射的锁</param>
/// <param name="merge_find_keys_dbs_novel_infos_map">存储 db 文件的关键字映射></param>
/// <param name="merge_find_keys_export_key_novel_infos_map_mutex">操作 export_关键字 文件的关键字映射的锁</param>
/// <param name="merge_find_keys_export_key_novel_infos_map">存储 export_关键字 文件的关键字映射></param>
/// <param name="merge_find_keys_export_novel_infos_map_mutex">操作 export 文件的关键字映射的锁</param>
/// <param name="merge_find_keys_export_novel_infos_map">存储 export 文件的关键字映射</param>
/// <param name="inster_map_thread_pool_mutex">插入任务线程池锁</param>
/// <param name="inster_map_thread_pool">插入任务线程池对象</param>
/// <returns>任务执行线程池</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getDBFindNovelInfo(
	const NovelDBJob::NovelTypePairVector_Shared &novel_type_pair_vector_shared,
	const std::unordered_map< QString, std::vector< interfacePlugsType::HtmlDocString > > &exis_find_file_path_map_key,
	QMutex &std_cout_mutex,
	QMutex &merge_find_keys_types_novel_infos_map_mutex,
	PathWriteNovelInfoUnMap &merge_find_keys_types_novel_infos_map,
	QMutex &merge_find_keys_dbs_novel_infos_map_mutex,
	PathWriteNovelInfoUnMap &merge_find_keys_dbs_novel_infos_map,
	QMutex &merge_find_keys_export_key_novel_infos_map_mutex,
	PathWriteNovelInfoUnMap &merge_find_keys_export_key_novel_infos_map,
	QMutex &merge_find_keys_export_novel_infos_map_mutex,
	PathWriteNovelInfoUnMap &merge_find_keys_export_novel_infos_map,
	QMutex &inster_map_thread_pool_mutex,
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > &inster_map_thread_pool );

/// <summary>
/// 调整关键字，存在子字符串则不需要源字符串(a -> aa / 其中只保留 a，不保留 aa)
/// </summary>
/// <param name="str_vector">匹配的字符串</param>
/// <returns>完成匹配的字符串</returns>
std::vector< interfacePlugsType::HtmlDocString > vectorStrAdjustSubStr( const std::vector< interfacePlugsType::HtmlDocString > &str_vector );

/// @brief 分解多个小说信息字段信息
/// @param novelInfos 分解列表
void spliteAppendDataField( std::vector< NovelInfo_Shared > &novelInfos );

/// @brief 对导出列表进行排序
/// @param write_info_vector 导出的列表
void novelInfoExportSort( std::vector< NovelInfo_Shared > &write_info_vector );
/// <summary>
/// 写入查找关键字路径
/// </summary>
/// <param name="disk_mute">磁盘锁</param>
/// <param name="count_mute">计数锁</param>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="all_file_path_name_name">写入路径名称</param>
/// <param name="write_novel_count">写入计数</param>
/// <param name="infos">写入信息</param>
/// <param name="callFunctionName">调用名称</param>
void writeDisk( QMutex &disk_mute,
				QMutex &count_mute,
				QMutex &std_cout_mutex,
				const QString &all_file_path_name_name,
				size_t &write_novel_count,
				const NovelInfoKeyPairVector &infos,
				const std::string &callFunctionName );
/// <summary>
/// 文件小说写入磁盘
/// </summary>
/// <param name="disk_mute">磁盘读写锁</param>
/// <param name="count_mute">写入计数统计对象锁</param>
/// <param name="std_cout_mutex">标准输出锁</param>
/// <param name="all_file_path_name_name">写入路径</param>
/// <param name="write_novel_count">写入小说计数返回</param>
/// <param name="infos">小说列表</param>
/// <param name="callFunctionName">调用者-函数名称</param>
void writeDisk( QMutex &disk_mute,
				QMutex &count_mute,
				QMutex &std_cout_mutex,
				const QString &all_file_path_name_name,
				size_t &write_novel_count,
				const NovelDBJob::NovelInfoVector &infos,
				const std::string &callFunctionName );

/// <summary>
/// 执行写入磁盘任务
/// </summary>
/// <param name="novel_infos_write_map">写入映射</param>
/// <param name="exis_legitimate_out_dir_path">写入路径列表</param>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="disk_mute">磁盘文件、路径操作锁</param>
/// <param name="count_mute">计数锁</param>
/// <param name="write_novel_count">写入计数返回引用变量-不重计，直接累加</param>
/// <param name="path_manages">存储修改路径</param>
/// <returns>任务线程池对象</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > writeDiskInForNovels( const NovelDBJob::NovelTypePairVector_Shared &novel_infos_write_map, const std::vector< QString > &exis_legitimate_out_dir_path, QMutex &std_cout_mutex, QMutex &disk_mute, QMutex &count_mute, size_t &write_novel_count, std::vector< PathManage > &path_manages );
/// <summary>
/// 激活查找归纳任务
/// </summary>
/// <param name="exis_legitimate_out_dir_path">写入路径</param>
/// <param name="novel_keys_map">归纳映射-小说|匹配关键字列表</param>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="disk_mute">磁盘读写锁</param>
/// <param name="count_mute">写入计数锁</param>
/// <param name="write_novel_count">写入计数-不重计，直接累加</param>
/// <param name="path_manages">存储写入路径</param>
/// <returns>任务线程池</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > writeDiskInForInductionNovels(
	const std::vector< QString > &exis_legitimate_out_dir_path,
	const PathWriteNovelInfoUnMap &novel_keys_map,
	QMutex &std_cout_mutex,
	QMutex &disk_mute,
	QMutex &count_mute,
	size_t &write_novel_count,
	std::vector< PathManage > &path_manages );
/// @brief 显示运行时间
/// @param arg_parser 运行时参数对象
void showRunTime( const std::shared_ptr< cylStd::ArgParser > &arg_parser );
/// @brief 显示开始运行时间
/// @param arg_parser 运行时参数对象
void showStartRunTime( const std::shared_ptr< cylStd::ArgParser > &arg_parser );
/// @brief 显示结束运行时间
/// @param arg_parser 运行时参数对象
void showEndRunTime( const std::shared_ptr< cylStd::ArgParser > &arg_parser );
/// <summary>
/// 合并映射对象
/// </summary>
/// <param name="merge_find_keys_novelInfos_map">合并到该对象</param>
/// <param name="merge_find_keys_types_novel_infos_map">合并的类型映射</param>
/// <param name="merge_find_keys_dbs_novel_infos_map">合并的数据库映射</param>
/// <param name="merge_find_keys_export_key_novel_infos_map">合并的关键字映射</param>
/// <param name="merge_find_keys_export_novel_infos_map">合并的全体对象</param>
inline void mergeFindMaps( PathWriteNovelInfoUnMap &merge_find_keys_novelInfos_map, const PathWriteNovelInfoUnMap &merge_find_keys_types_novel_infos_map, const PathWriteNovelInfoUnMap &merge_find_keys_dbs_novel_infos_map, const PathWriteNovelInfoUnMap &merge_find_keys_export_key_novel_infos_map, const PathWriteNovelInfoUnMap &merge_find_keys_export_novel_infos_map ) {
	for( auto &iter : merge_find_keys_types_novel_infos_map )
		merge_find_keys_novelInfos_map.emplace_back( iter );
	for( auto &iter : merge_find_keys_dbs_novel_infos_map )
		merge_find_keys_novelInfos_map.emplace_back( iter );
	for( auto &iter : merge_find_keys_export_key_novel_infos_map )
		merge_find_keys_novelInfos_map.emplace_back( iter );
	for( auto &iter : merge_find_keys_export_novel_infos_map )
		merge_find_keys_novelInfos_map.emplace_back( iter );
}

/// @brief 查找子字符串的值
/// @param find_key 查找的整段字符串
/// @param key 段内匹配到的字符串
/// @param str_vector 子字符串容器
/// @return true 表示匹配到
inline bool getValue( interfacePlugsType::HtmlDocString find_key, interfacePlugsType::HtmlDocString *key, const Str_Vector &str_vector ) {
	cylHtmlTools::HtmlStringTools::removeAllSpace( find_key );
	for( auto &compStr : str_vector )
		if( cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &find_key, &compStr ) ) {
			*key = compStr;
			return true;
		}
	return false;
}

/// <summary>
/// 匹配一个名称
/// </summary>
/// <param name="name">判定名称</param>
/// <param name="equ_name_s">完全匹配名称映射对象</param>
/// <param name="sub_name_s">段内匹配名称映射对象</param>
/// <returns>匹配成功返回 true</returns>
inline bool filterNovelName( const interfacePlugsType::HtmlDocString &name, const Len_Map_Str_Vector_S &equ_name_s, const Len_Map_Str_Vector_S &sub_name_s ) {
	if( lenMapHasEquStr( equ_name_s, name ) )
		return true;
	if( lenMapHasSubStr( sub_name_s, name ) )
		return true;
	return false;
}

/// <summary>
/// 根据 . 实现判定程序后缀名，并且返回该后缀名
/// </summary>
/// <param name="file_path">文件路径</param>
/// <returns>文件后缀名</returns>
inline QString getFileEndStr( const QString &file_path ) {
	qsizetype lastIndexOf = file_path.lastIndexOf( "." );
	if( lastIndexOf == -1 )
		return "";
	QString mid = file_path.mid( lastIndexOf );
	return mid;
}

/// <summary>
/// 比较难字符串长度的函数
/// </summary>
/// <param name="left">左操作数</param>
/// <param name="right">右操作数</param>
/// <returns>长度比较结果</returns>
inline bool compQStringLen( const QString &left, const QString &right ) {
	return left.length( ) < right.length( );
}
/// <summary>
/// 比较难字符串本身的函数
/// </summary>
/// <param name="left">左操作数</param>
/// <param name="right">右操作数</param>
/// <returns>字符串本身比较结果</returns>
inline bool compQString( const QString &left, const QString &right ) {
	return left < right;
}

inline void errorCout( const std::string &msg, const std::string &erro_file, const std::string &error_call, const size_t error_line ) {
	size_t sec, mill;
	getRunSepSecAndMill( sec, mill );
	std::cerr << "\n===============\n\t错误文件: " << erro_file << u8"\n\t调用名称: " << error_call << u8"\n\t信息行数: " << error_line << u8"\n\t错误信息: " << msg << "\t: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << "\n===============" << std::endl;
}
inline void errorCout( const char *msg, const std::string &erro_file, const std::string &error_call, const size_t error_line ) {
	size_t sec, mill;
	getRunSepSecAndMill( sec, mill );
	std::cerr << "\n===============\n\t错误文件: " << erro_file << u8"\n\t调用名称: " << error_call << u8"\n\t信息行数: " << error_line << u8"\n\t错误信息: " << msg << "\t: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << "\n===============" << std::endl;
}
inline void errorCoutPath( const QString &msg, const QString &erro_file, const QString &error_call, const size_t error_line ) {
	errorCout( msg.toStdString( ), instance_function::getCmakeRootPathBuilderFilePath( erro_file ).toStdString( ), error_call.toStdString( ), error_line );
}
/// <summary>
/// 输出一个工作任务信息
/// </summary>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="mod_work_count">剩余工作数</param>
/// <param name="current_work_count">当前工作数</param>
/// <param name="call_function_name">调用函数名称</param>
/// <param name="work_msg">输出消息</param>
/// <param name="file_name">调用文件名</param>
/// <param name="line">调用行号</param>
inline void outStdCountStreamMsg( QMutex &std_cout_mutex, const size_t &mod_work_count, const size_t &current_work_count, const std::string &call_function_name, const std::string &work_msg, const std::string &file_name, const size_t line ) {
	std_cout_mutex.lock( );
	size_t sec, mill;
	getRunSepSecAndMill( sec, mill );
	std::stringstream msg;
	msg << u8"\n(进程 id : " << applicationPid << u8", 线程 id : " << std::this_thread::get_id( ) << u8" ) => [ " << call_function_name << u8" ] " << work_msg << u8" => 剩余工作数[" << mod_work_count << u8"]:正在工作数[" << current_work_count << u8"] << " << file_name << " : " << line;
	std::cout << msg.str( ) << "\t: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << std::endl;
	std_cout_mutex.unlock( );
}

/// <summary>
/// 获取文件的基本名称
/// </summary>
/// <param name="filePathInfo">文件路径信息</param>
/// <returns>文件的基本名称</returns>
inline QString getFileBaseName( const interfacePlugsType::HtmlDocString &filePathInfo ) {
	return getFileBaseName( QFileInfo( QString::fromStdWString( filePathInfo ) ) );
}

/// <summary>
/// 获取文件的基本名称
/// </summary>
/// <param name="filePathInfo">文件路径信息</param>
/// <returns>文件的基本名称</returns>
inline QString getFileBaseName( const QString &filePathInfo ) {
	return getFileBaseName( QFileInfo( filePathInfo ) );
}

/// @brief 向错误输出输出一个信息
/// @param msg 输出的错误信息
#define ErrorCout_MACRO( msg )  errorCoutPath((msg),  callFileName.c_str(  ) , __FUNCTION__, __LINE__)

/// @brief 使用错误输出输出信息
/// @param msg 输出的错误信息
/// @param function 输出的函数名称
#define ErrorCout_FunctionName_MACRO( msg , function )  errorCoutPath((msg),  callFileName.c_str() , function, __LINE__)

/// @brief 使用 std::cout 输出日志信息
/// @param Std_Cout_Mutex 输出锁
/// @param Mod_Work_Count 剩余工作数量
/// @param Current_Work_Count 正在工作数量
/// @param Call_Function_Name 调用函数名称
/// @param Msg 输出消息
#define Out_Std_Thread_Pool_Count_Stream_Msg_MACRO(Std_Cout_Mutex, Mod_Work_Count, Current_Work_Count, Call_Function_Name, Msg) \
		outStdCountStreamMsg( Std_Cout_Mutex, Mod_Work_Count, Current_Work_Count, Call_Function_Name, Msg, callFileName, __LINE__ )

/// @brief 使用 std::cout 输出日志信息
/// @param Std_Cout_Mutex 输出锁
/// @param Current_Work_Count 正在工作数量
/// @param Call_Function_Name 调用函数名称
/// @param Msg 输出消息
#define Out_Std_Count_Stream_Msg_MACRO(Std_Cout_Mutex,  Call_Function_Name, Msg) \
		outStdCountStreamMsg( Std_Cout_Mutex,  Call_Function_Name, Msg, callFileName, __LINE__ )

/// <summary>
/// 查找迭代器是否存在指定值，存在返回值
/// </summary>
/// <typeparam name="TValue">值类型</typeparam>
/// <param name="check_vector">检查迭代器对象</param>
/// <param name="check_key">检查值</param>
/// <param name="result_iterator">返回匹配的下标</param>
/// <returns>存在返回 true</returns>
template< typename TValue >
bool vectorFindValue( const std::vector< TValue > &check_vector, const TValue &check_key, std::_Vector_const_iterator< std::_Vector_val< std::_Simple_types< TValue > > > &result_iterator ) {
	if( check_vector.size( ) == 0 )
		return false;
	std::_Vector_const_iterator< std::_Vector_val< std::_Simple_types< TValue > > > iteratorBegin = check_vector.begin( );
	std::_Vector_const_iterator< std::_Vector_val< std::_Simple_types< TValue > > > end = check_vector.end( );
	result_iterator = std::find_if( iteratorBegin,
									end,
									[&] ( const TValue &it ) {
										if( check_key == it )
											return true;
										return false;
									} );
	return result_iterator != end;
}

/// <summary>
/// 查找迭代器是否存在指定值，存在返回值
/// </summary>
/// <typeparam name="TValue">值类型</typeparam>
/// <param name="check_vector">检查迭代器对象</param>
/// <param name="result_iterator">返回匹配的下标</param>
/// <param name="if_result_function">返回判定函数</param>
/// <returns>存在返回 true</returns>
template< typename TValue >
bool vectorFindValue( const std::vector< TValue > &check_vector, std::_Vector_const_iterator< std::_Vector_val< std::_Simple_types< TValue > > > &result_iterator, std::function< bool( const TValue & ) > if_result_function ) {
	if( check_vector.size( ) == 0 )
		return false;
	std::_Vector_const_iterator< std::_Vector_val< std::_Simple_types< TValue > > > iteratorBegin = check_vector.begin( );
	std::_Vector_const_iterator< std::_Vector_val< std::_Simple_types< TValue > > > end = check_vector.end( );
	result_iterator = std::find_if( iteratorBegin, end, if_result_function );
	return result_iterator != end;
}

/// <summary>
/// 检查迭代器是否存在指定值
/// </summary>
/// <typeparam name="TValue">值类型</typeparam>
/// <param name="check_vector">检查迭代器对象</param>
/// <param name="check_key">检查值</param>
/// <returns>存在返回 true</returns>
template< class TValue >
bool vectorHasValue( const std::vector< TValue > &check_vector, const TValue &check_key ) {
	if( check_vector.size( ) == 0 )
		return false;
	auto end = check_vector.end( );
	typename std::vector< TValue >::iterator result;
	if( vectorFindValue( check_vector, check_key, result ) )
		return true;
	return false;
}
#endif // FUNCTION_H_H_HEAD__FILE__
