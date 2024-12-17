#include "NovelNetJob.h"
#include <stream/OStream.h>

#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/htmlTools/XPath/XPath.h"
#include "htmls/htmlTools/HtmlWorkThread/HtmlWorkThread.h"
#include "htmls/HtmlNode/HtmlNode.h"
#include <qguiapplication.h>
#include <unordered_map>
#include <iostream>
#include "nameSpace/cylDB.h"
#include "../ioFile/IOFile.h"
#include "dateTime/DateTime.h"

#include "interface/INovelInfo.h"
#include "path/Dir.h"
#include <DB/sqlite/sqliteResult/SQLiteResult.h>
#include <QMutex>

#include "htmls/htmlTools/HtmlWorkThread/HtmlWorkThreadPool.h"
#include "interface/instance_function.h"
#include "NovelDBJob.h"


#ifdef Q_OS_WIN // 提供内存释放功能
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#pragma comment (lib,"psapi.lib")
inline bool releaseMemory( ) {
	//HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, TRUE, GetCurrentProcessId( ) );
	//if( hProcess ) {
	//	if( SetProcessWorkingSetSize( hProcess, -1, -1 ) )
	//		EmptyWorkingSet( hProcess );//内存整理
	//	CloseHandle( hProcess );
	//	return true;
	//}
	return false;
}
#else
inline bool releaseMemory( ) {
	return false;
}
#endif

using namespace interfacePlugsType;

/// <summary>
/// 输出信息到文件
/// </summary>
/// <param name="nowTimeDuration">执行的开始点</param>
/// <param name="oStream">可选输出流</param>
/// <param name="networkReply">请求体</param>
/// <param name="file">错误文件</param>
/// <param name="call_function_name">调用函数名称</param>
/// <param name="line">信息行数</param>
/// <param name="append_msg">写入信息</param>
/// <param name="write_root_path">写入的根目录</param>
/// <param name="file_name">文件名称</param>
/// <param name="file_suffix">后缀名称</param>
inline void error_write_file( const std::chrono::time_point< std::chrono::system_clock > &nowTimeDuration, OStream *oStream, QNetworkReply *networkReply, const QString &file, const QString &call_function_name, size_t line, const QString &append_msg, const QString &write_root_path, const QString &file_name, const QString &file_suffix ) {
	QDateTime currentDateTime = QDateTime::currentDateTime( );
	QString currentTime = currentDateTime.toString( "yyyy_MM_dd hh-mm-ss" );
	auto currentTimeDuration = std::chrono::system_clock::now( ) - nowTimeDuration;
	long long timeDurationToMilliseconds = std::chrono::duration_cast< std::chrono::milliseconds >( currentTimeDuration ).count( );
	QString seconds( QString::number( timeDurationToMilliseconds / 1000 ) ); // 秒
	QString msg;
	auto url = networkReply->url( );
	msg.append( "\n=========================		try : info" )
		.append( u8"\n\t当前时间 : " ).append( currentTime ).append( "\n\t" )
		.append( u8"\n\t执行时间 : " ).append( seconds ).append( "秒\n\t" )
		.append( u8"\n\t错误文件 : " ).append( instance_function::getCmakeRootPathBuilderFilePath( file ) ).append( "\n\t" )
		.append( u8"\n\t信息位置 : " ).append( QString::number( line ) )
		.append( u8"\n\t信息函数 : " ).append( __FUNCTION__ )
		.append( "\n=========================		try : message" )
		.append( u8"\n\t错误信息 : " ).append( getErrorQStr( networkReply->error( ) ) )
		.append( u8"\n\t类型 : " ).append( u8"请求页面" ).append( u8"(" ).append( url.toString( ) ).append( ")" )
		.append( "\n=========================		user : message" )
		.append( u8"\n\t自由信息 : " ).append( append_msg )
		.append( "\n=========================" );
	if( write_root_path.isEmpty( ) )
		OStream::anyStdCerr( msg, instance_function::getCmakeRootPathBuilderFilePath( file ), line, call_function_name, oStream );
	else {
		QString errorWriteFilePath = write_root_path;
		QString host = url.host( );
		QString timeForm = currentDateTime.toString( "yyyy-MM-dd" );
		auto sep = QDir::separator( );
		errorWriteFilePath.append( sep ).append( u8"logs" ).append( sep ).append( u8"log_write_file_s" ).append( sep ).append( host ).append( sep ).append( timeForm ).append( sep ).append( file_name ).append( file_suffix );
		OStream::anyStdCerr( msg, instance_function::getCmakeRootPathBuilderFilePath( file ), line, call_function_name, errorWriteFilePath, msg, oStream );
	}
}
QString getNormalQString( const QString &q_string ) {
	QString result;
	qsizetype length = q_string.length( );
	QChar *buff = new QChar[ length ];
	qsizetype index = 0, buffIndex = 0;
	for( ; index < length; ++index ) {
		const QChar &&qchar = q_string[ index ];
		if( qchar.isSpace( ) || qchar.isNonCharacter( ) || qchar.isMark( ) || qchar.isPunct( ) || !qchar.isPrint( ) )
			continue;
		buff[ buffIndex ] = qchar;
		++buffIndex;
	}
	if( buffIndex != 0 )
		result = QString( buff, buffIndex );
	delete buff;
	return result;
}
QStringList NovelNetJob::userAgentHeaderList = {
		QObject::tr( u8"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36" ),
		QObject::tr( u8"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36" ),
		QObject::tr( u8"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.186 Safari/537.36" ),
		QObject::tr( u8"Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.62 Safari/537.36" ),
		QObject::tr( u8"Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36" ),
		QObject::tr( u8"Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0)" ),
		QObject::tr( u8"Mozilla/5.0 (Macintosh; U; PPC Mac OS X 10.5; en-US; rv:1.9.2.15) Gecko/20110303 Firefox/3.6.15" ),
		QObject::tr( u8"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36" )
};
size_t NovelNetJob::waiteMilliseconds( const size_t &milliseconds ) {
	if( milliseconds == 0 )
		return milliseconds;
	auto point = std::chrono::system_clock::now( );
	auto timePoint = std::chrono::system_clock::now( );
	do {
		auto duration = timePoint - point;
		auto nowPoint = getTimeDurationToMilliseconds( duration );
		if( nowPoint > milliseconds )
			break;
		qApp->processEvents( );
		timePoint = std::chrono::system_clock::now( );
	} while( true );
	return milliseconds;
}
NovelNetJob::NovelNetJob( OStream *o_stream, QObject *interface_obj_ptr, IRequestNetInterface *interface_this_ptr ): interfaceObjPtr( interface_obj_ptr ), interfaceThisPtr( interface_this_ptr ), oStream( o_stream ) {
	initObj( );
	initObjProperty( );
	initConnect( );
}

