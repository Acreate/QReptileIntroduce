﻿#include "NovelDBJob.h"

#include <QString>
#include <QDir>
#include <QSqlQuery>
#include <htmls/htmlTools/HtmlWorkThread/HtmlWorkThread.h>
#include <nameSpace/cylDB.h>
#include <DB/DBTools.h>
#include <DB/dbInterface/I_DB.h>
#include <DB/dbInterface/i_depositoryInterface/I_Depository.h>
#include <DB/dbInterface/i_resultInfo/I_ResultInfo.h>
#include <stream/OStream.h>
#include <interface/INovelInfo.h>
#include <qguiapplication.h>
#include <QUrl>
#include <mutex>
#include "../ioFile/IOFile.h"
#include "../novelBase/NovelBase.h"
#include "htmlString/HtmlStringTools.h"

QString NovelDBJob::currentTimeForm = QObject::tr( u8R"(yyyy-MM-dd hh:mm:ss)" );
QDateTime NovelDBJob::currentTime = QDateTime::currentDateTime( );
QStringList NovelDBJob::tabFieldNames = { "rootUrl", "novelName", "info", "updateTime", "format", "lastRequestTime", "lastRequestTimeFormat", "author", "url", "lastItem", "additionalData", "typePageUrl", "typeName" };
QString NovelDBJob::updateCmd = R"(UPDATE `%1` SET `updateTime`=:updateTime, `lastRequestTime`=:lastRequestTime, `additionalData`=:additionalData, `lastItem`=:lastItem , `format`=:format  WHERE `url`=:url;)";
QString NovelDBJob::insterCmd = R"(INSERT INTO `%1`( `rootUrl`, `novelName`, `info`, `updateTime`, `format`, `lastRequestTime`, `lastRequestTimeFormat`, `author`, `url`, `lastItem`, `additionalData`, `typePageUrl`, `typeName`  ) VALUES ( :rootUrl,:novelName,:info,:updateTime,:format,:lastRequestTime,:lastRequestTimeFormat,:author,:url,:lastItem,:additionalData,:typePageUrl,:typeName  );)";
namespace instance_function {
	struct NovelNodeXPathInfo;
	/// <summary>
	/// 分解列表数据<br/>
	///	根据游离列表 src_vector 与永久列表 db_result 进行匹配解析<br/>
	/// updateList : src_vector 与 db_result 交集<br/>
	/// interList : src_vector 与 db_result 的不同部分<br/>
	/// </summary>
	/// <param name="src_vector">游离列表</param>
	/// <param name="db_result">磁盘列表</param>
	/// <param name="updateList">更新列表</param>
	/// <param name="interList">插入列表</param>
	inline void separate_list( const NovelDBJob::NovelInfoVector &src_vector, cylDB::IResultInfo_Shared &db_result, NovelDBJob::NovelInfoVector &updateList, NovelDBJob::NovelInfoVector &interList ) {
		db_result->resetColIndex( );
		auto currentRows = db_result->getCurrentRows( );
		interfacePlugsType::HtmlDocString url;
		interfacePlugsType::HtmlDocString lastItem;
		interfacePlugsType::HtmlDocString compUrl;
		interfacePlugsType::HtmlDocString compLastItem;
		NovelDBJob::NovelInfoVector removeVectorINovelInfoS;
		interfacePlugsType::INovelInfo_Shared insterItem = nullptr;
		bool urlEqu = false;
		interList = src_vector;
		do {
			while( currentRows->size( ) > 0 ) {
				// 获取 url
				url = currentRows->at( 8 )->toString( ).toStdWString( );
				// 获取最后更新项
				compLastItem = currentRows->at( 9 )->toString( ).toStdWString( );
				// 遍历所有已经保存的小说
				auto iterator = interList.begin( );
				auto srcVectorEnd = interList.end( );
				for( ; iterator != srcVectorEnd; ++iterator )
					// 比较 url
					if( iterator->get( )->getNovelUrl( &compUrl ) && url == compUrl ) {
						urlEqu = true; // 找到匹配的 url
						// 比较更新项目，不相等则赋予更新标识
						if( iterator->get( )->getNovelLastItem( &lastItem ) && lastItem != compLastItem )
							insterItem = *iterator;
						break;
					}
				// 是否找到匹配的 url
				if( urlEqu ) {
					if( insterItem ) // url 相同，但是更新项不同，可以更新
						updateList.emplace_back( insterItem );
					interList.erase( iterator ); // 删除已经识别的项
					insterItem = nullptr; // 对象赋空
					urlEqu = false; // 重置标识位
				}
				break;
			}
			if( !db_result->nextCol( ) ) // 没有下一行，则跳出循环，结束调用
				break;
			currentRows = db_result->getCurrentRows( ); // 同步下一行
		} while( true );
	}
	/// <summary>
	/// 对字符串使用双引号 -> str => "str"
	/// </summary>
	/// <param name="updateMap">进化的列表</param>
	/// <returns>进化完成的对象列表</returns>
	inline QVariantMap conver_str_sql_text( const QVariantMap &updateMap ) {
		QVariantMap result;
		auto iterator = updateMap.begin( );
		auto end = updateMap.end( );
		QString form( R"("%1")" );
		for( ; iterator != end; ++iterator )
			result.insert( iterator.key( ), QVariant( form.arg( iterator.value( ).toString( ) ) ) );
		return result;

	}
	/// <summary>
	/// 对字符串使用双引号 -> str => "str"
	/// </summary>
	/// <param name="tabValues">进化的列表</param>
	/// <returns>进化完成的对象列表</returns>
	inline QStringList conver_str_sql_text( const QStringList &tabValues ) {
		QStringList result;
		auto iterator = tabValues.begin( );
		auto end = tabValues.end( );
		QString form( R"("%1")" );
		for( ; iterator != end; ++iterator )
			result << form.arg( *iterator );
		return result;
	}
	inline bool generate_db_tab( const cylDB::DB_Shared &dbInterface, const cylDB::Depository_Shared &depositoryShared, const QString &tab_name, OStream *thisOStream ) {
		auto supportType = dbInterface->getSupportType( );
		QString dbTextType = supportType.find( typeid( QString ).name( ) )->toString( );
		bool hasTab = false;
		if( dbTextType.isEmpty( ) )
			dbTextType = "TEXT";
		hasTab = depositoryShared->createTab( tab_name
			, { { "rootUrl", dbTextType }
				, { "novelName", dbTextType }
				, { "info", dbTextType }
				, { "updateTime", dbTextType }
				, { "format", dbTextType }
				, { "lastRequestTime", dbTextType }
				, { "lastRequestTimeFormat", dbTextType }
				, { "author", dbTextType }
				, { "url", dbTextType }
				, { "lastItem", dbTextType }
				, { "additionalData", dbTextType }
				, { "typePageUrl", dbTextType }
				, { "typeName", dbTextType }
			} );
		if( !hasTab )
			OStream::anyDebugOut( thisOStream, "无法创建正确的 db 文件", __FILE__, __LINE__, __FUNCTION__ );
		return hasTab;
	}
	/// <summary>
	/// 更新命令拼接
	/// </summary>
	/// <param name="cmd">起始拼接对象</param>
	/// <param name="append_map">拼接对象映射</param>
	/// <returns>拼接个数</returns>
	inline QString append_map_update( const QString &cmd, const QVariantMap &append_map ) {
		QString result;
		auto iterator = append_map.begin( );
		auto end = append_map.end( );
		if( iterator == end )
			return result;
		result = cmd;
		do {
			result.append( " `" );
			result.append( iterator.key( ) );
			result.append( "` =" );
			result.append( iterator.value( ).toString( ) );
			++iterator;
			if( iterator == end )
				break;
			result.append( ", " );
		} while( true );

		return result;
	}
	/// <summary>
	/// 输出错误信息到指定文件
	/// </summary>
	/// <param name="os">流，可选为nullptr</param>
	/// <param name="url">错误的 url - 节选 host 为路径名</param>
	/// <param name="root_path">根路径</param>
	/// <param name="dir_name">存储目录-中间目录，写入 路径宏（Cache_Path_Dir）跟后</param>
	/// <param name="error_type">错误的类型 - 节点路径名</param>
	/// <param name="error_novel_type_name">错误的小说类型名称 - 节选路径名</param>
	/// <param name="error_file_suffix">文件路径后缀</param>
	/// <param name="error_call_path_file_name">错误诞生的文件</param>
	/// <param name="error_file_call_function_name">错误诞生的描述调用函数</param>
	/// <param name="error_file_call_function_line">错误诞生的描述行号</param>
	/// <param name="error_info_text">错误信息</param>
	/// <param name="error_write_info_content">错误内容</param>
	/// <returns>成功写入文件返回 true</returns>
	inline bool write_error_info_file( OStream *os, const QUrl &url, const QString &root_path, const QString &dir_name, const QString &error_type, const QString &error_novel_type_name, const QString &error_file_suffix, const QString &error_call_path_file_name, const QString &error_file_call_function_name, const size_t error_file_call_function_line, const QString &error_info_text, const QString &error_write_info_content ) {
		QString currentTime = QDateTime::currentDateTime( ).toString( "yyyy_MM_dd hh mm ss" );

		QString msg;
		msg.append( "\n<!--" )
			.append( "\n=========================		try : info" )
			.append( u8"\n\t当前时间 : " ).append( currentTime ).append( "\n\t" )
			.append( u8"\n\t错误文件 : " ).append( error_call_path_file_name ).append( "\n\t" )
			.append( u8"\n\t信息位置 : " ).append( QString::number( error_file_call_function_line ) )
			.append( u8"\n\t信息函数 : " ).append( error_file_call_function_name )
			.append( "\n=========================		try : message" )
			.append( u8"\n\t类型 : " ).append( u8"请求页面" ).append( u8"(" ).append( url.toString( ) ).append( ")" )
			.append( "\n=========================		user : message" )
			.append( u8"\n\t自由信息 : " ).append( error_info_text )
			.append( "\n=========================" )
			.append( "\n-->" );
		OStream::anyDebugOut( os, msg );
		auto path = QString( root_path ).append( QDir::separator( ) )
										.append( "write_error_info_file" ).append( QDir::separator( ) )
										.append( dir_name ).append( QDir::separator( ) )
										.append( url.host( ) ).append( QDir::separator( ) )
										.append( error_type )
										.append( '-' )
										.append( currentTime )
										.append( '-' )
										.append( error_novel_type_name ).append( error_file_suffix );
		QFileInfo fileInfo( path );
		auto dir = fileInfo.dir( );
		if( !dir.exists( ) )
			if( !dir.mkpath( dir.absolutePath( ) ) )
				return false;
		QFile writeHtmlFile( fileInfo.absoluteFilePath( ) );
		if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
			msg.append( '\n' );
			msg.append( error_write_info_content );
			writeHtmlFile.write( msg.toUtf8( ) );
			writeHtmlFile.close( );
		}
		return true;
	}
}


