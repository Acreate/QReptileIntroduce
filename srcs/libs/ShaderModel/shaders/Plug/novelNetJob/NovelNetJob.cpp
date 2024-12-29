#include "NovelNetJob.h"
#include <stream/OStream.h>
#include <mutex>
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
const QString currentFilePathName = instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ); // 当前编译文件的相对路径名称

/// @brief 结束设置函数
#define End_Current_Show_Message(  ) do { \
		showMessageThreadMutex->lock( ); \
		currentThreadLine = 0; \
		showMessageThreadMutex->unlock( ); \
	} while(false)

/// @brief 设置一个当前信息，此宏改变 currentThreadLine 与 currentMsg 的值
/// @param line size_t 类型，设置当前行信息
/// @param msg std::string 类型，设置当前信息
#define Set_Current_Show_1_Message(line, msg ) do { \
		showMessageThreadMutex->lock( ); \
		currentThreadLine = line; \
		currentMsg = msg; \
		currentMsg.append( u8" 函数名称 : [" ).append( __FUNCTION__ ).append(u8" ] "); \
		showMessageThreadMutex->unlock( ); \
	} while(false)

/// @brief 设置 2 个当前信息，并且以 空格隔开，此宏改变 currentThreadLine 与 currentMsg 的值
/// @param line size_t 类型，设置当前行信息
/// @param msg std::string 类型，设置当前信息 1
/// @param msg std::string 类型，设置当前信息 2
#define Set_Current_Show_2_Message(line, msg1, msg2 ) do { \
		showMessageThreadMutex->lock( ); \
		currentThreadLine = line; \
		currentMsg = msg1; \
		currentMsg.append( " " ).append( msg2 ); \
		currentMsg.append( u8" 函数名称 : [" ).append( __FUNCTION__ ).append(u8" ] "); \
		showMessageThreadMutex->unlock( ); \
	} while(false)

/// @brief 设置 3 个当前信息，并且以 空格隔开，此宏改变 currentThreadLine 与 currentMsg 的值
/// @param line size_t 类型，设置当前行信息
/// @param msg std::string 类型，设置当前信息 1
/// @param msg std::string 类型，设置当前信息 2
/// @param msg std::string 类型，设置当前信息 3
#define Set_Current_Show_3_Message( line,msg1, msg2 , msg3 ) do { \
		showMessageThreadMutex->lock( ); \
		currentThreadLine = line; \
		currentMsg = msg1; \
		currentMsg.append( " " ).append( msg2 ); \
		currentMsg.append( " " ).append( msg3 ); \
		currentMsg.append( u8" 函数名称 : [" ).append( __FUNCTION__ ).append(u8" ] "); \
		showMessageThreadMutex->unlock( ); \
	} while(false)

/// @brief 自动设置当前行，并且设置 1 个信息
/// @param msg1 设置输出信息
#define Set_Current_Show_1_Message_Auto_Line( msg1 ) Set_Current_Show_1_Message(__LINE__, msg1 )

/// @brief 自动设置当前行，并且设置 2 个信息
/// @param msg1 设置输出 1 信息
/// @param msg2 设置输出 2 信息
#define Set_Current_Show_2_Message_Auto_Line( msg1, msg2 ) Set_Current_Show_2_Message(__LINE__, msg1, msg2  )

