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
#include <ioFile/IOFile.h>
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
	QMutex qMutex; // 工作锁
	QMutex coutMutex; // 输出锁
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
	std::shared_ptr< cylStd::ArgParser > argParser = cylStd::ArgParser::parser( argc, argv );

	std::vector< QString > requesTypeNameVector;

	if( argParser->getOptionValues( "-name" ) )
		std::cout << instance->applicationName( ).toStdString( ).c_str( ) << std::endl;

	auto writeFilePaths = argParser->getOptionValues( "-w" ); // 导出目录
	auto isOutAll = argParser->getOptionValues( "-out" ); // 是否存在导出全部
	std::shared_ptr< std::vector< cylStd::ArgParser::String > > requetTypeNameOption = argParser->getOptionValues( "-t" );
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
			"-edb" u8"\n\t  "
			u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
			u8"说明:\n\t\t" u8"导出数据库"
			"-ex" u8"\n\t  "
			u8"  参数" "\n\t\t" u8"天数" u8"\n\t  "
			u8"说明:\n\t\t" u8"校验过期，将会删除过期小说"
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
			runPrintfUrlSubProcess( appPath, qMutex, count, runPlugPath, argParser, requetTypeNameOption, pathValues, novelPlugPath );
			loadPrintfUrlLastPlugOnThisProcess( qMutex, count, path, runPlugPath, requetTypeNameOption, requesTypeNameVector, novelPlugPath, __FILE__, __FUNCTION__, __LINE__ );
		} else { // 除了首个爬虫，其他爬虫均有子程序进行
			count = novelPlugPath.size( );
			runSubProcess( appPath, qMutex, count, runPlugPath, argParser, requetTypeNameOption, pathValues, novelPlugPath );
			loadLastPlugOnThisProcess( qMutex, count, path, runPlugPath, requetTypeNameOption, requesTypeNameVector, novelPlugPath, __FILE__, __FUNCTION__, __LINE__ );
		}
	}
	currentTime = std::chrono::system_clock::now( );
	auto dbPaths = argParser->getOptionValues( "-rdb" ); // 是否存在导出
	/// 忽略子名称
	LenMap lenSubStrKeyMap; // 子字符串匹配
	/// 忽略名称
	LenMap lenEquStrKeyMap; // 完全匹配映射
	/// 关键字查找
	FileLenMap fileLenFindStrKeyMap; // 查找文件的映射-文件<长度，名称列表>

	if( dbPaths && writeFilePaths ) { // 如果存在 -rdb 与 -w 选项，那么可以使用过滤选项
		auto inNameKeys = argParser->getOptionValues( "-isn" ); // 忽略名称
		auto inNameFiles = argParser->getOptionValues( "-isnf" ); // 忽略文件路径
		std::cout << u8"开始获取子字符串文件" << std::endl;
		loadingSubKeyFiles( inNameKeys, inNameFiles, lenSubStrKeyMap );

		auto inEquNameKeys = argParser->getOptionValues( "-ien" ); // 忽略名称
		auto inEquNameFiles = argParser->getOptionValues( "-ienf" ); // 忽略文件路径
		std::cout << u8"开始获取对等字符串文件" << std::endl;
		loadingEquKeyFiles( inEquNameKeys, inEquNameFiles, lenEquStrKeyMap ); // 处理完全匹配名称忽略文件

		auto findKeysOptions = argParser->getOptionValues( "-fk" ); // 小说查找名称
		auto findNameFiles = argParser->getOptionValues( "-fkf" ); // 小说查找文件路径
		std::cout << u8"开始获取查找文件字符串" << std::endl;
		loadFindKeyFiles( findKeysOptions, findNameFiles, applicationName, fileLenFindStrKeyMap ); // 处理查找文件
	}

	do {
		instance->processEvents( );
		if( count ) {
			auto cur = std::chrono::system_clock::now( );
			auto sep = cur - currentTime;
			auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
			if( second > 2 ) {
				QMutexLocker lock( &coutMutex );
				std::cout << u8"准备工作进行当中 [" << count << "] ..." << std::endl;
				currentTime = cur;
			}
			continue;
		}
		break;
	} while( true );

	cylHtmlTools::HtmlWorkThreadPool readLinkDBSThreadPools;
	if( dbPaths && writeFilePaths ) {
		auto expireOption = argParser->getOptionValues( "-ex" );
		size_t expire = 0; // 过期，如果是 0，表示不删除
		if( expireOption ) {
			for( auto str : *expireOption ) {
				auto expireStr = QString::fromLocal8Bit( str );
				bool isOk = false;
				auto day = expireStr.toULongLong( &isOk );
				if( isOk ) {
					expire = day;
					break;
				}
			}
		}
		std::cout << u8"检测到 -rdb 与 -w 选项" << std::endl;
		interfacePlugsType::Vector_INovelInfoSPtr readDBNovels;
		for( auto &str : *dbPaths ) {
			auto pathInfo = Path::getPathInfo( QString::fromLocal8Bit( str ) );
			for( auto &file : pathInfo.second ) {
				readLinkDBSThreadPools.appendWork( [
						file,expire,
						&readDBNovels, &qMutex, &coutMutex, &lenEquStrKeyMap, &lenSubStrKeyMap, &currentTime, &count
					]( cylHtmlTools::HtmlWorkThread * ) ->void {
						QString currentFilePtah = file.getCurrentFilePtah( );
						std::string stdString = currentFilePtah.toStdString( );
						coutMutex.lock( );
						std::cout << u8"检测路径 " << stdString << std::endl;
						coutMutex.unlock( );
						QFileInfo fileInfo( currentFilePtah );
						if( fileInfo.isFile( ) ) {
							QString absolutePath = fileInfo.absoluteDir( ).absolutePath( );
							QString fileName = fileInfo.fileName( );
							auto novelInfoSPtrShared = NovelDBJob::readDB( nullptr
								, absolutePath
								, fileName );
							if( !novelInfoSPtrShared ) {
								coutMutex.lock( );
								std::cout << u8"路径 " << stdString << u8" 无法加载小说列表" << std::endl;
								coutMutex.unlock( );
								return;
							}
							if( expire ) {
								auto isExpire = NovelDBJob::novelVectorIsExpire( expire, *novelInfoSPtrShared );
								if( isExpire.size( ) > 0 ) {
									auto novelUrlVector = NovelDBJob::novelVectorGetNovleUrl( isExpire );
									NovelDBJob::removeNovelVectorDB( nullptr, absolutePath, fileName, novelUrlVector );
									*novelInfoSPtrShared = NovelDBJob::formVectorINovelInfoRemoveVectorINovelInfo( *novelInfoSPtrShared, isExpire );
								}
							}
							if( novelInfoSPtrShared ) {
								count = novelInfoSPtrShared->size( );
								coutMutex.lock( );
								std::cout << u8"读取数据库成功 : " << stdString << u8"\n\t有效小说数量为 " << count << std::endl;
								coutMutex.unlock( );
								if( lenEquStrKeyMap.size( ) > 0 ) {
									coutMutex.lock( );
									std::cout << u8"删除匹配的符串关 : " << stdString << std::endl;
									coutMutex.unlock( );
									auto result = NovelDBJob::removeEquName( *novelInfoSPtrShared, lenEquStrKeyMap );
									novelInfoSPtrShared->clear( );
									novelInfoSPtrShared->insert( novelInfoSPtrShared->end( ), result.begin( ), result.end( ) );
									count = novelInfoSPtrShared->size( );
									if( count == 0 ) {
										std::cout << u8"\t删除匹配的符串关键字后，有效小说数量为 0，退出操作" << std::endl;
										return;
									} else
										std::cout << u8"\t有效小说数量为 " << count << std::endl;
								}
								if( lenSubStrKeyMap.size( ) > 0 ) {
									coutMutex.lock( );
									std::cout << u8"删除匹配的子符串关 : " << stdString << std::endl;
									coutMutex.unlock( );
									auto result = NovelDBJob::removeSubName( *novelInfoSPtrShared, lenSubStrKeyMap );
									novelInfoSPtrShared->clear( );
									novelInfoSPtrShared->insert( novelInfoSPtrShared->end( ), result.begin( ), result.end( ) );
									count = novelInfoSPtrShared->size( );
									if( count == 0 ) {
										std::cout << u8"\t删除子字符串关键字后，有效小说数量为 0，退出操作" << std::endl;
										return;
									} else
										std::cout << u8"\t有效小说数量为 " << count << std::endl;
								}
								coutMutex.lock( );
								std::cout << u8"加入小说进行中...:" << stdString << std::endl;
								coutMutex.unlock( );
								QMutexLocker lock( &qMutex );
								readDBNovels.insert( readDBNovels.begin( ), novelInfoSPtrShared->begin( ), novelInfoSPtrShared->end( ) );
							}
						} else {
							QMutexLocker lock( &coutMutex );
							std::cout << u8"路径不正确，该路径为目录...:" << stdString << std::endl;
						}
					} );
			}
		}
		readLinkDBSThreadPools.start( 8
			, [&]( cylHtmlTools::HtmlWorkThreadPool *, const size_t &residueWorks, const size_t &currentWorks ) {
				QMutexLocker lock( &coutMutex );
				auto cur = std::chrono::system_clock::now( );
				auto sep = cur - currentTime;
				auto second = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
				if( second < 10 )
					return;
				std::cout << u8"正在读取数据库...[" << residueWorks << "]=[" << currentWorks << "]\n";
				currentTime = cur;
			} );
		readLinkDBSThreadPools.waiteOverJob( );
		std::cout << u8"数据库读取完毕" << std::endl;
		count = readDBNovels.size( );
		if( count == 0 ) {
			std::cout << u8"有效小说数量为 0，退出操作" << std::endl;
			return 0;
		} else
			std::cout << u8"有效小说数量为 " << count << std::endl;

		cylHtmlTools::HtmlWorkThreadPool threadPool;

		QMutex saveMapMutex;
		std::unordered_map< QString, interfacePlugsType::Vector_INovelInfoSPtr_Shared > findFileKeyResult; // 文件返回
		for( auto &iterator : fileLenFindStrKeyMap ) {
			threadPool.appendWork( [iterator, &readDBNovels,&findFileKeyResult,&saveMapMutex
				]( cylHtmlTools::HtmlWorkThread * ) {
					interfacePlugsType::Vector_INovelInfoSPtr_Shared vectorINovelInfoS( std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( ) );
					QFileInfo fileInfo( iterator.first );
					QString fileName = fileInfo.fileName( );
					for( auto &novelInfoShared : readDBNovels )
						if( NovelDBJob::findNovelKey( novelInfoShared, iterator.second ) )
							vectorINovelInfoS->emplace_back( novelInfoShared );
					saveMapMutex.lock( );
					findFileKeyResult.emplace( fileName, vectorINovelInfoS );
					saveMapMutex.unlock( );
				} );
		}
		threadPool.start( 32
			, [&]( cylHtmlTools::HtmlWorkThreadPool *, const size_t &residueWorks, const size_t &currentWorks ) {
				std::cout << u8"正在查找关键字，剩余任务数 [" << residueWorks << "]，进行任务数[" << currentWorks << ']' << std::endl;
				instance->processEvents( );
				std::this_thread::sleep_for( std::chrono::seconds( 10 ) );
			} );
		threadPool.waiteOverJob( );
		QPathManage removePath;
		auto exportDB = argParser->getOptionValues( "-edb" );
		if( exportDB ) {
			auto novelHostMap = NovelDBJob::decompose( readDBNovels
				, [&currentTime,&qMutex,&coutMutex]( ) {
					QMutexLocker lock( &coutMutex );
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
				qMutex.lock( );
				removePath.appendPath( writeFilePath );
				qMutex.unlock( );
				for( auto hostPair : novelHostMap ) {
					auto typeFilePathName = writeFilePath + hostPair.first + QDir::separator( );
					auto second = hostPair.second;
					for( auto typePair : *second ) {
						auto allFilePathName = typeFilePathName + typePair.first + ".txt";
						threadPool.appendWork( [=,&qMutex,&coutMutex,&removePath]( cylHtmlTools::HtmlWorkThread * ) {
							qMutex.lock( );
							bool isCreate = Path::creatFilePath( allFilePathName );
							qMutex.unlock( );
							if( isCreate ) {
								auto result = NovelDBJob::identical( *typePair.second );
								result = NovelDBJob::sort( result );
								auto list = NovelDBJob::getNovelNames( result );
								QStringList novelNameList( list.begin( ), list.end( ) );
								auto novelNameListJoin = novelNameList.join( "\n" );
								auto allContents = NovelDBJob::jionNovels( result ) + u8"\n+++++\t小说名称列表\t" + QString::number( novelNameList.size( ) ) + " 个\t+++++++\n" + novelNameListJoin + u8"\n++++++++++++\n";
								QFile writeFile( allFilePathName );
								if( writeFile.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
									writeFile.write( allContents.toUtf8( ) );
									writeFile.close( );
									coutMutex.lock( );
									std::cout << u8"导出数据库成功 : " << allFilePathName.toStdString( ) << std::endl;
									coutMutex.unlock( );
									qMutex.lock( );
									removePath.removePath( allFilePathName );
									qMutex.unlock( );
									return;
								}
							}
							coutMutex.lock( );
							std::cout << u8"导出数据库失败 : " << allFilePathName.toStdString( ) << std::endl;
							coutMutex.unlock( );
						} );
					}
				}
			}
		}


		auto fileMapIterator = findFileKeyResult.begin( );
		auto fileMapEnd = findFileKeyResult.end( );
		for( ; fileMapIterator != fileMapEnd; ++fileMapIterator ) {
			QString writeFilePath; // 写入小说

			auto novelHostMap = NovelDBJob::decompose( *fileMapIterator->second
				, [&currentTime,&qMutex,&coutMutex]( ) {
					QMutexLocker lock( &coutMutex );
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

				qMutex.lock( );
				removePath.appendPath( writeFilePath );
				qMutex.unlock( );
				QString fileName = QFileInfo( fileMapIterator->first ).fileName( );
				auto pairs = fileLenFindStrKeyMap.at( fileName );
				QStringList keys;
				for( auto &keyPairs : pairs )
					for( auto &key : *keyPairs.second )
						keys.append( QString::fromStdWString( key ) );
				auto writeKeyContes = u8"\n-------------\t匹配关键字\t" + QString::number( keys.size( ) ) + " 个\t-------------\n" + keys.join( ", " ) + u8"\n-------------\n";
				for( auto hostPair : novelHostMap ) {
					auto typeFilePathName = writeFilePath + hostPair.first + QDir::separator( );
					auto second = hostPair.second;
					for( auto typePair : *second ) {
						auto allFilePathName = typeFilePathName + typePair.first + ".txt";
						threadPool.appendWork( [=,&qMutex,&coutMutex,&removePath]( cylHtmlTools::HtmlWorkThread * ) {
							qint64 writeCount = 0;
							qMutex.lock( );
							bool isCreate = Path::creatFilePath( allFilePathName );
							qMutex.unlock( );
							if( isCreate ) {
								auto result = NovelDBJob::identical( *typePair.second );
								result = NovelDBJob::sort( result );
								auto list = NovelDBJob::getNovelNames( result );
								QStringList novelNameList( list.begin( ), list.end( ) );
								auto novelNameListJoin = novelNameList.join( "\n" );
								auto allContents = NovelDBJob::jionNovels( result ) + u8"\n+++++\t小说名称列表\t" + QString::number( novelNameList.size( ) ) + " 个\t+++++++\n" + novelNameListJoin + u8"\n++++++++++++\n" + writeKeyContes;
								QFile writeFile( allFilePathName );
								if( writeFile.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
									writeCount = writeFile.write( allContents.toUtf8( ) );
									writeFile.close( );
									qMutex.lock( );
									removePath.removePath( allFilePathName );
									qMutex.unlock( );
									QMutexLocker locker( &coutMutex );
									std::cout << u8"导出查找结果 : " << allFilePathName.toStdString( ) << std::endl;
								}
							}
							if( writeCount == 0 )
								std::cout << u8"导出查找结果失败 : " << allFilePathName.toStdString( ) << std::endl;
						} );
					}

				}
			}

		}
		threadPool.start( 8
			, [&]( cylHtmlTools::HtmlWorkThreadPool *, const size_t &residueWorks, const size_t &currentWorks ) {
				instance->processEvents( );
				std::this_thread::sleep_for( std::chrono::seconds( 2 ) );
				QMutexLocker lock( &coutMutex );
				std::cout << u8"正在写入查找小说，剩余任务数 [" << residueWorks << "]，进行任务数[" << currentWorks << ']' << std::endl;
			} );
		threadPool.waiteOverJob( );
		std::vector< QString > emptyPtah = removePath.removeEmptyPtah( );
		for( auto &removeDirPath : emptyPtah )
			std::cout << u8"删除路径 : " << removeDirPath.toStdString( ) << std::endl;
		emptyPtah = removePath.removeEmptyPtah( );
		for( auto &removeDirPath : emptyPtah )
			std::cout << u8"删除路径 : " << removeDirPath.toStdString( ) << std::endl;
	}


	//return instance->exec( );
	return 0;
}