NovelNetJob::~NovelNetJob( ) {
	interfaceObjPtr->deleteLater( );
}
void NovelNetJob::initObj( ) {
	netMutex = std::make_shared< QMutex >( );
	this->networkAccessManager = new QNetworkAccessManager;
}
void NovelNetJob::initObjProperty( ) {
	outPath = QDir::currentPath( ) + QDir::separator( ) + "novel_net_job" + QDir::separator( );
	requestMaxCount = 1500;
	requestMaxMilliseconds = 4 * 60 * 1000;
	sepMs = interfaceThisPtr->getRequestInterval( );
	requestTime = NovelNetJob::getCurrentTimePoint( );
	interfaceThisPtr->setOStream( oStream );
	QString agentHeader = getRandomUserAgentHeader( );
	networkRequest.setHeader( QNetworkRequest::UserAgentHeader, agentHeader );
	QSslConfiguration sslConfiguration = networkRequest.sslConfiguration( );
	sslConfiguration.setPeerVerifyMode( QSslSocket::VerifyNone );
	networkRequest.setSslConfiguration( sslConfiguration );
	typeCount = 0;

	runStatus = 0;
	HtmlDocString resultUrl;
	size_t size = interfaceThisPtr->getRootUrl( &resultUrl );
	QString qUrl = QString::fromStdWString( resultUrl );
	if( size == 0 )
		return;
	this->networkRequest.setUrl( qUrl );
	QUrl url = this->networkRequest.url( );
	QString qstrUrl = url.toString( );
	if( OStream::hasDefaultOStream( qstrUrl ) == false )
		OStream::setDefaultOStream( qstrUrl, oStream );
	networkRequest.setHeader( QNetworkRequest::UserAgentHeader, getRandomUserAgentHeader( ) );
	QSslConfiguration configuration = networkRequest.sslConfiguration( );
	sslConfiguration.setPeerVerifyMode( QSslSocket::VerifyNone );
	networkRequest.setSslConfiguration( configuration );
}

