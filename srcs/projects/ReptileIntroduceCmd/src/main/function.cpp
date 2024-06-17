#include "function.h"
#include <string>
#include <codecvt>
#include <locale>
#include <QProcess>

#include "htmlString/HtmlStringTools.h"
#include "interface/INovelInfo.h"
#include "novelNetJob/NovelDBJob.h"
#include "novelNetJob/NovelNetJob.h"
#include "path/Path.h"
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
std::wstring conver( const std::string &str ) {
	std::wstring_convert< std::codecvt_utf8_utf16< wchar_t > > converter;
	return converter.from_bytes( str );
}
std::vector< QString > readIngoreNameFiles( std::vector< cylStd::ArgParser::String > &paths ) {
	std::vector< QString > nameKeys;
	std::vector< QString > fileKeys;
	QFile readFile;
	for( auto &path : paths )
		for( auto &fileInfo : Path::getPathInfo( QString::fromLocal8Bit( path ) ).second ) {
			auto currentFilePtah = fileInfo.getCurrentFilePtah( );
			fileKeys = readIngoreNameFile( currentFilePtah );
			fileKeys = vectorStrduplicate( fileKeys );
			fileKeys = vectorStrSort( fileKeys );
			fileKeys = vectorStrLenSort( fileKeys );
			QString writeContents;
			auto iterator = fileKeys.begin( );
			auto end = fileKeys.end( );
			while( iterator != end ) {
				auto value = *iterator;
				nameKeys.emplace_back( value );
				writeContents.append( value );
				writeContents.append( '\n' );
				++iterator;
			}
			QFile openFile( currentFilePtah );
			if( openFile.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
				openFile.write( writeContents.toUtf8( ) );
				openFile.close( );
			}
		}
	return nameKeys;
}

inline QStringList splite( const QString &splite_str_obj, const std::vector< QString > &spls_list ) {
	QStringList result, stringList;
	result.append( splite_str_obj );
	auto begin = spls_list.begin( );
	auto end = spls_list.end( );

	while( begin != end ) {
		for( auto &str : result )
			for( auto &subStr : str.split( *begin ) )
				stringList.append( subStr );
		result.clear( );
		for( auto &str : stringList )
			result.append( str );
		++begin;
	}
	return result;
}

std::vector< QString > readIngoreNameFile( const QString &path ) {
	std::vector< QString > nameKeys;
	QFile readFile( path );
	if( readFile.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) )
		for( auto str : splite( readFile.readAll( ), { "\n" } ) ) {
			str = removeAllSpace( str );
			if( !str.isEmpty( ) )
				nameKeys.emplace_back( str.toUpper( ) );
		}
	return nameKeys;
}
qsizetype writeIngoreNameFile( const QString &path, const std::vector< QString > &content ) {
	std::vector< QString > nameKeys;
	QFile writeFile( path );
	if( writeFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
		QStringList writeContent( content.begin( ), content.end( ) );
		auto string = writeContent.join( "\n" );
		QByteArray byteArray = string.toUtf8( );
		if( writeFile.write( byteArray ) )
			return writeContent.size( );
	}

	return 0;
}
std::vector< QString > vectorStrAdjustSubStr( std::vector< QString > &str_vector ) {
	if( str_vector.size( ) < 2 )
		return str_vector;
	// 长度排序
	std::list< QString > lenSort;
	for( auto &str : str_vector ) {
		auto iterator = lenSort.begin( ), end = lenSort.end( );
		for( ; iterator != end; ++iterator )
			if( iterator->length( ) < str.length( ) )
				break;
		lenSort.insert( iterator, str );
	}
	std::vector< QString > clone( lenSort.begin( ), lenSort.end( ) );
	lenSort.clear( );
	size_t listCount = clone.size( );
	size_t index = 0;
	size_t subIndex = 0;
	for( ; index < listCount; ++index ) {
		QString &currentStr = clone[ index ];
		subIndex = index + 1;
		for( ; subIndex < listCount; ++subIndex )
			if( currentStr.indexOf( clone[ subIndex ] ) != -1 )
				break;
		if( subIndex == listCount )
			lenSort.insert( lenSort.begin( ), currentStr );
	}
	return std::vector< QString >( lenSort.begin( ), lenSort.end( ) );
}
std::vector< QString > vectorStrduplicate( std::vector< QString > &str_vector ) {
	std::vector< QString > result;
	for( auto &str : str_vector ) {
		auto iterator = result.begin( );
		auto end = result.end( );
		for( ; iterator != end; ++iterator )
			if( *iterator == str )
				break;
		if( iterator != end )
			continue;
		result.emplace_back( str );
	}
	return result;
}
std::vector< QString > vectorStrSort( std::vector< QString > &str_vector ) {
	std::list< QString > result;
	for( auto &str : str_vector ) {
		auto iterator = result.begin( ), end = result.end( );
		for( ; iterator != end; ++iterator )
			if( *iterator > str )
				break;
		result.insert( iterator, str );
	}
	return std::vector< QString >( result.begin( ), result.end( ) );
}
std::vector< QString > vectorStrLenSort( std::vector< QString > &str_vector ) {
	std::list< QString > result;
	for( auto &str : str_vector ) {
		auto iterator = result.begin( ), end = result.end( );
		for( ; iterator != end; ++iterator )
			if( iterator->length( ) > str.length( ) )
				break;
		result.insert( iterator, str );
	}
	return std::vector< QString >( result.begin( ), result.end( ) );
}
std::unordered_map< size_t, std::shared_ptr< std::vector< QString > > > vectorStrToLenKeyMap( const std::vector< QString > &str_vector ) {
	std::unordered_map< size_t, std::shared_ptr< std::vector< QString > > > result;
	for( auto &str : str_vector ) {

		size_t length = str.length( );
		auto pair = result.begin( );
		auto end = result.end( );
		for( ; pair != end; ++pair )
			if( pair->first == length ) {
				pair->second->emplace_back( str );
				break;
			}
		if( pair == end ) {
			std::shared_ptr< std::vector< QString > > resultShared( std::make_shared< std::vector< QString > >( ) );
			resultShared->emplace_back( str );
			result.emplace( length, resultShared );
		}

	}
	return result;
}

