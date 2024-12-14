#include "argParser/ArgParser.h"
#include "novelNetJob/NovelNetJob.h"
#include "path/Dir.h"
#include "plug/LoadPlug.h"
#include "./function.h"
#include <iostream>
#include <QProcess>
#include <ioFile/IOFile.h>
#include <QMutex>
#include <htmls/htmlTools/HtmlWorkThread/HtmlWorkThreadPool.h>
#include <htmls/htmlTools/HtmlWorkThread/HtmlWorkThread.h>

#include "DB/DBTools.h"
#include "htmls/htmlDoc/Tools/HtmlDocTools.h"
#include "htmlString/HtmlStringTools.h"
#include "novelNetJob/NovelDBJob.h"
#include "path/Path.h"
#include <sstream>

using NovelInfo_Shared = interfacePlugsType::INovelInfo_Shared; // 小说信息指针
using NovelInfoKeyPair = std::pair< NovelInfo_Shared, QStringList >; // 小说关键字映射
using NovelInfoKeyPairVector = std::vector< NovelInfoKeyPair >; // 多小说映射
using PathWriteNovelInfoUnMap = std::unordered_map< QString, NovelInfoKeyPairVector >; // 多路径小说映射

const QString exportFindDirMidName = u8"export_find"; // 查找的中间名称 (-fkf 选项)

const QString exportAllDirMidName = u8"export_all"; // 全部导出的中间名称 (-edb 选项)

qint64 applicationPid; // app id
/// <summary>
/// 文件的 cmake 项目相对路径
/// </summary>
static std::string callFileName = getCmakeRootPathBuilderFilePath( __FILE__ ).toStdString( );
/// <summary>
/// 输出间隔
/// </summary>
static std::chrono::seconds duration( 25 );
/// <summary>
/// 显示 app 名称
/// </summary>
/// <param name="args">选项解析对象</param>
void showAppName( std::shared_ptr< cylStd::ArgParser > &args ) {
	if( args->getOptionValues( "-name" ) )
		std::cout << QString( u8"小说爬虫命令行版本" ).toStdString( ).c_str( ) << std::endl;; // 程序标题
}
/// <summary>
/// 显示版本与命令行参数信息
/// </summary>
/// <param name="args">选项解析对象</param>
void showVersion( std::shared_ptr< cylStd::ArgParser > &args ) {
	QString compilerString = getBuilderInfo( );
	std::cout << "------------\n" u8"qt 版本 :" << qVersion( ) << u8"\n\t" u8"编译版本 :" << compilerString.toStdString( ).c_str( ) << u8"\n\t程序位数 :" << QSysInfo::WordSize << std::endl;
	std::cout << "=======================\n";
	std::cout << u8"输出命令行参数\n";
	auto map = args->getPairs( );
	for( auto &it : map ) {
		std::cout << u8"\t名称 : \'" << it.first << u8"\'\n";
		for( auto value : *it.second ) {
			size_t size = value.size( );
			QString outBuffString = QString::fromLocal8Bit( value.c_str( ) );
			std::cout << u8"\t\t值 (" << size << "): \'" << outBuffString.toStdString( ).c_str( ) << u8"\'\n";
		}
	}
	std::cout << "=======================\n";
	std::cout << "------------" << std::endl;
}
/// <summary>
/// 显示帮助信息
/// </summary>
void showHelp( ) {
	std::cout << "========="
			"\n"
			u8"帮助:" "\n\t"
			"-name" u8"\n\t  "
			u8"说明:\n\t\t" u8"显示软件帮助信息" "\n\t"
			"-h" u8"\n\t  "
			u8"说明:\n\t\t" u8"显示选项帮助信息" "\n\t"
			"-l" u8"  参数"
			"\n\t\t" u8"插件路径[,插件路径1[,插件路径2[,...]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"加载文件到该程序" "\n\t"
			"-s"
			u8"  参数" "\n\t\t" u8"插件路径[,插件路径1[,插件路径2[,...]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"开始该文件的工作程序" "\n\t"
			"-as" u8"\n\t  "
			u8"说明:\n\t\t" u8"开始所有已加载的插件工作程序" "\n\t"
			"-p"
			u8"  参数" "\n\t\t" u8"路径" u8"\n\t  "
			u8"说明:\n\t\t" u8"指定输出的路径" "\n\t"
			"-edb" u8"\n\t  "
			u8"说明:\n\t\t" u8"全部导出" "\n\t"
			"-ijtenf"
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"指定跳过名称子关键字存放文件路径-完全匹配小说名称，需要配合 -rdb 与 -w、-fkf 选项使用" "\n\t"
			"-ijtsnf"
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"指定跳过名称子关键字存放文件路径-段内匹配小说名称，需要配合 -rdb 与 -w、-fkf 选项使用" "\n\t"
			"-ifkf"
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"指定查找关键字存放文件，需要配合 -rdb 与 -w 选项使用" "\n\t"
			u8"\n\t\t，当指定路径不存在可用文件时候，自动激活 -edb""\n\t""\n\t"
			"-t"
			u8"  参数" "\n\t\t" u8"路径" u8"\n\t  "
			u8"说明:\n\t\t" u8"指定获取的小说类型配置文件路径-单个类型为一行" "\n\t"
			"-rdb"
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"导出数据库""\n\t"
			"-fkrrlkse"
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"获取删除目标的行关键字文件，必须存在有效 -fkrrlkd 选项""\n\t"
			u8"\n\t\t-fkrrlkss 或 -fkrrlkse 允许同时使用""\n\t"
			"-fkrrlkss"
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"获取删除目标的行关键字文件，必须存在有效 -fkrrlkd 选项""\n\t"
			u8"\n\t\t-fkrrlkss 或 -fkrrlkse 允许同时使用""\n\t"
			"-fkrrlkd"
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"处理删除目标的行关键字文件，必须存在有效 -fkrrlkss 或 -fkrrlkse 选项"
			u8"\n\t\t-fkrrlkss 或 -fkrrlkse 允许同时使用""\n\t"
			"-ex"
			u8"  参数" "\n\t\t" u8"天数" u8"\n\t  "
			u8"说明:\n\t\t" u8"校验过期，将会删除过期小说"
			"\n"
			"=========" << std::endl;
}
/// <summary>
/// 输出帮助信息
/// </summary>
/// <param name="args">选项解析对象</param>
void showHelp( std::shared_ptr< cylStd::ArgParser > &args ) {
	if( args->getOptionValues( "-h" ) )
		showHelp( );
}

/// <summary>
/// 获取路径选项的全部文件
/// </summary>
/// <param name="args">选项对象</param>
/// <param name="option">选项名称</param>
/// <returns>存在的文件列表</returns>
std::vector< QString > getFilePathsOptionPaths( std::shared_ptr< cylStd::ArgParser > &args, const cylStd::ArgParser::String &option ) {
	std::vector< QString > filePaths;
	auto optionValues = args->getOptionValues( option );
	if( optionValues ) {
		for( auto &value : *optionValues ) {
			QString fromStdString = QString::fromLocal8Bit( value );
			auto dirInfo = Path::getPathInfo( fromStdString );
			for( auto &file : dirInfo.second )
				filePaths.emplace_back( file.getCurrentFilePtah( ) ); // 存储
		}

		return vectorStrDuplicate( filePaths );
	}
	return filePaths;
}
/// <summary>
/// 获取首个非文件地址。（路径不存在，则返回该地址）
/// </summary>
/// <param name="args">选项对象</param>
/// <param name="option">选项名称</param>
/// <returns>首个文件夹地址</returns>
QString getFirstDirPathsOptionPath( std::shared_ptr< cylStd::ArgParser > &args, const cylStd::ArgParser::String &option ) {
	auto requestDownloadPathsOption = args->getOptionValues( option );
	if( requestDownloadPathsOption ) {
		for( auto &dirPath : *requestDownloadPathsOption ) {
			auto path = QString::fromLocal8Bit( dirPath );
			QFileInfo fileInfo( path );
			// 目标是一个文件，则跳过
			if( fileInfo.isFile( ) )
				continue;
			return fileInfo.absoluteFilePath( );
		}
	}
	return "";
}


/// <summary>
/// 获取文本文件
/// </summary>
/// <param name="filePaths">获取文件列表</param>
/// <returns>路径与文本文件内容的映射</returns>
std::unordered_map< QString, QString > getFileText( const std::vector< QString > &filePaths ) {
	std::unordered_map< QString, QString > result;
	if( filePaths.size( ) == 0 )
		return result;
	for( auto &filePath : filePaths ) {
		QFile file( filePath );
		if( file.exists( ) && file.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) ) {
			auto byteArray = file.readAll( );
			if( byteArray.size( ) )
				result.insert_or_assign( file.fileName( ), byteArray );
		}
	}
	return result;
}

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
			[&]( const TValue &it ) {
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
/// <summary>
/// 对一个字符串进行多次切分
/// </summary>
/// <param name="split_str_obj">切分对象</param>
/// <param name="split_str_key_s">切分的匹配字符串</param>
/// <returns>切分后的字符串列表</returns>
std::vector< QString > splitMultipleStrList( const QString &split_str_obj, const std::vector< QString > &split_str_key_s ) {
	std::vector< QString > splitStringBuff; // 临时存储
	std::vector< QString > splitStringOverBuff; // 处理完成
	std::vector< QString > resultSplitList; // 返回列表
	splitStringOverBuff.emplace_back( split_str_obj );
	// 切分关键字
	for( auto &splitStrKey : split_str_key_s ) {
		splitStringBuff.clear( );
		// 切分处理列表
		for( auto &checkStr : splitStringOverBuff ) {
			auto stringList = checkStr.split( splitStrKey );
			for( auto subStr : stringList )
				splitStringBuff.emplace_back( subStr );
		}
		resultSplitList.clear( );
		splitStringOverBuff.clear( );
		for( auto rsultSubString : splitStringBuff ) {
			resultSplitList.emplace_back( rsultSubString );
			splitStringOverBuff.emplace_back( rsultSubString );
		}
	}
	return resultSplitList;
}

/// <summary>
/// 输出一个工作任务信息
/// </summary>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="call_function_name">调用函数名称</param>
/// <param name="work_name">工作名称</param>
/// <param name="file_name">调用文件名</param>
/// <param name="line">调用行号</param>
void outStdCountStreamMsg( QMutex &std_cout_mutex, const std::string &call_function_name, const std::string &work_name, const std::string &file_name, const size_t line ) {
	std_cout_mutex.lock( );
	std::stringstream msg;
	msg << u8"\n(进程 id : " << applicationPid << u8", 线程 id : " << std::this_thread::get_id( ) << u8" ) => [ " << call_function_name << u8" ] " << work_name << u8" << " << file_name << " : " << line;
	std::cout << msg.str( ) << std::endl;
	std_cout_mutex.unlock( );
}
/// <summary>
/// 输出一个工作任务信息
/// </summary>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="mod_work_count">剩余工作数</param>
/// <param name="current_work_count">当前工作数</param>
/// <param name="call_function_name">调用函数名称</param>
/// <param name="work_name">工作名称</param>
/// <param name="file_name">调用文件名</param>
/// <param name="line">调用行号</param>
void outStdCountStreamMsg( QMutex &std_cout_mutex, const size_t &mod_work_count, const size_t &current_work_count, const std::string &call_function_name, const std::string &work_name, const std::string &file_name, const size_t line ) {
	std_cout_mutex.lock( );
	std::stringstream msg;
	msg << u8"\n(进程 id : " << applicationPid << u8", 线程 id : " << std::this_thread::get_id( ) << u8" ) => [ " << call_function_name << u8" ] " << work_name << u8" => 剩余工作数[" << mod_work_count << u8"]:正在工作数[" << current_work_count << u8"] << " << file_name << " : " << line;
	std::cout << msg.str( ) << std::endl;
	std_cout_mutex.unlock( );
}

/// @brief 使用 std::cout 输出日志信息
/// @param Std_Cout_Mutex 输出锁
/// @param Work_Count 剩余工作数量
/// @param Current_Work_Count 正在工作数量
/// @param Call_Function_Name 调用函数名称
/// @param Msg 输出消息
#define Out_Std_Thread_Pool_Count_Stream_Msg_MACRO(Std_Cout_Mutex, Mod_Work_Count, Current_Work_Count, Call_Function_Name, Msg) \
		outStdCountStreamMsg( Std_Cout_Mutex, Mod_Work_Count, Current_Work_Count, Call_Function_Name, Msg, callFileName, __LINE__ )

/// @brief 使用 std::cout 输出日志信息
/// @param Std_Cout_Mutex 输出锁
/// @param Work_Count 剩余工作数量
/// @param Current_Work_Count 正在工作数量
/// @param Call_Function_Name 调用函数名称
/// @param Msg 输出消息
#define Out_Std_Count_Stream_Msg_MACRO(Std_Cout_Mutex,  Call_Function_Name, Msg) \
		outStdCountStreamMsg( Std_Cout_Mutex,  Call_Function_Name, Msg, callFileName, __LINE__ )

/// <summary>
/// 获取文本文件，并且切分到列表
/// </summary>
/// <param name="file_paths">获取文件列表</param>
/// <param name="split_vector">字符串切分关键字</param>
/// <param name="is_remove_space">是否删除关键字空格</param>
/// <returns>路径与文本文件内容的映射</returns>
std::vector< QString > getFileTextSplit( const std::vector< QString > &file_paths, const std::vector< QString > &split_vector, bool is_remove_space = false ) {
	std::vector< QString > result;
	if( file_paths.size( ) == 0 )
		return result;
	cylHtmlTools::HtmlWorkThreadPool pool;
	QMutex mutex;
	for( auto &filePath : file_paths )
		pool.appendWork( [=,&result,&mutex]( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
			QFile file( filePath );
			if( file.exists( ) && file.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) ) {
				auto byteArray = file.readAll( );
				QString splitStrObj = byteArray;
				file.close( );
				std::vector< QString > buffStringList = splitMultipleStrList( splitStrObj, split_vector );
				if( is_remove_space ) {
					for( auto &appendString : buffStringList )
						if( !removeAllSpace( appendString ).isEmpty( ) ) {
							mutex.lock( );
							result.emplace_back( appendString );
							mutex.unlock( );
						}
					return;
				}
				for( auto &appendString : buffStringList ) {
					mutex.lock( );
					result.emplace_back( appendString );
					mutex.unlock( );
				}
			}
		} );
	pool.start( );
	while( !pool.isOverJob( ) )
		qApp->processEvents( );
	return result;
}