/// @brief 自动设置当前行，并且设置 3 个信息
/// @param msg1 设置输出信息
/// @param msg2 设置输出 2 信息
/// @param msg3 设置输出 3 信息
#define Set_Current_Show_3_Message_Auto_Line( msg1, msg2 , msg3 ) Set_Current_Show_3_Message(__LINE__, msg1, msg2 , msg3  )
/// <summary>
/// 输出信息到文件
/// </summary>
/// <param name="nowTimeDuration">执行的开始点</param>
/// <param name="oStream">可选输出流</param>
/// <param name="url">链接</param>
/// <param name="file">错误文件</param>
/// <param name="call_function_name">调用函数名称</param>
/// <param name="line">信息行数</param>
/// <param name="append_msg">写入信息</param>
/// <param name="write_root_path">写入的根目录</param>
/// <param name="file_name">文件名称</param>
/// <param name="file_suffix">后缀名称</param>
/// <param name="file_start">写入的开始文件夹名称</param>
inline void error_write_file( const std::chrono::time_point< std::chrono::system_clock > &nowTimeDuration, OStream *oStream, QUrl url, const QString &file, const QString &call_function_name, size_t line, const QString &append_msg, const QString &write_root_path, const QString &file_name, const QString &file_suffix, const QString &file_start ) {
	QDateTime currentDateTime = QDateTime::currentDateTime( );
	QString currentTime = currentDateTime.toString( "yyyy_MM_dd hh-mm-ss" );
	auto currentTimeDuration = std::chrono::system_clock::now( ) - nowTimeDuration;
	long long timeDurationToMilliseconds = std::chrono::duration_cast< std::chrono::milliseconds >( currentTimeDuration ).count( );
	QString seconds( QString::number( timeDurationToMilliseconds / 1000 ) ); // 秒
	QString msg;
	msg.append( "\n=========================		try : info" )
		.append( u8"\n\t当前时间 : " ).append( currentTime )
		.append( u8"\n\t执行时间 : " ).append( seconds ).append( "秒" )
		.append( u8"\n\t错误文件 : " ).append( instance_function::getCmakeRootPathBuilderFilePath( file ) )
		.append( u8"\n\t信息位置 : " ).append( QString::number( line ) )
		.append( u8"\n\t信息函数 : " ).append( call_function_name )
		.append( "\n=========================		try : message" )
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
		errorWriteFilePath.append( sep ).append( u8"logs" ).append( sep ).append( timeForm ).append( sep ).append( file_start ).append( sep ).append( host ).append( sep ).append( file_name ).append( file_suffix );
		OStream::anyStdCerr( msg, instance_function::getCmakeRootPathBuilderFilePath( file ), line, call_function_name, errorWriteFilePath, msg, oStream );
	}
}

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
/// <param name="file_start">写入的开始文件夹名称</param>
inline void error_write_file( const std::chrono::time_point< std::chrono::system_clock > &nowTimeDuration, OStream *oStream, QNetworkReply *networkReply, const QString &file, const QString &call_function_name, size_t line, const QString &append_msg, const QString &write_root_path, const QString &file_name, const QString &file_suffix, const QString &file_start ) {
	error_write_file( nowTimeDuration, oStream, networkReply->url( ), file, call_function_name, line, append_msg, write_root_path, file_name, file_suffix, file_start );
}


