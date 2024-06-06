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

	QString version = QString( "\nqt 版本 %1\n编译版本 %2\n程序位数: %3 " ).arg( qVersion( ) ).arg( compilerString ).arg( QString::number( QSysInfo::WordSize ) );

	QString description = u8"小说爬虫命令行版本";
	auto argParser = cylStd::ArgParser::parser( argc, argv );

	QString typeFilePath;

	if( argParser->getOptionValues( "-name" ) )
		std::cout << instance->applicationName( ).toStdString( ).c_str( ) << std::endl;

	auto novelTypeFile = argParser->getOptionValues( "-t" );
	if( novelTypeFile )
		for( auto &path : *novelTypeFile ) {
			QFileInfo fileInfo( QString::fromStdString( path ) );
			if( fileInfo.exists( ) && fileInfo.isFile( ) ) {
				typeFilePath = fileInfo.absoluteFilePath( );
				break;
			}
		}
	QString appPathDir = instance->applicationDirPath( );
	QString appPath = instance->applicationFilePath( );
	QString workPath = QDir::currentPath( );
	if( typeFilePath.isEmpty( ) )
		typeFilePath.append( appPathDir ).append( QDir::separator( ) ).append( "progress" ).append( QDir::separator( ) ).append( "ini" ).append( QDir::separator( ) ).append( "ReptileIntroduce.ini" );

	if( argParser->getOptionValues( "-v" ) ) {
		std::cout << u8"输出命令行参数" << std::endl << version.toStdString( ).c_str( ) << std::endl;
		auto map = argParser->getPairs( );
		for( auto &it : map ) {
			std::cout << u8"名称:" << it.first << std::endl;
			for( auto &value : *it.second )
				std::cout << u8"\t值:" << value << std::endl;
		}
		std::cout << "------------";
	}

	if( argParser->getOptionValues( "-h" ) )
		std::cout << "========="
			"\n\t"
			u8"帮助:"
			"\n"
			"-l" "\t\t" u8"插件路径[,插件路径1[,插件路径2[,...]]]""\t" u8"加载文件到该程序"
			"\n"
			"-s" "\t\t" u8"插件路径[,插件路径1[,插件路径2[,...]]]""\t" u8"开始该文件的工作程序"
			"\n"
			"-as" "\t\t" u8"开始所有已加载的插件工作程序"
			"\n"
			"-p" "\t\t" u8"路径" "\t" u8"指定输出的路径"
			"\n"
			"-t" "\t\t" u8"路径" "\t" u8"指定获取的小说类型配置文件路径-单个类型为一行"
			"\n"
			"-url" "\t\t" u8"输出加载的插件指向的网络"
			"\n"
			"-name" "\t\t" u8"输出程序名称"
			"\n"
			"=========" << std::endl;
	auto pathValues = argParser->getOptionValues( "-p" );
	QString path = workPath + QDir::separator( ) + "cmd_download_novels_info" + QDir::separator( );
	if( pathValues )
		path = QString::fromStdString( pathValues->at( 0 ) );
	auto loadPathOption = argParser->getOptionValues( "-l" );
	std::vector< QString > novelPlugPath;
	std::vector< QString > runPlugPath;

	size_t count = 0;
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
				if( novelTypeFile ) {
					cmd.append( " -t " );
					cmd.append( typeFilePath );
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
					if( novelTypeFile )
						novelNetJob->setInPath( typeFilePath );
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
				if( novelTypeFile ) {
					cmd.append( " -t " );
					cmd.append( typeFilePath );
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
					novelNetJob->setInPath( typeFilePath );
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
	std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > > lenSubStrKeyMap;
	cylHtmlTools::HtmlWorkThread ingSubNameThread;
	auto inNameKeys = argParser->getOptionValues( "-isn" ); // 忽略名称
	auto inNameFiles = argParser->getOptionValues( "-isnf" ); // 忽略文件路径
	loadingSubKeyFiles( inNameKeys, inNameFiles, ingSubNameThread, lenSubStrKeyMap );

	std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > > lenEquStrKeyMap;
	cylHtmlTools::HtmlWorkThread ingEquNameThread;
	auto inEquNameKeys = argParser->getOptionValues( "-ien" ); // 忽略名称
	auto inEquNameFiles = argParser->getOptionValues( "-ienf" ); // 忽略文件路径
	loadingEquKeyFiles( inEquNameKeys, inEquNameFiles, ingEquNameThread, lenEquStrKeyMap ); // 处理完全匹配名称忽略文件

	std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > > lenFindStrKeyMap; // 查找文件的映射
	cylHtmlTools::HtmlWorkThread ingFindStrKeyThread;
	auto findKeysOptions = argParser->getOptionValues( "-fk" ); // 小说查找名称
	auto findNameFiles = argParser->getOptionValues( "-fkf" ); // 小说查找文件路径
	loadFindKeyFiles( findKeysOptions, findNameFiles, ingFindStrKeyThread, lenFindStrKeyMap ); // 处理查找文件

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


	auto dbPaths = argParser->getOptionValues( "-rdb" ); // 是否存在导出
	auto writeFilePaths = argParser->getOptionValues( "-w" ); // 是否存在导出

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
				, lenSubStrKeyMap
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