/// <summary>
/// 获取文件的基本名称
/// </summary>
/// <param name="filePathInfo">文件路径信息</param>
/// <returns>文件的基本名称</returns>
QString getFileBaseName( const QFileInfo &filePathInfo ) {
	QString absoluteDirPath = filePathInfo.absoluteDir( ).absolutePath( );
	QString absoluteFilePath = filePathInfo.absoluteFilePath( );
	QString fileName = absoluteFilePath.remove( 0, absoluteDirPath.size( ) + 1 );
	qsizetype indexOf = fileName.lastIndexOf( "." );
	if( indexOf > 0 )
		return fileName.slice( 0, indexOf );
	return fileName;
}
/// <summary>
/// 获取文件的基本名称
/// </summary>
/// <param name="filePathInfo">文件路径信息</param>
/// <returns>文件的基本名称</returns>
QString getFileBaseName( const QString &filePathInfo ) {
	return getFileBaseName( QFileInfo( filePathInfo ) );
}
/// <summary>
/// 在子进程运行一个爬虫插件
/// </summary>
/// <param name="app_path">子进程文件路径</param>
/// <param name="qt_mutex">进程锁</param>
/// <param name="ref_count">引用计数</param>
/// <param name="novel_plug_paths">插件路径</param>
/// <param name="request_type_name_options">请求名称选项参数</param>
/// <param name="call_function_file_path">调用该函数的文件路径-错误时使用</param>
/// <param name="call_function_name">调用该函数的函数名称-错误时使用</param>
/// <param name="call_function_line">调用该函数的行数-错误时使用</param>
/// <param name="out_path_values">请求完成路径选项</param>
std::vector< QProcess * > runPluginToSubProcess( const QString &app_path, QMutex &qt_mutex, size_t &ref_count, const std::vector< QString > &novel_plug_paths, const QString &request_type_name_options, const QString &out_path_values, const char *call_function_file_path, const char *call_function_name, const size_t &call_function_line ) {
	QStringList cmd; // 命令行
	std::vector< QProcess * > result; // 返回进程列表
	QString currentPath = QDir::currentPath( ); // 当前工作路径
	for( const QString &plugFilePath : novel_plug_paths ) {
		auto subProcess = new QProcess;
		cmd.append( " -l " );
		cmd.append( plugFilePath );

		cmd.append( " -s " );
		cmd.append( getFileBaseName( plugFilePath ) );

		cmd.append( " -p " );
		cmd.append( out_path_values );

		cmd.append( " -t " );
		cmd.append( request_type_name_options );

		QObject::connect( subProcess,
				&QProcess::readyReadStandardOutput,
				[=]( ) {
					QString msg( subProcess->readAllStandardOutput( ) );
					std::cout << u8"(进程 id :" << subProcess->processId( ) << ")[readyReadStandardOutput] : " << msg.toStdString( ) << std::endl;
				} );

		QObject::connect( subProcess,
				&QProcess::readyReadStandardError,
				[=]( ) {
					QString msg( subProcess->readAllStandardError( ) );
					std::cerr << u8"(进程 id :" << subProcess->processId( ) << ")[readyReadStandardError] : " << msg.toStdString( ) << std::endl;
				} );

		QObject::connect( subProcess,
				&QProcess::finished,
				[&ref_count,&qt_mutex,subProcess]( ) {
					std::cout << u8"子进程结束(进程 id :" << subProcess->processId( ) << ")  " << std::endl;
					subProcess->deleteLater( );
					QMutexLocker lock( &qt_mutex );
					--ref_count;
				} );

		QObject::connect( subProcess,
				&QProcess::started,
				[cmd,app_path,subProcess]( ) {
					QString msg( u8"子进程运行(进程 id :%1) :\n\t%2 %3 " );
					std::cout << msg.arg( subProcess->processId( ) ).arg( app_path ).arg( cmd.join( " " ) ).toStdString( ).c_str( ) << std::endl;
				} );

		QObject::connect( subProcess,
				&QProcess::errorOccurred,
				[cmd,app_path,subProcess]( QProcess::ProcessError error ) {
					QString msg( u8"子进程异常(进程 id :%1) :\n\t%2 %3 \n\t:: (%4) =>:(%5)" );
					std::cout << msg.arg( subProcess->processId( ) ).arg( app_path ).arg( cmd.join( " " ) ).arg( error ).arg( subProcess->errorString( ) ).toStdString( ).c_str( ) << std::endl;
				} );
		subProcess->setWorkingDirectory( currentPath );
		subProcess->setProgram( app_path );
		subProcess->setArguments( cmd );
		if( subProcess->open( ) )
			result.emplace_back( subProcess );
		else if( !subProcess->waitForStarted( ) ) {
			--ref_count;
			subProcess->deleteLater( );
			errorCoutPath( QString( "(进程 id = %1) : %2" ).arg( applicationPid ).arg( u8"无法正常启动" ), call_function_file_path, call_function_name, call_function_line );
		} else
			result.emplace_back( subProcess );
		cmd.clear( );
	}
	return result;
}
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
NovelNetJob * runPluginToThisProcess( QMutex &qt_mutex, size_t &ref_count, const QString &out_path, const std::vector< QString > &reques_type_names_vector, const QString &novel_plug_path, const char *call_function_file_path, const char *call_function_name, const size_t &call_function_line ) {
	// 加载第一个插件
	QString error;
	auto plugInterFace = LoadPlug::getIRequestNetInterface( novel_plug_path, error );
	if( plugInterFace.second ) {
		NovelNetJob *novelNetJob = new NovelNetJob( nullptr, plugInterFace.first, plugInterFace.second );
		bool *isRun = new bool( true );
		std::thread *thread = new std::thread( [=]( ) {
			auto cur = std::chrono::system_clock::now( );
			auto stdString = novelNetJob->getUrl( ).toStdString( );
			auto cStr = stdString.c_str( );
			long long second = 0, min = 0;
			while( *isRun ) {
				auto runTime = std::chrono::system_clock::now( ) - cur;
				second = std::chrono::duration_cast< std::chrono::seconds >( runTime ).count( );
				min = second / 60;
				second = second % 60;
				std::cout << u8"\n(进程 id : " << applicationPid << u8") 正在运行 : " << cStr << u8"(运行时间: " << min << " 分 " << second << " 秒);" << std::endl;
				std::this_thread::sleep_for( duration );
			}
			std::cout << u8"运行结束 : " << cStr << u8"(运行时间: " << min << " 分 " << second << " 秒);" << std::endl;
		} );
		QObject::connect( novelNetJob,
				&NovelNetJob::endJob,
				[&,isRun,thread,novelNetJob]( ) {
					*isRun = false;
					thread->join( );
					QMutexLocker lock( &qt_mutex );
					--ref_count;
					delete thread;
					delete isRun;
					novelNetJob->deleteLater( );
				} );

		novelNetJob->setPath( out_path );
		if( reques_type_names_vector.size( ) )
			novelNetJob->setRequestTypeNames( reques_type_names_vector );
		novelNetJob->start( );
		return novelNetJob;
	} else {
		QMutexLocker lock( &qt_mutex );
		--ref_count;
		errorCoutPath( QString( "(进程 id = %1) : %2" ).arg( applicationPid ).arg( error ), call_function_file_path, call_function_name, call_function_line );
		return nullptr;
	}
}


/// <summary>
/// 运行小说请求插件
/// </summary>
/// <param name="args">选项解析对象</param>
void runRequestDownloadPlugs( std::shared_ptr< cylStd::ArgParser > &args ) {
	auto optionValues = args->getOptionValues( "-l" );
	if( !optionValues )
		return;
	// 请求小说插件文件路径
	std::vector< QString > plugFilesPath = getFilePathsOptionPaths( args, "-l" );
	// 如果没有文件，则退出
	if( plugFilesPath.size( ) == 0 ) {
		ErrorCout_MACRO( u8"找不到有效的插件地址，请检查 -l 选项指定的路径是否正确" );
		return;// 找不到插件，则返回
	}

	// 运行的插件
	std::vector< QString > runPlugFilesPaths;
	// 运行模式判定
	if( args->getOptionValues( "-as" ) ) // 全部运行
		runPlugFilesPaths = plugFilesPath;
	else {
		auto runDesignateRunPlugNameOption = args->getOptionValues( "-s" );
		if( runDesignateRunPlugNameOption ) {
			// 转换到文件信息对象(全路径,大写文件名)
			std::unordered_map< QString, QString > plugFilesInfo;
			for( auto &path : plugFilesPath )
				plugFilesInfo.insert_or_assign( path, getFileBaseName( path ).toUpper( ) );

			// 转换到qt文本对象（大写）
			std::vector< QString > converQStringName;
			for( auto &plugNameOptionName : *runDesignateRunPlugNameOption )
				converQStringName.emplace_back( QString::fromLocal8Bit( plugNameOptionName ).toUpper( ) );

			auto end = plugFilesInfo.end( );
			for( auto &plugNameOptionName : converQStringName )
				for( auto vectorIterator = plugFilesInfo.begin( ); vectorIterator != end; ++vectorIterator )
					if( vectorIterator->second == plugNameOptionName ) {
						QString pluginPath = vectorIterator->first;
						if( vectorHasValue( runPlugFilesPaths, pluginPath ) ) // 插件列表已经存在该插件的全路径，则跳过
							continue;
						// 不存在则继续获取
						runPlugFilesPaths.emplace_back( vectorIterator->first );
					}
		}
	}
	if( runPlugFilesPaths.size( ) == 0 ) {// 找不到插件，则返回 
		ErrorCout_MACRO( QString( u8"(进程 id :%1)没有发件有效的插件路径，请检查 -s 是否发生错误" ).arg(applicationPid ) );
		return;
	}
	// 请求类型文件路径
	std::vector< QString > requestNamesTypeFilePaths = getFilePathsOptionPaths( args, "-t" );
	if( requestNamesTypeFilePaths.size( ) == 0 ) {
		ErrorCout_MACRO( QString( u8"(进程 id :%1)没有发现 -t 选项（该选项用于发现获取类型名称的文件路径）" ).arg( applicationPid) );
		return;
	}
	// 获取所有文件内容
	auto nameTypes = getFileTextSplit( requestNamesTypeFilePaths, { "\n" }, true );
	// 合并文件内容
	if( nameTypes.size( ) == 0 ) {
		ErrorCout_MACRO( QString( u8"(进程 id :%1)没有发现需要获取的类型名称" ).arg( applicationPid ) );
		return;
	}
	// 请求小说输出路径
	QString requestOutPath = getFirstDirPathsOptionPath( args, "-p" );
	// 如果不存在选项路径，则使用默认路径
	if( requestOutPath.isEmpty( ) )
		requestOutPath = QDir::currentPath( ) + QDir::separator( ) + "cmd_download_novels_info" + QDir::separator( );
	QString appFilePath( u8"\"%1\"" );
	appFilePath = appFilePath.arg( qApp->applicationFilePath( ) );
	QMutex mutex;
	size_t count = runPlugFilesPaths.size( );
	QString typeNameOption;
	std::vector< QString >::iterator begin = runPlugFilesPaths.begin( );
	QString firstPlugFilePath = *begin;
	runPlugFilesPaths.erase( begin );
	for( auto &optionValue : requestNamesTypeFilePaths )
		typeNameOption.append( optionValue ).append( " " );
	auto runPlug = runPluginToThisProcess( mutex, count, requestOutPath, nameTypes, firstPlugFilePath, __FILE__, __FUNCTION__, __LINE__ );
	auto subProcessRunPlug = runPluginToSubProcess( appFilePath, mutex, count, runPlugFilesPaths, typeNameOption, requestOutPath, __FILE__, __FUNCTION__, __LINE__ );
	// count 记录任务数量，任务为 0 时，退出循环，主进程重新掌控 cpu 时间
	while( count )
		qApp->processEvents( );
}