/// <summary>
/// 输出信息到流
/// </summary>
/// <param name="nowTimeDuration">执行的开始点</param>
/// <param name="oStream">可选输出流</param>
/// <param name="networkReply">请求体</param>
/// <param name="file">错误文件</param>
/// <param name="call_function_name">调用函数名称</param>
/// <param name="line">信息行数</param>
/// <param name="append_msg">写入信息</param>
inline void error_write_iostream( const std::chrono::time_point< std::chrono::system_clock > &nowTimeDuration, OStream *oStream, QNetworkReply *networkReply, const QString &file, const QString &call_function_name, size_t line, const QString &append_msg ) {
	QDateTime currentDateTime = QDateTime::currentDateTime( );
	QString currentTime = currentDateTime.toString( "yyyy_MM_dd hh-mm-ss" );
	auto currentTimeDuration = std::chrono::system_clock::now( ) - nowTimeDuration;
	long long timeDurationToMilliseconds = std::chrono::duration_cast< std::chrono::milliseconds >( currentTimeDuration ).count( );
	QString seconds( QString::number( timeDurationToMilliseconds / 1000 ) ); // 秒
	QString msg;
	auto url = networkReply->url( );
	msg.append( "\n=========================		try : info" )
		.append( u8"\n\t当前时间 : " ).append( currentTime )
		.append( u8"\n\t执行时间 : " ).append( seconds ).append( "秒" )
		.append( u8"\n\t错误文件 : " ).append( instance_function::getCmakeRootPathBuilderFilePath( file ) )
		.append( u8"\n\t信息位置 : " ).append( QString::number( line ) )
		.append( u8"\n\t信息函数 : " ).append( __FUNCTION__ )
		.append( "\n=========================		try : message" )
		.append( u8"\n\t错误信息 : " ).append( getErrorQStr( networkReply->error( ) ) )
		.append( u8"\n\t类型 : " ).append( u8"请求页面" ).append( u8"(" ).append( url.toString( ) ).append( ")" )
		.append( "\n=========================		user : message" )
		.append( u8"\n\t自由信息 : " ).append( append_msg )
		.append( "\n=========================" );
	OStream::anyStdCerr( msg, instance_function::getCmakeRootPathBuilderFilePath( file ), line, call_function_name, oStream );
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
		if( nowPoint > 0 && nowPoint > milliseconds )
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
	End_Current_Show_Message( );
	showMessageThread->join( );
}
void NovelNetJob::initObj( ) {
	showMessageThreadMutex = std::make_shared< std::mutex >( );
	Set_Current_Show_1_Message_Auto_Line( u8"执行初始化" );
	showMessageThread = std::make_shared< std::thread >( [this]( ) {
		auto oldTimePoint = std::chrono::system_clock::now( );
		auto wateSec = std::chrono::seconds( 10 ); // 等待 10 秒
		while( true ) {
			auto newTimePoint = std::chrono::system_clock::now( );
			auto duration = newTimePoint - oldTimePoint;
			auto durationCast = std::chrono::duration_cast< std::chrono::seconds >( duration );
			if( durationCast < wateSec ) { // 10 秒间隔
				std::chrono::seconds relTime = wateSec - durationCast;
				std::this_thread::sleep_for( relTime );
				continue;
			}
			oldTimePoint = newTimePoint;
			showMessageThreadMutex->lock( );
			std::cout.flush( );
			std::cout << u8"\n============\n\t\t当前信息 : " << currentMsg << u8" -> 行号 : ( " << currentThreadLine << u8" )\n============\n\n";
			std::cout.flush( );
			if( currentThreadLine == 0 ) {
				showMessageThreadMutex->unlock( );
				return;
			}
			showMessageThreadMutex->unlock( );
		}
	} );

	this->networkAccessManager = new QNetworkAccessManager;
}
void NovelNetJob::initObjProperty( ) {


	outPath = QDir::currentPath( ) + QDir::separator( ) + "novel_net_job" + QDir::separator( );
	Set_Current_Show_3_Message_Auto_Line( u8"初始化 outPath 路径 [", outPath.toStdString( ), "]" );
	requestMaxCount = 15;
	requestMaxMilliseconds = 60 * 1000;
	sepMs = interfaceThisPtr->getRequestInterval( );
	requestTime = NovelNetJob::getCurrentTimePoint( );
	interfaceThisPtr->setOStream( oStream );

	Set_Current_Show_1_Message_Auto_Line( u8"初始化请求信息" );
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
	if( size == 0 ) {
		error_write_file( requestTime, oStream, nullptr, currentFilePathName, __FUNCTION__, __LINE__, u8"不存在请求首页", outPath, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".root_request.error.log", u8"root_none_request_get_error_log" );
		return;
	}
	Set_Current_Show_1_Message_Auto_Line( u8"开始初始化 QNetworkRequest 配置信息" );
	this->networkRequest.setUrl( qUrl );
	QUrl url = this->networkRequest.url( );
	QString qstrUrl = url.toString( );
	if( OStream::hasDefaultOStream( qstrUrl ) == false )
		OStream::setDefaultOStream( qstrUrl, oStream );
	networkRequest.setHeader( QNetworkRequest::UserAgentHeader, getRandomUserAgentHeader( ) );
	QSslConfiguration configuration = networkRequest.sslConfiguration( );
	sslConfiguration.setPeerVerifyMode( QSslSocket::VerifyNone );
	networkRequest.setSslConfiguration( configuration );
	Set_Current_Show_1_Message_Auto_Line( u8"初始化 QNetworkRequest 配置信息完毕" );
}