void NovelNetJob::initConnect( ) {
	connect( this, &NovelNetJob::requesting_get_root_page_signals, this, &NovelNetJob::slots_requesting_get_root_page_signals, Qt::QueuedConnection );
	connect( this, &NovelNetJob::requested_get_web_page_signals_end, this, &NovelNetJob::slots_requested_get_web_page_signals_end, Qt::QueuedConnection );
}

QNetworkReply * NovelNetJob::requestUrl( QNetworkAccessManager *&result, const QNetworkRequest &network_request, const QUrl &url, TimePoint &last_request_time_point, size_t &sep_ms, const NetworkmanagerConnectFunction &callFunction, const QString &call_finle_path_name, const QString &call_function_name, const size_t call_line ) {
	auto newRequestTime = getCurrentTimePoint( );
	auto commonType = newRequestTime - last_request_time_point;
	auto milliseconds = getTimeDurationToMilliseconds( commonType );
	if( milliseconds < sep_ms )
		waiteMilliseconds( sep_ms - milliseconds );
	last_request_time_point = getCurrentTimePoint( );
	if( result == nullptr )
		result = new QNetworkAccessManager;
	auto networkRequest = network_request;
	networkRequest.setUrl( url );
	QNetworkReply *networkReply = result->get( networkRequest );
	while( !networkReply->isRunning( ) )
		waiteMilliseconds( 20 );
	while( !networkReply->isFinished( ) )
		waiteMilliseconds( 200 );
	callFunction( networkReply, instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __LINE__, __FUNCTION__ );
	return networkReply;
}
QNetworkReply * NovelNetJob::requestUrl( QNetworkAccessManager *&result, const QNetworkRequest &network_request, const QUrl &url, TimePoint &last_request_time_point, size_t &sep_ms ) {
	auto newRequestTime = getCurrentTimePoint( );
	auto commonType = newRequestTime - last_request_time_point;
	auto milliseconds = getTimeDurationToMilliseconds( commonType );
	if( milliseconds < sep_ms )
		waiteMilliseconds( sep_ms - milliseconds );
	last_request_time_point = getCurrentTimePoint( );
	if( result == nullptr )
		result = new QNetworkAccessManager;
	auto networkRequest = network_request;
	networkRequest.setUrl( url );
	QNetworkReply *networkReply = result->get( networkRequest );
	while( !networkReply->isRunning( ) )
		waiteMilliseconds( 20 );
	while( !networkReply->isFinished( ) )
		waiteMilliseconds( 200 );
	return networkReply;
}
QNetworkReply * NovelNetJob::requestGet( const QUrl &url, const size_t requestMaxCount, const size_t requestMaxMs, const QString &error_msg, const QString &error_file_append_base_name, const NetworkmanagerConnectFunction &call_function, const QString &call_finle_path_name, const QString &call_function_name, const size_t call_line ) {
	auto requestReplyTime = getCurrentTimePoint( );
	size_t requestCount = 0;
	do {
		auto networkReply = requestUrl( this->networkAccessManager, this->networkRequest, url, this->requestTime, this->sepMs, call_function, call_finle_path_name, call_function_name, call_line );
		if( networkReply->error( ) == QNetworkReply::NoError )
			return networkReply;
		this->networkRequest.setHeader( QNetworkRequest::UserAgentHeader, getRandomUserAgentHeader( ) );
		if( requestMaxCount ) {
			++requestCount;
			if( requestCount > requestMaxCount ) {
				error_write_file( requestTime, oStream, networkReply, call_function_name, call_finle_path_name, call_line, error_msg + u8"-请求次数超出限制", outPath, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".root_request.error.log" );
				break;
			}
		}
		if( requestMaxMs ) {
			auto compTime = getTimeDurationToMilliseconds( getCurrentTimePoint( ) - requestReplyTime );
			if( compTime > requestMaxMs ) {
				error_write_file( requestTime, oStream, networkReply, call_function_name, call_finle_path_name, call_line, error_msg + u8"-请求时间超出限制", outPath, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".root_request.error.log" );
				break;
			}
		}

	} while( true ) ;
	return nullptr;
}
QNetworkReply * NovelNetJob::requestGet( const QUrl &url, const size_t requestMaxCount, const size_t requestMaxMs, const QString &error_msg, const QString &error_file_append_base_name, const QString &call_finle_path_name, const QString &call_function_name, const size_t call_line ) {
	auto instance = qApp;
	auto requestReplyTime = getCurrentTimePoint( );
	size_t requestCount = 0;
	do {
		auto networkReply = requestUrl( this->networkAccessManager, this->networkRequest, url, this->requestTime, this->sepMs );
		if( networkReply->error( ) == QNetworkReply::NoError ) {
			interfaceThisPtr->initBefore( );
			return networkReply;
			break;
		}
		if( requestMaxCount ) {
			++requestCount;
			if( requestCount > requestMaxCount ) {
				error_write_file( requestTime, oStream, networkReply, call_function_name, call_finle_path_name, call_line, error_msg + u8"-请求次数超出限制", outPath, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".root_request.error.log" );
				break;
			}
		}
		if( requestMaxMs ) {
			auto compTime = getTimeDurationToMilliseconds( getCurrentTimePoint( ) - requestReplyTime );
			if( compTime > requestMaxMs ) {
				error_write_file( requestTime, oStream, networkReply, call_function_name, call_finle_path_name, call_line, error_msg + u8"-请求时间超出限制", outPath, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".root_request.error.log" );
				break;
			}
		}
		instance->processEvents( );
	} while( true ) ;
	return nullptr;
}
bool NovelNetJob::start( ) {
	if( getTypeNamelist.size( ) == 0 ) { // 类型不足，需要重新读取
		if( inPath.isEmpty( ) ) {
			inPath.append( qApp->applicationDirPath( ) ).append( QDir::separator( ) ).append( "progress" ).append( QDir::separator( ) ).append( "ini" ).append( QDir::separator( ) ).append( "ReptileIntroduce.ini" );
		}
		QFile qFile;
		qFile.setFileName( inPath );
		if( qFile.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) ) {
			auto byteArray = qFile.readAll( );
			QString fileContent( byteArray );
			auto list = fileContent.split( "\n" );
			for( auto &str : list ) {
				auto normalQString = getNormalQString( str );
				if( normalQString.isEmpty( ) )
					continue;
				getTypeNamelist << normalQString;
			}
		}
	}

	auto basicString = outPath.toStdWString( );
	interfaceThisPtr->setOutPath( &basicString );
	QUrl url = this->networkRequest.url( );
	QString qstrUrl = url.toString( );
	interfaceThisPtr->initAfter( );
	OStream::anyStdCOut( "开始获取 " + qstrUrl, oStream );
	NetworkmanagerConnectFunction noErrorRunCode = [this]( QNetworkReply *q_network_reply, const QString call_file_path_name, size_t call_line, const QString call_name ) {
		typeCount = 0;
		this->typeNovelsMap.clear( );
		emit requesting_get_root_page_signals( q_network_reply->url( ), q_network_reply );
		return false;
	};
	NetworkmanagerConnectFunction hasErrorRunCode = [this]( QNetworkReply *q_network_reply, const QString call_file_path_name, size_t call_line, const QString call_name ) {
		auto qUrl = q_network_reply->url( );
		error_write_file( requestTime, oStream, q_network_reply, call_name, call_file_path_name, call_line, u8"首页请求失败", outPath, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".root_request.error.log" );
		return false;
	};
	NetworkmanagerConnectFunction callFunction = [noErrorRunCode,hasErrorRunCode,this]( QNetworkReply *q_network_reply, const QString call_file_path_name, size_t call_line, const QString call_name ) {
		if( q_network_reply->error( ) == QNetworkReply::NoError )
			return noErrorRunCode( q_network_reply, call_file_path_name, call_line, call_name );
		else
			return hasErrorRunCode( q_network_reply, call_file_path_name, call_line, call_name );
	};
	auto networkReply = requestGet( url, requestMaxCount, requestMaxMilliseconds, u8"首页请求失败", u8".root_request.error.log", callFunction, instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__ );
	this->interfaceThisPtr->initBefore( );
	if( !networkReply )
		emit requested_get_web_page_signals_end( url );
	return networkReply;
}
QString NovelNetJob::getUrl( ) const {
	HtmlDocString resultUrl;
	size_t size = interfaceThisPtr->getRootUrl( &resultUrl );
	if( size == 0 )
		return "";
	cylHtmlTools::HtmlStringTools::removeBothSpace( resultUrl );
	return QString::fromStdWString( resultUrl );
}
void NovelNetJob::slots_requesting_get_root_page_signals( const QUrl &url, QNetworkReply *request_connect ) {
	QString rootUrl = url.scheme( ) + u8"://" + url.host( );
	QString htmlText( request_connect->readAll( ) );
	if( htmlText.isEmpty( ) )
		return;
	auto mapHtmlStrKHtmlStrV = interfaceThisPtr->formHtmlGetTypeTheUrls( rootUrl.toStdWString( ), htmlText.toStdWString( ) );
	if( !mapHtmlStrKHtmlStrV ) {
		QString msg;
		msg.append( u8"首页异常 : " ).append( u8"(没有找到类型节点)结束" ).append( u8"(" ).append( rootUrl ).append( ")" );
		OStream::anyStdCerr( msg, instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __LINE__, __FUNCTION__, oStream );
		emit endJob( );
		return;
	}
	this->typeNovelsMap.clear( );
	///////////////// 类型请求
	auto iterator = mapHtmlStrKHtmlStrV->begin( ); // 类型遍历
	auto end = mapHtmlStrKHtmlStrV->end( ); // 类型结束
	std::unordered_map< QString, QString > requests;
	do {
		QString typeName = getNormalQString( QString::fromStdWString( iterator->first ) );
		auto getTypeNamelistIterator = getTypeNamelist.begin( ); // 根据配置文件当中的可查找遍历类型名称来实现列表获取
		auto getTypeNamelistEnd = getTypeNamelist.end( ); // 配置文件列表的尾节点
		for( ; getTypeNamelistIterator != getTypeNamelistEnd; ++getTypeNamelistIterator )
			if( *getTypeNamelistIterator == typeName )
				break;
		if( getTypeNamelistIterator != getTypeNamelistEnd ) {
			requests.emplace( typeName, QString::fromStdWString( iterator->second ) ); // 等待处理
			this->typeNovelsMap.emplace( typeName, std::make_shared< Vector_INovelInfoSPtr >( ) );
			++typeCount;
		}
		++iterator;
	} while( iterator != end );

	// 开始获取数据
	auto requestVectorIterator = requests.begin( );
	auto requestVectorEnd = requests.end( );
	size_t pageIndex = 0;
	while( requestVectorIterator != requestVectorEnd ) {
		auto typeNmae = requestVectorIterator->first;
		auto qUrl = requestVectorIterator->second;
		auto networkReply = requestGet( qUrl, requestMaxCount, requestMaxMilliseconds, u8"类型页面请求失败", u8".type_request.error.log", instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__ );
		if( !networkReply )
			continue;
		++pageIndex;
		OStream::anyStdCOut( u8"发现 " + typeNmae + "(" + qUrl + ")" + "[" + QString::number( typeCount ) + "]", oStream );
		auto readAll = networkReply->readAll( );
		auto html = std::make_shared< cylHtmlTools::HtmlString >( QString( readAll ).toStdWString( ) );
		auto newQUrl = getPageInfo( typeNmae, qUrl, html );
		while( !newQUrl.isEmpty( ) ) {
			qUrl = newQUrl;
			networkReply = requestGet( qUrl, requestMaxCount, requestMaxMilliseconds, u8"下一页类型请求失败", u8".type_request_next.error.log", instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__ );
			if( !networkReply )
				break; // 请求失败，则终止
			++pageIndex;
			readAll = networkReply->readAll( );
			html = std::make_shared< cylHtmlTools::HtmlString >( QString( readAll ).toStdWString( ) );
			newQUrl = getPageInfo( typeNmae, qUrl, html );
		}
		// 小说类型页面终止
		novelPageInfoRequestEnd( typeNmae, qUrl, pageIndex );
		--typeCount;
		++requestVectorIterator;
	}
	emit requested_get_web_page_signals_end( rootUrl );
}