/// <summary>
/// 内容写入文件
/// </summary>
/// <param name="file_path">文件路径</param>
/// <param name="open_mode">打开模式</param>
/// <param name="write_content_vector">写入内容</param>
/// <param name="jion_str">拼接字符串</param>
/// <returns>写入个数</returns>
qint64 writeFile( const QString &file_path, const QFileDevice::OpenMode open_mode, const std::vector< QString > &write_content_vector, const QString &jion_str ) {
	qint64 writeCount = 0;
	if( Path::creatFilePath( file_path ) ) {
		QFile file( file_path );
		// 清除原本存在的文件内容
		if( file.open( open_mode ) ) {
			QString string;
			auto iterator = write_content_vector.begin( );
			auto end = write_content_vector.end( );
			do {
				string.append( *iterator );
				++iterator;
				if( iterator == end )
					break;
				string.append( "\n" );
			} while( true );
			auto byteArray = string.toUtf8( );
			writeCount = file.write( byteArray.data( ), byteArray.length( ) );
			file.close( );
		}
	}
	return writeCount;
}

/// <summary>
/// 比较难字符串长度的函数
/// </summary>
/// <param name="left">左操作数</param>
/// <param name="right">右操作数</param>
/// <returns>长度比较结果</returns>
bool compQStringLen( const QString &left, const QString &right ) {
	return left.length( ) < right.length( );
}
/// <summary>
/// 比较难字符串本身的函数
/// </summary>
/// <param name="left">左操作数</param>
/// <param name="right">右操作数</param>
/// <returns>字符串本身比较结果</returns>
bool compQString( const QString &left, const QString &right ) {
	return left < right;
}

/// <summary>
/// 对文件映射当中的文件内容进行切分，并且整理到新的映射当中 <br/>
/// 旧映射 => <文件路径,文件内容> <br/>
/// 新映射 => <文件路径,切分后的内容> 
/// </summary>
/// <param name="map">文件映射</param>
/// <param name="split_str_key">切分关键字</param>
/// <param name="isAdjustSubStr">是否需要取出子字符串</param>
/// <returns>新映射</returns>
std::unordered_map< QString, std::vector< QString > > getPairsKey( const std::unordered_map< QString, QString > &map, const QString &split_str_key, const bool isAdjustSubStr ) {
	std::unordered_map< QString, std::vector< QString > > result;
	for( auto &iter : map ) {
		auto list = splitMultipleStrList( iter.second, { split_str_key } );
		std::vector< QString > saveList;
		for( auto str : list )
			if( !removeAllSpace( str ).isEmpty( ) )
				saveList.emplace_back( str.toUpper( ) );
		if( saveList.size( ) == 0 )
			continue; // 没有关键字，则跳过这次循环
		if( isAdjustSubStr )
			saveList = vectorStrAdjustSubStr( saveList );
		else {
			saveList = vectorStrDuplicate( saveList );
			std::sort( saveList.begin( ), saveList.end( ), compQString );
			std::sort( saveList.begin( ), saveList.end( ), compQStringLen );
		}
		QString filePath = iter.first;
		if( writeFile( filePath, QIODeviceBase::ReadWrite | QIODeviceBase::Truncate, saveList, "\n" ) )
			result.insert_or_assign( filePath, saveList );// 只有真正可写内容才允许保存到列表当中
	}
	return result;
}

/// <summary>
/// 删除在映射当中相等匹配的关键字
/// </summary>
/// <param name="dest_file_text_map_keys">映射</param>
/// <param name="source_equ_keys">相等匹配的关键字</param>
void unmapRemoveIfEquKeys( std::unordered_map< QString, std::vector< QString > > &dest_file_text_map_keys, const std::vector< QString > &source_equ_keys ) {
	for( auto &iter : dest_file_text_map_keys ) {
		auto &vector = iter.second;
		std::vector< QString > newBuff;
		for( auto &key : vector )
			if( !vectorHasValue( source_equ_keys, key ) )
				newBuff.emplace_back( key );
		newBuff = vectorStrDuplicate( newBuff );
		// 不相等，则需要更新
		if( newBuff.size( ) == vector.size( ) )
			continue;
		std::sort( newBuff.begin( ), newBuff.end( ), compQString );
		std::sort( newBuff.begin( ), newBuff.end( ), compQStringLen );
		if( writeFile( iter.first, QIODeviceBase::ReadWrite | QIODeviceBase::Truncate, newBuff, "\n" ) )
			iter.second = newBuff;
	}
}
/// <summary>
/// 删除在映射当中段内匹配的关键字
/// </summary>
/// <param name="dest_file_text_map_keys">映射</param>
/// <param name="source_sub_keys">段内匹配的关键字</param>
void unmapRemoveIfSubKeys( std::unordered_map< QString, std::vector< QString > > &dest_file_text_map_keys, const std::vector< QString > &source_sub_keys ) {
	for( auto &iter : dest_file_text_map_keys ) {
		auto &vector = iter.second;
		std::vector< QString > newBuff;
		for( auto &key : vector ) {
			std::function< bool( const QString & ) > check = [&]( const QString &value ) {
				if( key.indexOf( value ) != -1 )
					return true;
				return false;
			};
			auto begin = source_sub_keys.begin( );
			auto end = source_sub_keys.end( );
			auto stdVectorFindIfResult = std::find_if( begin, end, check );
			if( stdVectorFindIfResult == end )
				newBuff.emplace_back( key );
		}

		newBuff = vectorStrDuplicate( newBuff );
		if( vector.size( ) == newBuff.size( ) )
			continue; // 数量一致，不需要更新目标文件
		std::sort( newBuff.begin( ), newBuff.end( ), compQString );
		std::sort( newBuff.begin( ), newBuff.end( ), compQStringLen );
		if( writeFile( iter.first, QIODeviceBase::ReadWrite | QIODeviceBase::Truncate, newBuff, "\n" ) )
			iter.second = newBuff;
	}
}
/// <summary>
/// 用于处理查找配置关键字文件内容
/// </summary>
/// <param name="args">参数列表解析对象</param>
void checkKeyFile( std::shared_ptr< cylStd::ArgParser > &args ) {
	// 读写文件列表
	auto dest = getFilePathsOptionPaths( args, "-fkrrlkd" );
	if( dest.size( ) == 0 )
		return;
	// 可读完全匹配文件路径
	auto sourceEqu = getFilePathsOptionPaths( args, "-fkrrlkse" );
	// 可读段内匹配文件路径
	auto sourceSub = getFilePathsOptionPaths( args, "-fkrrlkss" );
	// 发现完全匹配文件的个数
	size_t sourceEquCount = sourceEqu.size( );
	// 发现段内匹配文件的个数
	size_t sourceSubCount = sourceSub.size( );
	if( sourceEquCount == 0 && sourceSubCount == 0 ) {// 找不到目标配置文件，则返回 
		ErrorCout_MACRO( QString( u8"(进程 id : %1)没有发件有效的可读配置处理文件路径，请检查 -fkrrlkse 或 -fkrrlkss 是否发生错误" ).arg( applicationPid ) );
		return;
	}
	// 获取读写目标
	auto destFileTextMap = getFileText( dest );
	// 切分关键字
	auto destFileTextMapKeys = getPairsKey( destFileTextMap, "\n", false );
	// 处理完全匹配
	if( sourceEquCount ) {
		// 获取读写目标
		auto sourceEquFileTextMap = getFileText( sourceEqu );
		// 切分关键字
		auto sourceEquFileTextMapKeys = getPairsKey( sourceEquFileTextMap, "\n", false );
		// 保存到容器
		std::vector< QString > sourceEquKeys;
		for( auto &iter : sourceEquFileTextMapKeys )
			for( auto &checkStr : iter.second )
				sourceEquKeys.emplace_back( checkStr );
		std::sort( sourceEquKeys.begin( ), sourceEquKeys.end( ), compQString );
		std::sort( sourceEquKeys.begin( ), sourceEquKeys.end( ), compQStringLen );
		unmapRemoveIfEquKeys( destFileTextMapKeys, sourceEquKeys );
	}
	// 处理段内匹配
	if( sourceSubCount ) {
		// 获取读写目标
		auto sourceSubFileTextMap = getFileText( sourceSub );
		// 切分关键字
		auto sourceSubFileTextMapKeys = getPairsKey( sourceSubFileTextMap, "\n", true );
		// 保存到容器
		std::vector< QString > sourceSubKeys;
		for( auto &iter : sourceSubFileTextMapKeys )
			for( auto &checkStr : iter.second )
				sourceSubKeys.emplace_back( checkStr );
		sourceSubKeys = vectorStrAdjustSubStr( sourceSubKeys );
		unmapRemoveIfSubKeys( destFileTextMapKeys, sourceSubKeys );
	}
}
/// <summary>
/// 根据 . 实现判定程序后缀名，并且返回该后缀名
/// </summary>
/// <param name="file_path">文件路径</param>
/// <returns>文件后缀名</returns>
QString getFileEndStr( const QString &file_path ) {
	qsizetype lastIndexOf = file_path.lastIndexOf( "." );
	if( lastIndexOf == -1 )
		return "";
	QString mid = file_path.mid( lastIndexOf );
	return mid;
}
/// <summary>
/// 获取存在的文件路径
/// </summary>
/// <param name="arg_parser">参数列表解析对象</param>
/// <param name="option">选项</param>
/// <param name="end_str">文件的后缀名称</param>
/// <returns>存在的文件路径</returns>
std::vector< QString > getOptionExisFilePath( const std::shared_ptr< cylStd::ArgParser > &arg_parser, const cylStd::ArgParser::String &option, QString end_str = "" ) {
	std::vector< QString > filePath;

	auto optionValues = arg_parser->getOptionValues( option );
	end_str = end_str.toUpper( );
	if( optionValues )
		if( end_str.isEmpty( ) )
			for( auto &value : *optionValues ) {
				auto fileList = Path::getPathInfo( QString::fromLocal8Bit( value ) ).second;
				for( auto &fileInfo : fileList )
					filePath.emplace_back( fileInfo.getCurrentFilePtah( ) );
			}
		else
			for( auto &value : *optionValues )
				for( auto &fileInfo : Path::getPathInfo( QString::fromLocal8Bit( value ) ).second )
					filePath.emplace_back( fileInfo.getCurrentFilePtah( ) );
	return filePath;
}
/// <summary>
/// 检查合法的目录
/// </summary>
/// <param name="arg_parser">参数解析对象</param>
/// <param name="option">支持选项</param>
/// <returns>可使用的目录路径</returns>
std::vector< QString > getOptionLegitimateOutDirPath( const std::shared_ptr< cylStd::ArgParser > &arg_parser, const cylStd::ArgParser::String &option ) {
	std::vector< QString > result;

	auto optionValues = arg_parser->getOptionValues( option );
	if( optionValues )
		for( auto &value : *optionValues ) {
			QString path = QString::fromLocal8Bit( value );
			if( Path::creatDirPath( path ) )
				result.emplace_back( QFileInfo( path ).absoluteFilePath( ) + QDir::separator( ) );
		}
	return result;
}

