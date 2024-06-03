﻿#include <QCoreApplication>
#include <QFileInfo>
#include "../auto_generate_files/macro/cmake_to_c_cpp_header_env.h"
#include "argParser/ArgParser.h"
#include "nameSpace/cylStd.h"
#include "novelNetJob/NovelNetJob.h"
#include "path/Dir.h"
#include "plug/LoadPlug.h"
QString getBuilderInfo( ) {

	QString compilerString = "<unknown>";
#if defined(Q_CC_CLANG)
    QString isAppleString;
#if defined(__apple_build_version__)
    isAppleString = QLatin1String(" (Apple)");
#endif
    compilerString = QLatin1String("Clang ") + QString::number(__clang_major__) + QLatin1Char('.') + QString::number(__clang_minor__) + isAppleString;
#elif defined(Q_CC_GNU)
    compilerString = QLatin1String("GCC ") + QLatin1String(__VERSION__);
#elif defined(Q_CC_MSVC)
	if( _MSC_VER > 1999 ) {
		compilerString = QLatin1String( "MSVC <unknown>" );
	} else if( _MSC_VER >= 1930 ) {
		compilerString = QLatin1String( "MSVC 2022" );
	} else if( _MSC_VER >= 1920 ) {
		compilerString = QLatin1String( "MSVC 2019" );
	} else if( _MSC_VER >= 1910 ) {
		compilerString = QLatin1String( "MSVC 2017" );
	} else if( _MSC_VER >= 1900 ) {
		compilerString = QLatin1String( "MSVC 2015" );
	} else if( _MSC_VER >= 1800 ) {
		compilerString = QLatin1String( "MSVC 2013" );
	} else if( _MSC_VER >= 1700 ) {
		compilerString = QLatin1String( "MSVC 2012" );
	} else if( _MSC_VER >= 1600 ) {
		compilerString = QLatin1String( "MSVC 2010" );
	} else {
		compilerString = QLatin1String( "MSVC <old>" );
	}
#endif
	return compilerString;
}
#include <iostream>
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

	if( novelNetJobs.size( ) == 0 ) {
		return -1;
	} else {
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
			} else
				return 0;
		}
	}

	return instance->exec( );
}