NovelDBJob::NovelInfoVector NovelDBJob::sort( const NovelInfoVector &infos ) {
	std::list< interfacePlugsType::INovelInfo_Shared > result;
	interfacePlugsType::HtmlDocString timeLeft;
	interfacePlugsType::HtmlDocString timeRight;
	for( auto &novelSPtr : infos ) {
		if( novelSPtr.get( )->getNovelUpdateTime( &timeLeft ) ) {
			auto iterator = result.begin( ), end = result.end( );
			for( ; iterator != end; ++iterator )
				if( iterator->get( )->getNovelUpdateTime( &timeRight ) )
					if( timeLeft > timeRight )
						break;
			result.insert( iterator, novelSPtr );
		}
	}
	auto resultVector = NovelDBJob::NovelInfoVector( result.begin( ), result.end( ) );
	return resultVector;
}
NovelDBJob::NovelInfoVector NovelDBJob::identical( const NovelInfoVector &infos ) {
	std::list< interfacePlugsType::INovelInfo_Shared > result;
	interfacePlugsType::HtmlDocString urlLeft;
	interfacePlugsType::HtmlDocString urlRight;
	for( auto &novelSPtr : infos ) {
		if( novelSPtr.get( )->getNovelUrl( &urlLeft ) ) {
			auto iterator = result.begin( ), end = result.end( );
			for( ; iterator != end; ++iterator )
				if( iterator->get( )->getNovelUrl( &urlRight ) )
					if( urlLeft == urlRight )
						break;
			if( iterator == end )
				result.emplace_back( novelSPtr );
		}
	}
	auto resultVector = NovelDBJob::NovelInfoVector( result.begin( ), result.end( ) );
	return resultVector;
}
/// <summary>
/// 向列表查找匹配的类型列表
/// </summary>
/// <param name="vector">查找列表</param>
/// <param name="type_name">类型名称</param>
/// <returns>结果列表</returns>
inline NovelDBJob::NovelInfoVector_Shared getNovelTypeVector( NovelDBJob::NovelTypePairVector &vector, const NovelDBJob::NovelTypeNameType &type_name ) {
	auto iterator = vector.begin( );
	auto end = vector.end( );
	for( ; iterator != end; ++iterator )
		if( iterator->first == type_name )
			return iterator->second;
	NovelDBJob::NovelInfoVector_Shared result( std::make_shared< NovelDBJob::NovelInfoVector >( ) );
	vector.emplace_back( std::make_pair( type_name, result ) );
	return result;
}
/// <summary>
/// 获取映射当中的
/// </summary>
/// <param name="inster_map">映射</param>
/// <param name="host">host名称</param>
/// <returns>共享对象</returns>
inline NovelDBJob::NovelTypePairVector_Shared getNovelHostMap( NovelDBJob::NovelHostMap &inster_map, const NovelDBJob::NovelUrlHostType &host ) {
	auto iterator = inster_map.begin( );
	auto end = inster_map.end( );
	for( ; iterator != end; ++iterator )
		if( iterator->first == host )
			return iterator->second;
	NovelDBJob::NovelTypePairVector_Shared result( std::make_shared< NovelDBJob::NovelTypePairVector >( ) );
	inster_map.emplace( host, result );
	return result;
}
/// <summary>
/// 向映射插入一个小说对象
/// </summary>
/// <param name="inster_map">映射</param>
/// <param name="host">小说 host</param>
/// <param name="novel_info_shared">插入的小说</param>
inline void insterNovelHostMap( NovelDBJob::NovelHostMap &inster_map, const NovelDBJob::NovelUrlHostType &host, const interfacePlugsType::INovelInfo_Shared &novel_info_shared ) {
	interfacePlugsType::HtmlDocString typeNme;
	auto hostShared = getNovelHostMap( inster_map, host );
	novel_info_shared->getNovelTypeName( &typeNme );
	NovelDBJob::NovelUrlHostType novelTypeName = QString::fromStdWString( typeNme );
	auto novelTypeVector = getNovelTypeVector( *hostShared, novelTypeName );
	novelTypeVector->emplace_back( novel_info_shared );
}
/// <summary>
/// 向映射当中插入一个小说
/// </summary>
/// <param name="inster_map">映射</param>
/// <param name="novel_info_shared">小说</param>
inline void insterNovelHostMap( NovelDBJob::NovelHostMap &inster_map, const interfacePlugsType::INovelInfo_Shared &novel_info_shared ) {
	interfacePlugsType::HtmlDocString root;
	if( !novel_info_shared->getNovelUrl( &root ) )
		return;
	QUrl url( QString::fromStdWString( root ).trimmed( ) );
	insterNovelHostMap( inster_map, url.host( ), novel_info_shared );
}