using Str_Vector = std::vector< interfacePlugsType::HtmlDocString >; // 字符串序列
using Str_Vector_S = std::shared_ptr< Str_Vector >; // 字符串序列对象指针
using Len_Pair = std::pair< size_t, Str_Vector_S >;
using Len_Map_Str_Vector_S = std::vector< Len_Pair >; // 长度匹配的字符串序列映射对象
using Len_Map_Str_Vector_S_Shared = std::shared_ptr< Len_Map_Str_Vector_S >;// 长度匹配的字符串序列映射对象指针


/// @brief 查找长度映射对象当中是否存在指定字符串
/// @param inster_map_obj_shared_ptr 查找的长度匹配映射对象
/// @param comp_str 查找的字符串
/// @return 存在返回 true
bool lenMapHasEquStr( const Len_Map_Str_Vector_S &inster_map_obj_shared_ptr, const interfacePlugsType::HtmlDocString &comp_str ) {
	auto strLen = cylHtmlTools::HtmlStringTools::HtmlStringLen( &comp_str );
	auto begin = inster_map_obj_shared_ptr.begin( );
	auto end = inster_map_obj_shared_ptr.end( );
	for( ; begin != end; ++begin ) {
		size_t first = begin->first;
		if( first < strLen )
			continue;
		if( first == strLen ) {
			for( auto &str : *begin->second )
				if( cylHtmlTools::HtmlStringTools::equHtmlString( str, comp_str ) )
					return true;
			return false;
		} else if( first > strLen ) // 超出长度，则退出
			return false;
	}
	return false;
}

/// @brief comp_str 是否存在子字符串
/// @param inster_map_obj_shared_ptr 查找的长度匹配映射对象
/// @param comp_str 查找的字符串
/// @return 存在返回 true
bool lenMapHasSubStr( const Len_Map_Str_Vector_S &inster_map_obj_shared_ptr, const interfacePlugsType::HtmlDocString &comp_str ) {
	auto strLen = cylHtmlTools::HtmlStringTools::HtmlStringLen( &comp_str );
	auto begin = inster_map_obj_shared_ptr.begin( );
	auto end = inster_map_obj_shared_ptr.end( );
	for( ; begin != end; ++begin )
		if( begin->first > strLen )
			return false;
		else
			for( auto &str : *begin->second )
				if( cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &comp_str, &str ) )
					return true;
	return false;
}

/// @brief 序列字符串容器对象转换到长度匹配字符串容器映射
/// @param inster_map_obj 需要存储到的映射对象
/// @param vector 转换序列
/// @return 返回非空对象，但是存在 size() 为 0 的情况
void vectorConverToLenMap( Len_Map_Str_Vector_S &inster_map_obj, const std::vector< interfacePlugsType::HtmlDocString > &vector ) {
	for( auto &str : vector ) {
		auto strLen = cylHtmlTools::HtmlStringTools::HtmlStringLen( &str );
		auto begin = inster_map_obj.begin( );
		auto end = inster_map_obj.end( );
		for( ; begin != end; ++begin )
			if( begin->first == strLen ) {
				begin->second->emplace_back( str );
				break;
			}
		if( begin != end )
			continue;
		Str_Vector_S insterVectorShared = std::make_shared< Str_Vector >( );
		insterVectorShared->emplace_back( str );
		inster_map_obj.emplace_back( std::make_pair( strLen, insterVectorShared ) );
	}
}

/// <summary>
/// 匹配一个名称
/// </summary>
/// <param name="name">判定名称</param>
/// <param name="equ_name_s">完全匹配名称映射对象</param>
/// <param name="sub_name_s">段内匹配名称映射对象</param>
/// <returns>匹配成功返回 true</returns>
bool filterNovelName( const interfacePlugsType::HtmlDocString &name, const Len_Map_Str_Vector_S &equ_name_s, const Len_Map_Str_Vector_S &sub_name_s ) {
	if( lenMapHasEquStr( equ_name_s, name ) )
		return true;
	if( lenMapHasSubStr( sub_name_s, name ) )
		return true;
	return false;
}
/// <summary>
/// 执行数据库读取功能
/// </summary>
/// <param name="db_paths">数据库路径-多个数据库</param>
/// <param name="std_cout_mutex">输出锁-使用标准输出时将会锁定</param>
/// <param name="inster_novel_vector_map_mutex">插入锁-进行小说插入 inster_novel_vector_map_obj 时将会锁定</param>
/// <param name="inster_novel_vector_map_obj">小说保存对象</param>
/// <param name="novel_inster_count">返回小说个数</param>
/// <param name="expire">过期选项</param>
/// <param name="has_ex_option">是否存在过期功能</param>
/// <param name="jump_equ_name_len_map">跳过完全匹配的标题容器</param>
/// <param name="jump_sub_name_len_map">跳过段内匹配的标题容器</param>
/// <param name="is_db_path_ative">如果 db_paths 中存在有效的数据库，该值为 true</param>
/// <returns>正在执行任务的线程池</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getDBNovelsInfo( const std::vector< QString > &db_paths, QMutex &std_cout_mutex, QMutex &inster_novel_vector_map_mutex, NovelDBJob::NovelTypePairVector_Shared &inster_novel_vector_map_obj, size_t &novel_inster_count, const size_t expire, bool has_ex_option, const Len_Map_Str_Vector_S &jump_equ_name_len_map, const Len_Map_Str_Vector_S &jump_sub_name_len_map, bool &is_db_path_ative ) {
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	std::string callFunctionName = __FUNCTION__;
	for( auto &dbPath : db_paths )
		resultPool->appendWork( [=,&is_db_path_ative,
					&std_cout_mutex,&inster_novel_vector_map_mutex,
					&inster_novel_vector_map_obj,&novel_inster_count]( cylHtmlTools::HtmlWorkThread * ) {
					QFileInfo fileInfo( dbPath );
					QString absolutePath = fileInfo.absoluteDir( ).absolutePath( );
					QString absoluteFilePath = fileInfo.absoluteFilePath( );
					QString fileName = fileInfo.fileName( );
					std::shared_ptr< NovelDBJob::NovelInfoVector > novelInfoVectorShared = NovelDBJob::readDB( nullptr, absolutePath, fileName );
					size_t size = 0;
					// 是否读取到数据库
					if( novelInfoVectorShared ) {
						is_db_path_ative = true;
						// 是否存在删除过期选项
						if( has_ex_option ) {
							auto isExpire = NovelDBJob::novelVectorIsExpire( expire, *novelInfoVectorShared );
							if( isExpire.size( ) > 0 ) {
								auto novelUrlVector = NovelDBJob::novelVectorGetNovleUrl( isExpire );
								NovelDBJob::removeNovelVectorDB( nullptr, absolutePath, fileName, novelUrlVector );
								*novelInfoVectorShared = NovelDBJob::formVectorINovelInfoRemoveVectorINovelInfo( *novelInfoVectorShared, isExpire );
							}
						}
						auto jumpEquNameVectorDataSize = jump_equ_name_len_map.size( ); // 相等过滤
						auto jumpSubNameVectorDataSize = jump_sub_name_len_map.size( ); // 子过滤
						if( jumpSubNameVectorDataSize || jumpEquNameVectorDataSize ) { // 存在过滤，则开始过滤
							std::shared_ptr< NovelDBJob::NovelInfoVector > filterVector = std::make_shared< NovelDBJob::NovelInfoVector >( );
							QMutex vectorInsterMutex;
							cylHtmlTools::HtmlWorkThreadPool threadPool;
							threadPool.setCallSepMilliseconds( duration );
							threadPool.setIdleTimeCall( [&,callFunctionName]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &modWork, const unsigned long long &currentWork ) {
								Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, modWork, currentWork, callFunctionName, QString(u8"数据库( %1 )过滤小说名称进行中").arg( absoluteFilePath).toStdString( ) );
							} );
							for( auto &novel : *novelInfoVectorShared )
								threadPool.appendWork( [novel,
											jump_equ_name_len_map, jumpEquNameVectorDataSize,
											jump_sub_name_len_map, jumpSubNameVectorDataSize,
											&vectorInsterMutex,filterVector]( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
											interfacePlugsType::HtmlDocString name = novel->novelName; // 小说名称转换为大写
											name = NovelDBJob::converStringToUpper( name );
											cylHtmlTools::HtmlStringTools::removeAllSpace( name );
											if( !filterNovelName( name, jump_equ_name_len_map, jump_sub_name_len_map ) ) {
												vectorInsterMutex.lock( );
												filterVector->emplace_back( novel );
												vectorInsterMutex.unlock( );
											}
										} );
							threadPool.start( );
							while( !threadPool.isOverJob( ) )
								qApp->processEvents( );
							inster_novel_vector_map_mutex.lock( );
							inster_novel_vector_map_obj->emplace_back( absoluteFilePath, filterVector );
							size = filterVector->size( );
							novel_inster_count += size;
							inster_novel_vector_map_mutex.unlock( );
						} else {
							Out_Std_Count_Stream_Msg_MACRO( std_cout_mutex, callFunctionName, QString(u8"数据库( %1 )过滤小说名称进行中").arg( absoluteFilePath).toStdString( ) );
							inster_novel_vector_map_mutex.lock( );
							inster_novel_vector_map_obj->emplace_back( absoluteFilePath, novelInfoVectorShared );
							size = novelInfoVectorShared->size( );
							novel_inster_count += size;
							inster_novel_vector_map_mutex.unlock( );
						}

						Out_Std_Count_Stream_Msg_MACRO( std_cout_mutex, callFunctionName, QString(u8"数据库( %1 ) 有效小说为 : [ %2 ]").arg( absoluteFilePath).arg( size ).toStdString( ) );
					} else {
						std_cout_mutex.lock( );
						QString msg( "\n(进程 id : %1 )路径[ %2 ]无法加载小说列表" );
						ErrorCout_MACRO( msg.arg( applicationPid).arg( absoluteFilePath ) );
						std_cout_mutex.unlock( );
					}
				} );
	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	resultPool->setIdleTimeCall( [&,callFunctionName]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &modWork, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, modWork, currentWorkCount, callFunctionName, u8"数据库读取序列信息" );
	} );

	return resultPool;
}