void NovelNetJob::initConnect( ) {
	Set_Current_Show_1_Message_Auto_Line( u8"初始化槽函数" );
	connect( this, &NovelNetJob::requesting_get_root_page_signals, this, &NovelNetJob::slots_requesting_get_root_page_signals, Qt::QueuedConnection );
	connect( this, &NovelNetJob::requested_get_web_page_signals_end, this, &NovelNetJob::slots_requested_get_web_page_signals_end, Qt::QueuedConnection );
	Set_Current_Show_1_Message_Auto_Line( u8"初始化槽函数完毕" );
}

QNetworkReply * NovelNetJob::requestUrl( QNetworkAccessManager *&result, const QNetworkRequest &network_request, const QUrl &url, TimePoint &last_request_time_point, size_t &sep_ms, const NetworkmanagerConnectFunction &callFunction, const QString &call_finle_path_name, const QString &call_function_name, const size_t call_line ) {
	auto stdString = url.toString( ).toStdString( );
	Set_Current_Show_3_Message_Auto_Line( u8"开始请求[ ", stdString, u8" ]" );
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
	Set_Current_Show_3_Message_Auto_Line( u8"结束请求[ ", stdString, u8" ]" );
	return networkReply;
}
QNetworkReply * NovelNetJob::requestUrl( QNetworkAccessManager *&result, const QNetworkRequest &network_request, const QUrl &url, TimePoint &last_request_time_point, size_t &sep_ms ) {
	auto stdString = url.toString( ).toStdString( );
	Set_Current_Show_3_Message_Auto_Line( u8"开始请求[ ", stdString, u8" ]" );
	auto newRequestTime = getCurrentTimePoint( ); // 获取当前时间点
	auto commonType = newRequestTime - last_request_time_point; // 当前时间点减去上次请求时间
	auto milliseconds = getTimeDurationToMilliseconds( commonType ); // 获取分差
	if( milliseconds > 0 && milliseconds < sep_ms ) // 如果未到请求间隔，则开始等待
		waiteMilliseconds( sep_ms - milliseconds );
	last_request_time_point = getCurrentTimePoint( ); // 重新初始化上次请求时间
	if( result == nullptr )
		result = new QNetworkAccessManager; // 如果请求管理为 null，则申请一个对象
	auto networkRequest = network_request;
	networkRequest.setUrl( url );
	QNetworkReply *networkReply = result->get( networkRequest ); // 开始 get 请求
	auto app = qApp;
	while( !networkReply->isRunning( ) ) // 没有结束。则处理时间
		app->processEvents( );
	while( !networkReply->isFinished( ) ) // 没有结束。则处理时间
		app->processEvents( );
	Set_Current_Show_3_Message_Auto_Line( u8"结束请求[ ", stdString, u8" ]" );
	return networkReply;
}
QNetworkReply * NovelNetJob::requestRootGet( const QUrl &url, const size_t requestMaxCount, const size_t requestMaxMs, const QString &error_msg, const QString &error_file_append_base_name, const NetworkmanagerConnectFunction &call_function, const QString &call_finle_path_name, const QString &call_function_name, const size_t call_line, const QString &old_url, const QString &file_start ) {
	auto stdString = url.toString( ).toStdString( );
	Set_Current_Show_3_Message_Auto_Line( u8"开始请求[ ", stdString, u8" ]" );
	auto requestReplyTime = getCurrentTimePoint( );
	size_t requestCount = 0;
	do {
		auto networkReply = requestUrl( this->networkAccessManager, this->networkRequest, url, this->requestTime, this->sepMs, call_function, call_finle_path_name, call_function_name, call_line );
		QNetworkReply::NetworkError networkError = networkReply->error( );
		if( networkError == QNetworkReply::NoError ) {
			Set_Current_Show_3_Message_Auto_Line( u8"结束请求[ ", stdString, u8" ]" );
			return networkReply;
		}
		QString msg( u8"请求网址 : (%1) 异常 : [ %2 ], 正在重新获取请求 : [ %3/%4 : %5 毫秒 ] ( 上次请求网址 : %6 )" );

		auto newRequestTime = getCurrentTimePoint( );
		auto commonType = newRequestTime - requestTime;
		auto milliseconds = getTimeDurationToMilliseconds( commonType );
		msg = msg.arg( url.host( ) ).arg( networkError ).arg( requestCount ).arg( requestMaxCount ).arg( milliseconds ).arg( old_url );
		error_write_iostream( requestTime, oStream, networkReply, call_finle_path_name, call_function_name, call_line, msg );
		this->networkRequest.setHeader( QNetworkRequest::UserAgentHeader, getRandomUserAgentHeader( ) );
		if( requestMaxCount && requestCount++ > requestMaxCount ) {
			error_write_file( requestTime, oStream, networkReply, call_function_name, call_finle_path_name, call_line, error_msg + u8"-请求次数超出限制", outPath, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".root_request.error.log", file_start );
			break;
		}
		if( requestMaxMs ) {
			auto compTime = getTimeDurationToMilliseconds( getCurrentTimePoint( ) - requestReplyTime ); // 获取第一次请求的时间差
			if( compTime > 0 && compTime > requestMaxMs ) { // 差大于最大请求时间，则退出请求循环
				error_write_file( requestTime, oStream, networkReply, call_finle_path_name, call_function_name, call_line, error_msg + u8"-请求时间超出限制", outPath, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".root_request.error.log", file_start );
				break;
			}
		}

	} while( true ) ;
	Set_Current_Show_3_Message_Auto_Line( u8"结束请求[ ", stdString, u8" ]" );
	return nullptr;
}
QNetworkReply * NovelNetJob::requestPageGet( const QUrl &url, const size_t requestMaxCount, const size_t requestMaxMs, const QString &error_msg, const QString &error_file_append_base_name, const QString &call_finle_path_name, const QString &call_function_name, const size_t call_line, const QString &old_url, const QString &file_start ) {
	auto stdString = url.toString( ).toStdString( );
	Set_Current_Show_3_Message_Auto_Line( u8"开始请求[ ", stdString, u8" ]" );
	auto instance = qApp;
	auto requestReplyTime = getCurrentTimePoint( );
	size_t requestCount = 0;
	do {
		auto networkReply = requestUrl( this->networkAccessManager, this->networkRequest, url, this->requestTime, this->sepMs );
		QNetworkReply::NetworkError networkError = networkReply->error( );
		if( networkError == QNetworkReply::NoError ) {
			interfaceThisPtr->initBefore( );
			Set_Current_Show_3_Message_Auto_Line( u8"结束请求[ ", stdString, u8" ]" );
			return networkReply;
		}

		QString msg( u8"请求网址 : (%1) 异常 : [ %2 ], 正在重新获取请求 : [ %3/%4 : %5 毫秒 ]( 上次请求网址 : %6 )" );

		auto newRequestTime = getCurrentTimePoint( );
		auto commonType = newRequestTime - requestTime;
		auto milliseconds = getTimeDurationToMilliseconds( commonType );
		msg = msg.arg( url.host( ) ).arg( networkError ).arg( requestCount ).arg( requestMaxCount ).arg( milliseconds ).arg( old_url );
		error_write_iostream( requestTime, oStream, networkReply, call_finle_path_name, call_function_name, call_line, msg );

		if( requestMaxCount && requestCount++ > requestMaxCount ) {
			error_write_file( requestTime, oStream, networkReply, call_finle_path_name, call_function_name, call_line, error_msg + u8"-请求次数超出限制", outPath, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".root_request.error.log", file_start );
			break;
		}
		if( requestMaxMs ) {
			auto compTime = getTimeDurationToMilliseconds( getCurrentTimePoint( ) - requestReplyTime );
			if( compTime > 0 && compTime > requestMaxMs ) {
				error_write_file( requestTime, oStream, networkReply, call_finle_path_name, call_function_name, call_line, error_msg + u8"-请求时间超出限制", outPath, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".root_request.error.log", file_start );
				break;
			}
		}
		instance->processEvents( );
	} while( true ) ;
	Set_Current_Show_3_Message_Auto_Line( u8"结束请求[ ", stdString, u8" ]" );
	return nullptr;
}
bool NovelNetJob::start( ) {
	Set_Current_Show_1_Message_Auto_Line( u8"开始获取类型配置文件" );
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

	Set_Current_Show_1_Message_Auto_Line( u8"类型配置文件获取完毕 -> 开始配置网络请求" );
	auto basicString = outPath.toStdWString( );
	interfaceThisPtr->setOutPath( &basicString );
	QUrl url = this->networkRequest.url( );
	QString qstrUrl = url.toString( );
	interfaceThisPtr->initAfter( );
	OStream::anyStdCOut( "开始获取 -> " + qstrUrl, oStream );
	NetworkmanagerConnectFunction noErrorRunCode = [this]( QNetworkReply *q_network_reply, const QString call_file_path_name, size_t call_line, const QString call_name ) {
		typeCount = 0;
		this->typeNovelsMap.clear( );
		emit requesting_get_root_page_signals( q_network_reply->url( ), q_network_reply );
		return false;
	};
	NetworkmanagerConnectFunction hasErrorRunCode = [this]( QNetworkReply *q_network_reply, const QString call_file_path_name, size_t call_line, const QString call_name ) {
		auto qUrl = q_network_reply->url( );
		error_write_file( requestTime, oStream, q_network_reply, call_name, call_file_path_name, call_line, u8"首页请求失败", outPath, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".root_request.error.log", u8"request_root_start_hasErrorRunCode" );
		return false;
	};
	NetworkmanagerConnectFunction callFunction = [noErrorRunCode,hasErrorRunCode,this]( QNetworkReply *q_network_reply, const QString call_file_path_name, size_t call_line, const QString call_name ) {
		if( q_network_reply->error( ) == QNetworkReply::NoError )
			return noErrorRunCode( q_network_reply, call_file_path_name, call_line, call_name );
		else
			return hasErrorRunCode( q_network_reply, call_file_path_name, call_line, call_name );
	};

	Set_Current_Show_1_Message_Auto_Line( u8"开始请求首页" );
	auto networkReply = requestRootGet( url, requestMaxCount, requestMaxMilliseconds, u8"首页请求失败", u8".root_request.error.log", callFunction, __FILE__, __FUNCTION__, __LINE__, qstrUrl, u8"request_root_start_requestGet" );
	this->interfaceThisPtr->initBefore( );
	if( !networkReply )
		emit requested_get_web_page_signals_end( url );
	Set_Current_Show_1_Message_Auto_Line( u8"首页请求完毕" );
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

	auto msg = "从( " + rootUrl + " ) 解析小说类型链接信息 ";
	OStream::anyStdCOut( "从( " + rootUrl + " ) 解析小说类型链接信息 ", oStream );
	Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
	QString htmlText( request_connect->readAll( ) );
	if( htmlText.isEmpty( ) )
		return;
	auto mapHtmlStrKHtmlStrV = interfaceThisPtr->formHtmlGetTypeTheUrls( rootUrl.toStdWString( ), htmlText.toStdWString( ) );
	if( !mapHtmlStrKHtmlStrV ) {
		msg.append( u8"首页异常 : " ).append( u8"( 没有找到类型节点 )结束" ).append( u8"(" ).append( rootUrl ).append( ")" );
		OStream::anyStdCerr( msg, instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __LINE__, __FUNCTION__, oStream );
		Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
		emit endJob( );
		return;
	}
	this->typeNovelsMap.clear( );
	msg = "从( " + rootUrl + " ) 分配类型信息 ";
	Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
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
			auto urlLink = QString::fromStdWString( iterator->second );
			requests.emplace( typeName, urlLink ); // 等待处理
			this->typeNovelsMap.emplace( typeName, std::make_shared< Vector_INovelInfoSPtr >( ) );
			++typeCount;
			msg = "从( " + rootUrl + " ) 获得类型信息( " + typeName + " ) [ " + urlLink + " ] ";
			OStream::anyStdCOut( msg, oStream );
			Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
		}
		++iterator;
	} while( iterator != end );

	msg = "开始从( " + rootUrl + " ) 获取小说类型的请求解析";
	Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
	// 开始获取数据
	auto requestVectorIterator = requests.begin( );
	auto requestVectorEnd = requests.end( );
	size_t pageIndex = 0;
	size_t requestCount = 1;
	while( requestVectorIterator != requestVectorEnd ) {
		++requestCount;
		auto typeNmae = requestVectorIterator->first;
		auto qUrl = requestVectorIterator->second;
		msg = "( " + rootUrl + " )类型( " + typeNmae + " ) [ " + qUrl + " ] 页面请求";
		OStream::anyStdCOut( msg, oStream );
		Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
		auto startRequestTimePoint = std::chrono::system_clock::now( );
		auto networkReply = requestPageGet( qUrl, requestMaxCount, requestMaxMilliseconds, u8"类型页面请求失败", u8".type_request.error.log", __FILE__, __FUNCTION__, __LINE__, "none", u8"requestGet_type_log" );
		if( networkReply ) {
			msg = "( " + rootUrl + " )类型( " + typeNmae + " ) [ " + qUrl + " ] 页面解析小说";
			Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
			++pageIndex;
			auto readAll = networkReply->readAll( );
			auto html = std::make_shared< cylHtmlTools::HtmlString >( QString( readAll ).toStdWString( ) );
			auto newQUrl = getPageInfo( typeNmae, qUrl, html );
			NovelDBJob::removeAllSpace( newQUrl );
			while( !newQUrl.isEmpty( ) || newQUrl.length( ) > 0 ) {
				auto oldUrl = qUrl;
				qUrl = newQUrl;
				networkReply = requestPageGet( qUrl, requestMaxCount, requestMaxMilliseconds, u8"下一页请求失败", u8".type_request_next.error.log", __FILE__, __FUNCTION__, __LINE__, oldUrl, u8"next_page_request_get_error_log" );
				if( !networkReply ) {
					msg = "( " + rootUrl + " )类型( " + typeNmae + " ) [ " + qUrl + " ] 页面解析下一页失败";
					Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
					break; // 请求失败，则终止
				}
				++pageIndex;
				readAll = networkReply->readAll( );
				html = std::make_shared< cylHtmlTools::HtmlString >( QString( readAll ).toStdWString( ) );
				newQUrl = getPageInfo( typeNmae, qUrl, html );
				NovelDBJob::removeAllSpace( newQUrl );
			}
		} else if( requestMaxCount > requestCount ) {

			msg = "( " + rootUrl + " )类型( " + typeNmae + " ) [ " + qUrl + " ] 请求失败";
			Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
			auto endRequestTimePoint = std::chrono::system_clock::now( );
			auto sepTimePoint = endRequestTimePoint - startRequestTimePoint;
			auto sepRequestTimePointMs = std::chrono::duration_cast< std::chrono::milliseconds >( sepTimePoint ).count( );
			if( sepRequestTimePointMs < sepMs )
				waiteMilliseconds( sepRequestTimePointMs - sepMs );
			continue;
		}
		auto endRequestTimePoint = std::chrono::system_clock::now( );
		auto sepTimePoint = endRequestTimePoint - startRequestTimePoint;
		auto sepRequestTimePointMs = std::chrono::duration_cast< std::chrono::milliseconds >( sepTimePoint ).count( );
		if( sepRequestTimePointMs < sepMs )
			waiteMilliseconds( sepRequestTimePointMs - sepMs );

		msg = "( " + rootUrl + " )类型[ " + typeNmae + " ] 类型请求完毕";
		Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
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
	Set_Current_Show_1_Message_Auto_Line( u8"更新小说" );
	return nullptr;
}


