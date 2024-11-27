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
#include "htmlString/HtmlStringTools.h"
#include "novelNetJob/NovelDBJob.h"
#include "path/Path.h"

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
/// 输出帮助信息
/// </summary>
/// <param name="args">选项解析对象</param>
void showHelp( std::shared_ptr< cylStd::ArgParser > &args ) {
	if( args->getOptionValues( "-h" ) || args->getKeys( )->size( ) == 0 )
		std::cout << "========="
			"\n"
			u8"帮助:" "\n\t"
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
			"-isn"
			u8"  参数" "\n\t\t" u8"关键字[,关键字1 [,关键字2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"指定跳过名称子字符串关键字，需要配合 -rdb 与 -w 选项使用" "\n\t"
			"-isnf"
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"指定跳过名称子字符串关键字存放文件" "\n\t"
			"-ien" u8"  参数" "\n\t\t" u8"关键字[,关键字1 [,关键字2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"指定跳过名称完全匹配关键字，需要配合 -rdb 与 -w 选项使用" "\n\t"
			"-ienf"
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"指定跳过名称子完全匹配关键字存放文件，需要配合 -rdb 与 -w 选项使用" "\n\t"
			"-ifk"
			u8"  参数" "\n\t\t" u8"关键字[,关键字1 [,关键字2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"指定查找关键字，需要配合 -rdb 与 -w 选项使用" "\n\t"
			"-ifkf"
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"指定查找关键字存放文件，需要配合 -rdb 与 -w 选项使用" "\n\t"
			"-t"
			u8"  参数" "\n\t\t" u8"路径" u8"\n\t  "
			u8"说明:\n\t\t" u8"指定获取的小说类型配置文件路径-单个类型为一行" "\n\t"
			"-url" u8"\n\t  "
			u8"说明:\n\t\t" u8"输出加载的插件指向的网络" "\n\t"
			"-name" u8"\n\t  "
			u8"说明:\n\t\t" u8"输出程序名称""\n\t"
			"-edb" u8"\n\t  "
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"导出数据库""\n\t"
			"-fkrrlks" u8"\n\t  "
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"获取删除目标的行关键字文件，必须存在有效 -fkrrlkd 选项""\n\t"
			"-fkrrlkd" u8"\n\t  "
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"处理删除目标的行关键字文件，必须存在有效 -fkrrlks 选项""\n\t"
			"-ex" u8"\n\t  "
			u8"  参数" "\n\t\t" u8"天数" u8"\n\t  "
			u8"说明:\n\t\t" u8"校验过期，将会删除过期小说"
			"\n"
			"=========" << std::endl;
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

		return vectorStrduplicate( filePaths );
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
	result_iterator = std::find_if( iteratorBegin
		, end
		, [&]( const TValue &it ) {
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
	result_iterator = std::find_if( iteratorBegin
		, end
		, if_result_function );
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
	pool.start( 8 );
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

		QObject::connect( subProcess
			, &QProcess::readyReadStandardOutput
			, [=]( ) {
				QString msg( subProcess->readAllStandardOutput( ) );
				std::cout << u8"(" << subProcess->processId( ) << ")[readyReadStandardOutput] : " << msg.toStdString( ) << std::endl;
			} );

		QObject::connect( subProcess
			, &QProcess::readyReadStandardError
			, [=]( ) {
				QString msg( subProcess->readAllStandardError( ) );
				std::cerr << u8"(" << subProcess->processId( ) << ")[readyReadStandardError] : " << msg.toStdString( ) << std::endl;
			} );

		QObject::connect( subProcess
			, &QProcess::finished
			, [&ref_count,&qt_mutex,subProcess]( ) {
				std::cout << u8"子进程结束(" << subProcess->processId( ) << ")  " << std::endl;
				subProcess->deleteLater( );
				QMutexLocker lock( &qt_mutex );
				--ref_count;
			} );

		QObject::connect( subProcess
			, &QProcess::started
			, [cmd,app_path,subProcess]( ) {
				QString msg( u8"子进程运行(%1) :\n\t%2 %3 " );
				std::cout << msg.arg( subProcess->processId( ) ).arg( app_path ).arg( cmd.join( " " ) ).toStdString( ).c_str( ) << std::endl;
			} );

		QObject::connect( subProcess
			, &QProcess::errorOccurred
			, [cmd,app_path,subProcess]( QProcess::ProcessError error ) {
				QString msg( u8"子进程异常(%1) :\n\t%2 %3 \n\t:: (%4) =>:(%5)" );
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
			errorCout( QString( "(进程 id = %1) : %2" ).arg( qApp->applicationPid( ) ).arg( u8"无法正常启动" ), call_function_file_path, call_function_name, call_function_line );
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
	auto interface = LoadPlug::getIRequestNetInterface( novel_plug_path, error );
	if( interface.second ) {
		NovelNetJob *novelNetJob = new NovelNetJob( nullptr, interface.first, interface.second );
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
				std::cout << u8"正在运行 : " << cStr << u8"(运行时间: " << min << " 分 " << second << " 秒);" << std::endl;
				std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
			}
			std::cout << u8"运行结束 : " << cStr << u8"(运行时间: " << min << " 分 " << second << " 秒);" << std::endl;
		} );
		QObject::connect( novelNetJob
			, &NovelNetJob::endJob
			, [&,isRun,thread,novelNetJob]( ) {
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
		errorCout( QString( "(进程 id = %1) : %2" ).arg( qApp->applicationPid( ) ).arg( error ), call_function_file_path, call_function_name, call_function_line );
		return nullptr;
	}
}


/// <summary>
/// 运行小说请求插件
/// </summary>
/// <param name="args">选项解析对象</param>
void runRequestDownloadPlugs( std::shared_ptr< cylStd::ArgParser > &args ) {

	// 请求小说插件文件路径
	std::vector< QString > plugFilesPath = getFilePathsOptionPaths( args, "-l" );
	// 如果没有文件，则退出
	if( plugFilesPath.size( ) == 0 ) {
		//errorCout( QString( u8"(%1)没有发件有效的插件路径，请检查 -l 是否发生错误" ).arg( qApp->applicationPid( ) ), __FILE__, __FUNCTION__, __LINE__ );
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
		ErrorCout_MACRO( QString( u8"(%1)没有发件有效的插件路径，请检查 -s 是否发生错误" ).arg( qApp->applicationPid( ) ) );
		return;
	}
	// 请求类型文件路径
	std::vector< QString > requestNamesTypeFilePaths = getFilePathsOptionPaths( args, "-t" );
	if( requestNamesTypeFilePaths.size( ) == 0 ) {
		ErrorCout_MACRO( QString( u8"(%1)没有发现 -t 选项（该选项用于发现获取类型名称的文件路径）" ).arg( qApp->applicationPid( ) ) );
		return;
	}
	// 获取所有文件内容
	auto nameTypes = getFileTextSplit( requestNamesTypeFilePaths, { "\n" }, true );
	// 合并文件内容
	if( nameTypes.size( ) == 0 ) {
		ErrorCout_MACRO( QString( u8"(%1)没有发现需要获取的类型名称" ).arg( qApp->applicationPid( ) ) );
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
/// 对文件映射当中的文件内容进行切分，并且整理到新的映射当中 <br/>
/// 旧映射 => <文件路径,文件内容> <br/>
/// 新映射 => <文件路径,切分后的内容> 
/// </summary>
/// <param name="map">文件映射</param>
/// <param name="split_str_key_s">切分关键字</param>
/// <returns>新映射</returns>
std::unordered_map< QString, std::vector< QString > > getPairsKey( const std::unordered_map< QString, QString > &map, const std::vector< QString > &split_str_key_s ) {
	std::unordered_map< QString, std::vector< QString > > result;
	for( auto &iter : map )
		result.insert_or_assign( iter.first, splitMultipleStrList( iter.second, split_str_key_s ) );
	return result;
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
/// 对文件映射当中的文件内容进行切分，并且整理到新的映射当中 <br/>
/// 旧映射 => <文件路径,文件内容> <br/>
/// 新映射 => <文件路径,切分后的内容> 
/// </summary>
/// <param name="map">文件映射</param>
/// <param name="split_str_key">切分关键字</param>
/// <returns>新映射</returns>
std::unordered_map< QString, std::vector< QString > > getPairsKey( const std::unordered_map< QString, QString > &map, const QString &split_str_key ) {
	std::unordered_map< QString, std::vector< QString > > result;
	for( auto &iter : map ) {
		auto list = splitMultipleStrList( iter.second, { split_str_key } );
		std::vector< QString > saveList;
		for( auto str : list )
			if( !removeAllSpace( str ).isEmpty( ) )
				saveList.emplace_back( str.toUpper( ) );
		if( saveList.size( ) == 0 )
			continue; // 没有关键字，则跳过这次循环
		saveList = vectorStrduplicate( saveList );
		saveList = vectorStrSort( saveList );
		saveList = vectorStrLenSort( saveList );
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
		auto vector = iter.second;
		std::vector< QString > newBuff;
		for( auto &key : vector )
			if( !vectorHasValue( source_equ_keys, key ) )
				newBuff.emplace_back( key );
		// 不相等，则需要更新
		if( newBuff.size( ) != vector.size( ) )
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
		auto vector = iter.second;
		std::vector< QString > newBuff;
		std::vector< QString >::iterator findResult;
		for( auto &key : vector ) {
			std::function< bool( const QString & ) > check = [&]( const QString &value ) {
				if( key.indexOf( value ) != -1 )
					return true;
				return false;
			};
			auto begin = source_sub_keys.begin( );
			auto end = source_sub_keys.end( );
			auto stdVectorFindIfResult = std::find_if( begin
				, end
				, check );
			if( stdVectorFindIfResult == end )
				newBuff.emplace_back( key );
		}
		// 不相等，则需要更新
		if( newBuff.size( ) != vector.size( ) )
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
	if( dest.size( ) == 0 ) {// 找不到目标配置文件，则返回 
		//errorCout( QString( u8"(%1)没有发件有效的读写配置处理文件路径，请检查 -fkrrlkd 是否发生错误" ).arg( qApp->applicationPid( ) ), __FILE__, __FUNCTION__, __LINE__ );
		return;
	}
	// 可读完全匹配文件路径
	auto sourceEqu = getFilePathsOptionPaths( args, "-fkrrlkse" );
	// 可读段内匹配文件路径
	auto sourceSub = getFilePathsOptionPaths( args, "-fkrrlkss" );
	// 发现完全匹配文件的个数
	size_t sourceEquCount = sourceEqu.size( );
	// 发现段内匹配文件的个数
	size_t sourceSubCount = sourceSub.size( );
	if( sourceEquCount == 0 && sourceSubCount == 0 ) {// 找不到目标配置文件，则返回 
		ErrorCout_MACRO( QString( u8"(%1)没有发件有效的可读配置处理文件路径，请检查 -fkrrlkse 或 -fkrrlkss 是否发生错误" ).arg( qApp->applicationPid( ) ) );
		return;
	}
	// 获取读写目标
	auto destFileTextMap = getFileText( dest );
	// 切分关键字
	auto destFileTextMapKeys = getPairsKey( destFileTextMap, "\n" );
	// 处理完全匹配
	if( sourceEquCount ) {
		// 获取读写目标
		auto sourceEquFileTextMap = getFileText( sourceEqu );
		// 切分关键字
		auto sourceEquFileTextMapKeys = getPairsKey( sourceEquFileTextMap, "\n" );
		// 保存到容器
		std::vector< QString > sourceEquKeys;
		for( auto &iter : sourceEquFileTextMapKeys )
			for( auto &checkStr : iter.second )
				sourceEquKeys.emplace_back( checkStr );
		sourceEquKeys = vectorStrSort( sourceEquKeys );
		sourceEquKeys = vectorStrLenSort( sourceEquKeys );
		unmapRemoveIfEquKeys( destFileTextMapKeys, sourceEquKeys );
	}
	// 处理段内匹配
	if( sourceSubCount ) {
		// 获取读写目标
		auto sourceSubFileTextMap = getFileText( sourceSub );
		// 切分关键字
		auto sourceSubFileTextMapKeys = getPairsKey( sourceSubFileTextMap, "\n" );
		// 保存到容器
		std::vector< QString > sourceSubKeys;
		for( auto &iter : sourceSubFileTextMapKeys )
			for( auto &checkStr : iter.second )
				sourceSubKeys.emplace_back( checkStr );
		sourceSubKeys = vectorStrSort( sourceSubKeys );
		sourceSubKeys = vectorStrLenSort( sourceSubKeys );
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
				result.emplace_back( QFileInfo( path ).absoluteFilePath( ) );
		}
	return result;
}
/// <summary>
/// 数据库操作
/// </summary>
/// <param name="arg_parser">参数列表解析对象</param>
void dbReadWriteChanger( const std::shared_ptr< cylStd::ArgParser > &arg_parser ) {
	auto exisDbFilePath = getOptionExisFilePath( arg_parser, "-rdb", ".db" );
	if( exisDbFilePath.size( ) == 0 )
		return; // 不存在可读数据库
	auto expireOption = arg_parser->getOptionValues( "-ex" );
	size_t expire = 0; // 过期，如果是 0，表示不删除
	bool hasExOption = false; // 转换失败或者不存在选项则为 false
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
			ErrorCout_MACRO( QString( u8"(%1) 过期选项发生错误，请检查 -ex 是否发生错误" ).arg( qApp->applicationPid( ) ) );
	}
	// 查找输出路径
	auto exisLegitimateOutDirPath = getOptionLegitimateOutDirPath( arg_parser, "-w" );
	if( !hasExOption && exisLegitimateOutDirPath.size( ) == 0 ) {
		ErrorCout_MACRO( QString( u8"(%1) 输出路径设置错误，请检查 -w 是否发生错误" ).arg( qApp->applicationPid( ) ) );
		return;
	}
	// 数据库读取线程池
	cylHtmlTools::HtmlWorkThreadPool readBDFileThreadPool;
	// 线程当中使用输出的锁
	QMutex stdCoutMutex;
	// 线程当中插入容器使用的锁
	QMutex insterNovelVectosMutex;
	NovelDBJob::NovelTypePairVector_Shared novelInfosMap = std::make_shared< NovelDBJob::NovelTypePairVector >( );
	// 小说计数
	size_t novelCount = 0;
	for( auto &dbPath : exisDbFilePath )
		readBDFileThreadPool.appendWork( [=,&stdCoutMutex,&insterNovelVectosMutex,&novelInfosMap,&novelCount]( cylHtmlTools::HtmlWorkThread * ) {
			QFileInfo fileInfo( dbPath );
			QString absolutePath = fileInfo.absoluteDir( ).absolutePath( );
			QString absoluteFilePath = fileInfo.absoluteFilePath( );
			QString fileName = fileInfo.fileName( );
			auto novelInfoVectorShared = NovelDBJob::readDB( nullptr, absolutePath, fileName );
			// 是否读取到数据库
			if( novelInfoVectorShared ) {
				// 是否存在删除过期选项
				if( hasExOption ) {
					auto isExpire = NovelDBJob::novelVectorIsExpire( expire, *novelInfoVectorShared );
					if( isExpire.size( ) > 0 ) {
						auto novelUrlVector = NovelDBJob::novelVectorGetNovleUrl( isExpire );
						NovelDBJob::removeNovelVectorDB( nullptr, absolutePath, fileName, novelUrlVector );
						*novelInfoVectorShared = NovelDBJob::formVectorINovelInfoRemoveVectorINovelInfo( *novelInfoVectorShared, isExpire );
					}
				}
				insterNovelVectosMutex.lock( );
				novelInfosMap->emplace_back( absoluteFilePath, novelInfoVectorShared );
				insterNovelVectosMutex.unlock( );
				stdCoutMutex.lock( );
				size_t size = novelInfoVectorShared->size( );
				novelCount += size;
				std::cout << u8"\n路径 " << absoluteFilePath.toStdString( ).c_str( ) << u8" 有效小说为 :[" << size << u8"]\n" << std::endl;
				stdCoutMutex.unlock( );
			} else {
				stdCoutMutex.lock( );
				QString msg( "\n路径 %1 无法加载小说列表" );
				ErrorCout_MACRO( msg.arg( absoluteFilePath ) );
				stdCoutMutex.unlock( );
			}
		} );
	readBDFileThreadPool.start( 8
		, [&]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
			stdCoutMutex.lock( );
			std::cout << u8"\n数据库读取序列信息 => 剩余工作数[" << workCount << u8"]:正在工作数[" << currentWorkCount << u8"]<<" << std::endl;
			stdCoutMutex.unlock( );
			std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
		} );


	// 查找文件配置线程池
	cylHtmlTools::HtmlWorkThreadPool readExisFindFileThreadPool;
	// 保存查找文件的信息<文件路径, 关键字列表>
	std::unordered_map< QString, std::vector< QString > > exisFindFilePathMapKey;
	readExisFindFileThreadPool.appendWork( [&exisFindFilePathMapKey, &arg_parser]( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
		// 查找关键字配置文件
		auto exisFindFilePath = getOptionExisFilePath( arg_parser, "-fkf" );
	} );
	// 完全匹配跳过处理线程池
	cylHtmlTools::HtmlWorkThreadPool readExisJumptEquTitleFileThreadPool;
	// 保存跳过关键字列表
	std::vector< QString > equJumpKeys;
	readExisJumptEquTitleFileThreadPool.appendWork( [&equJumpKeys, &arg_parser]( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
		// 获取跳过完全匹配标题关键字配置文件选项路径
		auto exisFindJumptEquTitleFilePath = getOptionExisFilePath( arg_parser, "-ijtenf" );
		// 切分跳过完全匹配关键字
		auto exisFindJumpEquTitleFileKeys = getFileTextSplit( exisFindJumptEquTitleFilePath, { "\n" } , true);
		// 长度排序
		equJumpKeys = vectorStrLenSort( exisFindJumpEquTitleFileKeys );

	} );

	// 完全匹配跳过处理线程池
	cylHtmlTools::HtmlWorkThreadPool readExisJumptSubTitleFileThreadPool;
	// 保存跳过关键字列表
	std::vector< QString > subJumpKeys;
	readExisJumptSubTitleFileThreadPool.appendWork( [&subJumpKeys, &arg_parser]( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
		// 获取跳过段内匹配标题关键字配置文件选项路径
		auto exisJumptSubTitleFilePath = getOptionExisFilePath( arg_parser, "-ijtsnf" );
		// 切分跳过段内匹配关键字
		auto exisJumpSubTitleFileKeys = getFileTextSplit( exisJumptSubTitleFilePath, { "\n" }, true );
		// 长度排序
		subJumpKeys = vectorStrLenSort( exisJumpSubTitleFileKeys );
	} );


	readExisFindFileThreadPool.start( 8
		, [&]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
			stdCoutMutex.lock( );
			std::cout << u8"\n查找文件配置读取序列信息 => 剩余工作数[" << workCount << u8"]:正在工作数[" << currentWorkCount << u8"]<<" << std::endl;
			stdCoutMutex.unlock( );
			std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
		} );
	readExisJumptEquTitleFileThreadPool.start( 8
		, [&]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
			stdCoutMutex.lock( );
			std::cout << u8"\n完全匹配读取序列信息 => 剩余工作数[" << workCount << u8"]:正在工作数[" << currentWorkCount << u8"]<<" << std::endl;
			stdCoutMutex.unlock( );
			std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
		} );
	readExisJumptSubTitleFileThreadPool.start( 8
		, [&]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
			stdCoutMutex.lock( );
			std::cout << u8"\n段内匹配读取序列信息 => 剩余工作数[" << workCount << u8"]:正在工作数[" << currentWorkCount << u8"]<<" << std::endl;
			stdCoutMutex.unlock( );
			std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
		} );
	while( !readBDFileThreadPool.isOverJob( ) )
		qApp->processEvents( );

	stdCoutMutex.lock( );
	std::cout << u8"\n累计有效小说数量 :[ " << novelCount << u8" ]\n" << std::endl;
	stdCoutMutex.unlock( );

	while( !readExisFindFileThreadPool.isOverJob( ) )
		qApp->processEvents( );

	while( !readExisJumptEquTitleFileThreadPool.isOverJob( ) )
		qApp->processEvents( );

	while( !readExisJumptSubTitleFileThreadPool.isOverJob( ) )
		qApp->processEvents( );

	if( novelCount == 0 ) {
		ErrorCout_MACRO( QString( u8"(%1) 数据库无法获取小说信息，请检查 -rdb 是否发生错误" ).arg( qApp->applicationPid( ) ) );
		return;
	}
	// 存储过滤后的小说信息<写入文件路径, 小说列表>
	NovelDBJob::NovelTypePairVector_Shared novelInfosWriteMap = std::make_shared< NovelDBJob::NovelTypePairVector >( );

	if( exisFindFilePathMapKey.size( ) == 0 ) { // 全导出。-w 选项的 export_all 目录
		for( auto &iter : *novelInfosMap ) {
			readBDFileThreadPool.appendWork( [iter,&novelInfosWriteMap,&equJumpKeys,&subJumpKeys,&insterNovelVectosMutex]( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
				QFileInfo orgDbFilePathInfo( iter.first );
				// 中间路径(-w选项路径/中间路径/小说类型.txt)
				QString midDirPath = QString( u8"%1%2%3%2" ).arg( u8"export_all" ).arg( QDir::separator( ) ).arg( orgDbFilePathInfo.fileName( ) );
				std::unordered_map< QString, NovelDBJob::NovelInfoVector_Shared > typeMap;
				// 遍历小说
				interfacePlugsType::HtmlDocString name;
				for( auto &novel : *iter.second ) {
					if( novel->getNovelName( &name ) ) {
						auto qstrName = QString::fromStdWString( name );
						qstrName = qstrName.toUpper( );
						for( auto &compEquTitle : equJumpKeys ) {
							if( compEquTitle == qstrName ) {
								qstrName = "";
								break;
							}
						}
						if( qstrName.isEmpty( ) )
							continue;
						for( auto &compSubTitle : subJumpKeys ) {
							if( qstrName.indexOf( compSubTitle ) != -1 ) {
								qstrName = "";
								break;
							}
						}
						if( qstrName.isEmpty( ) || novel->getNovelTypeName( &name ) == 0 )
							continue;
						qstrName = QString::fromStdWString( name );
						qstrName = midDirPath + qstrName + u8".txt";
						auto iterator = typeMap.begin( );
						auto end = typeMap.end( );

						insterNovelVectosMutex.lock( );
						for( ; iterator != end; ++iterator )
							if( iterator->first == qstrName ) {
								iterator->second->emplace_back( novel );
								break;
							}
						if( iterator == end ) {
							auto vectorShared = std::make_shared< NovelDBJob::NovelInfoVector >( );
							vectorShared->emplace_back( novel );
							typeMap.insert_or_assign( qstrName, vectorShared );
						}
						insterNovelVectosMutex.unlock( );
					}
				}
			} );
		}
		readBDFileThreadPool.start( 8
			, [&]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &modWorkCount, const unsigned long long &currentWorkCount ) {
				stdCoutMutex.lock( );
				std::cout << u8"小说列表过滤信息 => 剩余工作数[" << modWorkCount << u8"]:正在工作数[" << currentWorkCount << u8"]<<" << std::endl;
				stdCoutMutex.unlock( );
				std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
			} );
		while( !readBDFileThreadPool.isOverJob( ) )
			qApp->processEvents( );
	} else {// 查找导出。-w 选项的 export_find  目录
		// todo : 查找匹配
	}


	return;
}
int main( int argc, char *argv[ ] ) {
	std::locale locale( "zh_CN.UTF8" );
	std::locale::global( locale );
	std::cout.imbue( locale );
	std::wcout.imbue( locale );

	QCoreApplication application( argc, argv ); // 初始化程序
	std::shared_ptr< cylStd::ArgParser > argParser = cylStd::ArgParser::parser( argc, argv ); // 命令行参数解析对象
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
	return 0;
}