/// <summary>
/// 执行获取关键字文件的任务
/// </summary>
/// <param name="arg_parser">命令行解析列表对象</param>
/// <param name="option">查找任务的命令行选项</param>
/// <param name="save_obj">关键字信息保存对象<文件路径, 关键字列表></param>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="inster_read_find_key_map_mutex">文件关键字读取锁</param>
/// <returns>任务线程池对象指针</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getFindKeyFileKeyToMap( const std::shared_ptr< cylStd::ArgParser > &arg_parser, const cylStd::ArgParser::String &option, std::unordered_map< QString, std::vector< interfacePlugsType::HtmlDocString > > &save_obj, QMutex &std_cout_mutex, QMutex &inster_read_find_key_map_mutex ) {
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );

	// 查找关键字配置文件
	auto exisFindFilePath = getOptionExisFilePath( arg_parser, option );
	auto pairs = getFileText( exisFindFilePath );
	for( auto &iter : pairs ) {
		resultPool->appendWork( [iter,&save_obj,
					&inster_read_find_key_map_mutex]( cylHtmlTools::HtmlWorkThread *html_work_thread
				) {
					auto stringList = iter.second.split( "\n" );
					std::vector< QString > strBuff;
					for( auto &str : stringList )
						if( !removeAllSpace( str ).isEmpty( ) )
							strBuff.emplace_back( str.toUpper( ) );
					strBuff = vectorStrAdjustSubStr( strBuff );
					writeJionStringVector( iter.first, strBuff, "\n" );
					inster_read_find_key_map_mutex.lock( );
					auto begin = save_obj.begin( );
					auto end = save_obj.end( );
					auto baseName = getFileBaseName( iter.first );
					for( ; begin != end; ++begin ) {
						if( begin->first == baseName ) {
							std::vector< interfacePlugsType::HtmlDocString > &saveVector = begin->second;
							for( auto converQStr : strBuff )
								saveVector.emplace_back( converQStr.toStdWString( ) );
							break;
						}
					}
					inster_read_find_key_map_mutex.unlock( );
					if( begin != end )
						return;
					std::vector< interfacePlugsType::HtmlDocString > saveVector;
					for( auto converQStr : strBuff )
						saveVector.emplace_back( converQStr.toStdWString( ) );
					inster_read_find_key_map_mutex.lock( );
					save_obj.insert_or_assign( baseName, saveVector );
					inster_read_find_key_map_mutex.unlock( );
				} );
	}
	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	std::string callFunctionName = __FUNCTION__;
	resultPool->setIdleTimeCall( [&,callFunctionName]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, workCount, currentWorkCount, callFunctionName, u8"获取关键字文件信息-分发映射-" );
	} );
	return resultPool;
}
/// <summary>
/// 执行获取关键字文件的任务
/// </summary>
/// <param name="arg_parser">命令行解析列表对象</param>
/// <param name="option">查找任务的命令行选项</param>
/// <param name="save_obj">关键字信息保存对象-<关键字列表></param>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="inster_read_find_key_map_mutex">对象插入更改锁</param>
/// <returns>任务线程池对象指针</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getFindKeyFileKeyToVector( const std::shared_ptr< cylStd::ArgParser > &arg_parser, const cylStd::ArgParser::String &option, Len_Map_Str_Vector_S_Shared &save_obj, QMutex &std_cout_mutex, QMutex &inster_read_find_key_map_mutex ) {
	// 查找关键字配置文件
	auto exisFindFilePath = getOptionExisFilePath( arg_parser, option );
	auto pairs = getFileText( exisFindFilePath );
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	for( auto &iter : pairs ) {
		resultPool->appendWork( [iter,&save_obj,&std_cout_mutex,&inster_read_find_key_map_mutex]( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
			auto stringList = iter.second.split( "\n" );
			std::vector< QString > strBuff;
			for( auto &str : stringList )
				if( !removeAllSpace( str ).isEmpty( ) )
					strBuff.emplace_back( str.toUpper( ) );
			strBuff = vectorStrDuplicate( strBuff );
			vectorStrLenSort( strBuff );
			writeJionStringVector( iter.first, strBuff, "\n" );
			Str_Vector strVector;
			for( auto converQStr : strBuff )
				strVector.emplace_back( converQStr.toStdWString( ) );
			inster_read_find_key_map_mutex.lock( );
			vectorConverToLenMap( *save_obj, strVector );
			inster_read_find_key_map_mutex.unlock( );
		} );
	}
	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	std::string callFunctionName = __FUNCTION__;
	resultPool->setIdleTimeCall( [&,callFunctionName]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, workCount, currentWorkCount, callFunctionName, u8"获取关键字文件信息-序列容器-" );
	} );
	return resultPool;
}

/// <summary>
/// 存储一个写入对象
/// </summary>
/// <param name="write_path">写入路径</param>
/// <param name="write_novel_shared_ptr">写入对象</param>
/// <param name="novel_infos_write_map">写入映射列表<路径，写入对象列表></param>
/// <returns>成功返回 true</returns>
bool emplaceWriteMap( const QString &write_path, const interfacePlugsType::INovelInfo_Shared &write_novel_shared_ptr, NovelDBJob::NovelTypePairVector &novel_infos_write_map ) {
	for( auto &iterator : novel_infos_write_map )
		if( iterator.first == write_path ) {
			iterator.second->emplace_back( write_novel_shared_ptr );
			return true;
		}
	auto vectorShared = std::make_shared< NovelDBJob::NovelInfoVector >( );
	vectorShared->emplace_back( write_novel_shared_ptr );
	novel_infos_write_map.emplace_back( write_path, vectorShared );
	return true;
}
/// <summary>
/// 激活全导出功能线程池
/// </summary>
/// <param name="vector_db_novelinfo_pairt">小说列表映射序列</param>
/// <param name="novel_infos_write_map">匹配完成的写入列表映射</param>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="inster_novel_vectos_mutex">插入锁</param>
/// <returns>正在工作的线程池</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getDBFilterNovelInfo( const NovelDBJob::NovelTypePairVector_Shared &vector_db_novelinfo_pairt, NovelDBJob::NovelTypePairVector_Shared novel_infos_write_map, QMutex &std_cout_mutex, QMutex &inster_novel_vectos_mutex ) {
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	QString allRequestGetFileBaseName = u8"合并小说_全部小说节点归纳存档.txt"; // 全部获取的基本名称
	for( auto &iter : *vector_db_novelinfo_pairt )
		for( auto &novel : *iter.second )
			resultPool->appendWork( [iter,novel,novel_infos_write_map,&inster_novel_vectos_mutex,allRequestGetFileBaseName]( cylHtmlTools::HtmlWorkThread *html_work_thread ) ->void {
				const QFileInfo orgDbFilePathInfo( iter.first ); // 数据库文件
				// 中间路径(-w选项路径/中间路径/小说类型.txt)
				const QString typeMidDirPath = QString( u8"%1%2%3%2" ).arg( exportAllDirMidName ).arg( QDir::separator( ) ).arg( orgDbFilePathInfo.fileName( ) );
				// 中间路径(-w选项路径/中间路径/全部获取.txt)
				const QString allRootRequestGetNovelInfoPath = QString( u8"%1%2%3%2%4" ).arg( exportAllDirMidName ).arg( QDir::separator( ) ).arg( orgDbFilePathInfo.fileName( ) ).arg( allRequestGetFileBaseName );
				// 中间路径(-w选项路径/全部获取.txt)
				const QString allRequestGetNovelInfoPath = QString( u8"%1%2%3" ).arg( exportAllDirMidName ).arg( QDir::separator( ) ).arg( allRequestGetFileBaseName );

				////
				/// 存储到匹配的类型文件
				////
				inster_novel_vectos_mutex.lock( );
				auto qstrName = QString::fromStdWString( novel->typeName ); // 获取类型名称
				qstrName = typeMidDirPath + qstrName + u8".txt";
				emplaceWriteMap( qstrName, novel, *novel_infos_write_map );
				inster_novel_vectos_mutex.unlock( );

				////
				/// 存储到匹配的首页文件
				////
				inster_novel_vectos_mutex.lock( );
				emplaceWriteMap( allRootRequestGetNovelInfoPath, novel, *novel_infos_write_map );
				inster_novel_vectos_mutex.unlock( );

				///
				/// 插入全部获取
				////
				inster_novel_vectos_mutex.lock( );
				emplaceWriteMap( allRequestGetNovelInfoPath, novel, *novel_infos_write_map );
				inster_novel_vectos_mutex.unlock( );
			} );


	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	std::string callFunctionName = __FUNCTION__;
	resultPool->setIdleTimeCall( [&,callFunctionName]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, workCount, currentWorkCount, callFunctionName, u8"小说过滤后全导出" );
	} );
	return resultPool;
}
/// <summary>
/// 插入一个小说key
/// </summary>
/// <param name="save_path">小说保存路径</param>
/// <param name="novel_info">小说信息对象</param>
/// <param name="key">关键字</param>
/// <param name="merge_find_keys_novel_infos_map">插入对象</param>
void emplaceNovelKeyMap( const QString &save_path, const std::shared_ptr< interfacePlugsType::INovelInfo > &novel_info, const QString &key, PathWriteNovelInfoUnMap &merge_find_keys_novel_infos_map ) {
	auto iterator = merge_find_keys_novel_infos_map.begin( );
	auto end = merge_find_keys_novel_infos_map.end( );
	while( iterator != end ) {
		if( iterator->first == save_path ) {
			auto &pairVector = iterator->second;
			for( auto pair : pairVector )
				if( pair.first == novel_info ) {
					auto &keys = pair.second;
					auto keyVectorIterator = keys.begin( ), keyVectorEnd = keys.end( );
					for( ; keyVectorIterator != keyVectorEnd; ++keyVectorIterator )
						if( *keyVectorIterator == key )
							return;
					keys.emplace_back( key );
					return; // 存储关键字后退出
				}
			std::pair< interfacePlugsType::INovelInfo_Shared, QStringList > makePair( novel_info, { key } );
			// 没有找到匹配小说
			pairVector.emplace_back( makePair );
			return; // 存储关键字后退出
		}
		++iterator; // 继续寻找下一个
	}
	std::pair< interfacePlugsType::INovelInfo_Shared, QStringList > makePair( novel_info, { key } );
	// 没有找到匹配小说
	std::vector< std::pair< interfacePlugsType::INovelInfo_Shared, QStringList > > pairVector;
	pairVector.emplace_back( makePair );
	merge_find_keys_novel_infos_map.insert_or_assign( save_path, pairVector );
}

