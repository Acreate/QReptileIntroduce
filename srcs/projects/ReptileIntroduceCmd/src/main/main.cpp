#include <QCoreApplication>
#include <QFileInfo>
#include "../auto_generate_files/macro/cmake_to_c_cpp_header_env.h"
#include "argParser/ArgParser.h"
#include "nameSpace/cylStd.h"
#include "novelNetJob/NovelNetJob.h"
#include "path/Dir.h"
#include "plug/LoadPlug.h"
#include "./function.h"
#include <iostream>
#include <QProcess>
#include <QMutex>
#include <QMutexLocker>
#include <htmls/htmlTools/HtmlWorkThread/HtmlWorkThread.h>
#include <QSharedMemory>

#include "novelNetJob/NovelDBJob.h"
#include "path/Path.h"
int main( int argc, char *argv[ ] ) {
	std::locale locale( "zh_CN.UTF8" );
	std::locale::global( locale );
	std::cout.imbue( locale );
	std::wcout.imbue( locale );
	QMutex qMutex;

	QCoreApplication application( argc, argv );
	auto *instance = qApp;// 指定一个键，这个键用来标识共享内存区域

	QString compilerString = getBuilderInfo( );


	QString description = u8"小说爬虫命令行版本";
	auto argParser = cylStd::ArgParser::parser( argc, argv );

	std::vector< QString > requesTypeNameVector;

	if( argParser->getOptionValues( "-name" ) )
		std::cout << instance->applicationName( ).toStdString( ).c_str( ) << std::endl;

	auto requetTypeNameOption = argParser->getOptionValues( "-t" );
	if( requetTypeNameOption )
		for( auto &path : *requetTypeNameOption ) {
			QFileInfo fileInfo( QString::fromStdString( path ) );
			if( fileInfo.exists( ) && fileInfo.isFile( ) ) {
				auto absoluteFilePath = fileInfo.absoluteFilePath( );
				auto typeKeyS = readIngoreNameFile( absoluteFilePath );
				typeKeyS = vectorStrduplicate( typeKeyS );
				writeIngoreNameFile( absoluteFilePath, typeKeyS );
				requesTypeNameVector.insert( requesTypeNameVector.end( ), typeKeyS.begin( ), typeKeyS.end( ) );
			}
		}
	requesTypeNameVector = vectorStrduplicate( requesTypeNameVector );
	QString appPathDir = instance->applicationDirPath( );
	QString appPath = instance->applicationFilePath( );
	QString workPath = QDir::currentPath( );
	if( requesTypeNameVector.size( ) == 0 ) {
		QString filePath;
		filePath.append( appPathDir ).append( QDir::separator( ) ).append( "progress" ).append( QDir::separator( ) ).append( "ini" ).append( QDir::separator( ) ).append( "ReptileIntroduce.ini" );
		auto vector = readIngoreNameFile( filePath );
		if( vector.size( ) > 0 ) {
			vector = vectorStrduplicate( vector );
			vector = vectorStrLenSort( vector );
			writeIngoreNameFile( filePath, vector );
			requesTypeNameVector = vectorStrduplicate( vector );
		}
	}

	if( argParser->getOptionValues( "-v" ) ) {
		std::cout << "------------\n" u8"qt 版本 :" << qVersion( ) << u8"\n\t" u8"编译版本 :" << compilerString.toStdString( ).c_str( ) << u8"\n\t程序位数 :" << QSysInfo::WordSize << std::endl;
		auto map = argParser->getPairs( );
		std::cout << u8"输出命令行参数\n";
		for( auto &it : map ) {
			std::cout << u8"\t名称 : \'" << it.first << u8"\'\n";
			for( auto &value : *it.second )
				std::cout << u8"\t\t值 : \'" << value << u8"\'\n";
		}
		std::cout << "------------" << std::endl;
	}

	auto htmlOption = argParser->getOptionValues( "-h" );

	size_t count = argParser->getPairs( ).size( );
	if( htmlOption || count == 1 )
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
			u8"说明:\n\t\t" u8"输出程序名称"
			"\n"
			"=========" << std::endl;
	if( count == 1 )
		return 0;
	auto pathValues = argParser->getOptionValues( "-p" );
	QString path = workPath + QDir::separator( ) + "cmd_download_novels_info" + QDir::separator( );
	if( pathValues )
		path = QString::fromStdString( pathValues->at( 0 ) );
	auto loadPathOption = argParser->getOptionValues( "-l" );
	std::vector< QString > novelPlugPath;
	std::vector< QString > runPlugPath;

	count = 0;
	if( loadPathOption ) {
		for( auto &value : *loadPathOption ) {
			QString fromStdString = QString::fromStdString( value );
			auto dirInfo = Path::getPathInfo( fromStdString );
			for( auto &file : dirInfo.second )
				if( !findVector( novelPlugPath, file.getCurrentFilePtah( ) ) )
					novelPlugPath.emplace_back( file.getCurrentFilePtah( ) ); // 存储
		}
		auto startOption = argParser->getOptionValues( "-s" );
		auto allStartOption = argParser->getOptionValues( "-as" );
		if( allStartOption ) // 全部加载
			runPlugPath = novelPlugPath;
		else if( startOption ) { // 选择加载
			std::vector< QString > paths; // 全路径保存
			auto startIterator = startOption->begin( );
			auto startEnd = startOption->end( );
			for( ; startIterator != startEnd; ++startIterator ) {
				QFileInfo info( QString::fromLocal8Bit( *startIterator ) );
				auto absPath = info.absoluteFilePath( );
				if( !findVector( paths, absPath ) )
					paths.emplace_back( absPath );
				for( auto &value : *loadPathOption ) {
					auto absoluteFilePath = QFileInfo( QString::fromStdString( value ) ).absoluteFilePath( );
					if( findVector( paths, absoluteFilePath ) && !findVector( runPlugPath, absoluteFilePath ) )
						// 不存在选择路径当中，需要直接跳过
						runPlugPath.emplace_back( absoluteFilePath ); // 存储
				}
			}
		}
		auto urlOption = argParser->getOptionValues( "-url" );
		if( urlOption ) {
			count = novelPlugPath.size( );
			QStringList cmd;
			auto optionToString = cylStd::ArgParser::converOptionToString( requetTypeNameOption );
			for( size_t index = 1; index < count; ++index ) {
				// 加载插件
				QString plugFilePath = novelPlugPath.at( index );
				auto subProcess = new QProcess;
				cmd.append( " -url " );

				cmd.append( " -l " );
				cmd.append( plugFilePath );
				if( findVector( runPlugPath, plugFilePath ) ) {
					cmd.append( " -s " );
					cmd.append( plugFilePath );
				}
				if( pathValues ) {
					cmd.append( " -p " );
					cmd.append( path );
				}
				if( requetTypeNameOption ) {
					cmd.append( " -t " );
					cmd.append( QString::fromLocal8Bit( optionToString ) );
				}
				QObject::connect( subProcess
					, &QProcess::readyReadStandardOutput
					, [=]( ) {
						QString msg( subProcess->readAllStandardOutput( ) );
						std::cout << msg.toStdString( ) << std::endl;
					} );

				QObject::connect( subProcess
					, &QProcess::readyReadStandardError
					, [=]( ) {
						QString msg( subProcess->readAllStandardError( ) );
						std::cerr << msg.toStdString( ) << std::endl;
					} );

				QObject::connect( subProcess
					, &QProcess::finished
					, [&,subProcess]( ) {
						QMutexLocker lock( &qMutex );
						subProcess->deleteLater( );
						--count;
					} );

				subProcess->start( appPath, cmd );
				cmd.clear( );
			}

			// 加载第一个插件
			QString plugFilePath = novelPlugPath.front( );
			QString error;
			auto interface = LoadPlug::getIRequestNetInterface( plugFilePath, error );
			if( interface.second ) {
				NovelNetJob *novelNetJob = new NovelNetJob( nullptr, interface.first, interface.second );
				std::cout << novelNetJob->getUrl( ).toStdString( ) << std::endl;
				if( findVector( runPlugPath, plugFilePath ) ) {
					QObject::connect( novelNetJob
						, &NovelNetJob::endJob
						, [&]( ) {
							QMutexLocker lock( &qMutex );
							--count;
						} );
					std::cout << novelNetJob->getUrl( ).toStdString( ).c_str( ) << std::endl;
					if( pathValues )
						novelNetJob->setPath( path );
					if( requetTypeNameOption )
						novelNetJob->setRequestTypeNames( requesTypeNameVector );
					if( findVector( runPlugPath, plugFilePath ) )
						novelNetJob->start( );
				} else {
					QMutexLocker lock( &qMutex );
					--count;
				}
			} else {
				QMutexLocker lock( &qMutex );
				--count;
				errorCout( error.toStdString( ), __FILE__, __FUNCTION__, __LINE__ );
			}
		} else { // 除了首个爬虫，其他爬虫均有子程序进行
			count = runPlugPath.size( );
			QStringList cmd;
			auto optionToString = cylStd::ArgParser::converOptionToString( requetTypeNameOption );
			for( size_t index = 1; index < count; ++index ) {
				// 加载插件
				QString plugFilePath = runPlugPath.at( index );
				auto subProcess = new QProcess;
				cmd.append( " -l " );
				cmd.append( plugFilePath );


				if( pathValues ) {
					cmd.append( " -p " );
					cmd.append( path );
				}
				if( requetTypeNameOption ) {
					cmd.append( " -t " );
					cmd.append( QString::fromLocal8Bit( optionToString ) );
				}
				cmd.append( " -s " );
				cmd.append( plugFilePath );
				QObject::connect( subProcess
					, &QProcess::readyReadStandardOutput
					, [=]( ) {
						QString msg( subProcess->readAllStandardOutput( ) );
						std::cout << msg.toStdString( ) << std::endl;
					} );

				QObject::connect( subProcess
					, &QProcess::readyReadStandardError
					, [=]( ) {
						QString msg( subProcess->readAllStandardError( ) );
						std::cerr << msg.toStdString( ) << std::endl;
					} );

				QObject::connect( subProcess
					, &QProcess::finished
					, [&,subProcess]( ) {
						subProcess->deleteLater( );
						QMutexLocker lock( &qMutex );
						--count;
					} );

				subProcess->start( appPath, cmd );
				cmd.clear( );
			}

			// 加载第一个插件
			QString plugFilePath = runPlugPath.front( );
			if( findVector( runPlugPath, plugFilePath ) ) {
				QString error;
				auto interface = LoadPlug::getIRequestNetInterface( plugFilePath, error );
				if( interface.second ) {
					NovelNetJob *novelNetJob = new NovelNetJob( nullptr, interface.first, interface.second );
					QObject::connect( novelNetJob
						, &NovelNetJob::endJob
						, [&] {
							QMutexLocker lock( &qMutex );
							--count;
						} );
					novelNetJob->setPath( path );
					novelNetJob->setRequestTypeNames( requesTypeNameVector );
					novelNetJob->start( );
				} else {
					QMutexLocker lock( &qMutex );
					--count;
					errorCout( error.toStdString( ), __FILE__, __FUNCTION__, __LINE__ );
				}
			} else {
				QMutexLocker lock( &qMutex );
				--count;
			}
		}
	}

	auto currentTime = std::chrono::system_clock::now( );
	auto dbPaths = argParser->getOptionValues( "-rdb" ); // 是否存在导出
	auto writeFilePaths = argParser->getOptionValues( "-w" ); // 是否存在导出
	/// 忽略子名称
	LenMap lenSubStrKeyMap; // 子字符串匹配
	cylHtmlTools::HtmlWorkThread ingSubNameThread;  // 子字符串匹配线程
	/// 忽略名称
	LenMap lenEquStrKeyMap; // 完全匹配映射
	cylHtmlTools::HtmlWorkThread ingEquNameThread; // 完全匹配线程
	/// 关键字查找
	LenMap lenFindStrKeyMap; // 查找文件的映射-汇总
	FileLenMap fileLenFindStrKeyMap; // 查找文件的映射-文件<长度，名称列表>
	cylHtmlTools::HtmlWorkThread ingFindStrKeyThread;

	if( dbPaths && writeFilePaths ) { // 如果存在 -rdb 与 -w 选项，那么可以使用过滤选项
		auto inNameKeys = argParser->getOptionValues( "-isn" ); // 忽略名称
		auto inNameFiles = argParser->getOptionValues( "-isnf" ); // 忽略文件路径
		loadingSubKeyFiles( inNameKeys, inNameFiles, ingSubNameThread, lenSubStrKeyMap );

		auto inEquNameKeys = argParser->getOptionValues( "-ien" ); // 忽略名称
		auto inEquNameFiles = argParser->getOptionValues( "-ienf" ); // 忽略文件路径
		loadingEquKeyFiles( inEquNameKeys, inEquNameFiles, ingEquNameThread, lenEquStrKeyMap ); // 处理完全匹配名称忽略文件

		auto findKeysOptions = argParser->getOptionValues( "-fk" ); // 小说查找名称
		auto findNameFiles = argParser->getOptionValues( "-fkf" ); // 小说查找文件路径
		loadFindKeyFiles( findKeysOptions, findNameFiles, ingFindStrKeyThread, lenFindStrKeyMap, fileLenFindStrKeyMap ); // 处理查找文件
	}

	do {
		instance->processEvents( );
		QMutexLocker lock( &qMutex );
		if( count ) {
			auto cur = std::chrono::system_clock::now( );
			auto sep = cur - currentTime;
			auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
			if( second > 2 ) {
				std::cout << u8"准备工作进行当中 [" << count << "] ..." << std::endl;
				currentTime = cur;
			}
			continue;
		}
		break;
	} while( true );


	if( dbPaths && writeFilePaths ) {
		std::cout << u8"检测到 -rdb 与 -w 选项" << std::endl;
		interfacePlugsType::Vector_INovelInfoSPtr novelInfoS;
		for( auto &str : *dbPaths ) {
			std::cout << u8"检测路径 " << str << std::endl;
			auto novelInfoSPtrShared = NovelDBJob::readDB( nullptr
				, QString::fromStdString( str )
				, [&currentTime]( ) {
					auto cur = std::chrono::system_clock::now( );
					auto sep = cur - currentTime;
					auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
					if( second < 2 )
						return;
					std::cout << u8"正在读取数据库" << std::endl;
					currentTime = cur;
				} );
			if( novelInfoSPtrShared ) {
				std::cout << u8"剔除相同小说 " << str << std::endl;
				*novelInfoSPtrShared = NovelDBJob::identical( *novelInfoSPtrShared );
				std::cout << u8"剔除相同小说完毕 " << str << std::endl;
				novelInfoS.insert( novelInfoS.begin( ), novelInfoSPtrShared->begin( ), novelInfoSPtrShared->end( ) );
			}
		}
		std::cout << u8"数据库读取完毕" << std::endl;

		std::cout << u8"开始分解小说到域名" << std::endl;
		auto novelHostMap = NovelDBJob::decompose( novelInfoS
			, [&currentTime]( ) {
				auto cur = std::chrono::system_clock::now( );
				auto sep = cur - currentTime;
				auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
				if( second < 2 )
					return;
				std::cout << u8"正在分解小说到域名" << std::endl;
				currentTime = cur;
			} );
		while( ingSubNameThread.isRun( ) ) {
			std::cout << u8"等待忽略选项完成" << std::endl;
			instance->processEvents( );
		}
		while( ingEquNameThread.isRun( ) ) {
			std::cout << u8"等待相等选项完成" << std::endl;
			instance->processEvents( );
		}
		while( ingFindStrKeyThread.isRun( ) ) {
			std::cout << u8"等待查找选项完成" << std::endl;
			instance->processEvents( );
		}
		if( lenEquStrKeyMap.size( ) > 0 )
			novelInfoS = NovelDBJob::removeEquName( novelInfoS
				, lenEquStrKeyMap
				, [&currentTime]( ) {
					auto cur = std::chrono::system_clock::now( );
					auto sep = cur - currentTime;
					auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
					if( second < 2 )
						return;
					std::cout << u8"正在删除匹配的符串关键字" << std::endl;
					currentTime = cur;
				} );
		if( lenSubStrKeyMap.size( ) > 0 )
			novelInfoS = NovelDBJob::removeSubName( novelInfoS
				, lenSubStrKeyMap
				, [&currentTime]( ) {
					auto cur = std::chrono::system_clock::now( );
					auto sep = cur - currentTime;
					auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
					if( second < 2 )
						return;
					std::cout << u8"正在删除子字符串关键字" << std::endl;
					currentTime = cur;
				} );
		std::cout << u8"开始写入小说到文件" << std::endl;
		std::vector< cylHtmlTools::HtmlWorkThread * > threads;
		for( auto &str : *writeFilePaths ) {
			size_t fileLenKeyMapSize = fileLenFindStrKeyMap.size( ); // 文件查找映射数量
			size_t lenFindAllKeySize = lenFindStrKeyMap.size( ); // 总查找长度
			for( auto iterator = novelHostMap.begin( ), end = novelHostMap.end( ); iterator != end; ++iterator )
				for( auto vit = iterator->second->begin( ), ven = iterator->second->end( ); vit != ven; ++vit ) {
					cylHtmlTools::HtmlWorkThread *writeThread = new cylHtmlTools::HtmlWorkThread;
					QString writePath;
					writePath.append( str ).append( QDir::separator( ) ).append( iterator->first );
					std::cout << u8"\t写入目录 " << writePath.toStdString( ).c_str( ) << std::endl;
					writeThread->setCurrentThreadRun( [=]( ) {
						NovelDBJob::writeFile( QString::fromStdString( str ), iterator->first, vit->first, *vit->second );
					} );
					threads.emplace_back( writeThread );
					writeThread->start( );
				}
		}
		do {
			auto begin = threads.begin( );
			auto end = threads.end( );
			if( begin == end )
				break;
			do {
				cylHtmlTools::HtmlWorkThread *writeThread = *begin;
				if( writeThread->isFinish( ) ) {
					threads.erase( begin );
					delete writeThread;
					break;
				}
				instance->processEvents( );
				auto cur = std::chrono::system_clock::now( );
				auto sep = cur - currentTime;
				auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
				++begin;
				if( second < 2 )
					continue;
				std::cout << u8"正在写入文件" << std::endl;
				currentTime = cur;
			} while( begin != end );
		} while( true );
	}

	//return instance->exec( );
	return 0;
}
