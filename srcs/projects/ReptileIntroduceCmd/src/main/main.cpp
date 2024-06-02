#include <QCoreApplication>
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
int main( int argc, char *argv[ ] ) {
	QCoreApplication application( argc, argv );
	auto *instance = qApp;
	QString compilerString = getBuilderInfo( );

	QString version = QString( "\nqt 版本 %1\n编译版本 %2\n程序位数: %3 " ).arg( qVersion( ) ).arg( compilerString ).arg( QString::number( QSysInfo::WordSize ) );

	QString description = u8"小说爬虫命令行版本";
	auto argParser = cylStd::ArgParser::parser( argc, argv );
	QString path = qApp->applicationDirPath( ) + QDir::separator( ) + "cmd_download_novels_info" + QDir::separator( );
	if( argParser->getOptionValues( "-h" ).size( ) > 0 )
		qDebug( ) << "========="
			"\n\t"
			u8"帮助:"
			"\n"
			"-l" "\t\t" u8"插件路径[,插件路径1[,插件路径2[,...]]]""\t" u8"加载文件到该程序";
	"-s" "\t\t" u8"插件路径[,插件路径1[,插件路径2[,...]]]""\t" u8"开始该文件的工作程序";
	"-as" "\t\t" u8"开始所有已加载的插件工作程序";
	"-p" "\t\t" u8"指定输出的路径";
	auto pathValues = argParser->getOptionValues( "-p" );
	if( pathValues.size( ) > 1 )
		path = QString::fromStdString( pathValues[ 0 ] );
	auto optionValues = argParser->getOptionValues( "-l" );
	std::unordered_map< QString, std::shared_ptr< NovelNetJob > > novelNetJobs;
	size_t count = 0;
	for( auto &value : optionValues ) {
		auto absoluteFilePath = QFileInfo( QString::fromStdString( value ) ).absoluteFilePath( );
		auto pair = LoadPlug::getIRequestNetInterface( absoluteFilePath );
		if( pair.second ) {
			auto novelNetJob = std::make_shared< NovelNetJob >( nullptr, pair.first, pair.second );
			novelNetJobs.emplace( absoluteFilePath, novelNetJob );
			qDebug( ) << value.c_str( ) << " 加载成功";
		} else
			qDebug( ) << value.c_str( ) << " 加载失败";
	}
	if( argParser->getOptionValues( "-as" ).size( ) > 0 ) {
		auto end = novelNetJobs.end( );
		auto iterator = novelNetJobs.begin( );
		count = novelNetJobs.size( );
		for( ; iterator != end; ++iterator ) {
			iterator->second->setPath( path );
			QObject::connect( iterator->second.get( )
				, &NovelNetJob::endJob
				, [&]( ) {
					--count;
					if( count == 0 )
						qApp->exit( 0 );
				} );
		}
	} else {
		auto runPaths = argParser->getOptionValues( "-s" );
		if( runPaths.size( ) > 0 ) {
			std::vector< std::shared_ptr< NovelNetJob > > runJobs;
			auto end = novelNetJobs.end( );
			for( auto startPath : runPaths ) {
				auto absoluteFilePath = QFileInfo( QString::fromStdString( startPath ) ).absoluteFilePath( );
				auto iterator = novelNetJobs.begin( );
				for( ; iterator != end; ++iterator )
					if( iterator->first == absoluteFilePath ) {
						iterator->second->setPath( path );
						QObject::connect( iterator->second.get( )
							, &NovelNetJob::endJob
							, [&]( ) {
								--count;
								if( count == 0 )
									qApp->exit( 0 );
							} );
						if( std::find_if( runJobs.begin( )
							, runJobs.end( )
							, [=]( const std::vector< std::shared_ptr< NovelNetJob > >::iterator &it ) {
								if( *it == iterator->second )
									return true;
								return false;
							} ) != runJobs.end( ) )
							runJobs.emplace_back( iterator->second );
					}
			}
			count = runJobs.size( );
			for( auto nodeJob : runJobs )
				nodeJob->start( );
		} else
			instance->exit( 0 );
	}

	return instance->exec( );
}