/// @brief 查找子字符串的值
/// @param find_key 查找的整段字符串
/// @param key 段内匹配到的字符串
/// @param str_vector 子字符串容器
/// @return true 表示匹配到
bool getValue( interfacePlugsType::HtmlDocString find_key, interfacePlugsType::HtmlDocString *key, const Str_Vector &str_vector ) {
	cylHtmlTools::HtmlStringTools::removeAllSpace( find_key );
	for( auto &compStr : str_vector )
		if( cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &find_key, &compStr ) ) {
			*key = compStr;
			return true;
		}
	return false;
}
/// @brief 匹配一个key，成功返回该 key
/// @param novel_info 匹配的小说信息
/// @param find_key_map 查找的长度匹配字符串数组映射
/// @param key 返回的匹配key
/// @return 成功返回 true，并且 key 设置为 key 内容
bool lenMapFindNovelKey( const NovelInfo &novel_info, const Len_Map_Str_Vector_S &find_key_map, interfacePlugsType::HtmlDocString *key ) {
	size_t novelNameLen = novel_info.novelName.length( ); // 名车长度
	size_t novelInfoLen = novel_info.info.length( ); // 详情长度
	size_t novelLastItemLen = novel_info.lastItem.length( ); // 最后更新项长度
	size_t novelAuthorLen = novel_info.author.length( ); // 作者长度
	size_t maxLen = min( novelNameLen, novelInfoLen );
	maxLen = max( maxLen, novelLastItemLen );
	maxLen = max( maxLen, novelAuthorLen );
	for( auto begin = find_key_map.begin( ), end = find_key_map.end( ); begin != end; ++begin ) {
		auto lenKey = begin->first;
		if( maxLen < lenKey )
			break;
		if( lenKey <= novelNameLen && getValue( novel_info.novelName, key, *begin->second ) )
			return true;
		if( lenKey <= novelInfoLen && getValue( novel_info.info, key, *begin->second ) )
			return true;
		if( lenKey <= novelLastItemLen && getValue( novel_info.lastItem, key, *begin->second ) )
			return true;
		if( lenKey <= novelAuthorLen && getValue( novel_info.author, key, *begin->second ) )
			return true;
	}
	return false;
}

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
		std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > &inster_map_thread_pool ) {

	std::string callFunctionName = __FUNCTION__;
	if( inster_map_thread_pool == nullptr )
		inster_map_thread_pool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	inster_map_thread_pool_mutex.lock( );
	inster_map_thread_pool->setCallSepMilliseconds( duration );
	inster_map_thread_pool->setWorkCount( cylHtmlTools::HtmlWorkThreadPool::getSystemCupCount( ) * 2 );
	inster_map_thread_pool->setIdleTimeCall( [&std_cout_mutex, callFunctionName]( cylHtmlTools::HtmlWorkThreadPool *htmlWorkThreadPool, const unsigned long long &modWorkCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, modWorkCount, currentWorkCount, callFunctionName, u8"正在进行插入任务" );
	} );
	inster_map_thread_pool_mutex.unlock( );
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	for( auto iter : *novel_type_pair_vector_shared ) {
		for( auto novel : *iter.second ) {
			QString dbBaseName = getFileBaseName( iter.first ); // 当前遍历的文件基本名称(root url)
			resultPool->appendWork( [iter,
						&exis_find_file_path_map_key,
						&merge_find_keys_dbs_novel_infos_map_mutex,
						&merge_find_keys_dbs_novel_infos_map,
						&merge_find_keys_types_novel_infos_map_mutex,
						&merge_find_keys_types_novel_infos_map,
						&merge_find_keys_export_key_novel_infos_map_mutex,
						&merge_find_keys_export_key_novel_infos_map,
						&merge_find_keys_export_novel_infos_map_mutex,
						&merge_find_keys_export_novel_infos_map,
						&std_cout_mutex,
						&inster_map_thread_pool_mutex,
						&inster_map_thread_pool,
						novel,
						dbBaseName,
						callFunctionName
					]( cylHtmlTools::HtmlWorkThread *html_work_thread )->void {
						////
						/// 遍历 key
						////
						for( auto &findSubStrMapIterator : exis_find_file_path_map_key ) {
							auto findKeys = findSubStrMapIterator.second;
							Len_Map_Str_Vector_S lenKeyMap;
							vectorConverToLenMap( lenKeyMap, findKeys );
							std::sort( lenKeyMap.begin( ),
									lenKeyMap.end( ),
									[]( Len_Pair &left, Len_Pair &right ) {
										return left.first < right.first;
									} );
							interfacePlugsType::HtmlDocString key;
							if( lenMapFindNovelKey( *novel, lenKeyMap, &key ) ) {
								QString keyFile = findSubStrMapIterator.first;

								inster_map_thread_pool_mutex.lock( );
								// 关键字路径 -w/export_find/关键字/关键字.txt 任务
								inster_map_thread_pool->appendWork( [novel,keyFile,key,
											&merge_find_keys_export_key_novel_infos_map_mutex,
											&merge_find_keys_export_key_novel_infos_map]( cylHtmlTools::HtmlWorkThread *htmlWorkThread ) {
											merge_find_keys_export_key_novel_infos_map_mutex.lock( );
											// 关键字路径 -w/export_find/关键字/关键字.txt
											QString savePath = QString( u8"%1%2%1%3%1%3.txt" ).arg( QDir::separator( ) ).arg( exportFindDirMidName ).arg( keyFile );
											// 保存到匹配映射
											emplaceNovelKeyMap( savePath, novel, QString::fromStdWString( key ), merge_find_keys_export_key_novel_infos_map );
											merge_find_keys_export_key_novel_infos_map_mutex.unlock( );

										} );

								// 关键字路径 -w/export_find/关键字/网址.txt 任务
								inster_map_thread_pool->appendWork( [novel,keyFile,dbBaseName,key,
											&merge_find_keys_dbs_novel_infos_map_mutex,
											&merge_find_keys_dbs_novel_infos_map](
										cylHtmlTools::HtmlWorkThread *htmlWorkThread ) {
											merge_find_keys_dbs_novel_infos_map_mutex.lock( );
											// 关键字路径 -w/export_find/关键字/网址.txt
											QString savePath = QString( u8"%1%2%1%3%1%4.txt" ).arg( QDir::separator( ) ).arg( exportFindDirMidName ).arg( keyFile ).arg( dbBaseName );
											// 保存到匹配映射
											emplaceNovelKeyMap( savePath, novel, QString::fromStdWString( key ), merge_find_keys_dbs_novel_infos_map );
											merge_find_keys_dbs_novel_infos_map_mutex.unlock( );
										} );

								// 关键字路径 -w/export_find/关键字/网址/类型.txt 任务
								inster_map_thread_pool->appendWork( [novel,keyFile,key,dbBaseName,
											&merge_find_keys_types_novel_infos_map_mutex,
											&merge_find_keys_types_novel_infos_map](
										cylHtmlTools::HtmlWorkThread *htmlWorkThread ) {
											merge_find_keys_types_novel_infos_map_mutex.lock( );
											// 关键字路径 -w/export_find/关键字/网址/类型.txt
											QString savePath = QString( u8"%1%2%1%3%1%4%1%5.txt" ).arg( QDir::separator( ) ).arg( exportFindDirMidName ).arg( keyFile ).arg( dbBaseName ).arg( novel->typeName );
											// 保存到匹配映射
											emplaceNovelKeyMap( savePath, novel, QString::fromStdWString( key ), merge_find_keys_types_novel_infos_map );
											merge_find_keys_types_novel_infos_map_mutex.unlock( );
										} );

								// 关键字路径 -w/export_find/export_find.txt 任务
								inster_map_thread_pool->appendWork( [novel,keyFile,key,
											&merge_find_keys_export_novel_infos_map_mutex,
											&merge_find_keys_export_novel_infos_map](
										cylHtmlTools::HtmlWorkThread *htmlWorkThread ) {
											merge_find_keys_export_novel_infos_map_mutex.lock( );
											// 关键字路径 -w/export_find/export_find.txt
											QString savePath = QString( u8"%1%2%1%2.txt" ).arg( QDir::separator( ) ).arg( exportFindDirMidName );
											// 保存到匹配映射
											emplaceNovelKeyMap( savePath, novel, getFileBaseName( keyFile ), merge_find_keys_export_novel_infos_map );
											merge_find_keys_export_novel_infos_map_mutex.unlock( );
										} );
								inster_map_thread_pool_mutex.unlock( );
							}
						}
					} );
		}
	}
	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	resultPool->setWorkCount( cylHtmlTools::HtmlWorkThreadPool::getSystemCupCount( ) * 2 );
	resultPool->setIdleTimeCall( [&,callFunctionName]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, workCount, currentWorkCount, callFunctionName, u8"小说过滤后查找" );
	} );
	return resultPool;
}

/// <summary>
/// 调整关键字，存在子字符串则不需要源字符串(a -> aa / 其中只保留 a，不保留 aa)
/// </summary>
/// <param name="str_vector">匹配的字符串</param>
/// <returns>完成匹配的字符串</returns>
std::vector< interfacePlugsType::HtmlDocString > vectorStrAdjustSubStr( const std::vector< interfacePlugsType::HtmlDocString > &str_vector ) {
	auto endIndex = str_vector.size( );
	if( endIndex < 2 )
		return str_vector;
	// 长度排序
	std::vector< const interfacePlugsType::HtmlDocString * > clone;
	for( auto &str : str_vector ) {
		auto iterator = clone.begin( ), end = clone.end( );
		for( ; iterator != end; ++iterator ) {
			const interfacePlugsType::HtmlDocString *value = *iterator;
			if( value->length( ) > str.length( ) )
				break;
		}
		clone.insert( iterator, &str );
	}

	std::vector< interfacePlugsType::HtmlDocString > resultVector;
	std::vector< const interfacePlugsType::HtmlDocString * > resultBuffVector;
	while( endIndex ) {
		auto startObj = clone.at( 0 );
		resultVector.emplace_back( *startObj );
		for( auto newIndex = 1; newIndex < endIndex; ++newIndex ) {
			const auto &sub = clone.at( newIndex );
			if( !cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( sub, startObj ) )
				resultBuffVector.emplace_back( sub );
		}
		clone = resultBuffVector;
		resultBuffVector.clear( );
		endIndex = clone.size( );
	}
	return resultVector;
}

/// @brief 分解多个小说信息字段信息
/// @param novelInfos 分解列表
void spliteAppendDataField( std::vector< NovelInfo_Shared > &novelInfos ) {
	for( auto &novelInfo : novelInfos ) {
		std::vector< cylHtmlTools::HtmlString > split = cylHtmlTools::HtmlStringTools::split( novelInfo->additionalData, L',' );
		auto oldSize = split.size( );
		split = vectorStrAdjustSubStr( split );
		if( split.size( ) == oldSize )
			continue; // 如果大小相等，则不更新
		novelInfo->additionalData.clear( );
		auto begin = split.begin( );
		auto end = split.end( );
		if( begin != end )
			do {
				novelInfo->additionalData.append( *begin );
				++begin;
				if( begin == end )
					break;
				novelInfo->additionalData.append( L", " );
			} while( true ) ;
	}
}
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
		const std::string &callFunctionName ) {
	disk_mute.lock( );
	bool isCreate = Path::creatFilePath( all_file_path_name_name );
	disk_mute.unlock( );
	if( !isCreate ) {
		// 无法创建，则返回
		std_cout_mutex.lock( );
		QString msg( u8"(进程 id :%1, 线程 id : %2 ) 路径创建失败 : \"%3\" => (数量为 : %4)" );
		std::stringstream ss;
		ss << std::this_thread::get_id( );
		msg = msg.arg( applicationPid ).arg( QString::fromStdString( ss.str( ) ) ).arg( all_file_path_name_name ).arg( 0 );
		ErrorCout_MACRO( msg );
		std_cout_mutex.unlock( );
		return;
	}

	auto data = infos.data( );
	size_t dataSize = infos.size( );
	if( data == nullptr || dataSize == 0 ) {
		// 没有数据。则返回
		std_cout_mutex.lock( );
		QString msg( u8"(进程 id :%1, 线程 id : %2 ) 导出结果失败 : \"%3\" => (数量为 : %4)" );
		std::stringstream ss;
		ss << std::this_thread::get_id( );
		msg = msg.arg( applicationPid ).arg( QString::fromStdString( ss.str( ) ) ).arg( all_file_path_name_name ).arg( 0 );
		ErrorCout_MACRO( msg );
		std_cout_mutex.unlock( );
		return;
	}
	std::vector< NovelInfo_Shared > writeInfoVector;
	for( size_t index = 0; index < dataSize; ++index ) {
		NovelInfo_Shared novelInfoShared = std::make_shared< NovelInfo >( );
		auto &pair = data[ index ];
		*novelInfoShared = *pair.first;
		novelInfoShared->additionalData = pair.second.join( "," ).toStdWString( );
		writeInfoVector.emplace_back( novelInfoShared );
	}

	writeInfoVector = *NovelDBJob::inductionNovelsForNameAndAuthor( writeInfoVector );
	auto fileBaseName = getFileBaseName( all_file_path_name_name );
	spliteAppendDataField( writeInfoVector );
	writeInfoVector = NovelDBJob::sort( writeInfoVector );
	auto list = NovelDBJob::getNovelNames( writeInfoVector );
	QStringList novelNameList( list.begin( ), list.end( ) );
	auto novelNameListJoin = novelNameList.join( "\n" );
	auto writeCount = novelNameList.size( ); // 小说计数
	if( writeCount ) {
		auto allContents = NovelDBJob::jionNovels( writeInfoVector ) + u8"\n+++++\t小说名称列表\t" + QString::number( novelNameList.size( ) ) + " 个\t+++++++\n" + novelNameListJoin + u8"\n++++++++++++\n";
		QFile writeFile( all_file_path_name_name );
		if( writeFile.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
			writeFile.write( allContents.toUtf8( ) );
			writeFile.close( );

			// 添加计数
			count_mute.lock( );
			write_novel_count += writeCount;
			count_mute.unlock( );
			// 添加输出
			std_cout_mutex.lock( );
			std::cout << u8"(进程 id : " << std::to_string( applicationPid ) << u8", 线程 id : " << std::this_thread::get_id( ) << u8" ) => [ " << callFunctionName << u8" ]导出结果 => \"" << all_file_path_name_name.toStdString( ) << u8"\" => (数量为 : " << writeCount << u8" )" << std::endl;
			std_cout_mutex.unlock( );
		}
	} else {
		std_cout_mutex.lock( );
		QString msg( u8"(进程 id :%1, 线程 id : %2 ) 导出结果失败 : \"%3\" => (数量为 : %4)" );
		std::stringstream ss;
		ss << std::this_thread::get_id( );
		msg = msg.arg( applicationPid ).arg( QString::fromStdString( ss.str( ) ) ).arg( all_file_path_name_name ).arg( writeCount );
		ErrorCout_MACRO( msg );
		std_cout_mutex.unlock( );
	}


}
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
		const std::string &callFunctionName ) {
	disk_mute.lock( );
	bool isCreate = Path::creatFilePath( all_file_path_name_name );
	disk_mute.unlock( );
	if( isCreate ) {
		auto result = NovelDBJob::identical( infos );
		result = *NovelDBJob::inductionNovelsForNameAndAuthor( result );
		result = NovelDBJob::sort( result );
		auto list = NovelDBJob::getNovelNames( result );
		QStringList novelNameList( list.begin( ), list.end( ) );
		auto novelNameListJoin = novelNameList.join( "\n" );
		auto writeCount = novelNameList.size( ); // 小说计数
		if( writeCount ) {
			count_mute.lock( );
			write_novel_count += writeCount;
			count_mute.unlock( );
			auto allContents = NovelDBJob::jionNovels( result ) + u8"\n+++++\t小说名称列表\t" + QString::number( novelNameList.size( ) ) + " 个\t+++++++\n" + novelNameListJoin + u8"\n++++++++++++\n";
			QFile writeFile( all_file_path_name_name );
			if( writeFile.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
				writeFile.write( allContents.toUtf8( ) );
				writeFile.close( );
				std_cout_mutex.lock( );
				std::cout << u8"(进程 id : " << std::to_string( applicationPid ) << u8", 线程 id : " << std::this_thread::get_id( ) << u8" ) => [ " << callFunctionName << u8" ]导出结果 => \"" << all_file_path_name_name.toStdString( ) << u8"\" => (数量为 : " << writeCount << u8" )" << std::endl;
				std_cout_mutex.unlock( );
			}
		} else {
			std_cout_mutex.lock( );
			QString msg( u8"(进程 id :%1, 线程 id : %2 ) 导出结果失败 : \"%3\" => (数量为 : %4)" );
			std::stringstream ss;
			ss << std::this_thread::get_id( );
			msg = msg.arg( applicationPid ).arg( QString::fromStdString( ss.str( ) ) ).arg( all_file_path_name_name ).arg( writeCount );
			ErrorCout_MACRO( msg );
			std_cout_mutex.unlock( );
		}
	} else {
		std_cout_mutex.lock( );
		QString msg( u8"(进程 id :%1, 线程 id : %2 ) 导出查找结果失败 : \"%3\" => (无法创建文件)" );
		std::stringstream ss;
		ss << std::this_thread::get_id( );
		msg = msg.arg( applicationPid ).arg( QString::fromStdString( ss.str( ) ) ).arg( all_file_path_name_name );
		ErrorCout_MACRO( msg );
		std_cout_mutex.unlock( );
	}
}