QString NovelNetJob::getPageInfo( const QString &type_name, const QUrl &type_url, cylHtmlTools::HtmlString_Shared html_string ) {
	QString msg = u8" 从 (" + type_url.toString( ) + u8" ), 类型 [ " + type_name + " ] 从小说页面获取小说";
	Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
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
	msg = u8"获取 ( " + type_name + " => " + typePageUrl + " ) 小说, 序列 : [ " + QString::number( typeCount ) + " ] ";
	OStream::anyStdCOut( msg, oStream );
	Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
	auto novelInfos = interfaceThisPtr->formHtmlGetTypePageNovels( type_name.toStdWString( ), typePageUrl.toStdWString( ), *html_string, *saveMapNovelInfos, nullptr );
	QString resultMsg;
	if( novelInfos.size( ) == 0 ) {
		// 没有小说则终止
		msg = "( " + type_name + " => " + typePageUrl + " ) 序列 : [ " + QString::number( typeCount ) + " ] 没有发现小说";
		OStream::anyStdCOut( msg, oStream );
		Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
		return "";
	}
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
	msg = "( " + type_name + " => " + typePageUrl + " ) 序列 : [ " + QString::number( typeCount ) + " ] 开始遍历存储";
	Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
	auto requestedGetVectorIterator = requestedGetVectorINovelInfoSPtrShared->begin( );
	auto requestedGetVectorEnd = requestedGetVectorINovelInfoSPtrShared->end( );
	size_t novelSaveCount = 0;
	size_t typeNovelCount = 0;
	if( requestedGetVectorIterator != requestedGetVectorEnd ) {
		auto vectorPtr = saveMapNovelInfos.get( );
		for( ; requestedGetVectorIterator != requestedGetVectorEnd; ++requestedGetVectorIterator ) {
			vectorPtr->emplace_back( *requestedGetVectorIterator );
			++novelSaveCount;
		}
		typeNovelCount = vectorPtr->size( );
	}
	msg = "( " + type_name + " => " + typePageUrl + " ), 序列 : [ " + QString::number( typeCount ) + " ] 获取小说统计数为 : " + QString::number( novelSaveCount ) + " / " + QString::number( typeNovelCount );
	Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
	OStream::anyStdCOut( msg, oStream );
	OStream::anyStdCOut( u8"获取下一页 " + type_name + "(" + typePageUrl + ")" + "[" + QString::number( typeCount ) + "]", oStream );
	auto formHtmlGetNext = interfaceThisPtr->formHtmlGetNext( type_name.toStdWString( ), typePageUrl.toStdWString( ), *html_string, *saveMapNovelInfos, *requestedGetVectorINovelInfoSPtrShared );
	if( formHtmlGetNext.empty( ) ) {

		msg = "( " + type_name + " => " + typePageUrl + " ), 序列: [ " + QString::number( typeCount ) + " ] 不存在下一页 ";
		Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
		OStream::anyStdCOut( msg, oStream );
		return "";
	}
	auto nextPageUrl = QString::fromStdWString( formHtmlGetNext );

	msg = "( " + type_name + " => " + typePageUrl + " ), 序列: [ " + QString::number( typeCount ) + " ] 返回下一页 -> { " + nextPageUrl + " }";
	Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
	OStream::anyStdCOut( msg, oStream );
	return nextPageUrl;  // 正常执行完毕
}


