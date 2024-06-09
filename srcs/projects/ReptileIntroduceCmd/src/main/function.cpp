#include "function.h"
#include <string>
#include <codecvt>
#include <locale>

#include "htmlString/HtmlStringTools.h"
#include "interface/INovelInfo.h"
#include "novelNetJob/NovelDBJob.h"
#include "path/Path.h"
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
	QFile readFile;
	for( auto &path : paths )
		for( auto &fileInfo : Path::getPathInfo( QString::fromLocal8Bit( path ) ).second )
			if( ( readFile.setFileName( fileInfo.getCurrentFilePtah( ) ), readFile.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) ) )
				for( auto str : readFile.readAll( ).split( '\n' ) )
					for( auto appendObj : str.split( ' ' ) )
						if( ( appendObj = appendObj.trimmed( ), !appendObj.isEmpty( ) ) )
							nameKeys.emplace_back( appendObj );
	return nameKeys;
}
std::vector< QString > readIngoreNameFile( const QString &path ) {
	std::vector< QString > nameKeys;
	QFile readFile( path );
	if( readFile.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) )
		for( auto str : readFile.readAll( ).split( '\n' ) )
			for( auto appendObj : str.split( ' ' ) )
				if( ( appendObj = appendObj.trimmed( ), !appendObj.isEmpty( ) ) )
					nameKeys.emplace_back( appendObj.toUpper( ) );
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
void loadFindKeyFiles( const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &find_key_option, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &find_key_files_option, cylHtmlTools::HtmlWorkThread &result_thread, QString &app_name, FileLenMap &result_file_name_map ) {
	result_thread.setCurrentThreadRun( [&]( cylHtmlTools::HtmlWorkThread * ) {
		if( !find_key_option && !find_key_files_option )
			return;
		std::vector< QString > findKeys;
		if( find_key_option )
			for( auto str : *find_key_option )
				findKeys.emplace_back( QString::fromLocal8Bit( NovelDBJob::converStringToLower( str ) ) );

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
				result_file_name_map.emplace( iterator->first, newMap );
				++iterator;
			} while( iterator != end );
		}
	} );
	result_thread.start( );
}
void loadingEquKeyFiles( const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &ignore_equ_key_option, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &ignore_equ_key_files_option, cylHtmlTools::HtmlWorkThread &result_thread, std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > > &result_map ) {
	if( !ignore_equ_key_option && !ignore_equ_key_files_option )
		return;

	result_thread.setCurrentThreadRun( [&]( cylHtmlTools::HtmlWorkThread * ) {
		std::vector< QString > findKeys;
		if( ignore_equ_key_option )
			for( auto str : *ignore_equ_key_option )
				findKeys.emplace_back( QString::fromLocal8Bit( str ) );
		findKeys = vectorStrAdjustSubStr( findKeys );
		if( ignore_equ_key_files_option ) {
			auto getBuff = readIngoreNameFiles( *ignore_equ_key_files_option );
			findKeys.insert( findKeys.end( ), getBuff.begin( ), getBuff.end( ) );
		}
		if( findKeys.size( ) > 0 ) {
			findKeys = vectorStrduplicate( findKeys );
			result_map = vectorStrToLenKeyMap( converToWString( findKeys ) );
		}
	} );
	result_thread.start( );
}
void loadingSubKeyFiles( const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &ignore_sub_key_option, const std::shared_ptr< std::vector< cylStd::ArgParser::String > > &ignore_sub_key_files_option, cylHtmlTools::HtmlWorkThread &result_thread, std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > > &result_map ) {
	if( !ignore_sub_key_option && !ignore_sub_key_files_option )
		return;
	// 忽略选项
	std::vector< QString > ignoreSubNames;
	if( ignore_sub_key_option )
		for( auto str : *ignore_sub_key_option )
			ignoreSubNames.emplace_back( QString::fromLocal8Bit( str ) );

	ignoreSubNames = vectorStrAdjustSubStr( ignoreSubNames );
	if( ignore_sub_key_files_option ) {
		auto getBuff = readIngoreNameFiles( *ignore_sub_key_files_option );
		ignoreSubNames.insert( ignoreSubNames.end( ), getBuff.begin( ), getBuff.end( ) );
	}
	if( ignoreSubNames.size( ) > 0 ) {
		ignoreSubNames = vectorStrAdjustSubStr( ignoreSubNames );
		auto wIgnoreNames = converToWString( ignoreSubNames );
		result_map = vectorStrToLenKeyMap( wIgnoreNames );
	}
	result_thread.start( );
}

std::vector< std::wstring > converToWString( std::vector< QString > &str_vector ) {
	std::vector< std::wstring > result;
	for( auto &str : str_vector )
		result.emplace_back( str.toStdWString( ) );
	return result;
}