/// <summary>
/// 执行写入磁盘任务
/// </summary>
/// <param name="novel_infos_write_map">写入映射</param>
/// <param name="exis_legitimate_out_dir_path">写入路径列表</param>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="disk_mute">磁盘文件、路径操作锁</param>
/// <param name="count_mute">计数锁</param>
/// <param name="write_novel_count">写入计数返回引用变量-不重计，直接累加</param>
/// <returns>任务线程池对象</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > writeDiskInForNovels( const NovelDBJob::NovelTypePairVector_Shared &novel_infos_write_map, const std::vector< QString > &exis_legitimate_out_dir_path, QMutex &std_cout_mutex, QMutex &disk_mute, QMutex &count_mute, size_t &write_novel_count ) {
	std::string callFunctionName = __FUNCTION__; // 调用函数名
	// 返回线程
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	for( auto &outDirPath : exis_legitimate_out_dir_path ) {

		for( auto &mapIterator : *novel_infos_write_map )
			resultPool->appendWork( [&std_cout_mutex,&disk_mute,&count_mute,&write_novel_count, mapIterator,outDirPath,callFunctionName]( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
				QString fileLastBody = mapIterator.first; // 路径尾部
				auto allFilePathName = outDirPath + fileLastBody;
				writeDisk( disk_mute, count_mute, std_cout_mutex, allFilePathName, write_novel_count, *mapIterator.second, callFunctionName );
			} );
	}
	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	resultPool->setIdleTimeCall( [&,callFunctionName]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, workCount, currentWorkCount, callFunctionName, u8"正在写入磁盘" );
	} );
	return resultPool;
}
/// <summary>
/// 激活查找归纳任务
/// </summary>
/// <param name="exis_legitimate_out_dir_path">写入路径</param>
/// <param name="novel_keys_map">归纳映射-小说|匹配关键字列表</param>
/// <param name="std_cout_mutex">输出锁</param>
/// <param name="disk_mute">磁盘读写锁</param>
/// <param name="count_mute">写入计数锁</param>
/// <param name="write_novel_count">写入计数-不重计，直接累加</param>
/// <returns>任务线程池</returns>
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > writeDiskInForInductionNovels(
		const std::vector< QString > &exis_legitimate_out_dir_path,
		const PathWriteNovelInfoUnMap &novel_keys_map,
		QMutex &std_cout_mutex,
		QMutex &disk_mute,
		QMutex &count_mute,
		size_t &write_novel_count ) {
	std::string callFunctionName = __FUNCTION__; // 调用函数名
	// 返回线程
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	for( auto &outDirPath : exis_legitimate_out_dir_path ) {
		for( auto &mapIterator : novel_keys_map ) {
			resultPool->appendWork(
					[ mapIterator,outDirPath,callFunctionName,
						&disk_mute,&novel_keys_map,
						&count_mute,
						&std_cout_mutex,
						&write_novel_count
					]( cylHtmlTools::HtmlWorkThread *html_work_thread
					) {
						auto allFilePathName = outDirPath + mapIterator.first;
						writeDisk( disk_mute, count_mute, std_cout_mutex, allFilePathName, write_novel_count, mapIterator.second, callFunctionName );
					} );
		}
	}
	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	resultPool->setIdleTimeCall( [&,callFunctionName]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, workCount, currentWorkCount, callFunctionName, u8"正在写入磁盘" );
	} );
	return resultPool;
}

/// <summary>
/// 合并映射对象
/// </summary>
/// <param name="merge_find_keys_novelInfos_map">合并到该对象</param>
/// <param name="merge_find_keys_types_novel_infos_map">合并的类型映射</param>
/// <param name="merge_find_keys_dbs_novel_infos_map">合并的数据库映射</param>
/// <param name="merge_find_keys_export_key_novel_infos_map">合并的关键字映射</param>
/// <param name="merge_find_keys_export_novel_infos_map">合并的全体对象</param>
void mergeFindMaps( PathWriteNovelInfoUnMap &merge_find_keys_novelInfos_map, const PathWriteNovelInfoUnMap &merge_find_keys_types_novel_infos_map, const PathWriteNovelInfoUnMap &merge_find_keys_dbs_novel_infos_map, const PathWriteNovelInfoUnMap &merge_find_keys_export_key_novel_infos_map, const PathWriteNovelInfoUnMap &merge_find_keys_export_novel_infos_map ) {
	for( auto &iter : merge_find_keys_types_novel_infos_map )
		merge_find_keys_novelInfos_map.insert( iter );
	for( auto &iter : merge_find_keys_dbs_novel_infos_map )
		merge_find_keys_novelInfos_map.insert( iter );
	for( auto &iter : merge_find_keys_export_key_novel_infos_map )
		merge_find_keys_novelInfos_map.insert( iter );
	for( auto &iter : merge_find_keys_export_novel_infos_map )
		merge_find_keys_novelInfos_map.insert( iter );
}

/// @brief 删除导出目录（-w 指定生成的 exis_legitimate_out_dir_path 路径对象）
/// @param exis_legitimate_out_dir_path 起始路径列表
/// @param stdCoutMutex 输出锁
/// @param callFunctionName 调用函数名称
/// @param line 调用行号
void rmoveExportPath( const std::vector< QString > &exis_legitimate_out_dir_path, QMutex &stdCoutMutex, const std::string &callFunctionName, const size_t &line ) {
	for( auto removePath : exis_legitimate_out_dir_path ) {
		QString pathStart = removePath + QDir::separator( );
		QFileInfo removePathFileInfo( pathStart + exportAllDirMidName );
		if( removePathFileInfo.exists( ) ) {
			QString absoluteFilePath = removePathFileInfo.absoluteFilePath( );
			outStdCountStreamMsg( stdCoutMutex, callFunctionName, QString( u8"(进程 id :%1) 删除路径 -> %2" ).arg( applicationPid ).arg( absoluteFilePath ).toStdString( ), callFileName, line );
			if( removePathFileInfo.isFile( ) )
				QFile( absoluteFilePath ).remove( );
			else
				QDir( absoluteFilePath ).removeRecursively( );
		}
		removePathFileInfo.setFile( pathStart + exportFindDirMidName );
		if( removePathFileInfo.exists( ) ) {
			QString absoluteFilePath = removePathFileInfo.absoluteFilePath( );
			outStdCountStreamMsg( stdCoutMutex, callFunctionName, QString( u8"(进程 id :%1) 删除路径 -> %2" ).arg( applicationPid ).arg( absoluteFilePath ).toStdString( ), callFileName, line );
			if( removePathFileInfo.isFile( ) )
				QFile( absoluteFilePath ).remove( );
			else
				QDir( absoluteFilePath ).removeRecursively( );
		}
	}
}

