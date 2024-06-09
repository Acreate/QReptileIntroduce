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
#include <htmls/htmlTools/HtmlWorkThread/HtmlWorkThreadPool.h>
#include <htmls/htmlTools/HtmlWorkThread/HtmlWorkThread.h>
#include <QSharedMemory>

#include "interface/INovelInfo.h"
#include "novelNetJob/NovelDBJob.h"
#include "path/Path.h"
int main( int argc, char *argv[ ] ) {
	std::locale locale( "zh_CN.UTF8" );
	std::locale::global( locale );
	std::cout.imbue( locale );
	std::wcout.imbue( locale );
	QMutex qMutex;
	auto currentTime = std::chrono::system_clock::now( );

	QCoreApplication application( argc, argv );
	auto *instance = qApp;// 指定一个键，这个键用来标识共享内存区域
	QString applicationDirPath = instance->applicationDirPath( ); // 当前 exe 所在目录
	QString applicationBaseName = instance->applicationName( ); // 当前 exe 名称
	QString applicationFilePath = instance->applicationFilePath( ); // 当前 exe 文件全路径名
	QString applicationName = QFileInfo( applicationFilePath ).fileName( ); // 当前 exe 名称，带后缀
	QString appRunCurrentPath = QDir::currentPath( ); // 当前运行目录

	QString compilerString = getBuilderInfo( );


	QString description = u8"小说爬虫命令行版本";
	auto argParser = cylStd::ArgParser::parser( argc, argv );

	std::vector< QString > requesTypeNameVector;

	if( argParser->getOptionValues( "-name" ) )
		std::cout << instance->applicationName( ).toStdString( ).c_str( ) << std::endl;

	auto writeFilePaths = argParser->getOptionValues( "-w" ); // 导出目录
	auto isOutAll = argParser->getOptionValues( "-out" ); // 是否存在导出全部
	auto requetTypeNameOption = argParser->getOptionValues( "-t" );
	if( requetTypeNameOption )
		for( auto &path : *requetTypeNameOption ) {
			auto pathInfo = Path::getPathInfo( QString::fromStdString( path ) );
			for( auto fileInfo : pathInfo.second ) {
				auto absoluteFilePath = fileInfo.getCurrentFilePtah( );
				auto typeKeyS = readIngoreNameFile( absoluteFilePath );
				typeKeyS = vectorStrduplicate( typeKeyS );
				if( writeFilePaths )
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
	currentTime = std::chrono::system_clock::now( );
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
	currentTime = std::chrono::system_clock::now( );
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
	currentTime = std::chrono::system_clock::now( );
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
			count = novelPlugPath.size( );
			QStringList cmd;
			auto optionToString = cylStd::ArgParser::converOptionToString( requetTypeNameOption );
			for( size_t index = 1; index < count; ++index ) {
				// 加载插件
				QString plugFilePath = novelPlugPath.at( index );
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
				if( findVector( runPlugPath, plugFilePath ) ) {
					cmd.append( " -s " );
					cmd.append( plugFilePath );
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
						subProcess->deleteLater( );
						QMutexLocker lock( &qMutex );
						--count;
					} );

				subProcess->start( appPath, cmd );
				cmd.clear( );
			}

			// 加载第一个插件
			QString plugFilePath = novelPlugPath.front( );
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
	currentTime = std::chrono::system_clock::now( );
	auto dbPaths = argParser->getOptionValues( "-rdb" ); // 是否存在导出
	/// 忽略子名称
	LenMap lenSubStrKeyMap; // 子字符串匹配
	cylHtmlTools::HtmlWorkThread ingSubNameThread;  // 子字符串匹配线程
	/// 忽略名称
	LenMap lenEquStrKeyMap; // 完全匹配映射
	cylHtmlTools::HtmlWorkThread ingEquNameThread; // 完全匹配线程
	/// 关键字查找
	FileLenMap fileLenFindStrKeyMap; // 查找文件的映射-文件<长度，名称列表>
	cylHtmlTools::HtmlWorkThread findStrKeyThread;

	if( dbPaths && writeFilePaths ) { // 如果存在 -rdb 与 -w 选项，那么可以使用过滤选项
		auto inNameKeys = argParser->getOptionValues( "-isn" ); // 忽略名称
		auto inNameFiles = argParser->getOptionValues( "-isnf" ); // 忽略文件路径
		loadingSubKeyFiles( inNameKeys, inNameFiles, ingSubNameThread, lenSubStrKeyMap );

		auto inEquNameKeys = argParser->getOptionValues( "-ien" ); // 忽略名称
		auto inEquNameFiles = argParser->getOptionValues( "-ienf" ); // 忽略文件路径
		loadingEquKeyFiles( inEquNameKeys, inEquNameFiles, ingEquNameThread, lenEquStrKeyMap ); // 处理完全匹配名称忽略文件

		auto findKeysOptions = argParser->getOptionValues( "-fk" ); // 小说查找名称
		auto findNameFiles = argParser->getOptionValues( "-fkf" ); // 小说查找文件路径
		loadFindKeyFiles( findKeysOptions, findNameFiles, findStrKeyThread, applicationName, fileLenFindStrKeyMap ); // 处理查找文件
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
		interfacePlugsType::Vector_INovelInfoSPtr readDBNovels;
		for( auto &str : *dbPaths ) {
			std::cout << u8"检测路径 " << str << std::endl;
			auto novelInfoSPtrShared = NovelDBJob::readDB( nullptr
				, QString::fromStdString( str )
				, [&currentTime,&qMutex]( ) {
					QMutexLocker lock( &qMutex );
					auto cur = std::chrono::system_clock::now( );
					auto sep = cur - currentTime;
					auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
					if( second < 2 )
						return;
					std::cout << u8"正在读取数据库" << std::endl;
					currentTime = cur;
				} );
			if( novelInfoSPtrShared ) {
				std::cout << u8"加入小说进行中... " << str << std::endl;
				readDBNovels.insert( readDBNovels.begin( ), novelInfoSPtrShared->begin( ), novelInfoSPtrShared->end( ) );
			}
		}
		std::cout << u8"数据库读取完毕" << std::endl;
		count = readDBNovels.size( );
		if( count == 0 ) {
			std::cout << u8"有效小说数量为 0，退出操作" << std::endl;
			return 0;
		} else
			std::cout << u8"有效小说数量为 " << count << std::endl;
		/*	std::cout << u8"开始分解小说到域名" << std::endl;
		auto novelHostMap = NovelDBJob::decompose( readDBNovels
				, [&currentTime,&qMutex]( ) {
					QMutexLocker lock( &qMutex );
					auto cur = std::chrono::system_clock::now( );
					auto sep = cur - currentTime;
					auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
					if( second < 2 )
						return;
					std::cout << u8"正在分解小说到域名" << std::endl;
					currentTime = cur;
				} );*/
		while( ingSubNameThread.isRun( ) ) {
			std::cout << u8"等待忽略选项完成" << std::endl;
			instance->processEvents( );
		}
		while( ingEquNameThread.isRun( ) ) {
			std::cout << u8"等待相等选项完成" << std::endl;
			instance->processEvents( );
		}
		while( findStrKeyThread.isRun( ) ) {
			std::cout << u8"等待查找选项完成" << std::endl;
			instance->processEvents( );
		}
		if( lenEquStrKeyMap.size( ) > 0 )
			readDBNovels = NovelDBJob::removeEquName( readDBNovels
				, lenEquStrKeyMap
				, [&currentTime,&qMutex]( ) {
					QMutexLocker lock( &qMutex );
					auto cur = std::chrono::system_clock::now( );
					auto sep = cur - currentTime;
					auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
					if( second < 2 )
						return;
					std::cout << u8"正在删除匹配的符串关键字" << std::endl;
					currentTime = cur;
				} );

		if( readDBNovels.size( ) == 0 ) {
			std::cout << u8"删除匹配的符串关键字后，有效小说数量为 0，退出操作" << std::endl;
			return 0;
		}
		if( lenSubStrKeyMap.size( ) > 0 )
			readDBNovels = NovelDBJob::removeSubName( readDBNovels
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
		if( readDBNovels.size( ) == 0 ) {
			std::cout << u8"删除子字符串关键字后，有效小说数量为 0，退出操作" << std::endl;
			return 0;
		}
		cylHtmlTools::HtmlWorkThreadPool threadPool;
		QMutex saveMapMutex;
		std::unordered_map< QString, interfacePlugsType::Vector_INovelInfoSPtr_Shared > findFileKeyResult; // 文件返回
		for( auto &novelInfoShared : readDBNovels ) {
			auto iterator = fileLenFindStrKeyMap.begin( );
			auto end = fileLenFindStrKeyMap.end( );
			for( ; iterator != end; ++iterator ) {
				QFileInfo fileInfo( iterator->first );
				QString fileName = fileInfo.fileName( );
				findFileKeyResult.emplace( fileName, std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( ) );
				threadPool.appendWork( [
						iterator,novelInfoShared,fileName
						,&findFileKeyResult,&saveMapMutex
					]( cylHtmlTools::HtmlWorkThread * ) {
						if( NovelDBJob::findNovelKey( novelInfoShared, iterator->second ) ) {
							// 找到引用指针
							for( auto saveIterator = findFileKeyResult.begin( ), saveEnd = findFileKeyResult.end( ); saveIterator != saveEnd; ++saveIterator )
								if( saveIterator->first == fileName ) {
									QMutexLocker< QMutex > locker( &saveMapMutex );
									interfacePlugsType::HtmlDocString name, url;
									auto novelInfo = novelInfoShared.get( );
									novelInfo->getNovelName( &name );
									novelInfo->getNovelUrl( &url );
									std::wcout << L"找到关键字 (" << url << L") 名称:\"" << name << '\"' << std::endl;
									saveIterator->second->emplace_back( novelInfoShared );
									return;
								}
						}
					} );
			}
		}
		threadPool.start( 32
			, [&]( cylHtmlTools::HtmlWorkThreadPool *, const size_t &residueWorks, const size_t &currentWorks ) {
				std::cout << u8"正在查找关键字，剩余任务数 [" << residueWorks << "]，进行任务数[" << currentWorks << ']' << std::endl;
				instance->processEvents( );
				std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
			} );
		threadPool.waiteOverJob( );

		auto exportDB = argParser->getOptionValues( "-edb" );
		if( exportDB ) {
			auto novelHostMap = NovelDBJob::decompose( readDBNovels
				, [&currentTime,&qMutex]( ) {
					QMutexLocker lock( &qMutex );
					auto cur = std::chrono::system_clock::now( );
					auto sep = cur - currentTime;
					auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
					if( second < 2 )
						return;
					std::cout << u8"正在分解小说到域名" << std::endl;
					currentTime = cur;
				} );
			for( auto writeRoot : *exportDB ) {
				QString writeFilePath = QString::fromLocal8Bit( writeRoot ) + QDir::separator( ) + u8"db_export_novels" + QDir::separator( );
				for( auto hostPair : novelHostMap ) {
					auto typeFilePathName = writeFilePath + hostPair.first + QDir::separator( );
					auto second = hostPair.second;
					for( auto typePair : *second ) {
						auto allFilePathName = typeFilePathName + typePair.first + ".txt";
						threadPool.appendWork( [=,&qMutex]( cylHtmlTools::HtmlWorkThread * ) {
							NovelDBJob::writeFile( allFilePathName, *typePair.second );
							QMutexLocker locker( &qMutex );
							std::cout << u8"导出数据库 : " << allFilePathName.toStdString( ) << std::endl;
						} );
					}
				}
			}
		}

		auto fileMapIterator = findFileKeyResult.begin( );
		auto fileMapEnd = findFileKeyResult.end( );
		for( ; fileMapIterator != fileMapEnd; ++fileMapIterator ) {
			QString writeFilePath;
			auto novelHostMap = NovelDBJob::decompose( *fileMapIterator->second
				, [&currentTime,&qMutex]( ) {
					QMutexLocker lock( &qMutex );
					auto cur = std::chrono::system_clock::now( );
					auto sep = cur - currentTime;
					auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
					if( second < 2 )
						return;
					std::cout << u8"正在分解小说到域名" << std::endl;
					currentTime = cur;
				} );
			for( auto writeRoot : *writeFilePaths ) {
				writeFilePath = QString::fromLocal8Bit( writeRoot ) + QDir::separator( ) + u8"find_keys_result" + QDir::separator( ) + fileMapIterator->first + QDir::separator( );
				for( auto hostPair : novelHostMap ) {
					auto typeFilePathName = writeFilePath + hostPair.first + QDir::separator( );
					auto second = hostPair.second;
					for( auto typePair : *second ) {
						auto allFilePathName = typeFilePathName + typePair.first + ".txt";
						threadPool.appendWork( [=,&qMutex]( cylHtmlTools::HtmlWorkThread * ) {
							NovelDBJob::writeFile( allFilePathName, *typePair.second );
							QMutexLocker locker( &qMutex );
							std::cout << u8"导出查找结果 : " << allFilePathName.toStdString( ) << std::endl;
						} );
					}
				}
			}
		}
		threadPool.start( 8
			, [&]( cylHtmlTools::HtmlWorkThreadPool *, const size_t &residueWorks, const size_t &currentWorks ) {
				instance->processEvents( );
				std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
				QMutexLocker lock( &qMutex );
				std::cout << u8"正在写入查找小说，剩余任务数 [" << residueWorks << "]，进行任务数[" << currentWorks << ']' << std::endl;
			} );
		threadPool.waiteOverJob( );

	}


	//return instance->exec( );
	return 0;
}