NovelDBJob::NovelHostMap NovelDBJob::decompose( const NovelInfoVector &infos, const std::function< void( ) > &call_back ) {
	NovelDBJob::NovelHostMap result;
	cylHtmlTools::HtmlWorkThread thread;
	thread.setCurrentThreadRun( [&infos,&result]( ) {

		for( auto &inoveShared : infos )
			insterNovelHostMap( result, inoveShared );
	} );
	thread.start( );
	auto instance = qApp;
	while( thread.isRun( ) ) {
		instance->processEvents( );
		call_back( );
	}
	return result;
}
size_t NovelDBJob::writeDB( OStream *thisOStream, const QString &outPath, const QUrl &url, const NovelInfoVector &saveNovelInfos, const std::function< bool( const std::chrono::system_clock::time_point::duration & ) > &run ) {
	QString linkPath( u8"%1%2%3%2" );
	linkPath = linkPath.arg( outPath ).arg( QDir::separator( ) ).arg( "dbs" );
	if( !QDir( ).mkpath( linkPath ) )
		linkPath = outPath + QDir::separator( );
	auto dbInterface = cylDB::DBTools::linkDB( linkPath );
	size_t result = 0;
	if( dbInterface->link( ) ) {
		std::function< void( ) > currentThreadRun = [
				dbInterface, url, outPath, run, thisOStream,
				&saveNovelInfos, &result
			]( ) {
			QString dbName = url.host( );
			QString tabName = dbName;
			dbName.append( ".db" );
			auto depositoryShared = dbInterface->openDepository( dbName );
			if( depositoryShared ) {
				if( !depositoryShared->open( ) ) {
					auto lastError = depositoryShared->getLastError( );
					OStream::anyDebugOut( thisOStream, lastError.text( ), __FILE__, __LINE__, __FUNCTION__ );
					return;
				}
				bool hasTab = depositoryShared->hasTab( tabName );

				if( !hasTab )
					if( !instance_function::generate_db_tab( dbInterface, depositoryShared, tabName, thisOStream ) )
						return;

				auto allItem = depositoryShared->findItems( tabName, tabFieldNames );
				NovelInfoVector updateList; // 更新列表
				NovelInfoVector interList; // 插入列表
				// 分解-插入/更新 列表
				if( allItem ) {
					instance_function::separate_list( saveNovelInfos, allItem, updateList, interList );
				} else
					interList = saveNovelInfos; // 数据库不存在数据的时候，全部拷贝到插入列表
				result = interList.size( ) + saveNovelInfos.size( );
				QString requestTime = currentTime.toString( currentTimeForm );
				// 开始更新
				interfacePlugsType::HtmlDocString rootUrl = url.toString( ).toStdWString( ),
					novelName,
					novelInfo,
					novelUpdateTime,
					novelFormat,
					novelAuthor,
					novelUrl,
					novelLastItem,
					novelAdditionalData,
					novelTypePageUrl,
					novelTypeName;
				QString cmd = updateCmd.arg( tabName );
				bool transaction = depositoryShared->transaction( );
				std::shared_ptr< QSqlQuery > sqlQuery = depositoryShared->generateSqlQuery( );
				sqlQuery.get( )->prepare( cmd );
				for( auto &novel : updateList ) {
					novel->getNovelUrl( &novelUrl );
					novel->getNovelUpdateTime( &novelUpdateTime );
					novel->getNovelUpdateTimeFormat( &novelFormat );
					void *ptr = &novelAuthor;
					novel->getNovelAttach( ptr );
					sqlQuery->bindValue( ":updateTime", QString::fromStdWString( novelUpdateTime ) );
					sqlQuery->bindValue( ":format", QString::fromStdWString( novelFormat ) );
					sqlQuery->bindValue( ":lastRequestTime", requestTime );
					sqlQuery->bindValue( ":lastItem", QString::fromStdWString( novelLastItem ) );
					sqlQuery->bindValue( ":additionalData", QString::fromStdWString( novelAuthor ) );
					sqlQuery->bindValue( ":url", QString::fromStdWString( novelUrl ) );
					if( !depositoryShared->exec( sqlQuery.get( ) ) ) {
						instance_function::write_error_info_file( thisOStream, QUrl( " " ), outPath, "db_updateList", "db_error", "update", "db.log", __FILE__, __FUNCTION__, __LINE__, "无法更新正确的小说内容", "无法更新正确的小说内容" );
						--result;
					}
				}
				if( transaction )
					depositoryShared->commit( );
				transaction = depositoryShared->transaction( );
				auto rootQStringUrl = QString::fromStdWString( rootUrl );
				cmd = insterCmd.arg( tabName );
				sqlQuery.get( )->prepare( cmd );
				QStringList tabValues;
				sqlQuery->bindValue( ":rootUrl", rootQStringUrl );
				for( auto &novel : interList ) {
					novel->getNovelName( &novelName );
					novel->getNovelInfo( &novelInfo );
					novel->getNovelUpdateTime( &novelUpdateTime );
					novel->getNovelUpdateTimeFormat( &novelFormat );
					novel->getNovelAuthor( &novelAuthor );
					novel->getNovelUrl( &novelUrl );
					novel->getNovelLastItem( &novelLastItem );
					void *ptr = &novelAuthor;
					novel->getNovelAttach( ptr );
					novel->getNovelUrlAtPageLocation( &novelTypePageUrl );
					novel->getNovelTypeName( &novelTypeName );

					sqlQuery->bindValue( ":novelName", QString::fromStdWString( novelName ) );
					sqlQuery->bindValue( ":info", QString::fromStdWString( novelInfo ) );
					sqlQuery->bindValue( ":updateTime", QString::fromStdWString( novelUpdateTime ) );
					sqlQuery->bindValue( ":format", QString::fromStdWString( novelFormat ) );
					sqlQuery->bindValue( ":lastRequestTime", requestTime );
					sqlQuery->bindValue( ":lastRequestTimeFormat", currentTimeForm );
					sqlQuery->bindValue( ":author", QString::fromStdWString( novelAuthor ) );
					sqlQuery->bindValue( ":url", QString::fromStdWString( novelUrl ) );
					sqlQuery->bindValue( ":lastItem", QString::fromStdWString( novelLastItem ) );
					sqlQuery->bindValue( ":additionalData", QString::fromStdWString( novelAdditionalData ) );
					sqlQuery->bindValue( ":typePageUrl", QString::fromStdWString( novelTypePageUrl ) );
					sqlQuery->bindValue( ":typeName", QString::fromStdWString( novelTypeName ) );
					if( !depositoryShared->exec( sqlQuery.get( ) ) ) {
						instance_function::write_error_info_file( thisOStream, QUrl( " " ), outPath, "db_interList", "db_error", "inster", "db.log", __FILE__, __FUNCTION__, __LINE__, "无法插入正确的小说内容", "无法插入正确的小说内容" );
						--result;
					}
				}

				if( transaction )
					depositoryShared->commit( );
				sqlQuery.reset( );
				depositoryShared->close( );
			}

		};
		cylHtmlTools::HtmlWorkThread thread( nullptr, currentThreadRun, nullptr );
		thread.start( );
		auto currentTime = std::chrono::system_clock::now( ).time_since_epoch( );
		auto instance = qApp;
		while( thread.isRun( ) ) {
			auto epoch = std::chrono::system_clock::now( ).time_since_epoch( );
			auto duration = currentTime - epoch;
			run( duration );
			currentTime = epoch;
			instance->processEvents( );
		}
	}

	return result;
}