/// <summary>
/// 数据库操作
/// </summary>
/// <param name="arg_parser">参数列表解析对象</param>
void dbReadWriteChanger( const std::shared_ptr< cylStd::ArgParser > &arg_parser ) {
	if( !arg_parser->getOptionValues( "-rdb" ) )
		return;
	// 是否存在-ex 选项
	auto expireOption = arg_parser->getOptionValues( "-ex" );
	size_t expire = 0; // 过期，如果是 0，表示不删除
	bool hasExOption = false; // 转换失败或者不存在选项则为 false
	std::string callFunctionName = __FUNCTION__;
	if( expireOption ) {
		for( auto str : *expireOption ) {
			auto expireStr = QString::fromLocal8Bit( str );
			auto day = expireStr.toULongLong( &hasExOption );
			if( hasExOption ) {
				expire = day;
				break;
			}
		}
		if( !hasExOption )
			ErrorCout_MACRO( QString( u8"(进程 id :%1) 过期选项发生错误，请检查 -ex 是否发生错误" ).arg(applicationPid) );
	}
	auto exisDbFilePath = getOptionExisFilePath( arg_parser, "-rdb", ".db" );
	if( exisDbFilePath.size( ) == 0 ) {
		ErrorCout_MACRO( QString(u8"(进程 id :%1) 数据库路径错误，请检查 -rdb 指定的路径是否正确").arg(applicationPid ) );
		return; // 不存在可读数据库
	}


	// 线程当中使用输出的锁
	QMutex stdCoutMutex;
	// 查找文件对象插入锁
	QMutex insterReadFindKeyMapmutex;
	// 保存跳过关键字列表
	Len_Map_Str_Vector_S_Shared equJumpKeys = std::make_shared< Len_Map_Str_Vector_S >( );
	// 完全匹配跳过处理线程池
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > readJumpEquKeyThreadpool = getFindKeyFileKeyToVector( arg_parser, "-ijtenf", equJumpKeys, stdCoutMutex, insterReadFindKeyMapmutex );

	// 保存跳过关键字列表
	Len_Map_Str_Vector_S_Shared subJumpKeys = std::make_shared< Len_Map_Str_Vector_S >( );
	// 完全匹配跳过处理线程池
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > readJumpSubKeyThreadpool = getFindKeyFileKeyToVector( arg_parser, "-ijtsnf", subJumpKeys, stdCoutMutex, insterReadFindKeyMapmutex );

	if( readJumpEquKeyThreadpool == nullptr )
		ErrorCout_MACRO( QString( u8"(进程 id :%1) 没发现 -ijtenf 功能任务需求" ).arg(applicationPid ) );
	else
		readJumpEquKeyThreadpool->start( cylHtmlTools::HtmlWorkThreadPool::getSystemCupCount( ) * 2 );

	if( readJumpSubKeyThreadpool == nullptr )
		ErrorCout_MACRO( QString( u8"(进程 id :%1) 没发现 -ijtsnf 功能任务需求" ).arg( applicationPid) );
	else
		readJumpSubKeyThreadpool->start( cylHtmlTools::HtmlWorkThreadPool::getSystemCupCount( ) * 2 );

	if( readJumpSubKeyThreadpool )
		while( !readJumpSubKeyThreadpool->isOverJob( ) )
			qApp->processEvents( );

	if( readJumpEquKeyThreadpool )
		while( !readJumpEquKeyThreadpool->isOverJob( ) )
			qApp->processEvents( );

	if( equJumpKeys->size( ) )
		std::sort( equJumpKeys->begin( ), equJumpKeys->end( ), []( Len_Pair &left, Len_Pair &right ) { return left.first < right.first; } );
	if( subJumpKeys->size( ) )
		std::sort( subJumpKeys->begin( ), subJumpKeys->end( ), []( Len_Pair &left, Len_Pair &right ) { return left.first < right.first; } );

	// 线程当中插入容器使用的锁
	QMutex insterNovelVectosMutex;
	// 存储小说的列表映射 vectlr<pairt<qstring,vector<novelinfo>>>
	NovelDBJob::NovelTypePairVector_Shared novelInfosMap = std::make_shared< NovelDBJob::NovelTypePairVector >( );
	// 小说计数
	size_t novelCount = 0;
	// 数据库指定路径是有效的
	bool isDBPathAtive = false;
	// 执行数据库读取任务
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > readDBThreadpool = getDBNovelsInfo( exisDbFilePath, stdCoutMutex, insterNovelVectosMutex, novelInfosMap, novelCount, expire, hasExOption, *equJumpKeys, *subJumpKeys, isDBPathAtive );

	if( readDBThreadpool == nullptr ) {
		ErrorCout_MACRO( QString( u8"(进程 id :%1) -rdb 功能任务需求任务失败，请与开发者联系" ).arg( applicationPid ) );
		return;
	}
	readDBThreadpool->start( );


	auto findKeyFileOption = arg_parser->getOptionValues( "-fkf" );
	// 保存查找文件的信息<文件路径, 关键字列表>
	std::unordered_map< QString, std::vector< interfacePlugsType::HtmlDocString > > exisFindFilePathMapKey;
	// 执行获取查找关键字任务
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > readFindKeyThreadpool = nullptr;
	if( findKeyFileOption ) {
		// 存在选项，则开始
		readFindKeyThreadpool = getFindKeyFileKeyToMap( arg_parser, "-fkf", exisFindFilePathMapKey, stdCoutMutex, insterReadFindKeyMapmutex );
		readFindKeyThreadpool->start( );
	}

	while( !readDBThreadpool->isOverJob( ) )
		qApp->processEvents( );

	Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"累计有效小说数量 :[ %1 ]").arg( novelCount).toStdString( ) );

	size_t size = 0; // 查找配置文件个数
	if( readFindKeyThreadpool ) {
		while( !readFindKeyThreadpool->isOverJob( ) )
			qApp->processEvents( );
		size = exisFindFilePathMapKey.size( );
		if( size == 0 )
			ErrorCout_MACRO( QString("(进程 id :%1) 读取查找配置文件失败，请检查选项是否有效").arg( applicationPid ) );
	}

	if( !isDBPathAtive ) {
		ErrorCout_MACRO( QString( u8"(进程 id :%1) 数据库无法获取小说信息，请检查 -rdb 是否发生错误" ).arg( applicationPid ) );
		return;
	}

	// 查找输出路径
	auto exisLegitimateOutDirPath = getOptionLegitimateOutDirPath( arg_parser, "-w" );
	if( exisLegitimateOutDirPath.size( ) == 0 ) {
		ErrorCout_MACRO( QString( u8"(进程 id :%1) 输出路径设置错误，请检查 -w 是否发生错误" ).arg( applicationPid ) );
		return;
	}
	if( novelCount == 0 ) {
		ErrorCout_MACRO( QString( u8"(进程 id :%1) 数据库过滤后导出小说数量为 0" ).arg( applicationPid ) );
		auto rmOption = arg_parser->getOptionValues( "-rm" );
		if( rmOption )
			rmoveExportPath( exisLegitimateOutDirPath, stdCoutMutex, callFunctionName, __LINE__ );
		return;
	}
	// 是否存在 -edb 选项
	auto isExportDbAllNovelInfo = arg_parser->getOptionValues( "-edb" );
	if( size == 0 && !isExportDbAllNovelInfo ) { // 如果不使用查找选项，不使用全导出选项，则退出程序
		if( expireOption )
			Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"已经实现删除过期小说功能，删除单位：%1 天").arg( expire).toStdString( ) );
		else
			ErrorCout_MACRO( QString( u8"(进程 id :%1) 导出选项配置错误，请检查 -edb 或 -fkf 是否有有效" ).arg(applicationPid ) );
		return;
	}
	
	rmoveExportPath( exisLegitimateOutDirPath, stdCoutMutex, callFunctionName, __LINE__ );

	// 存储过滤后的全导出小说信息<写入文件路径, 小说列表>
	NovelDBJob::NovelTypePairVector_Shared edbNovelInfosWriteMap = std::make_shared< NovelDBJob::NovelTypePairVector >( );
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > exportDBNovelInfoAllThreadPool = nullptr;

	if( isExportDbAllNovelInfo ) { // 全导出。-w 选项的 export_all 目录
		exportDBNovelInfoAllThreadPool = getDBFilterNovelInfo( novelInfosMap, edbNovelInfosWriteMap, stdCoutMutex, insterNovelVectosMutex );
		if( !exportDBNovelInfoAllThreadPool ) {
			ErrorCout_MACRO( QString("(进程 id :%1) 全导出功能错误。请联系开发人员反馈").arg( applicationPid ) );
			return;
		}
		exportDBNovelInfoAllThreadPool->start( cylHtmlTools::HtmlWorkThreadPool::getSystemCupCount( ) * 2 );
	}

	QMutex mergeFindKeysTypesNovelInfosMapMutex, // 类型映射锁
			mergeFindKeysDbsNovelInfosMapMutex, // 数据库映射锁
			mergeFindKeysExportKeyNovelInfosMapMutex, // 关键字文件映射锁
			mergeFindKeysExportNovelInfosMapMutex; // 整体导出锁
	PathWriteNovelInfoUnMap mergeFindKeysTypesNovelInfosMap, // 类型映射对象
			mergeFindKeysDbsNovelInfosMap, // 数据库映射对象
			mergeFindKeysExportKeyNovelInfosMap, // 关键字映射对象
			mergeFindKeysExportNovelInfosMap; // 整体导出对象

	// 小说查找任务线程池对象
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > exportDBNovelInfoFindThreadPool = nullptr;
	QMutex insterMapThreadPoolMutex; // 映射插入对象工作线程锁
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > insterMapThreadPool = nullptr; // 映射插入对象工作线程
	if( size ) {// 查找导出。-w 选项的 export_find  目录
		exportDBNovelInfoFindThreadPool = getDBFindNovelInfo( novelInfosMap, exisFindFilePathMapKey, stdCoutMutex, mergeFindKeysTypesNovelInfosMapMutex, mergeFindKeysTypesNovelInfosMap, mergeFindKeysDbsNovelInfosMapMutex, mergeFindKeysDbsNovelInfosMap, mergeFindKeysExportKeyNovelInfosMapMutex, mergeFindKeysExportKeyNovelInfosMap, mergeFindKeysExportNovelInfosMapMutex, mergeFindKeysExportNovelInfosMap, insterMapThreadPoolMutex, insterMapThreadPool );
		if( !exportDBNovelInfoFindThreadPool ) {
			ErrorCout_MACRO( QString("(进程 id :%1) 查找功能错误。请联系开发人员反馈").arg(applicationPid ) );
			return;
		}
		exportDBNovelInfoFindThreadPool->start( );
	}
	if( exportDBNovelInfoAllThreadPool )
		while( !exportDBNovelInfoAllThreadPool->isOverJob( ) )
			qApp->processEvents( );
	// 查找关键字合并映射
	PathWriteNovelInfoUnMap mergeFindKeysNovelInfosMap;
	if( exportDBNovelInfoFindThreadPool ) {
		while( !exportDBNovelInfoFindThreadPool->isOverJob( ) )
			qApp->processEvents( );
		Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"开始使用插入功能").arg( applicationPid).toStdString( ) );
		// 开始保存到映射列表
		insterMapThreadPool->start( );
		while( !insterMapThreadPool->isOverJob( ) )
			qApp->processEvents( );
		Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"合并所有匹配小说").arg( applicationPid).toStdString( ) );
		mergeFindMaps( mergeFindKeysNovelInfosMap, // 合并到该对象
				mergeFindKeysTypesNovelInfosMap, // 类型映射对象
				mergeFindKeysDbsNovelInfosMap, // 数据库映射对象
				mergeFindKeysExportKeyNovelInfosMap, // 关键字映射对象
				mergeFindKeysExportNovelInfosMap // 整体导出对象
				);
	}
	size_t edbResultNovelCount = edbNovelInfosWriteMap->size( ); // -edb 返回小说数量
	size_t findResultNovelCount = mergeFindKeysNovelInfosMap.size( ); // -fkf 返回小说数量
	if( edbResultNovelCount == 0 && findResultNovelCount == 0 ) {
		ErrorCout_MACRO( QString("(进程 id :%1) 不存在导出的列表。如与信息不匹配，请联系开发人员反馈").arg( applicationPid) );
		return;
	}
	Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"发现导出返回:[ %1 ] 发现查找返回:[ %2 ]\n").arg( edbResultNovelCount ).arg( findResultNovelCount).toStdString( ) );

	// 把 novelInfosWriteMap 映射写入磁盘
	size_t saveNovelCount = 0;
	QMutex diskMute; // 磁盘操作锁
	QMutex countMute;
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > writeDiskThreadPool = nullptr;
	if( edbResultNovelCount )
		writeDiskThreadPool = writeDiskInForNovels( edbNovelInfosWriteMap, exisLegitimateOutDirPath, stdCoutMutex, diskMute, countMute, saveNovelCount );
	// 写入归纳
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > writeDiskInductionThreadPool = nullptr;
	if( findResultNovelCount > 0 )
		writeDiskInductionThreadPool = writeDiskInForInductionNovels( exisLegitimateOutDirPath, mergeFindKeysNovelInfosMap, stdCoutMutex, diskMute, countMute, saveNovelCount );

	//// 开始 -edb 全导出写入文件任务
	if( writeDiskThreadPool )
		writeDiskThreadPool->start( cylHtmlTools::HtmlWorkThreadPool::getSystemCupCount( ) * 2 );

	//// 整合 key 写入
	if( writeDiskInductionThreadPool )
		writeDiskInductionThreadPool->start( );

	if( writeDiskThreadPool )
		while( !writeDiskThreadPool->isOverJob( ) )
			qApp->processEvents( );
	if( writeDiskInductionThreadPool )
		while( !writeDiskInductionThreadPool->isOverJob( ) )
			qApp->processEvents( );


	Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"累计写入小说数量 :[ %1 ]\n").arg( saveNovelCount ).toStdString( ) );

}
#include "dateTime/DateTime.h"
int main( int argc, char *argv[ ] ) {

	std::locale locale( "zh_CN.UTF8" );
	std::locale::global( locale );
	std::cout.imbue( locale );
	std::wcout.imbue( locale );
	QCoreApplication application( argc, argv ); // 初始化程序
	applicationPid = qApp->applicationPid( );
	auto currentTime = QDateTime::currentDateTime( );
	std::cout << u8"\n=============\n=============\n=============\n开始时间 : " << currentTime.toString( "hh:mm:ss.z" ).toStdString( ) << " \n=============\n=============\n=============\n" << std::endl;
	std::shared_ptr< cylStd::ArgParser > argParser = cylStd::ArgParser::parser( argc, argv ); // 命令行参数解析对象
	auto pairs = argParser->getPairs( );
	size_t pairsSize = pairs.size( );
	if( pairsSize == 1 ) {
		ErrorCout_MACRO( QString(u8"(进程 id :%1) 没有发生任何有效的行为，请检查选项是否正确").arg(applicationPid ) );
		showHelp( );
		return 0;
	}
	// 显示名称
	showAppName( argParser );
	// 显示版本信息
	showVersion( argParser );
	// 显示帮助信息
	showHelp( argParser );
	// 运行插件
	runRequestDownloadPlugs( argParser );
	// 检查关键字文件
	checkKeyFile( argParser );
	// 数据库操作
	dbReadWriteChanger( argParser );
	auto dateTime = QDateTime::currentDateTime( );

	std::cout << u8"\n=============\n=============\n=============\n结束时间 : " << dateTime.toString( "hh:mm:ss.z" ).toStdString( ) << " \n=============\n=============\n=============\n" << std::endl;
	qint64 dateTimeMSecsSinceEpoch = dateTime.toMSecsSinceEpoch( ); // 获取毫秒
	qint64 currentTimeMSecsSinceEpoch = currentTime.toMSecsSinceEpoch( ); // 获取毫秒
	size_t msecsSinceEpoch = abs( dateTimeMSecsSinceEpoch - currentTimeMSecsSinceEpoch );
	size_t s = msecsSinceEpoch / 1000 % 60; // 获得秒
	size_t m = msecsSinceEpoch / 1000 / 60 % 60; // 获得分钟
	size_t h = msecsSinceEpoch / 1000 / 60 / 60; // 获得小时
	std::cout << u8"\n=============\n=============\n=============\n耗时 : " << h << u8" 时 " << m << u8" 分 " << s << u8" 秒 " << msecsSinceEpoch % 60 << " 毫秒\n=============\n=============\n=============\n" << std::endl;
	return 0;
}