void NovelNetJob::novelPageInfoRequestEnd( const QString &type_name, const QUrl &url, size_t current_page_index ) {
	std::cout << u8"-----------------------" << std::endl;
	std::cout << type_name.toStdString( ) << "( " << url.toString( ).toStdString( ) << " ) 请求结束" << std::endl;
	std::cout << u8"-----------------------" << std::endl;
	auto rootUrl = url.host( );
	auto root_url = url.scheme( ) + "://" + rootUrl;
	Vector_INovelInfoSPtr_Shared infos = typeNovelsMap.at( type_name );
	interfaceThisPtr->novelTypeEnd( root_url.toStdWString( ), type_name.toStdWString( ), url.toString( ).toStdWString( ), *infos );
	releaseMemory( );

}


void NovelNetJob::slots_requested_get_web_page_signals_end( const QUrl &url ) {

	QString msg = "( " + url.toString( ) + u8" ) 请求结束，开始写入数据库";
	Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
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
	msg = QString( ).append( "请求 : (" ).append( url.scheme( ) ).append( u8"://" ).append( url.host( ) ).append( ") 结束 " );
	OStream::anyStdCOut( msg, oStream );
	Set_Current_Show_1_Message_Auto_Line( msg.toStdString( ) );
	typeNovelsMap.clear( );
	releaseMemory( );
	emit endJob( );
}