inline NovelDBJob::NovelInfoVector converNovelBaseVector( cylDB::IResultInfo_Shared &allItem ) {
	NovelDBJob::NovelInfoVector vectorINovelInfoSPtr;
	allItem->resetColIndex( );
	auto currentRows = allItem->getCurrentRows( );
	while( currentRows->size( ) > 0 ) {
		// QStringList NovelDBJob::tabFieldNames = { 0"rootUrl", 1"novelName", 2"info", 3"updateTime", 4"format", 5"lastRequestTime", 6"lastRequestTimeFormat", 7"author", 8"url", 9"lastItem", 10"additionalData", 11"typePageUrl", 12"typeName" };
		NovelBase *novelBase = new NovelBase;
		novelBase->rootUrl = currentRows->at( 0 )->toString( ).toStdWString( );
		novelBase->novelName = currentRows->at( 1 )->toString( ).toStdWString( );
		novelBase->info = currentRows->at( 2 )->toString( ).toStdWString( );
		novelBase->updateTime = currentRows->at( 3 )->toString( ).toStdWString( );
		novelBase->lastRequestTime = currentRows->at( 5 )->toString( ).toStdWString( );
		novelBase->author = currentRows->at( 7 )->toString( ).toStdWString( );
		novelBase->url = currentRows->at( 8 )->toString( ).toStdWString( );
		novelBase->lastItem = currentRows->at( 9 )->toString( ).toStdWString( );
		novelBase->typePageUrl = currentRows->at( 11 )->toString( ).toStdWString( );
		novelBase->typeName = currentRows->at( 12 )->toString( ).toStdWString( );
		vectorINovelInfoSPtr.emplace_back( std::shared_ptr< interfacePlugsType::INovelInfo >( novelBase ) );
		if( !allItem->nextCol( ) )
			break;
		currentRows = allItem->getCurrentRows( );
	}
	return vectorINovelInfoSPtr;
}

