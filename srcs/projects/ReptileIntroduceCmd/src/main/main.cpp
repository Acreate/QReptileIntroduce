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

#include "novelNetJob/NovelDBJob.h"
int main( int argc, char *argv[ ] ) {
	std::locale locale( "zh_CN.UTF8" );
	std::locale::global( locale );
	std::cout.imbue( locale );
	std::wcout.imbue( locale );

	QCoreApplication application( argc, argv );
	auto *instance = qApp;
	QString compilerString = getBuilderInfo( );

	QString version = QString( "\nqt 版本 %1\n编译版本 %2\n程序位数: %3 " ).arg( qVersion( ) ).arg( compilerString ).arg( QString::number( QSysInfo::WordSize ) );

	QString description = u8"小说爬虫命令行版本";
	auto argParser = cylStd::ArgParser::parser( argc, argv );

	QString typeFilePath;

	auto novelTypeFile = argParser->getOptionValues( "-t" );
	if( novelTypeFile )
		for( auto &path : *novelTypeFile ) {
			QFileInfo fileInfo( QString::fromStdString( path ) );
			if( fileInfo.exists( ) && fileInfo.isFile( ) ) {
				typeFilePath = fileInfo.absoluteFilePath( );
				break;
			}
		}
	if( typeFilePath.isEmpty( ) )
		typeFilePath.append( Project_Run_bin ).append( QDir::separator( ) ).append( "progress" ).append( QDir::separator( ) ).append( "ini" ).append( QDir::separator( ) ).append( "ReptileIntroduce.ini" );

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

	QString path = qApp->applicationDirPath( ) + QDir::separator( ) + "cmd_download_novels_info" + QDir::separator( );
	if( argParser->getOptionValues( "-name" ) )
		std::cout << instance->applicationName( ).toStdString( ).c_str( ) << std::endl;

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
			"-p" "\t\t" u8"指定输出的路径"
			"\n"
			"-url" "\t\t" u8"输出加载的插件指向的网络"
			"\n"
			"=========" << std::endl;
	auto pathValues = argParser->getOptionValues( "-p" );
	if( pathValues )
		path = QString::fromStdString( pathValues->at( 0 ) );
	auto optionValues = argParser->getOptionValues( "-l" );
	std::unordered_map< QString, std::shared_ptr< NovelNetJob > > novelNetJobs;
	size_t count = 0;
	if( optionValues ) {
		for( auto &value : *optionValues ) {
			auto absoluteFilePath = QFileInfo( QString::fromStdString( value ) ).absoluteFilePath( );
			auto mapEnd = novelNetJobs.end( );
			if( std::find_if( novelNetJobs.begin( )
				, mapEnd
				, [&]( const std::pair< QString, std::shared_ptr< NovelNetJob > > &pair ) {
					if( pair.first == absoluteFilePath )
						return true;
					return false;
				} ) == mapEnd ) {

				auto pair = LoadPlug::getIRequestNetInterface( absoluteFilePath );
				if( pair.second ) {
					auto novelNetJob = std::make_shared< NovelNetJob >( nullptr, pair.first, pair.second );
					QObject::connect( novelNetJob.get( )
						, &NovelNetJob::endJob
						, [&]( ) {
							--count;
							if( count == 0 )
								qApp->exit( 0 );
						} );

					novelNetJobs.emplace( absoluteFilePath, novelNetJob );
				}
			}
		}
	}

	if( novelNetJobs.size( ) != 0 ) {
		if( argParser->getOptionValues( "-url" ) ) {
			auto end = novelNetJobs.end( );
			auto iterator = novelNetJobs.begin( );
			for( ; iterator != end; ++iterator )
				std::cout << iterator->second->getUrl( ).toStdString( ).c_str( ) << std::endl;

		}
		if( argParser->getOptionValues( "-as" ) ) {
			auto end = novelNetJobs.end( );
			auto iterator = novelNetJobs.begin( );
			count = novelNetJobs.size( );
			for( ; iterator != end; ++iterator ) {
				iterator->second->setPath( path );
				iterator->second->setInPath( typeFilePath );
				iterator->second->start( );
			}
		} else {
			auto runPaths = argParser->getOptionValues( "-s" );
			if( runPaths ) {
				std::vector< std::shared_ptr< NovelNetJob > > runJobs;
				auto end = novelNetJobs.end( );
				for( auto startPath : *runPaths ) {
					auto absoluteFilePath = QFileInfo( QString::fromStdString( startPath ) ).absoluteFilePath( );
					auto iterator = novelNetJobs.begin( );
					for( ; iterator != end; ++iterator )
						if( iterator->first == absoluteFilePath ) {
							iterator->second->setPath( path );
							if( std::find_if( runJobs.begin( )
								, runJobs.end( )
								, [=]( std::shared_ptr< NovelNetJob > it ) {
									if( it == iterator->second )
										return true;
									return false;
								} ) == runJobs.end( ) )
								runJobs.emplace_back( iterator->second );
						}
				}
				count = runJobs.size( );
				for( auto nodeJob : runJobs )
					nodeJob->start( );
			}
		}
	}
	auto dbPaths = argParser->getOptionValues( "-rdb" ); // 是否存在导出
	auto writeFilePaths = argParser->getOptionValues( "-w" ); // 是否存在导出

	if( dbPaths && writeFilePaths ) {
		std::cout << u8"检测到 -rdb 与 -w 选项" << std::endl;
		auto currentTime = std::chrono::system_clock::now( );
		interfacePlugsType::Vector_INovelInfoSPtr novelInfoS;
		for( auto &str : *dbPaths ) {
			std::cout << u8"检测路径 " << str << std::endl;
			auto novelInfoSPtrShared = NovelDBJob::readDB( nullptr
				, QString::fromStdString( str )
				, [&currentTime]( ) {
					auto cur = std::chrono::system_clock::now( );
					auto sep = cur - currentTime;
					auto count = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
					if( count < 2 )
						return;
					std::cout << "正在读取数据库" << std::endl;
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
		std::cout << u8"检测 -in 与 -inf 选项" << std::endl;
		// 忽略选项
		std::vector< QString > ignoreNames;
		auto inNameKeys = argParser->getOptionValues( "-in" ); // 忽略名称
		auto inNameFiles = argParser->getOptionValues( "-inf" ); // 忽略文件路径
		if( inNameKeys )
			for( auto str : *inNameKeys )
				ignoreNames.emplace_back( QString::fromLocal8Bit( str ) );

		ignoreNames = vectorStrAdjustSubStr( ignoreNames );
		if( inNameFiles ) {
			std::cout << u8"检测到 -nnf 选项，正在读取文件内容" << std::endl;
			auto getBuff = readIngoreNameFiles( *inNameFiles );
			ignoreNames.insert( ignoreNames.end( ), getBuff.begin( ), getBuff.end( ) );
		}
		if( ignoreNames.size( ) > 0 ) {
			std::cout << u8"发现存在子字符串过滤功能被启用。" << std::endl;
			std::cout << u8"检测有效过滤子字符串" << std::endl;
			ignoreNames = vectorStrAdjustSubStr( ignoreNames );
			std::cout << u8"转换子字符串" << std::endl;
			auto wIgnoreNames = converToWString( ignoreNames );
			std::cout << u8"子字符串转换到长度映射表" << std::endl;
			auto lenKeyMap = vectorStrToLenKeyMap( wIgnoreNames );
			std::cout << u8"小说匹配长度映射表。并且删除匹配小说" << std::endl;
			currentTime = std::chrono::system_clock::now( );
			novelInfoS = NovelDBJob::removeSubName( novelInfoS
				, lenKeyMap
				, [&currentTime]( ) {
					auto cur = std::chrono::system_clock::now( );
					auto sep = cur - currentTime;
					auto count = std::chrono::duration_cast< std::chrono::seconds >( sep ).count( );
					if( count < 2 )
						return;
					std::cout << "正在删除子字符串关键字" << std::endl;
					currentTime = cur;
				} );
		}

		auto novelHostMap = NovelDBJob::decompose( novelInfoS );
		for( auto &str : *writeFilePaths )
			for( auto iterator = novelHostMap.begin( ), end = novelHostMap.end( ); iterator != end; ++iterator )
				for( auto vit = iterator->second->begin( ), ven = iterator->second->end( ); vit != ven; ++vit )
					NovelDBJob::writeFile( QString::fromStdString( str ), iterator->first, vit->first, *vit->second );

	}
	return 0;
	return instance->exec( );
}