std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > > vectorStrToLenKeyMap( const std::vector< std::wstring > &str_vector ) {
	std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > > result;
	for( auto &str : str_vector ) {

		size_t length = str.length( );
		auto pair = result.begin( );
		auto end = result.end( );
		for( ; pair != end; ++pair )
			if( pair->first == length ) {
				pair->second->emplace_back( str );
				break;
			}
		if( pair == end ) {
			std::shared_ptr< std::vector< std::wstring > > resultShared( std::make_shared< std::vector< std::wstring > >( ) );
			resultShared->emplace_back( str );
			result.emplace( length, resultShared );
		}

	}
	return result;
}
void loadFindKeyFiles( const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &find_key_option, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &find_key_files_option, QString &app_name, FileLenMap &result_file_name_map ) {

	if( !find_key_option && !find_key_files_option )
		return;
	std::vector< QString > findKeys;
	if( find_key_option )
		for( auto str : *find_key_option ) {
			cylHtmlTools::HtmlStringTools::removeAllSpace( str );
			str = NovelDBJob::converStringToUpper( str );
			findKeys.emplace_back( QString::fromLocal8Bit( NovelDBJob::converStringToLower( str ) ) );
		}
	findKeys = vectorStrAdjustSubStr( findKeys );
	if( findKeys.size( ) > 0 ) {
		findKeys = vectorStrduplicate( findKeys );
		result_file_name_map.emplace( app_name, vectorStrToLenKeyMap( converToWString( findKeys ) ) );
	}
	std::unordered_map< QString, std::vector< QString > > pathKeysMap;
	if( find_key_files_option ) {
		for( auto strPath : *find_key_files_option ) {
			auto pathInfo = Path::getPathInfo( QString::fromLocal8Bit( strPath ) );
			for( auto &filePath : pathInfo.second ) {
				QString currentFilePtah = filePath.getCurrentFilePtah( );
				auto vector = readIngoreNameFile( currentFilePtah );
				vector = vectorStrduplicate( vector );
				vector = vectorStrAdjustSubStr( vector );
				writeIngoreNameFile( currentFilePtah, vector );
				if( vector.size( ) == 0 )
					continue;
				pathKeysMap.emplace( currentFilePtah, vector );
			}
		}
	}
	if( pathKeysMap.size( ) > 0 ) {
		auto iterator = pathKeysMap.begin( );
		auto end = pathKeysMap.end( );
		do {
			auto newVector = vectorStrduplicate( iterator->second );
			auto newMap = vectorStrToLenKeyMap( converToWString( newVector ) );
			decltype(newMap) buff;
			QString fileName = QFileInfo( iterator->first ).fileName( );
			auto it = result_file_name_map.begin( ), en = result_file_name_map.end( );
			auto resultIteraotr = std::find_if( it
				, en
				, [&]( const std::pair< QString, LenMap > &pair ) {
					if( pair.first == fileName )
						return true;
					return false;
				} );
			if( resultIteraotr == en )
				result_file_name_map.emplace( fileName, newMap );
			else {
				newMap.insert( resultIteraotr->second.begin( ), resultIteraotr->second.end( ) );
				result_file_name_map[ fileName ] = newMap;
			}
			++iterator;
		} while( iterator != end );
	}
}
void loadingEquKeyFiles( const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &ignore_equ_key_option, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &ignore_equ_key_files_option, std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > > &result_map ) {
	if( !ignore_equ_key_option && !ignore_equ_key_files_option )
		return;

	std::vector< QString > findKeys;
	if( ignore_equ_key_option )
		for( auto str : *ignore_equ_key_option )
			findKeys.emplace_back( QString::fromLocal8Bit( str ).trimmed( ).toUpper( ) );
	if( ignore_equ_key_files_option ) {
		auto getBuff = readIngoreNameFiles( *ignore_equ_key_files_option );
		for( auto it = getBuff.begin( ), en = getBuff.end( ); it != en; ++it )
			findKeys.emplace_back( removeAllSpace( *it ) );
	}
	if( findKeys.size( ) > 0 ) {
		findKeys = vectorStrduplicate( findKeys );
		result_map = vectorStrToLenKeyMap( converToWString( findKeys ) );
	}
}
void loadingSubKeyFiles( const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &ignore_sub_key_option, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &ignore_sub_key_files_option, std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > > &result_map ) {
	if( !ignore_sub_key_option && !ignore_sub_key_files_option )
		return;
	// 忽略选项
	std::vector< QString > ignoreSubNames;
	if( ignore_sub_key_option )
		for( auto str : *ignore_sub_key_option )
			ignoreSubNames.emplace_back( QString::fromLocal8Bit( str ).toUpper( ) );

	ignoreSubNames = vectorStrAdjustSubStr( ignoreSubNames );
	if( ignore_sub_key_files_option ) {
		auto getBuff = readIngoreNameFiles( *ignore_sub_key_files_option );
		for( auto it = getBuff.begin( ), en = getBuff.end( ); it != en; ++it )
			if( !it->isEmpty( ) )
				ignoreSubNames.emplace_back( it->toUpper( ) );
	}
	if( ignoreSubNames.size( ) > 0 ) {
		ignoreSubNames = vectorStrAdjustSubStr( ignoreSubNames );
		auto wIgnoreNames = converToWString( ignoreSubNames );
		result_map = vectorStrToLenKeyMap( wIgnoreNames );
	}
}
void runSubProcess( const QString &app_path, QMutex &qt_mutex, size_t &ref_count, const std::vector< QString > &run_plug_path, const std::shared_ptr< cylStd::ArgParser > &ref_arg_parser_obj_sptr, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &requet_type_name_options, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &out_path_values, const std::vector< QString > &novel_plug_paths ) {
	QStringList cmd;
	auto optionToString = cylStd::ArgParser::converOptionToString( requet_type_name_options );
	for( size_t index = 1; index < ref_count; ++index ) {
		// 加载插件
		QString plugFilePath = novel_plug_paths.at( index );
		auto subProcess = new QProcess;
		cmd.append( " -l " );
		cmd.append( plugFilePath );

		if( out_path_values ) {
			cmd.append( " -p " );
			cmd.append( QString::fromLocal8Bit( ref_arg_parser_obj_sptr->converOptionToString( out_path_values ) ) );
		}
		if( requet_type_name_options ) {
			cmd.append( " -t " );
			cmd.append( QString::fromLocal8Bit( optionToString ) );
		}
		if( findVector( run_plug_path, plugFilePath ) ) {
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
				QMutexLocker lock( &qt_mutex );
				--ref_count;
			} );

		subProcess->start( app_path, cmd );
		cmd.clear( );
	}
}
void runPrintfUrlSubProcess( const QString &app_path, QMutex &qt_mutex, size_t &ref_count, const std::vector< QString > &run_plug_path, const std::shared_ptr< cylStd::ArgParser > &ref_arg_parser_obj_sptr, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &requet_type_name_options, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &out_path_values, const std::vector< QString > &novel_plug_paths ) {
	QStringList cmd;
	auto optionToString = cylStd::ArgParser::converOptionToString( requet_type_name_options );
	for( size_t index = 1; index < ref_count; ++index ) {
		// 加载插件
		QString plugFilePath = novel_plug_paths.at( index );
		auto subProcess = new QProcess;
		cmd.append( " -url " );

		cmd.append( " -l " );
		cmd.append( plugFilePath );
		if( findVector( run_plug_path, plugFilePath ) ) {
			cmd.append( " -s " );
			cmd.append( plugFilePath );
		}
		if( out_path_values ) {
			cmd.append( " -p " );
			cmd.append( QString::fromLocal8Bit( ref_arg_parser_obj_sptr->converOptionToString( out_path_values ) ) );
		}
		if( requet_type_name_options ) {
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
				QMutexLocker lock( &qt_mutex );
				subProcess->deleteLater( );
				--ref_count;
			} );

		subProcess->start( app_path, cmd );
		cmd.clear( );
	}
}
void loadPrintfUrlLastPlugOnThisProcess( QMutex &qt_mutex, size_t &ref_count, const QString &path, const std::vector< QString > &run_plug_path, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &requet_type_name_options, const std::vector< QString > &requesTypeNameVector, const std::vector< QString > &novel_plug_paths, const char *call_function_file_path, const char *call_function_name, const size_t &call_function_line ) {

	// 加载第一个插件
	QString plugFilePath = novel_plug_paths.front( );
	QString error;
	auto interface = LoadPlug::getIRequestNetInterface( plugFilePath, error );
	if( interface.second ) {
		NovelNetJob *novelNetJob = new NovelNetJob( nullptr, interface.first, interface.second );
		std::cout << novelNetJob->getUrl( ).toStdString( ) << std::endl;
		if( findVector( run_plug_path, plugFilePath ) ) {
			QObject::connect( novelNetJob
				, &NovelNetJob::endJob
				, [&]( ) {
					QMutexLocker lock( &qt_mutex );
					--ref_count;
				} );
			std::cout << novelNetJob->getUrl( ).toStdString( ).c_str( ) << std::endl;
			novelNetJob->setPath( path );
			if( requet_type_name_options )
				novelNetJob->setRequestTypeNames( requesTypeNameVector );
			if( findVector( run_plug_path, plugFilePath ) )
				novelNetJob->start( );
		} else {
			QMutexLocker lock( &qt_mutex );
			--ref_count;
		}
	} else {
		QMutexLocker lock( &qt_mutex );
		--ref_count;
		errorCout( error.toStdString( ), call_function_file_path, call_function_name, call_function_line );
	}
}