interfacePlugsType::INovelInfo_Shared NovelNetJob::saveToStoreNovels(
		interfacePlugsType::Vector_INovelInfoSPtr_Shared &requestedGetVectorINovelInfoSPtrShared,
		interfacePlugsType::Vector_INovelInfoSPtr_Shared &saveMapNovelInfos,
		interfacePlugsType::INovelInfo_Shared &novel,
		QString *result_msg ) {
	return nullptr;
}


QString NovelNetJob::getPageInfo( const QString &type_name, const QUrl &type_url, cylHtmlTools::HtmlString_Shared html_string ) {
	Vector_INovelInfoSPtr_Shared requestedGetVectorINovelInfoSPtrShared( std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( ) );
	// 存储获取的小说
	auto iterator = this->typeNovelsMap.begin( );
	auto end = this->typeNovelsMap.end( );
	Vector_INovelInfoSPtr_Shared saveMapNovelInfos = nullptr;
	for( ; iterator != end; ++iterator )
		if( iterator->first == type_name ) {
			saveMapNovelInfos = iterator->second;
			break;
		}

	QString typePageUrl = type_url.toString( );
	auto novelInfos = interfaceThisPtr->formHtmlGetTypePageNovels( type_name.toStdWString( ), typePageUrl.toStdWString( ), *html_string, *saveMapNovelInfos, nullptr );
	QString resultMsg;
	if( novelInfos.size( ) == 0 ) // 没有小说则终止
		return "";
	// 鉴定小说
	for( auto &novel : novelInfos ) {
		if( interfaceThisPtr->isRequestNovelInfoUrl( novel.get( ) ) ) {				// 如果小说需要到详情页申请，则临时存储
			auto resultINovelShared = saveToStoreNovels( requestedGetVectorINovelInfoSPtrShared, saveMapNovelInfos, novel, &resultMsg );
			if( !resultINovelShared.get( ) )  // 存在错误消息，即没有存储
				OStream::anyStdCerr( resultMsg, instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __LINE__, __FUNCTION__, oStream );
			else
				requestedGetVectorINovelInfoSPtrShared->emplace_back( resultINovelShared );
		} else
			requestedGetVectorINovelInfoSPtrShared->emplace_back( novel ); // 存储已知小说
	}
	auto requestedGetVectorIterator = requestedGetVectorINovelInfoSPtrShared->begin( );
	auto requestedGetVectorEnd = requestedGetVectorINovelInfoSPtrShared->end( );
	if( requestedGetVectorIterator != requestedGetVectorEnd ) {
		auto vectorPtr = saveMapNovelInfos.get( );
		for( ; requestedGetVectorIterator != requestedGetVectorEnd; ++requestedGetVectorIterator )
			vectorPtr->emplace_back( *requestedGetVectorIterator );
	}
	auto formHtmlGetNext = interfaceThisPtr->formHtmlGetNext( type_name.toStdWString( ), typePageUrl.toStdWString( ), *html_string, *saveMapNovelInfos, *requestedGetVectorINovelInfoSPtrShared );
	if( formHtmlGetNext.empty( ) )
		return "";
	return QString::fromStdWString( formHtmlGetNext );  // 正常执行完毕
}