NovelDBJob::NovelInfoVector_Shared NovelDBJob::readDB( OStream *thisOStream, const QString &outPath, const std::function< void( ) > &run ) {
	NovelInfoVector_Shared result( std::make_shared< NovelInfoVector >( ) );
	std::vector< QString > dbNames; // 存储所有db
	QString linkPath; // 存储根目录
	// 判定文件夹还是文件
	QFileInfo path( outPath );
	if( path.isDir( ) ) { // 文件夹
		linkPath = path.absoluteFilePath( ) + QDir::separator( );;
		QDir dir( linkPath );
		QFileInfoList entryInfoList = dir.entryInfoList( QDir::Files );
		for( auto &fileInfo : entryInfoList )
			if( fileInfo.suffix( ) == "db" )
				dbNames.emplace_back( fileInfo.fileName( ) );
	} else { // 文件
		linkPath = path.dir( ).absolutePath( ) + QDir::separator( );;
		dbNames.emplace_back( path.fileName( ) );
	}

	std::mutex *mutex = new std::mutex; // 返回列表对象操作锁

	std::vector< cylHtmlTools::HtmlWorkThread * > threaVector; // 线程池

	for( auto &dbFile : dbNames ) {
		std::function< void( ) > currentThreadRun = [
				outPath, run, thisOStream,mutex,linkPath,
				&result,dbFile
			]( ) {
			auto dbInterface = cylDB::DBTools::linkDB( linkPath );
			if( !dbInterface->link( ) )
				return;
			auto depositoryShared = dbInterface->openDepository( dbFile );
			if( !depositoryShared )
				return;
			QString tabName = dbFile.mid( 0, dbFile.length( ) - 3 );
			if( depositoryShared ) {
				if( !depositoryShared->open( ) ) {
					auto lastError = depositoryShared->getLastError( );
					OStream::anyDebugOut( thisOStream, lastError.text( ), __FILE__, __LINE__, __FUNCTION__ );
					return;
				}
				bool hasTab = depositoryShared->hasTab( tabName );
				if( !hasTab )
					return;
				auto allItem = depositoryShared->findItems( tabName, tabFieldNames );
				if( !allItem )
					return;
				auto vectorINovelInfoSPtr = converNovelBaseVector( allItem );
				mutex->lock( );
				for( auto &novel : vectorINovelInfoSPtr )
					result->emplace_back( novel );
				mutex->unlock( );
			}

		};
		auto thread = new cylHtmlTools::HtmlWorkThread( );
		thread->setCurrentThreadRun( currentThreadRun );
		threaVector.emplace_back( thread );
		thread->start( );
	}
	do {
		cylHtmlTools::HtmlWorkThread *deletePtr = nullptr;
		auto iterator = threaVector.begin( );
		auto end = threaVector.end( );
		if( iterator == end ) // 没有元素
			break;
		for( ; iterator != end; ++iterator ) {
			deletePtr = *iterator;
			auto instance = qApp;
			if( deletePtr->isRun( ) ) {
				run( );
				instance->processEvents( );
			} else
				break;
		}
		if( iterator != end ) {
			delete deletePtr;
			threaVector.erase( iterator );
		}
	} while( true );
	delete mutex;

	if( result->size( ) )
		return result;
	return nullptr;
}