void loadLastPlugOnThisProcess( QMutex &qt_mutex, size_t &ref_count, const QString &path, const std::vector< QString > &run_plug_path, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &requet_type_name_options, const std::vector< QString > &requesTypeNameVector, const std::vector< QString > &novel_plug_paths, const char *call_function_file_path, const char *call_function_name, const size_t &call_function_line ) {

	// 加载第一个插件
	QString plugFilePath = novel_plug_paths.front( );
	QString error;
	auto interface = LoadPlug::getIRequestNetInterface( plugFilePath, error );
	if( interface.second ) {
		NovelNetJob *novelNetJob = new NovelNetJob( nullptr, interface.first, interface.second );
		if( findVector( run_plug_path, plugFilePath ) ) {
			QObject::connect( novelNetJob
				, &NovelNetJob::endJob
				, [&]( ) {
					QMutexLocker lock( &qt_mutex );
					--ref_count;
				} );
			std::cout << novelNetJob->getUrl( ).toStdString( ).c_str( ) << std::endl;
			novelNetJob->setPath( path );
			if( requet_type_name_options )
				novelNetJob->setRequestTypeNames( requesTypeNameVector );
			if( findVector( run_plug_path, plugFilePath ) )
				novelNetJob->start( );
		} else {
			QMutexLocker lock( &qt_mutex );
			--ref_count;
		}
	} else {
		QMutexLocker lock( &qt_mutex );
		--ref_count;
		errorCout( error.toStdString( ), call_function_file_path, call_function_name, call_function_line );
	}
}

std::vector< std::wstring > converToWString( std::vector< QString > &str_vector ) {
	std::vector< std::wstring > result;
	for( auto &str : str_vector )
		result.emplace_back( str.toStdWString( ) );
	return result;
}
