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

const QString exportFindDirMidName = u8"export_find"; // 查找的中间名称 (-fkf 选项)

const QString exportAllDirMidName = u8"export_all"; // 全部导出的中间名称 (-edb 选项)
Time runTime; // 开始时间
/// <summary>
/// 文件的 cmake 项目相对路径
/// </summary>
std::string callFileName = instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ).toStdString( );
/// <summary>
/// 输出间隔
/// </summary>
std::chrono::seconds duration( 25 );

std::string applicationPid; // app id

bool PathManage::updateFilePath( const std::filesystem::path &update_path ) {
	objMutex.lock( );
	if( std::filesystem::is_directory( update_path ) ) {
		objMutex.unlock( );
		return false;
	}
	auto absFilePath = std::filesystem::absolute( update_path );
	for( auto &vectorPath : updateFileVector )
		if( vectorPath == absFilePath ) {
			objMutex.unlock( );
			return false; // 已经存在，则返回 false
		}
	updateFileVector.emplace_back( absFilePath );
	objMutex.unlock( );
	return true;
}
std::vector< std::filesystem::path > PathManage::getOldPath( ) {
	objMutex.lock( );
	std::vector< std::filesystem::path > result;
	if( !std::filesystem::is_directory( currentPath ) | !std::filesystem::exists( currentPath ) ) { // 如果不是文件夹，或者不存在，则返回
		objMutex.unlock( );
		return result;
	}
	std::filesystem::recursive_directory_iterator directoryIterator( currentPath );
	std::filesystem::recursive_directory_iterator endIterator;
	while( directoryIterator != endIterator ) {
		auto absFilePath = std::filesystem::absolute( directoryIterator->path( ) );
		bool isAppendResultPathVecor = true;
		if( !directoryIterator->is_directory( ) ) {
			for( auto &vectorPath : updateFileVector )
				if( vectorPath == absFilePath ) {
					isAppendResultPathVecor = false;
					break;
				}
			if( isAppendResultPathVecor )
				result.emplace_back( absFilePath );
		}
		++directoryIterator;
	}
	objMutex.unlock( );
	return result;
}
std::vector< std::filesystem::path > PathManage::getEmptyDir( ) {
	objMutex.lock( );
	std::vector< std::filesystem::path > result;
	if( !std::filesystem::is_directory( currentPath ) | !std::filesystem::exists( currentPath ) ) { // 如果不是文件夹，或者不存在，则返回
		objMutex.unlock( );
		return result;
	}
	if( isEmptyDir( ) ) { // 如果是空目录，则返回所有文件夹路径
		result.emplace_back( currentPath ); // 追加当前路径
		objMutex.unlock( );
		return result;
	}
	std::filesystem::directory_iterator directoryIterator( currentPath );
	std::filesystem::directory_iterator endIterator;
	for( ; directoryIterator != endIterator; ++directoryIterator ) {
		auto checkPath = directoryIterator->path( );
		PathManage pathManage( checkPath );
		auto emptyDir = pathManage.getEmptyDir( );
		for( auto &appendPath : emptyDir )
			result.emplace_back( appendPath );
	}
	objMutex.unlock( );
	return result;
}
std::vector< std::filesystem::path > PathManage::getUpdateFileVector( ) {

	objMutex.lock( );
	auto clone = std::vector< std::filesystem::path >( updateFileVector.begin( ), updateFileVector.end( ) );
	objMutex.unlock( );
	return clone;
}
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
std::chrono::seconds getRunSepSec( ) {
	Time newTime;
	auto speTime = newTime - runTime;
	return speTime.toDuration< std::chrono::seconds >( ); // 获取秒
}
std::chrono::milliseconds getRunSepMill( ) {
	Time newTime;
	auto speTime = newTime - runTime;
	return speTime.toDuration< std::chrono::milliseconds >( ); // 获取毫秒
}
std::chrono::minutes getRunSepMin( ) {
	Time newTime;
	auto speTime = newTime - runTime;
	return speTime.toDuration< std::chrono::minutes >( ); // 获取分钟
}
bool getRunSepSecAndMill( size_t &sec, size_t &mill ) {
	Time newTime;
	auto speTime = newTime - runTime;
	auto duration = speTime.toDuration< std::chrono::milliseconds >( ); // 获取毫秒
	long long count = duration.count( );
	if( count < 0 )
		return false;
	sec = count / 1000; // 秒
	mill = count % 1000; // 毫秒
	return true;
}
bool getRunSepSecAndMillAndMin( size_t &min, size_t &sec, size_t &mill ) {
	Time newTime;
	auto speTime = newTime - runTime;
	auto duration = speTime.toDuration< std::chrono::milliseconds >( ); // 获取毫秒
	long long count = duration.count( );
	if( count < 0 )
		return false;
	mill = count % 1000; // 毫秒
	sec = count / 1000 % 60; // 秒
	min = count / 1000 / 60; // 分钟
	return true;
}
bool getRunSepSecAndMillAndMinAndHour( size_t &hour, size_t &min, size_t &sec, size_t &mill ) {
	Time newTime;
	auto speTime = newTime - runTime;
	auto duration = speTime.toDuration< std::chrono::milliseconds >( ); // 获取毫秒
	long long count = duration.count( );
	if( count < 0 )
		return false;
	mill = count % 1000; // 毫秒
	sec = count / 1000 % 60; // 秒
	min = count / 1000 / 60 % 60; // 分钟
	hour = count / 1000 / 60 / 60; // 小时
	return true;
}
std::string getRunSepSecAndMillAndMinAndHourToStdString( ) {
	Time newTime;
	std::string result;
	auto speTime = newTime - runTime;
	auto duration = speTime.toDuration< std::chrono::milliseconds >( ); // 获取毫秒
	long long count = duration.count( );
	if( count < 0 )
		return result;
	size_t mill = count % 1000; // 毫秒
	size_t sec = count / 1000 % 60; // 秒
	size_t min = count / 1000 / 60 % 60; // 分钟
	size_t hour = count / 1000 / 60 / 60; // 小时
	if( hour > 0 )
		result = std::to_string( hour ) + " h " + std::to_string( min ) + " m " + std::to_string( sec ) + " s " + std::to_string( mill ) + " ms";
	else if( min > 0 )
		result = std::to_string( min ) + " m " + std::to_string( sec ) + " s " + std::to_string( mill ) + " ms";
	else if( sec > 0 )
		result = std::to_string( sec ) + " s " + std::to_string( mill ) + " ms";
	else
		result = std::to_string( mill ) + " ms";
	return result;
}

QString writeVector( const std::vector< QString > &vector, const QString &write_file_path, std::vector< QString > *result_key_s = nullptr ) {
	QString writeContents;
	auto iterator = vector.begin( );
	auto end = vector.end( );
	if( result_key_s )
		while( iterator != end ) {
			auto value = *iterator;
			result_key_s->emplace_back( value );
			writeContents.append( value );
			writeContents.append( '\n' );
			++iterator;
		}
	else
		while( iterator != end ) {
			auto value = *iterator;
			writeContents.append( value );
			writeContents.append( '\n' );
			++iterator;
		}
	QFile openFile( write_file_path );
	if( openFile.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
		openFile.write( writeContents.toUtf8( ) );
		openFile.close( );
	}
	return writeContents;
}

// { "\n", "·", ",", "、", ",", "`", "~", "。", "." ,"，"}
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
		stringList.clear( );
	}
	return result;
}