NovelDBJob::NovelInfoVector NovelDBJob::writeFile( const QString &writeFilePath, const NovelInfoVector &infos ) {
	NovelInfoVector result = NovelDBJob::identical( infos );
	cylHtmlTools::HtmlWorkThread thread;
	result = NovelDBJob::sort( result );
	IOFile ioFile( writeFilePath, result );
	ioFile.writeNoveInfoListToFile( );
	return result;
}
NovelDBJob::NovelInfoVector NovelDBJob::writeFile( const QString &root_path, const QString &novel_host, const NovelInfoVector &infos ) {
	NovelInfoVector result = NovelDBJob::identical( infos );
	QString writeFilePath( u8"%1%2%3" );
	writeFilePath = writeFilePath.arg( root_path ).arg( QDir::separator( ) ).arg( novel_host );
	return writeFile( writeFilePath, infos );
}
NovelDBJob::NovelInfoVector NovelDBJob::writeFile( const QString &root_path, const QString &novel_host, const QString &novel_type, const NovelInfoVector &infos ) {
	NovelInfoVector result = NovelDBJob::identical( infos );
	QString writeFilePath( u8"%1%2%3%2%4.txt" );
	writeFilePath = writeFilePath.arg( root_path ).arg( QDir::separator( ) ).arg( novel_host ).arg( novel_type );
	return writeFile( writeFilePath, infos );
}