void NovelNetJob::novelPageInfoRequestEnd( const QString &type_name, const QUrl &url, size_t current_page_index ) {
	std::cout << u8"-----------------------" << std::endl;
	std::cout << type_name.toStdString( ) << "(" << url.toString( ).toStdString( ) << ") 请求结束" << std::endl;
	std::cout << u8"-----------------------" << std::endl;
	auto rootUrl = url.host( );
	auto root_url = url.scheme( ) + "://" + rootUrl;
	Vector_INovelInfoSPtr_Shared infos = typeNovelsMap.at( type_name );
	interfaceThisPtr->novelTypeEnd( root_url.toStdWString( ), type_name.toStdWString( ), url.toString( ).toStdWString( ), *infos );
	releaseMemory( );

}


void NovelNetJob::slots_requested_get_web_page_signals_end( const QUrl &url ) {
	Vector_INovelInfoSPtr_Shared novelInfoSPtr( std::make_shared< Vector_INovelInfoSPtr >( ) );
	cylHtmlTools::HtmlWorkThreadPool threadPool;
	auto mapIterator = typeNovelsMap.begin( );
	auto mapEnd = typeNovelsMap.end( );
	if( mapIterator != mapEnd ) {
		QMutex qMutex;
		qMutex.lock( );
		threadPool.appendWork( [&qMutex]( cylHtmlTools::HtmlWorkThread *html_work_thread ) {
			qMutex.lock( );
			qMutex.unlock( );
		} );
		threadPool.setCallSepMilliseconds( 2000 );
		threadPool.setIdleTimeCall( [this]( cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool, const unsigned long long &mod_count, const unsigned long long &current_count ) {
			auto msg = QString( "===========" )
						.append( "\n\t消息 : " ).append( QString( u8"剩余工作数量:[ %1 ], 正在工作数量:[ %2 ]" ).arg( mod_count ).arg( current_count ) )
						.append( "\n\t\t" ).append( "正在合并小说列表" )
						.append( "\n\t文件 : " ).append( instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ) )
						.append( "\n\t行号 : " ).append( QString::number( __LINE__ ) )
						.append( "\n" ).append( "===========" );
			OStream::anyStdCOut( msg, oStream );
		} );
		threadPool.setWorkCount( 8 );
		threadPool.start( );
		for( ; mapIterator != mapEnd; ++mapIterator )
			for( auto &novel : *mapIterator->second )
				novelInfoSPtr->emplace_back( novel );
		QGuiApplication *guiApplication = qApp;
		qMutex.unlock( ); // 合并完毕，即可解锁线程
		while( !threadPool.isOverJob( ) )
			guiApplication->processEvents( );
		if( novelInfoSPtr->size( ) > 0 ) {
			auto run = [this]( const Duration &duration ) {
				if( getTimeDurationToMilliseconds( duration ) < 2000 )
					return false;
				auto msg = QString( "===========" )
							.append( "\n\t消息 : " ).append( "正在存储数据库" )
							.append( "\n\t文件 : " ).append( instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ) )
							.append( "\n\t行号 : " ).append( QString::number( __LINE__ ) )
							.append( "\n\t" ).append( "===========" );
				return true;
			};


			QString dbPath = outPath + QDir::separator( ) + "dbs" + QDir::separator( ) + url.host( ) + ".db";
			auto writeCount = NovelDBJob::writeDB( oStream, dbPath, *novelInfoSPtr, run );
			auto msg = QString( "===========" )
						.append( "\n\t消息 : " ).append( QString( u8"写入小说数量:[ %1 ]" ).arg( writeCount ) )
						.append( "\n\t\t" ).append( QString( u8"写入路径完成 [ %1 ]" ).arg( dbPath ) )
						.append( "\n\t文件 : " ).append( instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ) )
						.append( "\n\t行号 : " ).append( QString::number( __LINE__ ) )
						.append( "\n" ).append( "===========" );
			OStream::anyStdCOut( msg, oStream );
		}
	}

	interfaceThisPtr->endHost( *novelInfoSPtr );
	runStatus = 0; // 可以重新运行
	QString msg = QString( ).append( "请求 : (" ).append( url.scheme( ) ).append( u8"://" ).append( url.host( ) ).append( ") 结束 " );
	OStream::anyStdCOut( msg, oStream );
	typeNovelsMap.clear( );
	releaseMemory( );
	emit endJob( );
}