std::vector< QString > readIngoreNameFile( const QString &path, const std::vector< QString > &spls_list ) {
	std::vector< QString > nameKeys;
	QFile readFile( path );
	if( readFile.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) )
		for( auto str : splite( readFile.readAll( ), spls_list ) ) {
			str = removeAllSpace( str );
			if( !str.isEmpty( ) )
				nameKeys.emplace_back( str.toUpper( ) );
		}
	return nameKeys;
}
std::vector< QString > vectorStrAdjustSubStr( const std::vector< QString > &str_vector ) {
	auto endIndex = str_vector.size( );
	if( endIndex < 2 )
		return str_vector;
	// 长度排序
	std::vector< const QString * > clone;
	for( auto &str : str_vector ) {
		auto iterator = clone.begin( ), end = clone.end( );
		for( ; iterator != end; ++iterator ) {
			const QString *value = *iterator;
			if( value->length( ) > str.length( ) )
				break;
		}
		clone.insert( iterator, &str );
	}

	std::vector< QString > resultVector;
	std::vector< const QString * > resultBuffVector;
	while( endIndex ) {
		auto startObj = clone.at( 0 );
		resultVector.emplace_back( *startObj );
		for( auto newIndex = 1; newIndex < endIndex; ++newIndex ) {
			const auto &sub = clone.at( newIndex );
			if( sub->indexOf( *startObj ) == -1 )
				resultBuffVector.emplace_back( sub );
		}
		clone = resultBuffVector;
		resultBuffVector.clear( );
		endIndex = clone.size( );
	}
	return resultVector;
}
std::vector< QString > vectorStrDuplicate( const std::vector< QString > str_vector ) {

	auto endIndex = str_vector.size( );
	if( endIndex < 2 )
		return str_vector;
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
std::vector< QString > & vectorStrSort( std::vector< QString > &str_vector, bool isGreater ) {
	if( isGreater )
		std::sort( str_vector.begin( ),
					str_vector.end( ),
					[] ( const QString &left, const QString &right ) {
						return left > right;
					} );
	else
		std::sort( str_vector.begin( ),
					str_vector.end( ),
					[] ( const QString &left, const QString &right ) {
						return left < right;
					} );
	return str_vector;
}
std::vector< QString > & vectorStrLenSort( std::vector< QString > &str_vector, bool isGreater ) {
	if( isGreater )
		std::sort( str_vector.begin( ),
					str_vector.end( ),
					[] ( const QString &left, const QString &right ) {
						return left.length( ) > right.length( );
					} );
	else
		std::sort( str_vector.begin( ),
					str_vector.end( ),
					[] ( const QString &left, const QString &right ) {
						return left.length( ) < right.length( );
					} );
	return str_vector;
}

inline bool compHtmlKey( interfacePlugsType::HtmlDocString compStr, const interfacePlugsType::HtmlDocString &key, const size_t &key_str_len ) {
	cylHtmlTools::HtmlStringTools::removeAllSpace( compStr );
	if( compStr.length( ) >= key_str_len && cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &compStr, &key ) )
		return true;
	return false;
}
QString writeJionStringVector( const QString &write_file_path, const std::vector< QString > &qt_str_vector, const QString &jion ) {
	auto iterator = qt_str_vector.begin( );
	auto end = qt_str_vector.end( );
	QString result;
	if( iterator != end )
		do {
			result.append( *iterator );
			++iterator;
			if( iterator == end ) {
				if( Path::creatFilePath( write_file_path ) ) {
					QFile write( write_file_path );
					if( write.open( QIODeviceBase::ReadWrite | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
						write.write( result.toUtf8( ) );
						write.close( );
					}
				}
				return result;
			}
			result.append( jion );
		} while( true );
	return result;
}
void showAppName( std::shared_ptr< cylStd::ArgParser > &args ) {
	if( args->getOptionValues( "-name" ) )
		std::cout << QString( u8"小说爬虫命令行版本" ).toStdString( ).c_str( ) << std::endl;; // 程序标题
}
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
void showHelp( ) {
	std::cout << "========="
		"\n"
		u8"帮助:" "\n\t"
		"-name" u8"\n\t  "
		u8"说明:\n\t\t" u8"显示软件帮助信息" "\n\t"
		"-h" u8"\n\t  "
		u8"说明:\n\t\t" u8"显示选项帮助信息" "\n\t"
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
		"-edb" u8"\n\t  "
		u8"说明:\n\t\t" u8"全部导出" "\n\t"
		"-ijtenf"
		u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
		u8"说明:\n\t\t" u8"指定跳过名称子关键字存放文件路径-完全匹配小说名称，需要配合 -rdb 与 -w、-fkf 选项使用" "\n\t"
		"-ijtsnf"
		u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
		u8"说明:\n\t\t" u8"指定跳过名称子关键字存放文件路径-段内匹配小说名称，需要配合 -rdb 与 -w、-fkf 选项使用" "\n\t"
		"-ifkf"
		u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
		u8"说明:\n\t\t" u8"指定查找关键字存放文件，需要配合 -rdb 与 -w 选项使用" "\n\t"
		u8"\n\t\t，当指定路径不存在可用文件时候，自动激活 -edb""\n\t""\n\t"
		"-t"
		u8"  参数" "\n\t\t" u8"路径" u8"\n\t  "
		u8"说明:\n\t\t" u8"指定获取的小说类型配置文件路径-单个类型为一行" "\n\t"
		"-rdb"
		u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
		u8"说明:\n\t\t" u8"导出数据库""\n\t"
		"-fkrrlkse"
		u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
		u8"说明:\n\t\t" u8"获取删除目标的行关键字文件，必须存在有效 -fkrrlkd 选项""\n\t"
		u8"\n\t\t-fkrrlkss 或 -fkrrlkse 允许同时使用""\n\t"
		"-fkrrlkss"
		u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
		u8"说明:\n\t\t" u8"获取删除目标的行关键字文件，必须存在有效 -fkrrlkd 选项""\n\t"
		u8"\n\t\t-fkrrlkss 或 -fkrrlkse 允许同时使用""\n\t"
		"-fkrrlkd"
		u8"  参数" "\n\t\t" u8"路径[,路径1 [,路径2[,....]]]" u8"\n\t  "
		u8"说明:\n\t\t" u8"处理删除目标的行关键字文件，必须存在有效 -fkrrlkss 或 -fkrrlkse 选项"
		u8"\n\t\t-fkrrlkss 或 -fkrrlkse 允许同时使用""\n\t"
		"-ex"
		u8"  参数" "\n\t\t" u8"天数" u8"\n\t  "
		u8"说明:\n\t\t" u8"校验过期，将会删除过期小说"
		"\n"
		"=========" << std::endl;
}
void showHelp( std::shared_ptr< cylStd::ArgParser > &args ) {
	if( args->getOptionValues( "-h" ) )
		showHelp( );
}
std::vector< QString > getFilePathsOptionPaths( const std::shared_ptr< cylStd::ArgParser > &args, const cylStd::ArgParser::String &option ) {
	std::vector< QString > filePaths;
	auto optionValues = args->getOptionValues( option );
	if( optionValues ) {
		for( auto &value : *optionValues ) {
			QString fromStdString = QString::fromLocal8Bit( value );
			auto dirInfo = Path::getPathInfo( fromStdString );
			for( auto &file : dirInfo.second )
				filePaths.emplace_back( file.getCurrentFilePtah( ) ); // 存储
		}

		return vectorStrDuplicate( filePaths );
	}
	return filePaths;
}
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
void outStdCountStreamMsg( QMutex &std_cout_mutex, const std::string &call_function_name, const std::string &work_msg, const std::string &file_name, const size_t line ) {
	std_cout_mutex.lock( );
	std::stringstream msg;
	msg << u8"\n(进程 id : " << applicationPid << u8", 线程 id : " << std::this_thread::get_id( ) << u8" ) => [ " << call_function_name << u8" ] " << work_msg << u8" << " << file_name << " : " << line;
	std::cout << msg.str( ) << "\t: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << std::endl;
	std_cout_mutex.unlock( );
}
std::vector< QString > getFileTextSplit( const std::vector< QString > &file_paths, const std::vector< QString > &split_vector, bool is_remove_space ) {
	std::vector< QString > result;
	if( file_paths.size( ) == 0 )
		return result;
	cylHtmlTools::HtmlWorkThreadPool pool;
	QMutex mutex;
	for( auto &filePath : file_paths )
		pool.appendWork( [=,&result,&mutex] ( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
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
	pool.start( );
	while( !pool.isOverJob( ) )
		qApp->processEvents( );
	return result;
}
QString getFileBaseName( const QFileInfo &filePathInfo ) {
	QString absoluteDirPath = filePathInfo.absoluteDir( ).absolutePath( );
	QString absoluteFilePath = filePathInfo.absoluteFilePath( );
	QString fileName = absoluteFilePath.remove( 0, absoluteDirPath.size( ) + 1 );
	qsizetype indexOf = fileName.lastIndexOf( "." );
	if( indexOf > 0 )
		return fileName.slice( 0, indexOf );
	return fileName;
}

std::vector< QProcess * > runPluginToSubProcess( const QString &app_path, QMutex &qt_mutex, size_t &ref_count, const std::vector< QString > &novel_plug_paths, const QString &request_type_name_options, bool is_nsts_option, bool is_nste_option, bool is_nsnstr_option, const QString &out_path_values, const std::string &call_function_file_path, const char *call_function_name, const size_t &call_function_line ) {
	QStringList cmd; // 命令行
	std::vector< QProcess * > result; // 返回进程列表
	QString currentPath = QDir::currentPath( ); // 当前工作路径
	is_nsts_option = !is_nsts_option;
	is_nste_option = !is_nste_option;
	is_nsnstr_option = !is_nsnstr_option;
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
		if( is_nsts_option )
			cmd.append( " -nsts " );
		if( is_nste_option )
			cmd.append( " -nste " );
		if( is_nsnstr_option )
			cmd.append( " -nstr " );
		QObject::connect( subProcess,
						&QProcess::readyReadStandardOutput,
						[=]( ) {
							QString msg( subProcess->readAllStandardOutput( ) );
							std::cout << u8"(进程 id : " << subProcess->processId( ) << " )[readyReadStandardOutput] : " << msg.toStdString( ) << "\t: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << "\n===============" << std::endl;
						} );

		QObject::connect( subProcess,
						&QProcess::readyReadStandardError,
						[=]( ) {
							QString msg( subProcess->readAllStandardError( ) );
							std::cerr << u8"(进程 id : " << subProcess->processId( ) << " )[readyReadStandardError] : " << msg.toStdString( ) << "\t: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << "\n===============" << std::endl;
						} );

		QObject::connect( subProcess,
						&QProcess::finished,
						[&ref_count,&qt_mutex,subProcess]( ) {
							std::cout << u8"子进程结束(进程 id : " << subProcess->processId( ) << " )  " << "\t: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << "\n===============" << std::endl;
							subProcess->deleteLater( );
							QMutexLocker lock( &qt_mutex );
							--ref_count;
						} );

		QObject::connect( subProcess,
						&QProcess::started,
						[cmd,app_path,subProcess]( ) {
							QString msg( u8"子进程运行(进程 id : %1 ) :\n\t%2 %3 " );
							std::cout << msg.arg( subProcess->processId( ) ).arg( app_path ).arg( cmd.join( " " ) ).toStdString( ).c_str( ) << "\t: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << "\n===============" << std::endl;
						} );

		QObject::connect( subProcess,
						&QProcess::errorOccurred,
						[cmd,app_path,subProcess] ( QProcess::ProcessError error ) {
							QString msg( u8"子进程异常(进程 id : %1 ) :\n\t%2 %3 \n\t:: (%4) =>:(%5)" );
							std::cout << msg.arg( subProcess->processId( ) ).arg( app_path ).arg( cmd.join( " " ) ).arg( error ).arg( subProcess->errorString( ) ).toStdString( ).c_str( ) << "\t: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << "\n===============" << std::endl;
						} );
		subProcess->setWorkingDirectory( currentPath );
		subProcess->setProgram( app_path );
		subProcess->setArguments( cmd );
		if( subProcess->open( ) )
			result.emplace_back( subProcess );
		else if( !subProcess->waitForStarted( ) ) {
			--ref_count;
			subProcess->deleteLater( );
			errorCoutPath( QString( "( 进程 id = %1 ) : %2 " ).arg( applicationPid.c_str( ) ).arg( u8" 无法正常启动 " ), call_function_file_path.c_str( ), call_function_name, call_function_line );
		} else
			result.emplace_back( subProcess );
		cmd.clear( );
	}
	return result;
}
NovelNetJob * runPluginToThisProcess( QMutex &qt_mutex, size_t &ref_count, const QString &out_path, const std::vector< QString > &reques_type_names_vector, const QString &novel_plug_path, const std::string &call_function_file_path, const char *call_function_name, const size_t &call_function_line ) {
	// 加载第一个插件
	QString error;
	auto plugInterFace = LoadPlug::getIRequestNetInterface( novel_plug_path, error );
	if( plugInterFace.second ) {
		NovelNetJob *novelNetJob = new NovelNetJob( nullptr, plugInterFace.first, plugInterFace.second );
		auto stdString = novelNetJob->getUrl( ).toStdString( );
		bool *isRun = new bool( true );
		std::thread *thread = new std::thread( [=]( ) {
			auto cStr = stdString.c_str( );
			while( *isRun ) {
				std::cout << u8"\n(进程 id : " << applicationPid << u8") 正在运行 : " << cStr << u8" ( 运行时间: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << " )" << std::endl;
				std::this_thread::sleep_for( duration );
			}
			std::cout << u8"运行结束 : " << cStr << u8" ( 运行时间: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << " )" << std::endl;
		} );
		QObject::connect( novelNetJob,
						&NovelNetJob::endJob,
						[&qt_mutex,&ref_count,isRun,thread,novelNetJob,stdString]( ) {
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
		errorCoutPath( QString( "(进程 id = %1) : %2" ).arg( applicationPid.c_str( ) ).arg( error ), call_function_file_path.c_str( ), call_function_name, call_function_line );
		return nullptr;
	}
}
void runRequestDownloadPlugs( std::shared_ptr< cylStd::ArgParser > &args ) {
	auto optionValues = args->getOptionValues( "-l" );
	if( !optionValues )
		return;
	// 请求小说插件文件路径
	std::vector< QString > plugFilesPath = getFilePathsOptionPaths( args, "-l" );
	// 如果没有文件，则退出
	if( plugFilesPath.size( ) == 0 ) {
		ErrorCout_MACRO( u8"找不到有效的插件地址，请检查 -l 选项指定的路径是否正确" );
		return; // 找不到插件，则返回
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
	if( runPlugFilesPaths.size( ) == 0 ) { // 找不到插件，则返回 
		ErrorCout_MACRO( QString( u8"(进程 id : %1 )没有发件有效的插件路径，请检查 -s 是否发生错误" ).arg(applicationPid.c_str( ) ) );
		return;
	}
	// 请求类型文件路径
	std::vector< QString > requestNamesTypeFilePaths = getFilePathsOptionPaths( args, "-t" );
	if( requestNamesTypeFilePaths.size( ) == 0 ) {
		ErrorCout_MACRO( QString( u8"(进程 id : %1 )没有发现 -t 选项（该选项用于发现获取类型名称的文件路径）" ).arg( applicationPid.c_str( )) );
		return;
	}
	// 获取所有文件内容
	auto nameTypes = getFileTextSplit( requestNamesTypeFilePaths, {"\n"}, true );
	// 合并文件内容
	if( nameTypes.size( ) == 0 ) {
		ErrorCout_MACRO( QString( u8"(进程 id : %1 )没有发现需要获取的类型名称" ).arg( applicationPid.c_str( ) ) );
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
	auto runPlug = runPluginToThisProcess( mutex, count, requestOutPath, nameTypes, firstPlugFilePath, callFileName, __FUNCTION__, __LINE__ );
	auto isNstsOption = args->getOptionValues( "-dssts" ); // 存在，则输出子进程的开始时间
	auto isNsteOption = args->getOptionValues( "-dsste" ); // 存在，则输出子进程的结束时间
	auto isNnstrOption = args->getOptionValues( "-dsnstr" ); // 存在，则输出子进程的结束时间
	auto subProcessRunPlug = runPluginToSubProcess( appFilePath, mutex, count, runPlugFilesPaths, typeNameOption, ( isNstsOption ? true : false ), ( isNsteOption ? true : false ), ( isNnstrOption ? true : false ), requestOutPath, callFileName, __FUNCTION__, __LINE__ );
	// count 记录任务数量，任务为 0 时，退出循环，主进程重新掌控 cpu 时间
	while( count )
		qApp->processEvents( );
}
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
QString & removeAllSpace( QString &str ) {
	size_t leftLen = str.length( );
	QString::value_type *buff = new QString::value_type [ leftLen ];
	size_t index = 0, buffIndex = 0;
	for( ; index < leftLen; ++index ) {
		auto value = str.at( index );
		if( value.isSpace( ) || !value.isPrint( ) )
			continue;
		buff[ buffIndex ] = value;
		++buffIndex;
	}
	str = QString( buff, buffIndex );
	delete[] buff;
	return str;
}
std::unordered_map< QString, std::vector< QString > > getPairsKey( const std::unordered_map< QString, QString > &map, const QString &split_str_key, const bool isAdjustSubStr ) {
	std::unordered_map< QString, std::vector< QString > > result;
	for( auto &iter : map ) {
		auto list = splitMultipleStrList( iter.second, {split_str_key} );
		std::vector< QString > saveList;
		for( auto str : list )
			if( !removeAllSpace( str ).isEmpty( ) )
				saveList.emplace_back( str.toUpper( ) );
		if( saveList.size( ) == 0 )
			continue; // 没有关键字，则跳过这次循环
		if( isAdjustSubStr )
			saveList = vectorStrAdjustSubStr( saveList );
		else {
			saveList = vectorStrDuplicate( saveList );
			std::sort( saveList.begin( ), saveList.end( ), compQString );
			std::sort( saveList.begin( ), saveList.end( ), compQStringLen );
		}
		QString filePath = iter.first;
		if( writeFile( filePath, QIODeviceBase::ReadWrite | QIODeviceBase::Truncate, saveList, "\n" ) )
			result.insert_or_assign( filePath, saveList ); // 只有真正可写内容才允许保存到列表当中
	}
	return result;
}
void unmapRemoveIfEquKeys( std::unordered_map< QString, std::vector< QString > > &dest_file_text_map_keys, const std::vector< QString > &source_equ_keys ) {
	for( auto &iter : dest_file_text_map_keys ) {
		auto &vector = iter.second;
		std::vector< QString > newBuff;
		for( auto &key : vector )
			if( !vectorHasValue( source_equ_keys, key ) )
				newBuff.emplace_back( key );
		newBuff = vectorStrDuplicate( newBuff );
		// 不相等，则需要更新
		if( newBuff.size( ) == vector.size( ) )
			continue;
		std::sort( newBuff.begin( ), newBuff.end( ), compQString );
		std::sort( newBuff.begin( ), newBuff.end( ), compQStringLen );
		if( writeFile( iter.first, QIODeviceBase::ReadWrite | QIODeviceBase::Truncate, newBuff, "\n" ) )
			iter.second = newBuff;
	}
}
void unmapRemoveIfSubKeys( std::unordered_map< QString, std::vector< QString > > &dest_file_text_map_keys, const std::vector< QString > &source_sub_keys ) {
	for( auto &iter : dest_file_text_map_keys ) {
		auto &vector = iter.second;
		std::vector< QString > newBuff;
		for( auto &key : vector ) {
			std::function< bool( const QString & ) > check = [&] ( const QString &value ) {
				if( key.indexOf( value ) != -1 )
					return true;
				return false;
			};
			auto begin = source_sub_keys.begin( );
			auto end = source_sub_keys.end( );
			auto stdVectorFindIfResult = std::find_if( begin, end, check );
			if( stdVectorFindIfResult == end )
				newBuff.emplace_back( key );
		}

		newBuff = vectorStrDuplicate( newBuff );
		if( vector.size( ) == newBuff.size( ) )
			continue; // 数量一致，不需要更新目标文件
		std::sort( newBuff.begin( ), newBuff.end( ), compQString );
		std::sort( newBuff.begin( ), newBuff.end( ), compQStringLen );
		if( writeFile( iter.first, QIODeviceBase::ReadWrite | QIODeviceBase::Truncate, newBuff, "\n" ) )
			iter.second = newBuff;
	}
}
void checkFindKeYFiles( const std::shared_ptr< cylStd::ArgParser > &arg_parser ) {
	// 读写文件列表
	auto dest = getFilePathsOptionPaths( arg_parser, "-sff" );
	if( dest.size( ) == 0 )
		return;
	QMutex stdCountMutex;
	auto callFunctionName = __FUNCTION__;
	cylHtmlTools::HtmlWorkThreadPool threadPools;
	for( auto &filePath : dest )
		threadPools.appendWork( [&,filePath] ( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
			QFile file( filePath );
			Out_Std_Count_Stream_Msg_MACRO( stdCountMutex, callFunctionName, QString(u8"匹配查找文件路径 : [ %1 ]").arg( filePath ).toStdString( ) );
			if( file.exists( ) && file.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) ) {
				auto byteArray = file.readAll( );
				file.close( );
				auto splites = splite( byteArray, {"\n", ",", "，", "。", ".", ":", "|", "-", "—", "_"} );
				qsizetype spliteCount = splites.size( );
				if( spliteCount > 0 ) {
					Out_Std_Count_Stream_Msg_MACRO( stdCountMutex, callFunctionName, QString(u8"发现查找文件个数 : [ %1 ] -> ( %2 )").arg( filePath ).arg( spliteCount ).toStdString( ) );
					std::vector< QString > writeList;
					for( auto &str : splites )
						if( !removeAllSpace( str ).isEmpty( ) )
							writeList.emplace_back( str );
					writeList = vectorStrAdjustSubStr( writeList );
					spliteCount = writeList.size( );
					Out_Std_Count_Stream_Msg_MACRO( stdCountMutex, callFunctionName, QString(u8"重新写入文件 : [ %1 ] -> ( %2 )").arg( filePath ).arg( spliteCount ).toStdString( ) );
					if( spliteCount > 0 ) {
						writeFile( filePath, writeList, "\n" );
					}
					Out_Std_Count_Stream_Msg_MACRO( stdCountMutex, callFunctionName, QString(u8"写入完毕 : [ %1 ] -> ( %2 )").arg( filePath ).arg( spliteCount ).toStdString( ) );
				} else {
					stdCountMutex.lock( );
					ErrorCout_FunctionName_MACRO( QString(u8"数据错误异常 : [ %1 ] -> ( %2 )").arg( filePath ).arg( spliteCount ), callFunctionName );
					stdCountMutex.unlock( );
				}
			} else {
				stdCountMutex.lock( );
				ErrorCout_FunctionName_MACRO( QString(u8"打开错误 : [ %1 ] -> 无法打开或者不存在").arg( filePath ), callFunctionName );
				stdCountMutex.unlock( );
			}
		} );
	threadPools.setIdleTimeCall( [&stdCountMutex,&callFunctionName] ( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &modWork, const unsigned long long &currentWork ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( stdCountMutex, modWork, currentWork, callFunctionName, QString(u8"查找文件过滤当中").toStdString( ) );
	} );
	threadPools.setWorkCount( threadPools.getSystemCupCount( ) * 2 );
	threadPools.start( );
	auto app = qApp;
	while( !threadPools.isOverJob( ) )
		app->processEvents( );
}
void checkKeyFile( std::shared_ptr< cylStd::ArgParser > &args ) {
	// 读写文件列表
	auto dest = getFilePathsOptionPaths( args, "-fkrrlkd" );
	if( dest.size( ) == 0 )
		return;
	// 可读完全匹配文件路径
	auto sourceEqu = getFilePathsOptionPaths( args, "-fkrrlkse" );
	// 可读段内匹配文件路径
	auto sourceSub = getFilePathsOptionPaths( args, "-fkrrlkss" );
	// 发现完全匹配文件的个数
	size_t sourceEquCount = sourceEqu.size( );
	// 发现段内匹配文件的个数
	size_t sourceSubCount = sourceSub.size( );
	if( sourceEquCount == 0 && sourceSubCount == 0 ) { // 找不到目标配置文件，则返回 
		ErrorCout_MACRO( QString( u8"(进程 id : %1)没有发件有效的可读配置处理文件路径，请检查 -fkrrlkse 或 -fkrrlkss 是否发生错误" ).arg( applicationPid.c_str( ) ) );
		return;
	}
	// 获取读写目标
	auto destFileTextMap = getFileText( dest );
	// 切分关键字
	auto destFileTextMapKeys = getPairsKey( destFileTextMap, "\n", false );
	// 处理完全匹配
	if( sourceEquCount ) {
		// 获取读写目标
		auto sourceEquFileTextMap = getFileText( sourceEqu );
		// 切分关键字
		auto sourceEquFileTextMapKeys = getPairsKey( sourceEquFileTextMap, "\n", false );
		// 保存到容器
		std::vector< QString > sourceEquKeys;
		for( auto &iter : sourceEquFileTextMapKeys )
			for( auto &checkStr : iter.second )
				sourceEquKeys.emplace_back( checkStr );
		std::sort( sourceEquKeys.begin( ), sourceEquKeys.end( ), compQString );
		std::sort( sourceEquKeys.begin( ), sourceEquKeys.end( ), compQStringLen );
		unmapRemoveIfEquKeys( destFileTextMapKeys, sourceEquKeys );
	}
	// 处理段内匹配
	if( sourceSubCount ) {
		// 获取读写目标
		auto sourceSubFileTextMap = getFileText( sourceSub );
		// 切分关键字
		auto sourceSubFileTextMapKeys = getPairsKey( sourceSubFileTextMap, "\n", true );
		// 保存到容器
		std::vector< QString > sourceSubKeys;
		for( auto &iter : sourceSubFileTextMapKeys )
			for( auto &checkStr : iter.second )
				sourceSubKeys.emplace_back( checkStr );
		sourceSubKeys = vectorStrAdjustSubStr( sourceSubKeys );
		unmapRemoveIfSubKeys( destFileTextMapKeys, sourceSubKeys );
	}
}
std::vector< QString > getOptionExisFilePath( const std::shared_ptr< cylStd::ArgParser > &arg_parser, const cylStd::ArgParser::String &option, QString end_str ) {
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
std::vector< QString > getOptionLegitimateOutDirPath( const std::shared_ptr< cylStd::ArgParser > &arg_parser, const cylStd::ArgParser::String &option ) {
	std::vector< QString > result;

	auto optionValues = arg_parser->getOptionValues( option );
	if( optionValues )
		for( auto &value : *optionValues ) {
			QString path = QString::fromLocal8Bit( value );
			if( Path::creatDirPath( path ) )
				result.emplace_back( QFileInfo( path ).absoluteFilePath( ) + QDir::separator( ) );
		}
	return result;
}
bool lenMapHasEquStr( const Len_Map_Str_Vector_S &inster_map_obj_shared_ptr, const interfacePlugsType::HtmlDocString &comp_str ) {
	auto strLen = cylHtmlTools::HtmlStringTools::HtmlStringLen( &comp_str );
	auto begin = inster_map_obj_shared_ptr.begin( );
	auto end = inster_map_obj_shared_ptr.end( );
	for( ; begin != end; ++begin ) {
		size_t first = begin->first;
		if( first < strLen )
			continue;
		if( first == strLen ) {
			for( auto &str : *begin->second )
				if( cylHtmlTools::HtmlStringTools::equHtmlString( str, comp_str ) )
					return true;
			return false;
		} else if( first > strLen ) // 超出长度，则退出
			return false;
	}
	return false;
}
bool lenMapHasSubStr( const Len_Map_Str_Vector_S &inster_map_obj_shared_ptr, const interfacePlugsType::HtmlDocString &comp_str ) {
	auto strLen = cylHtmlTools::HtmlStringTools::HtmlStringLen( &comp_str );
	auto begin = inster_map_obj_shared_ptr.begin( );
	auto end = inster_map_obj_shared_ptr.end( );
	for( ; begin != end; ++begin )
		if( begin->first > strLen )
			return false;
		else
			for( auto &str : *begin->second )
				if( cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &comp_str, &str ) )
					return true;
	return false;
}
void vectorConverToLenMap( Len_Map_Str_Vector_S &inster_map_obj, const std::vector< interfacePlugsType::HtmlDocString > &vector ) {
	for( auto &str : vector ) {
		auto strLen = cylHtmlTools::HtmlStringTools::HtmlStringLen( &str );
		auto begin = inster_map_obj.begin( );
		auto end = inster_map_obj.end( );
		for( ; begin != end; ++begin )
			if( begin->first == strLen ) {
				begin->second->emplace_back( str );
				break;
			}
		if( begin != end )
			continue;
		Str_Vector_S insterVectorShared = std::make_shared< Str_Vector >( );
		insterVectorShared->emplace_back( str );
		inster_map_obj.emplace_back( std::make_pair( strLen, insterVectorShared ) );
	}
}
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getDBNovelsInfo( const std::vector< QString > &db_paths, QMutex &std_cout_mutex, QMutex &inster_novel_vector_map_mutex, NovelDBJob::NovelTypePairVector_Shared &inster_novel_vector_map_obj, size_t &novel_inster_count, size_t &db_novel_count, const size_t expire, bool has_ex_option, const Len_Map_Str_Vector_S &jump_equ_name_len_map, const Len_Map_Str_Vector_S &jump_sub_name_len_map, bool &is_db_path_ative ) {
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	std::string callFunctionName = __FUNCTION__;
	for( auto &dbPath : db_paths )
		resultPool->appendWork( [=,&is_db_path_ative,
				&std_cout_mutex,&inster_novel_vector_map_mutex,
				&inster_novel_vector_map_obj,&novel_inster_count,&db_novel_count] ( cylHtmlTools::HtmlWorkThread * ) {
				QFileInfo fileInfo( dbPath );
				QString absolutePath = fileInfo.absoluteDir( ).absolutePath( ); // 数据库所在文件夹名称路径
				QString absoluteFilePath = fileInfo.absoluteFilePath( ); // 数据库文件路径名称
				QString fileName = fileInfo.fileName( ); // 数据库基本名称
				QString dbDirTopLevePath = QFileInfo( absolutePath ).absoluteDir( ).absolutePath( ); // 数据库目录的父级目录
				std::shared_ptr< NovelDBJob::NovelInfoVector > novelInfoVectorShared = NovelDBJob::readDB( nullptr, absolutePath, fileName );
				size_t size = 0;
				// 是否读取到数据库
				if( novelInfoVectorShared ) {
					size_t readDBNovelInfoCount = novelInfoVectorShared->size( );
					while( readDBNovelInfoCount != 0 ) {
						// 是否存在删除过期选项
						if( has_ex_option ) {
							auto isExpire = NovelDBJob::novelVectorIsExpire( expire, *novelInfoVectorShared );
							if( isExpire.size( ) > 0 ) {
								auto novelUrlVector = NovelDBJob::novelVectorGetNovleUrl( isExpire );
								NovelDBJob::removeNovelVectorDB( nullptr, absolutePath, fileName, novelUrlVector, dbDirTopLevePath );
								*novelInfoVectorShared = NovelDBJob::formVectorINovelInfoRemoveVectorINovelInfo( *novelInfoVectorShared, isExpire );
							}
						}
						readDBNovelInfoCount = novelInfoVectorShared->size( );
						inster_novel_vector_map_mutex.lock( );
						db_novel_count += readDBNovelInfoCount;
						is_db_path_ative = true;
						inster_novel_vector_map_mutex.unlock( );
						if( readDBNovelInfoCount == 0 )
							break;
						auto jumpEquNameVectorDataSize = jump_equ_name_len_map.size( ); // 相等过滤
						auto jumpSubNameVectorDataSize = jump_sub_name_len_map.size( ); // 子过滤
						if( jumpSubNameVectorDataSize || jumpEquNameVectorDataSize ) {
							// 存在过滤，则开始过滤
							std::shared_ptr< NovelDBJob::NovelInfoVector > filterVector = std::make_shared< NovelDBJob::NovelInfoVector >( );
							QMutex vectorInsterMutex;
							cylHtmlTools::HtmlWorkThreadPool threadPool;
							threadPool.setCallSepMilliseconds( duration );
							threadPool.setIdleTimeCall( [&,callFunctionName] ( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &modWork, const unsigned long long &currentWork ) {
								Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, modWork, currentWork, callFunctionName, QString(u8"数据库( %1 )过滤小说名称进行中").arg( absoluteFilePath).toStdString( ) );
							} );
							for( auto &novel : *novelInfoVectorShared )
								threadPool.appendWork( [novel,
										jump_equ_name_len_map, jumpEquNameVectorDataSize,
										jump_sub_name_len_map, jumpSubNameVectorDataSize,
										&vectorInsterMutex,filterVector] ( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
										interfacePlugsType::HtmlDocString name = novel->novelName; // 小说名称转换为大写
										name = NovelDBJob::converStringToUpper( name );
										cylHtmlTools::HtmlStringTools::removeAllSpace( name );
										if( !filterNovelName( name, jump_equ_name_len_map, jump_sub_name_len_map ) ) {
											vectorInsterMutex.lock( );
											filterVector->emplace_back( novel );
											vectorInsterMutex.unlock( );
										}
									} );
							threadPool.start( );
							while( !threadPool.isOverJob( ) )
								qApp->processEvents( );
							inster_novel_vector_map_mutex.lock( );
							inster_novel_vector_map_obj->emplace_back( absoluteFilePath, filterVector );
							size = filterVector->size( );
							novel_inster_count += size;
							inster_novel_vector_map_mutex.unlock( );

						} else {
							Out_Std_Count_Stream_Msg_MACRO( std_cout_mutex, callFunctionName, QString(u8"数据库( %1 )过滤小说名称进行中").arg( absoluteFilePath).toStdString( ) );
							inster_novel_vector_map_mutex.lock( );
							inster_novel_vector_map_obj->emplace_back( absoluteFilePath, novelInfoVectorShared );
							size = novelInfoVectorShared->size( );
							novel_inster_count += size;
							inster_novel_vector_map_mutex.unlock( );
						}
						break;
					}
					if( readDBNovelInfoCount == 0 ) { // 如果数据库数量为 0
						if( Path::removePath( absoluteFilePath ) ) {
							QString msg( "( 进程 id : %1 ) 路径 [ %2 ] 加载数量为 0，删除行为成功" );
							Out_Std_Count_Stream_Msg_MACRO( std_cout_mutex, callFunctionName, msg.arg( applicationPid.c_str( )).arg( absoluteFilePath ).toStdString( ) );
						} else {
							std_cout_mutex.lock( );
							QString msg( "( 进程 id : %1 ) 路径 [ %2 ] 加载数量为 0，删除行为操作失败" );
							ErrorCout_FunctionName_MACRO( msg.arg( applicationPid.c_str( )).arg( absoluteFilePath ), callFunctionName.c_str( ) );
							std_cout_mutex.unlock( );
						}
					}
					Out_Std_Count_Stream_Msg_MACRO( std_cout_mutex, callFunctionName, QString(u8"数据库( %1 ) 有效小说为 : [ %2 ]").arg( absoluteFilePath).arg( size ).toStdString( ) );
				} else {
					std_cout_mutex.lock( );
					QString msg( "( 进程 id : %1 )路径[ %2 ]无法加载小说列表" );
					ErrorCout_MACRO( msg.arg( applicationPid.c_str( )).arg( absoluteFilePath ) );
					std_cout_mutex.unlock( );
				}
			} );
	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	resultPool->setIdleTimeCall( [&,callFunctionName] ( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &modWork, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, modWork, currentWorkCount, callFunctionName, u8"数据库读取序列信息" );
	} );

	return resultPool;
}
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getFindKeyFileKeyToMap( const std::shared_ptr< cylStd::ArgParser > &arg_parser, const cylStd::ArgParser::String &option, std::unordered_map< QString, std::vector< interfacePlugsType::HtmlDocString > > &save_obj, QMutex &std_cout_mutex, QMutex &inster_read_find_key_map_mutex ) {
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );

	// 查找关键字配置文件
	auto exisFindFilePath = getOptionExisFilePath( arg_parser, option );
	auto pairs = getFileText( exisFindFilePath );
	for( auto &iter : pairs ) {
		resultPool->appendWork( [iter,&save_obj,
				&inster_read_find_key_map_mutex] ( cylHtmlTools::HtmlWorkThread *html_work_thread
			) {
				auto stringList = iter.second.split( "\n" );
				std::vector< QString > strBuff;
				for( auto &str : stringList )
					if( !removeAllSpace( str ).isEmpty( ) )
						strBuff.emplace_back( str.toUpper( ) );
				strBuff = vectorStrAdjustSubStr( strBuff );
				writeJionStringVector( iter.first, strBuff, "\n" );
				inster_read_find_key_map_mutex.lock( );
				auto begin = save_obj.begin( );
				auto end = save_obj.end( );
				auto baseName = getFileBaseName( iter.first );
				for( ; begin != end; ++begin ) {
					if( begin->first == baseName ) {
						std::vector< interfacePlugsType::HtmlDocString > &saveVector = begin->second;
						for( auto converQStr : strBuff )
							saveVector.emplace_back( converQStr.toStdWString( ) );
						break;
					}
				}
				inster_read_find_key_map_mutex.unlock( );
				if( begin != end )
					return;
				std::vector< interfacePlugsType::HtmlDocString > saveVector;
				for( auto converQStr : strBuff )
					saveVector.emplace_back( converQStr.toStdWString( ) );
				inster_read_find_key_map_mutex.lock( );
				save_obj.insert_or_assign( baseName, saveVector );
				inster_read_find_key_map_mutex.unlock( );
			} );
	}
	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	std::string callFunctionName = __FUNCTION__;
	resultPool->setIdleTimeCall( [&,callFunctionName] ( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, workCount, currentWorkCount, callFunctionName, u8"获取关键字文件信息-分发映射-" );
	} );
	return resultPool;
}
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getFindKeyFileKeyToVector( const std::shared_ptr< cylStd::ArgParser > &arg_parser, const cylStd::ArgParser::String &option, Len_Map_Str_Vector_S_Shared &save_obj, QMutex &std_cout_mutex, QMutex &inster_read_find_key_map_mutex ) {
	// 查找关键字配置文件
	auto exisFindFilePath = getOptionExisFilePath( arg_parser, option );
	auto pairs = getFileText( exisFindFilePath );
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	for( auto &iter : pairs ) {
		resultPool->appendWork( [iter,&save_obj,&std_cout_mutex,&inster_read_find_key_map_mutex] ( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
			auto stringList = iter.second.split( "\n" );
			std::vector< QString > strBuff;
			for( auto &str : stringList )
				if( !removeAllSpace( str ).isEmpty( ) )
					strBuff.emplace_back( str.toUpper( ) );
			strBuff = vectorStrDuplicate( strBuff );
			vectorStrLenSort( strBuff );
			writeJionStringVector( iter.first, strBuff, "\n" );
			Str_Vector strVector;
			for( auto converQStr : strBuff )
				strVector.emplace_back( converQStr.toStdWString( ) );
			inster_read_find_key_map_mutex.lock( );
			vectorConverToLenMap( *save_obj, strVector );
			inster_read_find_key_map_mutex.unlock( );
		} );
	}
	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	std::string callFunctionName = __FUNCTION__;
	resultPool->setIdleTimeCall( [&,callFunctionName] ( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, workCount, currentWorkCount, callFunctionName, u8"获取关键字文件信息-序列容器-" );
	} );
	return resultPool;
}
bool emplaceWriteMap( const QString &write_path, const interfacePlugsType::INovelInfo_Shared &write_novel_shared_ptr, NovelDBJob::NovelTypePairVector &novel_infos_write_map ) {
	for( auto &iterator : novel_infos_write_map )
		if( iterator.first == write_path ) {
			iterator.second->emplace_back( write_novel_shared_ptr );
			return true;
		}
	auto vectorShared = std::make_shared< NovelDBJob::NovelInfoVector >( );
	vectorShared->emplace_back( write_novel_shared_ptr );
	novel_infos_write_map.emplace_back( write_path, vectorShared );
	return true;
}
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getDBFilterNovelInfo( const NovelDBJob::NovelTypePairVector_Shared &vector_db_novelinfo_pairt, NovelDBJob::NovelTypePairVector_Shared novel_infos_write_map, QMutex &std_cout_mutex, QMutex &inster_novel_vectos_mutex ) {
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	QString allRequestGetFileBaseName = u8"合并小说_全部小说节点归纳存档.txt"; // 全部获取的基本名称
	for( auto &iter : *vector_db_novelinfo_pairt )
		for( auto &novel : *iter.second )
			resultPool->appendWork( [iter,novel,novel_infos_write_map,&inster_novel_vectos_mutex,allRequestGetFileBaseName] ( cylHtmlTools::HtmlWorkThread *html_work_thread ) ->void {
				const QFileInfo orgDbFilePathInfo( iter.first ); // 数据库文件
				// 中间路径(-w选项路径/中间路径/小说类型.txt)
				const QString typeMidDirPath = QString( u8"%1%2%3%2" ).arg( exportAllDirMidName ).arg( QDir::separator( ) ).arg( orgDbFilePathInfo.fileName( ) );
				// 中间路径(-w选项路径/中间路径/全部获取.txt)
				const QString allRootRequestGetNovelInfoPath = QString( u8"%1%2%3%2%4" ).arg( exportAllDirMidName ).arg( QDir::separator( ) ).arg( orgDbFilePathInfo.fileName( ) ).arg( allRequestGetFileBaseName );
				// 中间路径(-w选项路径/全部获取.txt)
				const QString allRequestGetNovelInfoPath = QString( u8"%1%2%3" ).arg( exportAllDirMidName ).arg( QDir::separator( ) ).arg( allRequestGetFileBaseName );

				////
				/// 存储到匹配的类型文件
				////
				inster_novel_vectos_mutex.lock( );
				auto qstrName = QString::fromStdWString( novel->typeName ); // 获取类型名称
				qstrName = typeMidDirPath + qstrName + u8".txt";
				emplaceWriteMap( qstrName, novel, *novel_infos_write_map );
				inster_novel_vectos_mutex.unlock( );

				////
				/// 存储到匹配的首页文件
				////
				inster_novel_vectos_mutex.lock( );
				emplaceWriteMap( allRootRequestGetNovelInfoPath, novel, *novel_infos_write_map );
				inster_novel_vectos_mutex.unlock( );

				///
				/// 插入全部获取
				////
				inster_novel_vectos_mutex.lock( );
				emplaceWriteMap( allRequestGetNovelInfoPath, novel, *novel_infos_write_map );
				inster_novel_vectos_mutex.unlock( );
			} );

	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	std::string callFunctionName = __FUNCTION__;
	resultPool->setIdleTimeCall( [&,callFunctionName] ( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, workCount, currentWorkCount, callFunctionName, u8"小说过滤后全导出" );
	} );
	return resultPool;
}
void emplaceNovelKeyMap( const QString &save_path, const std::shared_ptr< interfacePlugsType::INovelInfo > &novel_info, const QString &key, PathWriteNovelInfoUnMap &merge_find_keys_novel_infos_map ) {
	auto iterator = merge_find_keys_novel_infos_map.begin( );
	auto end = merge_find_keys_novel_infos_map.end( );
	while( iterator != end ) {
		if( iterator->first == save_path ) {
			auto &pairVector = iterator->second;
			for( auto pair : pairVector )
				if( pair.first == novel_info ) {
					auto &keys = pair.second;
					auto keyVectorIterator = keys.begin( ), keyVectorEnd = keys.end( );
					for( ; keyVectorIterator != keyVectorEnd; ++keyVectorIterator )
						if( *keyVectorIterator == key )
							return;
					keys.emplace_back( key );
					return; // 存储关键字后退出
				}
			std::pair< interfacePlugsType::INovelInfo_Shared, QStringList > makePair( novel_info, {key} );
			// 没有找到匹配小说
			pairVector.emplace_back( makePair );
			return; // 存储关键字后退出
		}
		++iterator; // 继续寻找下一个
	}
	std::pair< interfacePlugsType::INovelInfo_Shared, QStringList > makePair( novel_info, {key} );
	// 没有找到匹配小说
	std::vector< std::pair< interfacePlugsType::INovelInfo_Shared, QStringList > > pairVector;
	pairVector.emplace_back( makePair );
	merge_find_keys_novel_infos_map.emplace_back( std::make_pair( save_path, pairVector ) );
}
bool lenMapFindNovelKey( const NovelInfo &novel_info, const Len_Map_Str_Vector_S &find_key_map, interfacePlugsType::HtmlDocString *key ) {
	size_t novelNameLen = novel_info.novelName.length( ); // 名车长度
	size_t novelInfoLen = novel_info.info.length( ); // 详情长度
	size_t novelLastItemLen = novel_info.lastItem.length( ); // 最后更新项长度
	size_t novelAuthorLen = novel_info.author.length( ); // 作者长度
	size_t maxLen = std::min( novelNameLen, novelInfoLen );
	maxLen = std::max( maxLen, novelLastItemLen );
	maxLen = std::max( maxLen, novelAuthorLen );
	for( auto begin = find_key_map.begin( ), end = find_key_map.end( ); begin != end; ++begin ) {
		auto lenKey = begin->first;
		if( maxLen < lenKey )
			break;
		if( lenKey <= novelNameLen && getValue( novel_info.novelName, key, *begin->second ) )
			return true;
		if( lenKey <= novelInfoLen && getValue( novel_info.info, key, *begin->second ) )
			return true;
		if( lenKey <= novelLastItemLen && getValue( novel_info.lastItem, key, *begin->second ) )
			return true;
		if( lenKey <= novelAuthorLen && getValue( novel_info.author, key, *begin->second ) )
			return true;
	}
	return false;
}
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > getDBFindNovelInfo( const NovelDBJob::NovelTypePairVector_Shared &novel_type_pair_vector_shared, const std::unordered_map< QString, std::vector< interfacePlugsType::HtmlDocString > > &exis_find_file_path_map_key, QMutex &std_cout_mutex, QMutex &merge_find_keys_types_novel_infos_map_mutex, PathWriteNovelInfoUnMap &merge_find_keys_types_novel_infos_map, QMutex &merge_find_keys_dbs_novel_infos_map_mutex, PathWriteNovelInfoUnMap &merge_find_keys_dbs_novel_infos_map, QMutex &merge_find_keys_export_key_novel_infos_map_mutex, PathWriteNovelInfoUnMap &merge_find_keys_export_key_novel_infos_map, QMutex &merge_find_keys_export_novel_infos_map_mutex, PathWriteNovelInfoUnMap &merge_find_keys_export_novel_infos_map, QMutex &inster_map_thread_pool_mutex, std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > &inster_map_thread_pool ) {

	std::string callFunctionName = __FUNCTION__;
	if( inster_map_thread_pool == nullptr )
		inster_map_thread_pool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	inster_map_thread_pool_mutex.lock( );
	inster_map_thread_pool->setCallSepMilliseconds( duration );
	inster_map_thread_pool->setWorkCount( cylHtmlTools::HtmlWorkThreadPool::getSystemCupCount( ) * 2 );
	inster_map_thread_pool->setIdleTimeCall( [&std_cout_mutex, callFunctionName] ( cylHtmlTools::HtmlWorkThreadPool *htmlWorkThreadPool, const unsigned long long &modWorkCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, modWorkCount, currentWorkCount, callFunctionName, u8"正在进行插入任务" );
	} );
	inster_map_thread_pool_mutex.unlock( );
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	for( auto iter : *novel_type_pair_vector_shared ) {
		for( auto novel : *iter.second ) {
			QString dbBaseName = getFileBaseName( iter.first ); // 当前遍历的文件基本名称(root url)
			resultPool->appendWork( [iter,
					&exis_find_file_path_map_key,
					&merge_find_keys_dbs_novel_infos_map_mutex,
					&merge_find_keys_dbs_novel_infos_map,
					&merge_find_keys_types_novel_infos_map_mutex,
					&merge_find_keys_types_novel_infos_map,
					&merge_find_keys_export_key_novel_infos_map_mutex,
					&merge_find_keys_export_key_novel_infos_map,
					&merge_find_keys_export_novel_infos_map_mutex,
					&merge_find_keys_export_novel_infos_map,
					&std_cout_mutex,
					&inster_map_thread_pool_mutex,
					&inster_map_thread_pool,
					novel,
					dbBaseName,
					callFunctionName
				] ( cylHtmlTools::HtmlWorkThread *html_work_thread )->void {
					////
					/// 遍历 key
					////
					for( auto &findSubStrMapIterator : exis_find_file_path_map_key ) {
						auto findKeys = findSubStrMapIterator.second;
						Len_Map_Str_Vector_S lenKeyMap;
						vectorConverToLenMap( lenKeyMap, findKeys );
						std::sort( lenKeyMap.begin( ),
									lenKeyMap.end( ),
									[] ( Len_Pair &left, Len_Pair &right ) {
										return left.first < right.first;
									} );
						interfacePlugsType::HtmlDocString key;
						if( lenMapFindNovelKey( *novel, lenKeyMap, &key ) ) {
							QString keyFile = findSubStrMapIterator.first;

							inster_map_thread_pool_mutex.lock( );
							// 关键字路径 -w/export_find/关键字/关键字.txt 任务
							inster_map_thread_pool->appendWork( [novel,keyFile,key,
									&merge_find_keys_export_key_novel_infos_map_mutex,
									&merge_find_keys_export_key_novel_infos_map] ( cylHtmlTools::HtmlWorkThread *htmlWorkThread ) {
									merge_find_keys_export_key_novel_infos_map_mutex.lock( );
									// 关键字路径 -w/export_find/关键字/关键字.txt
									QString savePath = QString( u8"%1%2%1%3%1%3.txt" ).arg( QDir::separator( ) ).arg( exportFindDirMidName ).arg( keyFile );
									// 保存到匹配映射
									emplaceNovelKeyMap( savePath, novel, QString::fromStdWString( key ), merge_find_keys_export_key_novel_infos_map );
									merge_find_keys_export_key_novel_infos_map_mutex.unlock( );

								} );

							// 关键字路径 -w/export_find/关键字/网址.txt 任务
							inster_map_thread_pool->appendWork( [novel,keyFile,dbBaseName,key,
									&merge_find_keys_dbs_novel_infos_map_mutex,
									&merge_find_keys_dbs_novel_infos_map] (
								cylHtmlTools::HtmlWorkThread *htmlWorkThread ) {
									merge_find_keys_dbs_novel_infos_map_mutex.lock( );
									// 关键字路径 -w/export_find/关键字/网址.txt
									QString savePath = QString( u8"%1%2%1%3%1%4.txt" ).arg( QDir::separator( ) ).arg( exportFindDirMidName ).arg( keyFile ).arg( dbBaseName );
									// 保存到匹配映射
									emplaceNovelKeyMap( savePath, novel, QString::fromStdWString( key ), merge_find_keys_dbs_novel_infos_map );
									merge_find_keys_dbs_novel_infos_map_mutex.unlock( );
								} );

							// 关键字路径 -w/export_find/关键字/网址/类型.txt 任务
							inster_map_thread_pool->appendWork( [novel,keyFile,key,dbBaseName,
									&merge_find_keys_types_novel_infos_map_mutex,
									&merge_find_keys_types_novel_infos_map] (
								cylHtmlTools::HtmlWorkThread *htmlWorkThread ) {
									merge_find_keys_types_novel_infos_map_mutex.lock( );
									// 关键字路径 -w/export_find/关键字/网址/类型.txt
									QString savePath = QString( u8"%1%2%1%3%1%4%1%5.txt" ).arg( QDir::separator( ) ).arg( exportFindDirMidName ).arg( keyFile ).arg( dbBaseName ).arg( novel->typeName );
									// 保存到匹配映射
									emplaceNovelKeyMap( savePath, novel, QString::fromStdWString( key ), merge_find_keys_types_novel_infos_map );
									merge_find_keys_types_novel_infos_map_mutex.unlock( );
								} );

							// 关键字路径 -w/export_find/export_find.txt 任务
							inster_map_thread_pool->appendWork( [novel,keyFile,key,
									&merge_find_keys_export_novel_infos_map_mutex,
									&merge_find_keys_export_novel_infos_map] (
								cylHtmlTools::HtmlWorkThread *htmlWorkThread ) {
									merge_find_keys_export_novel_infos_map_mutex.lock( );
									// 关键字路径 -w/export_find/export_find.txt
									QString savePath = QString( u8"%1%2%1%2.txt" ).arg( QDir::separator( ) ).arg( exportFindDirMidName );
									// 保存到匹配映射
									emplaceNovelKeyMap( savePath, novel, getFileBaseName( keyFile ), merge_find_keys_export_novel_infos_map );
									merge_find_keys_export_novel_infos_map_mutex.unlock( );
								} );
							inster_map_thread_pool_mutex.unlock( );
						}
					}
				} );
		}
	}
	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	resultPool->setWorkCount( cylHtmlTools::HtmlWorkThreadPool::getSystemCupCount( ) * 2 );
	resultPool->setIdleTimeCall( [&,callFunctionName] ( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, workCount, currentWorkCount, callFunctionName, u8"小说过滤后查找" );
	} );
	return resultPool;
}
std::vector< interfacePlugsType::HtmlDocString > vectorStrAdjustSubStr( const std::vector< interfacePlugsType::HtmlDocString > &str_vector ) {
	auto endIndex = str_vector.size( );
	if( endIndex < 2 )
		return str_vector;
	// 长度排序
	std::vector< const interfacePlugsType::HtmlDocString * > clone;
	for( auto &str : str_vector ) {
		auto iterator = clone.begin( ), end = clone.end( );
		for( ; iterator != end; ++iterator ) {
			const interfacePlugsType::HtmlDocString *value = *iterator;
			if( value->length( ) > str.length( ) )
				break;
		}
		clone.insert( iterator, &str );
	}

	std::vector< interfacePlugsType::HtmlDocString > resultVector;
	std::vector< const interfacePlugsType::HtmlDocString * > resultBuffVector;
	while( endIndex ) {
		auto startObj = clone.at( 0 );
		resultVector.emplace_back( *startObj );
		for( auto newIndex = 1; newIndex < endIndex; ++newIndex ) {
			const auto &sub = clone.at( newIndex );
			if( !cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( sub, startObj ) )
				resultBuffVector.emplace_back( sub );
		}
		clone = resultBuffVector;
		resultBuffVector.clear( );
		endIndex = clone.size( );
	}
	return resultVector;
}
void spliteAppendDataField( std::vector< NovelInfo_Shared > &novelInfos ) {
	for( auto &novelInfo : novelInfos ) {
		std::vector< cylHtmlTools::HtmlString > split = cylHtmlTools::HtmlStringTools::split( novelInfo->additionalData, L',' );
		auto oldSize = split.size( );
		split = vectorStrAdjustSubStr( split );
		if( split.size( ) == oldSize )
			continue; // 如果大小相等，则不更新
		novelInfo->additionalData.clear( );
		auto begin = split.begin( );
		auto end = split.end( );
		if( begin != end )
			do {
				novelInfo->additionalData.append( *begin );
				++begin;
				if( begin == end )
					break;
				novelInfo->additionalData.append( L", " );
			} while( true ) ;
	}
}
void novelInfoExportSort( std::vector< NovelInfo_Shared > &write_info_vector ) {
	std::list< std::pair< qint64, interfacePlugsType::INovelInfo_Shared > > nameSort, // 名称排序容器
																			timeSort; // 日期排序容器
	auto timeForm = NovelDBJob::getCurrentTimeForm( );
	for( auto &novelSPtr : write_info_vector ) {
		auto comp_ptr = novelSPtr.get( )->novelName;
		auto iterator = nameSort.begin( ), end = nameSort.end( );
		for( ; iterator != end; ++iterator )
			if( comp_ptr > iterator->second.get( )->novelName )
				break;
		auto updateTime = novelSPtr.get( )->updateTime;
		QString fromStdWString = QString::fromStdWString( updateTime );
		QDateTime dateTime = QDateTime::fromString( fromStdWString, timeForm );
		qint64 mSecsSinceEpoch = dateTime.toMSecsSinceEpoch( );
		auto makePair = std::make_pair( mSecsSinceEpoch, novelSPtr );
		nameSort.insert( iterator, makePair );
	}

	for( auto &novelSPtr : nameSort ) {
		auto comp_ptr = novelSPtr.first;
		auto iterator = timeSort.begin( ), end = timeSort.end( );
		for( ; iterator != end; ++iterator )
			if( comp_ptr > iterator->first )
				break;
		timeSort.insert( iterator, novelSPtr );
	}
	write_info_vector.clear( );
	for( auto &novelSPtr : timeSort )
		write_info_vector.emplace_back( novelSPtr.second );
}
void writeDisk( QMutex &disk_mute, QMutex &count_mute, QMutex &std_cout_mutex, const QString &all_file_path_name_name, size_t &write_novel_count, const NovelInfoKeyPairVector &infos, const std::string &callFunctionName ) {
	disk_mute.lock( );
	bool isCreate = Path::creatFilePath( all_file_path_name_name );
	disk_mute.unlock( );
	if( !isCreate ) {
		// 无法创建，则返回
		std_cout_mutex.lock( );
		QString msg( u8"(进程 id : %1 , 线程 id : %2 ) 路径创建失败 : \"%3\" => (数量为 : %4)" );
		std::stringstream ss;
		ss << std::this_thread::get_id( );
		msg = msg.arg( applicationPid.c_str( ) ).arg( QString::fromStdString( ss.str( ) ) ).arg( all_file_path_name_name ).arg( 0 );
		ErrorCout_MACRO( msg );
		std_cout_mutex.unlock( );
		return;
	}

	auto data = infos.data( );
	size_t dataSize = infos.size( );
	if( data == nullptr || dataSize == 0 ) {
		// 没有数据。则返回
		std_cout_mutex.lock( );
		QString msg( u8"(进程 id : %1 , 线程 id : %2 ) 导出结果失败 : \"%3\" => (数量为 : %4)" );
		std::stringstream ss;
		ss << std::this_thread::get_id( );
		msg = msg.arg( applicationPid.c_str( ) ).arg( QString::fromStdString( ss.str( ) ) ).arg( all_file_path_name_name ).arg( 0 );
		ErrorCout_MACRO( msg );
		std_cout_mutex.unlock( );
		return;
	}
	std::vector< NovelInfo_Shared > writeInfoVector;
	for( size_t index = 0; index < dataSize; ++index ) {
		NovelInfo_Shared novelInfoShared = std::make_shared< NovelInfo >( );
		auto &pair = data[ index ];
		*novelInfoShared = *pair.first;
		novelInfoShared->additionalData = pair.second.join( "," ).toStdWString( );
		writeInfoVector.emplace_back( novelInfoShared );
	}

	// 排序
	std::sort( writeInfoVector.begin( ),
				writeInfoVector.end( ),
				[] ( const NovelInfo_Shared &left, const NovelInfo_Shared &right ) {
					return left->url > right->url;
				} );

	writeInfoVector = *NovelDBJob::inductionNovelsForNameAndAuthor( writeInfoVector );
	auto fileBaseName = getFileBaseName( all_file_path_name_name );
	spliteAppendDataField( writeInfoVector );
	// writeInfoVector = NovelDBJob::sort( writeInfoVector );
	novelInfoExportSort( writeInfoVector );
	auto list = NovelDBJob::getNovelNames( writeInfoVector );
	QStringList novelNameList( list.begin( ), list.end( ) );
	auto novelNameListJoin = novelNameList.join( "\n" );
	auto writeCount = novelNameList.size( ); // 小说计数
	if( writeCount ) {
		auto allContents = NovelDBJob::jionNovels( writeInfoVector ) + u8"\n+++++\t小说名称列表\t" + QString::number( novelNameList.size( ) ) + " 个\t+++++++\n" + novelNameListJoin + u8"\n++++++++++++\n";
		QFile writeFile( all_file_path_name_name );
		if( writeFile.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
			writeFile.write( allContents.toUtf8( ) );
			writeFile.close( );

			// 添加计数
			count_mute.lock( );
			write_novel_count += writeCount;
			count_mute.unlock( );
			// 添加输出
			std_cout_mutex.lock( );
			std::cout << u8"(进程 id : " << applicationPid << u8", 线程 id : " << std::this_thread::get_id( ) << u8" ) => [ " << callFunctionName << u8" ]导出结果 => \"" << all_file_path_name_name.toStdString( ) << u8"\" => (数量为 : " << writeCount << u8" )" << "\t: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << std::endl;
			std_cout_mutex.unlock( );
		}
	} else {
		std_cout_mutex.lock( );
		QString msg( u8"(进程 id : %1 , 线程 id : %2 ) 导出结果失败 : \"%3\" => (数量为 : %4)" );
		std::stringstream ss;
		ss << std::this_thread::get_id( );
		msg = msg.arg( applicationPid.c_str( ) ).arg( QString::fromStdString( ss.str( ) ) ).arg( all_file_path_name_name ).arg( writeCount );
		ErrorCout_MACRO( msg );
		std_cout_mutex.unlock( );
	}

}
void writeDisk( QMutex &disk_mute, QMutex &count_mute, QMutex &std_cout_mutex, const QString &all_file_path_name_name, size_t &write_novel_count, const NovelDBJob::NovelInfoVector &infos, const std::string &callFunctionName ) {
	disk_mute.lock( );
	bool isCreate = Path::creatFilePath( all_file_path_name_name );
	disk_mute.unlock( );
	if( isCreate ) {
		auto result = NovelDBJob::identical( infos );

		// 排序
		std::sort( result.begin( ),
					result.end( ),
					[] ( const interfacePlugsType::INovelInfo_Shared &left, const interfacePlugsType::INovelInfo_Shared &right ) {
						return left->url > right->url;
					} );

		result = *NovelDBJob::inductionNovelsForNameAndAuthor( result );
		/*result = NovelDBJob::sort( result );*/
		novelInfoExportSort( result );
		auto list = NovelDBJob::getNovelNames( result );
		QStringList novelNameList( list.begin( ), list.end( ) );
		auto novelNameListJoin = novelNameList.join( "\n" );
		auto writeCount = novelNameList.size( ); // 小说计数
		if( writeCount ) {
			count_mute.lock( );
			write_novel_count += writeCount;
			count_mute.unlock( );
			auto allContents = NovelDBJob::jionNovels( result ) + u8"\n+++++\t小说名称列表\t" + QString::number( novelNameList.size( ) ) + " 个\t+++++++\n" + novelNameListJoin + u8"\n++++++++++++\n";
			QFile writeFile( all_file_path_name_name );
			if( writeFile.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
				writeFile.write( allContents.toUtf8( ) );
				writeFile.close( );
				std_cout_mutex.lock( );
				std::cout << u8"(进程 id : " << applicationPid << u8", 线程 id : " << std::this_thread::get_id( ) << u8" ) => [ " << callFunctionName << u8" ]导出结果 => \"" << all_file_path_name_name.toStdString( ) << u8"\" => (数量为 : " << writeCount << u8" )" << "\t: " << getRunSepSecAndMillAndMinAndHourToStdString( ) << std::endl;
				std_cout_mutex.unlock( );
			}
		} else {
			std_cout_mutex.lock( );
			QString msg( u8"(进程 id : %1 , 线程 id : %2 ) 导出结果失败 : \"%3\" => (数量为 : %4)" );
			std::stringstream ss;
			ss << std::this_thread::get_id( );
			msg = msg.arg( applicationPid.c_str( ) ).arg( QString::fromStdString( ss.str( ) ) ).arg( all_file_path_name_name ).arg( writeCount );
			ErrorCout_MACRO( msg );
			std_cout_mutex.unlock( );
		}
	} else {
		std_cout_mutex.lock( );
		QString msg( u8"(进程 id : %1 , 线程 id : %2 ) 导出查找结果失败 : \"%3\" => (无法创建文件)" );
		std::stringstream ss;
		ss << std::this_thread::get_id( );
		msg = msg.arg( applicationPid.c_str( ) ).arg( QString::fromStdString( ss.str( ) ) ).arg( all_file_path_name_name );
		ErrorCout_MACRO( msg );
		std_cout_mutex.unlock( );
	}
}
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > writeDiskInForNovels( const NovelDBJob::NovelTypePairVector_Shared &novel_infos_write_map, const std::vector< QString > &exis_legitimate_out_dir_path, QMutex &std_cout_mutex, QMutex &disk_mute, QMutex &count_mute, size_t &write_novel_count, std::vector< PathManage > &path_manages ) {
	std::string callFunctionName = __FUNCTION__; // 调用函数名
	// 返回线程
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	for( auto &outDirPath : exis_legitimate_out_dir_path ) {

		for( auto &mapIterator : *novel_infos_write_map )
			resultPool->appendWork( [&std_cout_mutex,&disk_mute,&count_mute,&write_novel_count,&path_manages, mapIterator,outDirPath,callFunctionName] ( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
				QString fileLastBody = mapIterator.first; // 路径尾部
				auto allFilePathName = outDirPath + fileLastBody;
				for( auto &pathmanage : path_manages )
					pathmanage.updateFilePath( allFilePathName.toStdString( ) );
				writeDisk( disk_mute, count_mute, std_cout_mutex, allFilePathName, write_novel_count, *mapIterator.second, callFunctionName );
			} );
	}
	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	resultPool->setIdleTimeCall( [&,callFunctionName] ( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, workCount, currentWorkCount, callFunctionName, u8"正在写入磁盘" );
	} );
	return resultPool;
}
std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > writeDiskInForInductionNovels( const std::vector< QString > &exis_legitimate_out_dir_path, const PathWriteNovelInfoUnMap &novel_keys_map, QMutex &std_cout_mutex, QMutex &disk_mute, QMutex &count_mute, size_t &write_novel_count, std::vector< PathManage > &path_manages ) {
	std::string callFunctionName = __FUNCTION__; // 调用函数名
	// 返回线程
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > resultPool = std::make_shared< cylHtmlTools::HtmlWorkThreadPool >( );
	for( auto &outDirPath : exis_legitimate_out_dir_path ) {
		for( auto &mapIterator : novel_keys_map ) {
			resultPool->appendWork(
				[ mapIterator,outDirPath,callFunctionName,
					&disk_mute,&novel_keys_map,
					&count_mute,
					&std_cout_mutex,
					&write_novel_count,
					&path_manages
				] ( cylHtmlTools::HtmlWorkThread *html_work_thread
				) {
					auto allFilePathName = outDirPath + mapIterator.first;
					for( auto &pathmanage : path_manages )
						pathmanage.updateFilePath( allFilePathName.toStdString( ) );
					writeDisk( disk_mute, count_mute, std_cout_mutex, allFilePathName, write_novel_count, mapIterator.second, callFunctionName );
				} );
		}
	}
	if( resultPool->getThreadCount( ) == 0 )
		return nullptr;
	resultPool->setCallSepMilliseconds( duration );
	resultPool->setIdleTimeCall( [&,callFunctionName] ( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &workCount, const unsigned long long &currentWorkCount ) {
		Out_Std_Thread_Pool_Count_Stream_Msg_MACRO( std_cout_mutex, workCount, currentWorkCount, callFunctionName, u8"正在写入磁盘" );
	} );
	return resultPool;
}
void showRunTime( const std::shared_ptr< cylStd::ArgParser > &arg_parser ) {
	if( arg_parser->getOptionValues( "-nstr" ) )
		return;
	auto sepSecAndMillAndMinAndHourToStdString = getRunSepSecAndMillAndMinAndHourToStdString( );
	std::cout << u8"\n============= 进程 id : " << applicationPid << "\n============= 耗时 : " << sepSecAndMillAndMinAndHourToStdString << " \n=============\n" << std::endl;
}
void showStartRunTime( const std::shared_ptr< cylStd::ArgParser > &arg_parser ) {
	if( arg_parser->getOptionValues( "-nsts" ) )
		return;
	auto currentTime = QDateTime::currentDateTime( );
	std::cout << u8"\n============= 进程 id : " << applicationPid << "\n============= 开始时间 : " << currentTime.toString( "hh:mm:ss.z" ).toStdString( ) << " \n=============\n" << std::endl;
}
void showEndRunTime( const std::shared_ptr< cylStd::ArgParser > &arg_parser ) {
	if( arg_parser->getOptionValues( "-nste" ) )
		return;
	auto dateTime = QDateTime::currentDateTime( );
	std::cout << u8"\n============= 进程 id : " << applicationPid << "\n============= 结束时间 : " << dateTime.toString( "hh:mm:ss.z" ).toStdString( ) << " \n=============\n" << std::endl;

}
void rmoveExportPath( const std::vector< QString > &exis_legitimate_out_dir_path, QMutex &stdCoutMutex, const std::string &callFunctionName, const size_t &line ) {
	for( auto removePath : exis_legitimate_out_dir_path ) {
		QString pathStart = removePath + QDir::separator( );
		QFileInfo removePathFileInfo( pathStart + exportAllDirMidName );
		if( removePathFileInfo.exists( ) ) {
			QString absoluteFilePath = removePathFileInfo.absoluteFilePath( );
			Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString( u8"删除路径 -> %1" ).arg( absoluteFilePath ).toStdString( ) );
			if( removePathFileInfo.isFile( ) )
				QFile( absoluteFilePath ).remove( );
			else
				QDir( absoluteFilePath ).removeRecursively( );
		}
		removePathFileInfo.setFile( pathStart + exportFindDirMidName );
		if( removePathFileInfo.exists( ) ) {
			QString absoluteFilePath = removePathFileInfo.absoluteFilePath( );
			Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString( u8"删除路径 -> %1" ).arg( absoluteFilePath ).toStdString( ) );
			if( removePathFileInfo.isFile( ) )
				QFile( absoluteFilePath ).remove( );
			else
				QDir( absoluteFilePath ).removeRecursively( );
		}
	}
}
void dbReadWriteChanger( const std::shared_ptr< cylStd::ArgParser > &arg_parser ) {
	if( !arg_parser->getOptionValues( "-rdb" ) )
		return;
	// 是否存在-ex 选项
	auto expireOption = arg_parser->getOptionValues( "-ex" );
	size_t expire = 0; // 过期，如果是 0，表示不删除
	bool hasExOption = false; // 转换失败或者不存在选项则为 false
	std::string callFunctionName = __FUNCTION__;
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
			ErrorCout_MACRO( QString( u8"(进程 id : %1 ) 过期选项发生错误，请检查 -ex 是否发生错误" ).arg(applicationPid.c_str( )) );
	}
	auto exisDbFilePath = getOptionExisFilePath( arg_parser, "-rdb", ".db" );
	if( exisDbFilePath.size( ) == 0 ) {
		ErrorCout_MACRO( QString(u8"(进程 id : %1 ) 数据库路径错误，请检查 -rdb 指定的路径是否正确").arg(applicationPid .c_str( )) );
		return; // 不存在可读数据库
	}

	// 线程当中使用输出的锁
	QMutex stdCoutMutex;
	// 查找文件对象插入锁
	QMutex insterReadFindKeyMapmutex;
	// 保存跳过关键字列表
	Len_Map_Str_Vector_S_Shared equJumpKeys = std::make_shared< Len_Map_Str_Vector_S >( );
	// 完全匹配跳过处理线程池
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > readJumpEquKeyThreadpool = getFindKeyFileKeyToVector( arg_parser, "-ijtenf", equJumpKeys, stdCoutMutex, insterReadFindKeyMapmutex );

	// 保存跳过关键字列表
	Len_Map_Str_Vector_S_Shared subJumpKeys = std::make_shared< Len_Map_Str_Vector_S >( );
	// 完全匹配跳过处理线程池
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > readJumpSubKeyThreadpool = getFindKeyFileKeyToVector( arg_parser, "-ijtsnf", subJumpKeys, stdCoutMutex, insterReadFindKeyMapmutex );

	if( readJumpEquKeyThreadpool == nullptr )
		ErrorCout_MACRO( QString( u8"(进程 id : %1 ) 没发现 -ijtenf 功能任务需求" ).arg(applicationPid .c_str( )) );
	else
		readJumpEquKeyThreadpool->start( cylHtmlTools::HtmlWorkThreadPool::getSystemCupCount( ) * 2 );

	if( readJumpSubKeyThreadpool == nullptr )
		ErrorCout_MACRO( QString( u8"(进程 id : %1 ) 没发现 -ijtsnf 功能任务需求" ).arg( applicationPid.c_str( )) );
	else
		readJumpSubKeyThreadpool->start( cylHtmlTools::HtmlWorkThreadPool::getSystemCupCount( ) * 2 );

	if( readJumpSubKeyThreadpool )
		while( !readJumpSubKeyThreadpool->isOverJob( ) )
			qApp->processEvents( );

	if( readJumpEquKeyThreadpool )
		while( !readJumpEquKeyThreadpool->isOverJob( ) )
			qApp->processEvents( );

	if( equJumpKeys->size( ) )
		std::sort( equJumpKeys->begin( ), equJumpKeys->end( ), [] ( Len_Pair &left, Len_Pair &right ) { return left.first < right.first; } );
	if( subJumpKeys->size( ) )
		std::sort( subJumpKeys->begin( ), subJumpKeys->end( ), [] ( Len_Pair &left, Len_Pair &right ) { return left.first < right.first; } );

	// 线程当中插入容器使用的锁
	QMutex insterNovelVectosMutex;
	// 存储小说的列表映射 vectlr<pairt<qstring,vector<novelinfo>>>
	NovelDBJob::NovelTypePairVector_Shared novelInfosMap = std::make_shared< NovelDBJob::NovelTypePairVector >( );
	// 小说计数
	size_t novelCount = 0;
	// 数据库小说总计数
	size_t dbNovelCount = 0;
	// 数据库指定路径是有效的
	bool isDBPathAtive = false;
	// 执行数据库读取任务
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > readDBThreadpool = getDBNovelsInfo( exisDbFilePath, stdCoutMutex, insterNovelVectosMutex, novelInfosMap, novelCount, dbNovelCount, expire, hasExOption, *equJumpKeys, *subJumpKeys, isDBPathAtive );

	if( readDBThreadpool == nullptr ) {
		ErrorCout_MACRO( QString( u8"(进程 id : %1 ) -rdb 功能任务需求任务失败，请与开发者联系" ).arg( applicationPid .c_str( )) );
		return;
	}
	readDBThreadpool->start( );

	auto findKeyFileOption = arg_parser->getOptionValues( "-fkf" );
	// 保存查找文件的信息<文件路径, 关键字列表>
	std::unordered_map< QString, std::vector< interfacePlugsType::HtmlDocString > > exisFindFilePathMapKey;
	// 执行获取查找关键字任务
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > readFindKeyThreadpool = nullptr;
	if( findKeyFileOption ) {
		// 存在选项，则开始
		readFindKeyThreadpool = getFindKeyFileKeyToMap( arg_parser, "-fkf", exisFindFilePathMapKey, stdCoutMutex, insterReadFindKeyMapmutex );
		readFindKeyThreadpool->start( );
	}

	while( !readDBThreadpool->isOverJob( ) )
		qApp->processEvents( );

	size_t size = 0; // 查找配置文件个数
	if( readFindKeyThreadpool ) {
		while( !readFindKeyThreadpool->isOverJob( ) )
			qApp->processEvents( );
		size = exisFindFilePathMapKey.size( );
		if( size == 0 )
			ErrorCout_MACRO( QString("(进程 id : %1 ) 读取查找配置文件失败，请检查选项是否有效").arg( applicationPid.c_str( )) );
	}

	if( !isDBPathAtive ) {
		ErrorCout_MACRO( QString( u8"(进程 id : %1 ) 数据库无法获取小说信息，请检查 -rdb 是否发生错误" ).arg( applicationPid .c_str( )) );
		return;
	}

	// 查找输出路径
	auto exisLegitimateOutDirPath = getOptionLegitimateOutDirPath( arg_parser, "-w" );
	if( exisLegitimateOutDirPath.size( ) == 0 ) {
		if( expireOption )
			Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"已经实现删除过期小说功能，删除单位：%1 天。所有数据库小说数量 : [ %2 ]").arg( expire).arg( dbNovelCount ).toStdString( ) );
		else
			ErrorCout_MACRO( QString( u8"(进程 id : %1 ) 输出路径设置错误，请检查 -w 是否发生错误" ).arg( applicationPid .c_str( )) );
		return;
	}
	Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"所有数据库已存量[ %1 ], 累计有效小说数量 :[ %2 ]").arg( dbNovelCount ).arg( novelCount).toStdString( ) );
	if( novelCount == 0 ) {
		ErrorCout_MACRO( QString( u8"(进程 id : %1 ) 数据库过滤后导出小说数量为 0" ).arg( applicationPid .c_str( )) );
		auto rmOption = arg_parser->getOptionValues( "-rm" );
		if( rmOption )
			rmoveExportPath( exisLegitimateOutDirPath, stdCoutMutex, callFunctionName, __LINE__ );
		return;
	}
	// 是否存在 -edb 选项
	auto isExportDbAllNovelInfo = arg_parser->getOptionValues( "-edb" );
	if( size == 0 && !isExportDbAllNovelInfo ) { // 如果不使用查找选项，不使用全导出选项，则退出程序
		if( expireOption )
			Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"已经实现删除过期小说功能，删除单位：%1 天。所有数据库小说数量 : [ %2 ]").arg( expire).arg( dbNovelCount ).toStdString( ) );
		else
			ErrorCout_MACRO( QString( u8"(进程 id : %1 ) 导出选项配置错误，请检查 -edb 或 -fkf 是否有有效" ).arg(applicationPid.c_str( ) ) );
		return;
	}

	// 存储过滤后的全导出小说信息<写入文件路径, 小说列表>
	NovelDBJob::NovelTypePairVector_Shared edbNovelInfosWriteMap = std::make_shared< NovelDBJob::NovelTypePairVector >( );
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > exportDBNovelInfoAllThreadPool = nullptr;

	if( isExportDbAllNovelInfo ) { // 全导出。-w 选项的 export_all 目录
		exportDBNovelInfoAllThreadPool = getDBFilterNovelInfo( novelInfosMap, edbNovelInfosWriteMap, stdCoutMutex, insterNovelVectosMutex );
		if( !exportDBNovelInfoAllThreadPool ) {
			ErrorCout_MACRO( QString("(进程 id : %1 ) 全导出功能错误。请联系开发人员反馈").arg( applicationPid .c_str( )) );
			return;
		}
		exportDBNovelInfoAllThreadPool->start( cylHtmlTools::HtmlWorkThreadPool::getSystemCupCount( ) * 2 );
	}

	QMutex mergeFindKeysTypesNovelInfosMapMutex, // 类型映射锁
			mergeFindKeysDbsNovelInfosMapMutex, // 数据库映射锁
			mergeFindKeysExportKeyNovelInfosMapMutex, // 关键字文件映射锁
			mergeFindKeysExportNovelInfosMapMutex; // 整体导出锁
	PathWriteNovelInfoUnMap mergeFindKeysTypesNovelInfosMap, // 类型映射对象
							mergeFindKeysDbsNovelInfosMap, // 数据库映射对象
							mergeFindKeysExportKeyNovelInfosMap, // 关键字映射对象
							mergeFindKeysExportNovelInfosMap; // 整体导出对象

	// 小说查找任务线程池对象
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > exportDBNovelInfoFindThreadPool = nullptr;
	QMutex insterMapThreadPoolMutex; // 映射插入对象工作线程锁
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > insterMapThreadPool = nullptr; // 映射插入对象工作线程
	if( size ) { // 查找导出。-w 选项的 export_find  目录
		exportDBNovelInfoFindThreadPool = getDBFindNovelInfo( novelInfosMap, exisFindFilePathMapKey, stdCoutMutex, mergeFindKeysTypesNovelInfosMapMutex, mergeFindKeysTypesNovelInfosMap, mergeFindKeysDbsNovelInfosMapMutex, mergeFindKeysDbsNovelInfosMap, mergeFindKeysExportKeyNovelInfosMapMutex, mergeFindKeysExportKeyNovelInfosMap, mergeFindKeysExportNovelInfosMapMutex, mergeFindKeysExportNovelInfosMap, insterMapThreadPoolMutex, insterMapThreadPool );
		if( !exportDBNovelInfoFindThreadPool ) {
			ErrorCout_MACRO( QString("(进程 id : %1 ) 查找功能错误。请联系开发人员反馈").arg(applicationPid .c_str( )) );
			return;
		}
		exportDBNovelInfoFindThreadPool->start( );
	}
	if( exportDBNovelInfoAllThreadPool )
		while( !exportDBNovelInfoAllThreadPool->isOverJob( ) )
			qApp->processEvents( );
	// 查找关键字合并映射
	PathWriteNovelInfoUnMap mergeFindKeysNovelInfosMap;
	if( exportDBNovelInfoFindThreadPool ) {
		while( !exportDBNovelInfoFindThreadPool->isOverJob( ) )
			qApp->processEvents( );
		Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"开始使用插入功能").toStdString( ) );
		// 开始保存到映射列表
		insterMapThreadPool->start( );
		while( !insterMapThreadPool->isOverJob( ) )
			qApp->processEvents( );
		Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"合并所有匹配小说").toStdString( ) );
		mergeFindMaps( mergeFindKeysNovelInfosMap, // 合并到该对象
						mergeFindKeysTypesNovelInfosMap, // 类型映射对象
						mergeFindKeysDbsNovelInfosMap, // 数据库映射对象
						mergeFindKeysExportKeyNovelInfosMap, // 关键字映射对象
						mergeFindKeysExportNovelInfosMap // 整体导出对象
			);
	}
	size_t edbResultNovelCount = edbNovelInfosWriteMap->size( ); // -edb 返回小说数量
	size_t findResultNovelCount = mergeFindKeysNovelInfosMap.size( ); // -fkf 返回小说数量
	if( edbResultNovelCount == 0 && findResultNovelCount == 0 ) {
		ErrorCout_MACRO( QString("(进程 id : %1 ) 不存在导出的列表。如与信息不匹配，请联系开发人员反馈").arg( applicationPid.c_str( )) );

		// 更新目录
		rmoveExportPath( exisLegitimateOutDirPath, stdCoutMutex, callFunctionName, __LINE__ );
		return;
	}
	Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"发现导出返回:[ %1 ] 发现查找返回:[ %2 ]\n").arg( edbResultNovelCount ).arg( findResultNovelCount).toStdString( ) );

	std::vector< PathManage > exisLegitimateOutDirPathPathManage;
	for( auto &savePath : exisLegitimateOutDirPath )
		exisLegitimateOutDirPathPathManage.emplace_back( PathManage( savePath.toStdString( ) ) );
	// 把 novelInfosWriteMap 映射写入磁盘
	size_t saveNovelCount = 0;
	QMutex diskMute; // 磁盘操作锁
	QMutex countMute;
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > writeDiskThreadPool = nullptr;
	if( edbResultNovelCount )
		writeDiskThreadPool = writeDiskInForNovels( edbNovelInfosWriteMap, exisLegitimateOutDirPath, stdCoutMutex, diskMute, countMute, saveNovelCount, exisLegitimateOutDirPathPathManage );
	// 写入归纳
	std::shared_ptr< cylHtmlTools::HtmlWorkThreadPool > writeDiskInductionThreadPool = nullptr;
	if( findResultNovelCount > 0 )
		writeDiskInductionThreadPool = writeDiskInForInductionNovels( exisLegitimateOutDirPath, mergeFindKeysNovelInfosMap, stdCoutMutex, diskMute, countMute, saveNovelCount, exisLegitimateOutDirPathPathManage );

	//// 开始 -edb 全导出写入文件任务
	if( writeDiskThreadPool )
		writeDiskThreadPool->start( cylHtmlTools::HtmlWorkThreadPool::getSystemCupCount( ) * 2 );

	//// 整合 key 写入
	if( writeDiskInductionThreadPool )
		writeDiskInductionThreadPool->start( );

	if( writeDiskThreadPool )
		while( !writeDiskThreadPool->isOverJob( ) )
			qApp->processEvents( );
	if( writeDiskInductionThreadPool )
		while( !writeDiskInductionThreadPool->isOverJob( ) )
			qApp->processEvents( );

	Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"累计写入小说数量 :[ %1 ]\n").arg( saveNovelCount ).toStdString( ) );
	/// 开始删除路径
	for( auto &pathmanage : exisLegitimateOutDirPathPathManage ) {
		Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"(进程 id : %1 ) 正在检查写入文件列表\n").arg( applicationPid.c_str( ) ).toStdString( ) );
		auto updateList = pathmanage.getUpdateFileVector( );
		for( auto &outFilePath : updateList )
			Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"(进程 id : %1 ) 发现更新路径 :[ %2 ]\n").arg( applicationPid.c_str( ) ).arg( outFilePath .string( ).c_str( ) ).toStdString( ) );

		Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"(进程 id : %1 ) 正在删除旧文件列表\n").arg( applicationPid.c_str( ) ).toStdString( ) );
		auto oldPath = pathmanage.getOldPath( );
		for( auto &removepath : oldPath )
			try {
				if( std::filesystem::remove( removepath ) ) {
					auto removeAbsolutePath = std::filesystem::absolute( removepath ).string( );
					Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"(进程 id : %1 )路径删除 :[ %2 ]\n").arg( applicationPid.c_str( ) ).arg( removeAbsolutePath.c_str( ) ).toStdString( ) );
				}
			} catch( const std::exception &exception ) {
				auto removeAbsolutePath = std::filesystem::absolute( removepath ).string( );
				ErrorCout_MACRO( QString("(进程 id : %1 ) 路径[%2]删除错误,信息:[%3]").arg( applicationPid.c_str( ) ).arg( removeAbsolutePath.c_str( ) ).arg(exception.what( )) );
			}

		Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"(进程 id : %1 ) 正在删除空目录列表\n").arg( applicationPid.c_str( ) ).toStdString( ) );
		auto emptyDir = pathmanage.getEmptyDir( );
		for( auto &removepath : emptyDir )
			try {
				if( std::filesystem::remove_all( removepath ) )
					Out_Std_Count_Stream_Msg_MACRO( stdCoutMutex, callFunctionName, QString(u8"(进程 id : %1 )路径删除 :[ %2 ]\n").arg( applicationPid.c_str( ) ).arg( std::filesystem::absolute( removepath ).string( ).c_str( ) ).toStdString( ) );
			} catch( const std::exception &exception ) {
				ErrorCout_MACRO( QString("(进程 id : %1 ) 路径[%2]删除错误,信息:[%3]").arg( applicationPid.c_str( ) ).arg( std::filesystem::absolute( removepath ).string( ).c_str( ) ).arg(exception.what( )) );
			}
	}
}
