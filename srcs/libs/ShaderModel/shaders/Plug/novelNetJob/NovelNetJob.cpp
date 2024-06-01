#include "NovelNetJob.h"
#include <HttpNetWork/RequestConnect.h>
#include <HttpNetWork/Request.h>
#include <HttpNetWork/NetworkRequest.h>
#include <stream/OStream.h>

#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/htmlTools/XPath/XPath.h"
#include "htmls/htmlTools/HtmlWorkThread/HtmlWorkThread.h"
#include "htmls/HtmlNode/HtmlNode.h"
#include "thread/FileThread.h"
#include <HttpNetWork/NetworkAccessManager.h>
#include <qguiapplication.h>
#include <unordered_map>

#include "../../../../../git/QtExtendDB/srcs/QtExtendDB/nameSpace/cylDB.h"
#include "../ioFile/IOFile.h"
#include "dateTime/DateTime.h"

#include "interface/INovelInfo.h"
#include "auto_generate_files/macro/cmake_to_c_cpp_header_env.h"
#include "path/Dir.h"
#include <DB/sqlite/sqliteResult/SQLiteResult.h>
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
/// <param name="write_root_path">写入的根目录</param>
/// <param name="file_name">文件名称</param>
/// <param name="file_suffix">后缀名称</param>
inline void error_write_file( std::chrono::system_clock::time_point::duration &nowTimeDuration, OStream *oStream, QNetworkReply *networkReply, const QString &file, const QString &call_function_name, size_t line, const QString &write_root_path, const QString &file_name, const QString &file_suffix ) {
	QDateTime currentDateTime = QDateTime::currentDateTime( );
	QString currentTime = currentDateTime.toString( "yyyy_MM_dd hh-mm-ss" );
	nowTimeDuration = cylHttpNetWork::TimeTools::getNowTimeDuration( ) - nowTimeDuration;
	long long timeDurationToMilliseconds = cylHttpNetWork::TimeTools::getTimeDurationToMilliseconds( nowTimeDuration );
	QString seconds( QString::number( timeDurationToMilliseconds / 1000 ) ); // 秒
	QString msg;
	auto url = networkReply->url( );
	msg.append( "=========================		try : info" )
		.append( "\n\t当前时间 : " ).append( currentTime ).append( "\n\t" )
		.append( "\n\t执行时间 : " ).append( seconds ).append( "秒\n\t" )
		.append( "\n\t错误文件 : " ).append( file ).append( "\n\t" )
		.append( "\n\t信息位置 : " ).append( QString::number( line ) )
		.append( "\n\t信息函数 : " ).append( __FUNCTION__ )
		.append( "=========================		try : message" )
		.append( "\n\t错误信息 : " ).append( getErrorQStr( networkReply->error( ) ) )
		.append( u8"\n\t类型 : " ).append( "首页" ).append( u8"(" ).append( url.toString( ) ).append( ")" )
		.append( "=========================" );
	if( write_root_path.isEmpty( ) )
		OStream::anyDebugOut( oStream, msg, file, line, call_function_name );
	else {
		QString errorWriteFilePath( u8R"(%2%1%3%1%4%1%5.%6)" );
		QString host = url.host( );
		QString timeForm = currentDateTime.toString( "yyyy-MM-dd" );
		errorWriteFilePath = errorWriteFilePath.arg( QDir::separator( ) ).arg( write_root_path ).arg( host ).arg( timeForm ).arg( file_name ).arg( file_suffix );
		OStream::anyDebugOut( oStream, msg, file, line, call_function_name, errorWriteFilePath, msg );
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

NovelNetJob::NovelNetJob( OStream *o_stream, QObject *interface_obj_ptr, IRequestNetInterface *interface_this_ptr ): interfaceObjPtr( interface_obj_ptr ), interfaceThisPtr( interface_this_ptr ), oStream( o_stream ) {
	initObj( );
	initObjProperty( );
	initConnect( );
}

NovelNetJob::~NovelNetJob( ) {
	interfaceObjPtr->deleteLater( );
}
void NovelNetJob::initObj( ) {
	networkAccessManager = std::make_shared< cylHttpNetWork::NetworkAccessManager >( this );
	networkRequest = std::make_shared< cylHttpNetWork::NetworkRequest >( );
	root = std::make_shared< std::pair< RequestConnect_Shared, Request_Shared > >( );
	root->first = std::make_shared< cylHttpNetWork::RequestConnect >( );
	root->second = std::make_shared< cylHttpNetWork::Request >( networkAccessManager.get( ), root->first.get( ) );
}
void NovelNetJob::initObjProperty( ) {
	interfaceThisPtr->setOStream( oStream );
	QString agentHeader = networkRequest->getRandomUserAgentHeader( );
	networkRequest->setHeader( QNetworkRequest::UserAgentHeader, agentHeader );
	QSslConfiguration sslConfiguration = networkRequest->sslConfiguration( );
	sslConfiguration.setPeerVerifyMode( QSslSocket::VerifyNone );
	networkRequest->setSslConfiguration( sslConfiguration );
	typeCount = 0;
	QString ingFile( u8"%1%2%3%2%4%5" );
	ingFile = ingFile.arg( Project_Plug_bin ).arg( QDir::separator( ) ).arg( u8"ini" ).arg( u8"get_type" ).arg( u8".txt" );
	QFile qFile;
	qFile.setFileName( ingFile );
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
	runStatus = 0;
}

void NovelNetJob::initConnect( ) {
}

bool NovelNetJob::start( ) {
	if( runStatus != 0 )
		return false;
	HtmlDocString resultUrl;
	size_t size = interfaceThisPtr->getRootUrl( &resultUrl );
	QString qUrl = QString::fromStdWString( resultUrl );
	if( size == 0 )
		return false;
	qDebug( ) << networkAccessManager->supportedSchemes( );
	qDebug( ) << QSslSocket::sslLibraryBuildVersionString( );
	if( OStream::hasDefaultOStream( qUrl ) == false )
		OStream::setDefaultOStream( qUrl, oStream );
	interfaceThisPtr->initAfter( );
	interfaceThisPtr->initBefore( );
	runStatus = 1;
	cylHttpNetWork::Request *request = root->second.get( );
	std::chrono::system_clock::time_point::duration nowTimeDuration = cylHttpNetWork::TimeTools::getNowTimeDuration( );
	request->netGetWork( qUrl, *networkRequest );
	size_t line = __LINE__;
	std::string stdString = qUrl.toStdString( );
	auto networkReply = request->getNetworkReply( 150
		, 3 * 60 * 1000
		, 2000
		, [qUrl,stdString,this]( cylHttpNetWork::RequestConnect *request_connect, cylHttpNetWork::NetworkRequest *network_request ) {
			QString msg = ( QStringList( ) << "\t:" << __FUNCTION__ << " 正在请求 : " << stdString.c_str( ) ).join( ' ' );
			OStream::anyDebugOut( oStream, msg );
			network_request->setHeader( QNetworkRequest::UserAgentHeader, cylHttpNetWork::NetworkRequest::getRandomUserAgentHeader( ) );
			return 1;
		} );
	if( networkReply->error( ) == QNetworkReply::NoError ) {
		this->typeNovelsMap.clear( );
		auto requestConnect = root->first.get( );
		auto url = networkReply->url( );
		OStream::anyDebugOut( oStream, "开始获取 " + qUrl );
		slots_requesting_get_root_page_signals( url, requestConnect );
		return true;
	} else
		error_write_file( nowTimeDuration, oStream, networkReply, __FILE__, __FUNCTION__, line, Cache_Path_Dir, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".root_request.error.log" );
	return false;
}
QString NovelNetJob::getUrl( ) const {
	HtmlDocString resultUrl;
	size_t size = interfaceThisPtr->getRootUrl( &resultUrl );
	if( size )
		return QString::fromStdWString( resultUrl );
	return "";
}
void NovelNetJob::slots_requesting_get_root_page_signals( const QUrl &url, cylHttpNetWork::RequestConnect *request_connect ) {
	emit requesting_get_root_page_signals( url, request_connect );
	auto networkReply = request_connect->getNetworkReply( );
	QString rootUrl = url.scheme( ) + u8"://" + url.host( );
	QString htmlText( networkReply->readAll( ) );
	if( htmlText.isEmpty( ) )
		return;
	auto mapHtmlStrKHtmlStrV = interfaceThisPtr->formHtmlGetTypeTheUrls( rootUrl.toStdWString( ), htmlText.toStdWString( ) );
	if( !mapHtmlStrKHtmlStrV ) {
		QString msg;
		msg.append( u8"首页异常 : " ).append( u8"(没有找到类型节点)结束" ).append( u8"(" ).append( rootUrl ).append( ")" );
		OStream::anyDebugOut( oStream, msg, __FILE__, __LINE__, __FUNCTION__ );
		return;
	}

	///////////////// 类型请求
	auto iterator = mapHtmlStrKHtmlStrV->begin( ); // 类型遍历
	auto end = mapHtmlStrKHtmlStrV->end( ); // 类型结束
	std::unordered_map< cylHttpNetWork::Request *, std::pair< QString, QString > > requests;
	do {
		QString typeName = getNormalQString( QString::fromStdWString( iterator->first ) );
		auto getTypeNamelistIterator = getTypeNamelist.begin( ); // 根据配置文件当中的可查找遍历类型名称来实现列表获取
		auto getTypeNamelistEnd = getTypeNamelist.end( ); // 配置文件列表的尾节点
		for( ; getTypeNamelistIterator != getTypeNamelistEnd; ++getTypeNamelistIterator )
			if( *getTypeNamelistIterator == typeName )
				break;
		if( getTypeNamelistIterator != getTypeNamelistEnd ) {
			QString typeUrl = QString::fromStdWString( iterator->second );
			auto requestConnect = new cylHttpNetWork::RequestConnect;
			auto request = new cylHttpNetWork::Request( networkAccessManager.get( ), requestConnect );
			auto makePair = std::make_pair( typeName, typeUrl );
			requests.emplace( request, makePair ); // 等待处理
		}
		++iterator;
	} while( iterator != end );

	// 开始获取数据
	auto requestVectorIterator = requests.begin( );
	auto requestVectorEnd = requests.end( );
	do {
		cylHttpNetWork::Request *request = requestVectorIterator->first;
		auto pair = requestVectorIterator->second;
		auto nowTimeDuration = cylHttpNetWork::TimeTools::getNowTimeDuration( );
		size_t line = __LINE__;
		auto stdString = pair.second.toStdString( );
		request->netGetWork( pair.second, *networkRequest ); // 发送请求
		networkReply = request->getNetworkReply( 15
			, 3 * 60 * 1000
			, 2000
			, [pair, stdString,this]( cylHttpNetWork::RequestConnect *request_connect, cylHttpNetWork::NetworkRequest *network_request ) {
				QString msg = ( QStringList( ) << "\t:" << __FUNCTION__ << " 正在请求 : " << stdString.c_str( ) ).join( ' ' );
				OStream::anyDebugOut( oStream, msg );
				network_request->setHeader( QNetworkRequest::UserAgentHeader, cylHttpNetWork::NetworkRequest::getRandomUserAgentHeader( ) );
				return 1;
			} );
		if( networkReply->error( ) == QNetworkReply::NoError ) { // 没有错误
			++typeCount;
			QString msg;
			msg.append( u8"发现 : " )
				.append( pair.first ).append( "[" ).append( pair.second ).append( "](" )
				.append( QString::number( typeCount ) ).append( ")" );
			OStream::anyDebugOut( oStream, msg );
			auto htmlString = std::make_shared< cylHtmlTools::HtmlString >( QString( networkReply->readAll( ) ).toStdWString( ) );
			slots_requesting_get_type_page_url_signals( rootUrl, pair.first, pair.second, htmlString );
			request->getRequestConnect( )->deleteLater( );
			request->deleteLater( );
		} else  // 存在错误
			error_write_file( nowTimeDuration, oStream, networkReply, __FILE__, __FUNCTION__, line,Cache_Path_Dir, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".type_request.error.log" );
		++requestVectorIterator;
	} while( requestVectorIterator != requestVectorEnd );
	slots_requested_get_web_page_signals_end( rootUrl );
}

interfacePlugsType::INovelInfo_Shared NovelNetJob::saveToStoreNovels(
	interfacePlugsType::Vector_INovelInfoSPtr_Shared &requestedGetVectorINovelInfoSPtrShared
	, interfacePlugsType::Vector_INovelInfoSPtr_Shared &saveMapNovelInfos
	, interfacePlugsType::INovelInfo_Shared &novel
	, QString *result_msg ) {
	INovelInfo_Shared formHtmlGetUrlNovelInfo = nullptr;
	NovelTypeNamePair_Shared urlPair( std::make_shared< NovelTypeNamePair >( ) );
	HtmlDocString buffStr;
	novel->getNovelUrl( &buffStr );
	QString novelUrl = QString::fromStdWString( buffStr );
	novel->getNovelName( &buffStr );
	QString novelName = QString::fromStdWString( buffStr );
	auto requestConnect = new cylHttpNetWork::RequestConnect;
	auto request = new cylHttpNetWork::Request( networkAccessManager.get( ), requestConnect );
	request->netGetWork( novelUrl, *networkRequest );

	auto nowTimeDuration = cylHttpNetWork::TimeTools::getNowTimeDuration( );
	size_t line = __LINE__;
	auto stdString = novelUrl.toStdString( );
	auto reply = request->getNetworkReply( 15
		, 3 * 60 * 1000
		, 2000
		, [novelUrl,stdString,this]( cylHttpNetWork::RequestConnect *request_connect, cylHttpNetWork::NetworkRequest *network_request ) {
			QString msg = ( QStringList( ) << "\t:" << __FUNCTION__ << " 正在请求 : " << stdString.c_str( ) ).join( ' ' );
			OStream::anyDebugOut( oStream, msg );
			network_request->setHeader( QNetworkRequest::UserAgentHeader, cylHttpNetWork::NetworkRequest::getRandomUserAgentHeader( ) );
			return 1;
		} );
	if( reply->error( ) != QNetworkReply::NoError ) { // 异常则跳过这次获取
		*result_msg = getErrorQStr( reply->error( ) );
		result_msg->append( u8"\n小说地址:" ).append( novelUrl );
		error_write_file( nowTimeDuration, oStream, reply, __FILE__, __FUNCTION__, line,Cache_Path_Dir, QDateTime::currentDateTime( ).toString( "hh-mm-ss" ), ".saveToStoreNovels.error.log" );
		return formHtmlGetUrlNovelInfo;
	}
	// 解析小说
	auto readAll = reply->readAll( );
	if( readAll.size( ) == 0 ) { // 异常则跳过这次获取
		*result_msg = u8"没有找到正确的内容";
		result_msg->append( u8"\t小说地址:" ).append( novelUrl );
		return formHtmlGetUrlNovelInfo;
	}
	QString htmlText( readAll );
	request->deleteLater( ); // 释放
	requestConnect->deleteLater( ); // 释放
	formHtmlGetUrlNovelInfo = interfaceThisPtr->formHtmlGetUrlNovelInfo( novelUrl.toStdWString( ), htmlText.toStdWString( ), *saveMapNovelInfos, novel );
	if( formHtmlGetUrlNovelInfo ) // 返回非 nullptr 则存储
		requestedGetVectorINovelInfoSPtrShared->emplace_back( novel ); // 存储已知小说
	else
		*result_msg = u8"获取到的小说为空";
	return formHtmlGetUrlNovelInfo;
}


void NovelNetJob::slots_requesting_get_type_page_url_signals( const QString &root_url, const QString &type_name, const QUrl &type_url, cylHtmlTools::HtmlString_Shared &html_string ) {
	emit requesting_get_type_page_url_signals( root_url, type_name, type_url, html_string );
	size_t count = 0;
	if( this->typeCountMap.count( type_name ) )
		count = this->typeCountMap.at( type_name );

	Vector_INovelInfoSPtr_Shared requestedGetVectorINovelInfoSPtrShared( std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( ) );
	// 存储获取的小说
	auto iterator = this->typeNovelsMap.begin( );
	auto end = this->typeNovelsMap.end( );
	Vector_INovelInfoSPtr_Shared saveMapNovelInfos = nullptr;
	for( ; iterator != end; ++iterator )
		if( iterator->first == type_name ) { // 这是已经被调用了
			saveMapNovelInfos = iterator->second;
			break;
		}
	if( iterator == end ) { // 这是首次调用
		saveMapNovelInfos = std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( );
		this->typeNovelsMap.emplace( type_name, saveMapNovelInfos );
	}

	QString typePageUrl = type_url.toString( );
	auto novelInfos = interfaceThisPtr->formHtmlGetTypePageNovels( type_name.toStdWString( ), typePageUrl.toStdWString( ), *html_string, *saveMapNovelInfos, nullptr );
	QString resultMsg;
	do {
		if( novelInfos.size( ) == 0 )
			break;
		// 没有小说则终止
		for( auto &novel : novelInfos ) {
			if( interfaceThisPtr->isRequestNovelInfoUrl( novel.get( ) ) ) {				// 如果小说需要到详情页申请，则临时存储
				auto resultINovelShared = saveToStoreNovels( requestedGetVectorINovelInfoSPtrShared, saveMapNovelInfos, novel, &resultMsg );
				if( !resultINovelShared.get( ) )  // 存在错误消息，即没有存储
					OStream::anyDebugOut( oStream, resultMsg );
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
			break;
		QUrl nextUrl( QString::fromStdWString( formHtmlGetNext ) );
		++count;
		this->typeCountMap.emplace( type_name, count );
		size_t index = count + 1;
		slots_requesting_get_next_type_page_url_signals( root_url, type_name, type_url, nextUrl, count, index, saveMapNovelInfos, requestedGetVectorINovelInfoSPtrShared );
		return;  // 正常执行完毕
	} while( false );

	// 没有使用 return 返回，则表示终止调用
	slots_requested_get_type_page_url_end( getUrl( ), type_name, type_url, count, requestedGetVectorINovelInfoSPtrShared );
}
void NovelNetJob::slots_requesting_get_next_type_page_url_signals( const QString &root_url, const QString &type_name, const QUrl &old_url, const QUrl &url, size_t old_page_index, size_t current_page_index, const interfacePlugsType::Vector_INovelInfoSPtr_Shared &saveMapNovelInfos, const interfacePlugsType::Vector_INovelInfoSPtr_Shared &novel_s_ptr_shared ) {
	emit requesting_get_next_type_page_url_signals( root_url, type_name, old_url, url, old_page_index, current_page_index, saveMapNovelInfos, novel_s_ptr_shared );
	if( novel_s_ptr_shared->size( ) == 0 ) {
		slots_requested_get_type_page_url_end( root_url, type_name, old_url, old_page_index, novel_s_ptr_shared );
		return;
	}
	auto requestConnect = new cylHttpNetWork::RequestConnect;
	auto request = new cylHttpNetWork::Request( networkAccessManager.get( ), requestConnect );
	auto stdString = url.toString( ).toStdString( );
	request->netGetWork( url, *networkRequest );
	auto reply = request->getNetworkReply( 15
		, 3 * 60 * 1000
		, 2000
		, [url,stdString,this]( cylHttpNetWork::RequestConnect *request_connect, cylHttpNetWork::NetworkRequest *network_request ) {
			QString msg = ( QStringList( ) << "\t:" << __FUNCTION__ << " 正在请求 : " << stdString.c_str( ) ).join( ' ' );
			OStream::anyDebugOut( oStream, msg );
			network_request->setHeader( QNetworkRequest::UserAgentHeader, cylHttpNetWork::NetworkRequest::getRandomUserAgentHeader( ) );
			return 1;
		} );
	if( reply->error( ) != QNetworkReply::NoError ) { // 异常则跳过这次获取
		auto msg = getErrorQStr( reply->error( ) );
		msg.append( u8"\n类型 : " ).append( type_name ).append( u8"(" ).append( url.toString( ) ).append( ")" );
		OStream::anyDebugOut( oStream, msg, __FILE__, __LINE__, __FUNCTION__ );
		requestConnect->deleteLater( );
		request->deleteLater( );
		return;
	}
	QString string( reply->readAll( ) );
	auto stdWString = string.toStdWString( );
	auto htmlString = std::make_shared< cylHtmlTools::HtmlString >( stdWString );
	slots_requesting_get_type_page_url_signals( root_url, type_name, url, htmlString );
	requestConnect->deleteLater( );
	request->deleteLater( );
}

void NovelNetJob::slots_requested_get_type_page_url_end( const QString &root_url, const QString &type_name, const QUrl &url, size_t current_page_index, const interfacePlugsType::Vector_INovelInfoSPtr_Shared &novel_s_ptr_shared ) {
	emit requested_get_type_page_url_end( root_url, type_name, url, current_page_index, novel_s_ptr_shared );

	--typeCount;
	qDebug( ) << u8"-----------------------";
	qDebug( ) << type_name << "(" << url.toString( ) << ") 请求结束";
	qDebug( ) << u8"-----------------------";
	QString writeFilePath( u8"%2%3%1%4%1%5%6" );
	writeFilePath = writeFilePath.arg( QDir::separator( ) ).arg( Cache_Path_Dir ).arg( url.host( ) ).arg( u8"txt_out" ).arg( type_name ).arg( u8".txt" );

	Vector_INovelInfoSPtr_Shared infos = typeNovelsMap.at( type_name );

	IOFile ioFile( writeFilePath, infos );
	size_t writeQStringListToFile = ioFile.writeNoveInfoListToFile( );
	OStream::anyDebugOut( oStream, QString( u8"================\n\t路径: (%1)\n\t写入数量 : [%2]\n\t类型计数 : [%3]\n================\n" ).arg( writeFilePath ).arg( writeQStringListToFile ).arg( typeCount ) );
	interfaceThisPtr->novelTypeEnd( root_url.toStdWString( ), type_name.toStdWString( ), url.toString( ).toStdWString( ), *infos );
}
void NovelNetJob::slots_requested_get_web_page_signals_end( const QUrl &url ) {
	emit requested_get_web_page_signals_end( url );
	Vector_INovelInfoSPtr_Shared novelInfoSPtr( std::make_shared< Vector_INovelInfoSPtr >( ) );

	auto mapIterator = typeNovelsMap.begin( );
	auto mapEnd = typeNovelsMap.end( );

	cylHtmlTools::HtmlWorkThread< bool * >::Current_Thread_Run currentThreadRun = [&]( const cylHtmlTools::HtmlWorkThread< bool * > *html_work_thread, const std::thread *run_std_cpp_thread, std::mutex *html_work_thread_mutex, std::mutex *std_cpp_thread_mutex, bool *data, const time_t *startTime ) {
		for( ; mapIterator != mapEnd; ++mapIterator )
			for( auto &novel : *mapIterator->second )
				novelInfoSPtr->emplace_back( novel );
	};
	bool has = true;
	cylHtmlTools::HtmlWorkThread< bool * > thread( nullptr, currentThreadRun, nullptr, &has );
	thread.start( );
	auto nowTimeDuration = cylHttpNetWork::TimeTools::getNowTimeDuration( );
	while( thread.isRun( ) ) {
		long long timeDurationToMilliseconds = cylHttpNetWork::TimeTools::getTimeDurationToMilliseconds( cylHttpNetWork::TimeTools::getNowTimeDuration( ) - nowTimeDuration );
		if( timeDurationToMilliseconds > 2000 ) {
			auto msg = QString( "===========" )
						.append( "\n\t" ).append( "正在合并小说列表" )
						.append( "\n\t" ).append( __FILE__ )
						.append( "\n\t" ).append( QString::number( __LINE__ ) )
						.append( "\n\t" ).append( "===========" );
			OStream::anyDebugOut( oStream, msg );
			nowTimeDuration = cylHttpNetWork::TimeTools::getNowTimeDuration( );
		}
		qApp->processEvents( );
	}
	interfaceThisPtr->endHost( *novelInfoSPtr
		, [this]( const cylHttpNetWork::TimeTools::Time_Duration &duration ) {
			if( cylHttpNetWork::TimeTools::getTimeDurationToMilliseconds( duration ) < 2000 )
				return false;
			auto msg = QString( "===========" )
						.append( "\n\t" ).append( "正在存储数据库" )
						.append( "\n\t" ).append( __FILE__ )
						.append( "\n\t" ).append( QString::number( __LINE__ ) )
						.append( "\n\t" ).append( "===========" );
			return true;
		} );
	runStatus = 0; // 可以重新运行
	QString msg = QString( ).append( "请求 : (" ).append( url.scheme( ) ).append( u8"://" ).append( url.host( ) ).append( ") 结束 " );
	OStream::anyDebugOut( oStream, msg );
	typeNovelsMap.clear( );
}