NovelDBJob::NovelInfoVector NovelDBJob::removeSubName( const NovelInfoVector &infos, const std::vector< interfacePlugsType::HtmlDocString > &remove_name_s ) {
	interfacePlugsType::HtmlDocString name;
	NovelDBJob::NovelInfoVector result;
	auto iterator = remove_name_s.begin( );
	auto iteratorEnd = remove_name_s.end( );
	for( auto &info : infos )
		if( info->getNovelName( &name ) ) {
			size_t length = name.length( );
			auto cloneIterator = iterator;
			do {
				if( name.find( *cloneIterator ) < length )
					break;
				++cloneIterator;
			} while( cloneIterator != iteratorEnd );
			if( cloneIterator == iteratorEnd )
				result.emplace_back( info );
		}
	return result;
}


/// <summary>
/// 检测任务
/// </summary>
/// <param name="threads">检测队列</param>
/// <param name="call_function">工作时，会调用函数</param>
/// <param name="max_size">最大允许任务</param>
inline void checkThreadsWork( std::vector< cylHtmlTools::HtmlWorkThread * > &threads, const std::function< void( ) > &call_function, size_t max_size = 0 ) {
	// 最大限制不能等于0，否则跳出函数
	if( max_size < threads.size( ) )
		do {
			auto iterator = threads.begin( );
			auto end = threads.end( );
			if( max_size > threads.size( ) ) // 剩余个数不足 max_size，则跳出
				break;
			do {
				cylHtmlTools::HtmlWorkThread *thread = iterator.operator*( );
				call_function( );
				if( thread->isFinish( ) ) {
					threads.erase( iterator );
					delete thread;
					break;
				}
				++iterator;
			} while( iterator != end );
		} while( true );
}
/// <summary>
/// 完成任务
/// </summary>
/// <param name="threads">检测队列</param>
/// <param name="call_function">工作时，会调用函数</param>
inline void overThreadsWork( std::vector< cylHtmlTools::HtmlWorkThread * > &threads, const std::function< void( ) > &call_function ) {
	// 最大限制不能等于0，否则跳出函数

	do {
		auto iterator = threads.begin( );
		auto end = threads.end( );
		if( iterator == end )
			return;
		do {
			cylHtmlTools::HtmlWorkThread *thread = iterator.operator*( );
			call_function( );
			if( thread->isFinish( ) ) {
				threads.erase( iterator );
				delete thread;
				break;
			}
			++iterator;
		} while( iterator != end );
	} while( true );
}
NovelDBJob::NovelInfoVector NovelDBJob::removeSubName( const NovelInfoVector &infos, const std::unordered_map< size_t, std::shared_ptr< std::vector< interfacePlugsType::HtmlDocString > > > &remove_name_s, const std::function< void( ) > &call_function ) {
	std::mutex *writeMutex = new std::mutex; // 数组写入锁
	NovelDBJob::NovelInfoVector result;
	std::vector< cylHtmlTools::HtmlWorkThread * > threads;
	std::vector< size_t > mapLenKeyS; // 存储所有的 key
	for( auto it = remove_name_s.begin( ), en = remove_name_s.end( ); it != en; ++it )
		mapLenKeyS.emplace_back( it->first );
	std::sort( mapLenKeyS.begin( ), mapLenKeyS.end( ) );

	for( auto &node : infos ) {
		cylHtmlTools::HtmlWorkThread *thread = new cylHtmlTools::HtmlWorkThread;
		thread->setCurrentThreadRun( [
				node,writeMutex
				,&mapLenKeyS,&remove_name_s,&result
			]( ) ->void {
				// todo : ......
				interfacePlugsType::HtmlDocString name;
				if( !node->getNovelName( &name ) )
					return;
				size_t length = name.length( );
				for( auto &key : mapLenKeyS ) {
					if( length < key )
						break;
					auto &&sharedPtr = remove_name_s.at( key );
					for( auto &str : *sharedPtr )
						if( cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &name, 0, &str ) )
							return; // 小说当属匹配当前子字符串
				}
				writeMutex->lock( );
				result.emplace_back( node );
				writeMutex->unlock( );
			} );
		thread->start( );
		threads.emplace_back( thread );
		call_function( );
		checkThreadsWork( threads, call_function, 8 );
	}
	overThreadsWork( threads, call_function );
	return result;
}
