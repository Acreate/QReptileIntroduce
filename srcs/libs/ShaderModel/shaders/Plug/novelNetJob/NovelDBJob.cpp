﻿#include "NovelDBJob.h"

#include <QString>
#include <QDir>
#include <QSqlQuery>
#include <htmls/htmlTools/HtmlWorkThread/HtmlWorkThread.h>
#include <htmls/htmlTools/HtmlWorkThread/HtmlWorkThreadPool.h>
#include <nameSpace/cylDB.h>
#include <DB/DBTools.h>
#include <DB/dbInterface/I_DB.h>
#include <DB/dbInterface/i_depositoryInterface/I_Depository.h>
#include <DB/dbInterface/i_resultInfo/I_ResultInfo.h>
#include <stream/OStream.h>
#include <interface/INovelInfo.h>
#include <qguiapplication.h>
#include <htmlString/HtmlStringTools.h>
#include <iostream>
#include <QUrl>
#include <mutex>
#include "../ioFile/IOFile.h"
#include "htmlString/HtmlStringTools.h"
#include "interface/instance_function.h"

QString NovelDBJob::currentTimeForm = QObject::tr( u8R"(yyyy-MM-dd hh:mm:ss)" );
QDateTime NovelDBJob::currentTime = QDateTime::currentDateTime( );
QStringList NovelDBJob::tabFieldNames = { "rootUrl", "novelName", "info", "updateTime", "format", "lastRequestTime", "lastRequestTimeFormat", "author", "url", "lastItem", "additionalData", "typePageUrl", "typeName" };
QString NovelDBJob::updateCmd = R"(UPDATE `%1` SET `updateTime`=:updateTime, `lastRequestTime`=:lastRequestTime, `additionalData`=:additionalData, `lastItem`=:lastItem , `format`=:format  WHERE `url`=:url;)";
QString NovelDBJob::insterCmd = R"(INSERT INTO `%1`( `rootUrl`, `novelName`, `info`, `updateTime`, `format`, `lastRequestTime`, `lastRequestTimeFormat`, `author`, `url`, `lastItem`, `additionalData`, `typePageUrl`, `typeName`  ) VALUES ( :rootUrl,:novelName,:info,:updateTime,:format,:lastRequestTime,:lastRequestTimeFormat,:author,:url,:lastItem,:additionalData,:typePageUrl,:typeName  );)";
QString NovelDBJob::deleteCmd = R"(DELETE FROM `%1` WHERE `url`=:url;)";
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
		hasTab = depositoryShared->createTab( tab_name,
		{ { "rootUrl", dbTextType },
				{ "novelName", dbTextType },
				{ "info", dbTextType },
				{ "updateTime", dbTextType },
				{ "format", dbTextType },
				{ "lastRequestTime", dbTextType },
				{ "lastRequestTimeFormat", dbTextType },
				{ "author", dbTextType },
				{ "url", dbTextType },
				{ "lastItem", dbTextType },
				{ "additionalData", dbTextType },
				{ "typePageUrl", dbTextType },
				{ "typeName", dbTextType }
		} );
		if( !hasTab )
			OStream::anyStdCerr( "无法创建正确的 db 文件", instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __LINE__, __FUNCTION__, thisOStream );
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
	/// 输出错误信息到指定流
	/// </summary>
	/// <param name="os">流，可选为nullptr</param>
	/// <param name="error_call_path_file_name">错误诞生的文件</param>
	/// <param name="error_file_call_function_name">错误诞生的描述调用函数</param>
	/// <param name="error_file_call_function_line">错误诞生的描述行号</param>
	/// <param name="error_info_text">错误信息</param>
	/// <returns>成功写入文件返回 true</returns>
	inline bool write_error_info_ostream_error( OStream *os, const QString &error_call_path_file_name, const QString &error_file_call_function_name, const size_t error_file_call_function_line, const QString &error_info_text ) {
		QDateTime currentDateTime = QDateTime::currentDateTime( );
		QString day = currentDateTime.toString( "yyyy_MM_dd" );
		QString time = currentDateTime.toString( "hh mm ss" );
		QString msg;
		msg.append( "\n<!--" )
			.append( "\n=========================		try : info" )
			.append( u8"\n\t当前时间 : " ).append( day + " " + time ).append( "\n\t" )
			.append( u8"\n\t错误文件 : " ).append( instance_function::getCmakeRootPathBuilderFilePath( error_call_path_file_name ) ).append( "\n\t" )
			.append( u8"\n\t信息位置 : " ).append( QString::number( error_file_call_function_line ) )
			.append( u8"\n\t信息函数 : " ).append( error_file_call_function_name )
			.append( "\n=========================		user : message" )
			.append( u8"\n\t自由信息 : " ).append( error_info_text )
			.append( "\n=========================" )
			.append( "\n-->" );
		OStream::anyStdCerr( msg, os );
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
	thread.setCurrentThreadRun( [&infos,&result]( cylHtmlTools::HtmlWorkThread * ) {

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
size_t NovelDBJob::writeDB( OStream *thisOStream, const QString &outPath, const NovelInfoVector &saveNovelInfos, const std::function< bool( const std::chrono::system_clock::time_point::duration & ) > &run ) {
	QFileInfo fileInfo( outPath );
	QDir absoluteDir = fileInfo.absoluteDir( );
	auto absolutePath = absoluteDir.absolutePath( );
	if( !absoluteDir.exists( ) )
		if( !QDir( ).mkpath( absolutePath ) )
			return 0; // 无法写入
	auto dbName = fileInfo.fileName( );
	qsizetype lastIndexOf = dbName.lastIndexOf( '.' );
	QString tabName;
	if( lastIndexOf != -1 )
		tabName = dbName.mid( 0, lastIndexOf );
	else
		tabName = dbName;
	auto dbInterface = cylDB::DBTools::linkDB( absolutePath );
	size_t result = 0;
	if( dbInterface->link( ) ) {
		auto depositoryShared = dbInterface->openDepository( dbName );
		while( depositoryShared ) {
			if( !depositoryShared->open( ) ) {
				auto lastError = depositoryShared->getLastError( );
				OStream::anyStdCerr( lastError.text( ), instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __LINE__, __FUNCTION__, thisOStream );
				break;
			}
			bool hasTab = depositoryShared->hasTab( tabName );

			if( !hasTab )
				if( !instance_function::generate_db_tab( dbInterface, depositoryShared, tabName, thisOStream ) )
					break;

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

			QUrl qUrl( QString::fromStdWString( saveNovelInfos.at( 0 )->rootUrl ) );
			auto rootQStringUrl = qUrl.scheme( ) + "://" + qUrl.host( );
			auto rootUrl = rootQStringUrl.toStdWString( );
			auto outLogPath = outPath + QDir::separator( ) + u8"logs" + QDir::separator( );
			QString cmd = updateCmd.arg( tabName );
			bool transaction = depositoryShared->transaction( );
			std::shared_ptr< QSqlQuery > sqlQuery = depositoryShared->generateSqlQuery( );
			sqlQuery.get( )->prepare( cmd );
			for( auto &novel : updateList ) {
				sqlQuery->bindValue( ":updateTime", QString::fromStdWString( novel->updateTime ) );
				sqlQuery->bindValue( ":format", QString::fromStdWString( novel->format ) );
				sqlQuery->bindValue( ":lastRequestTime", requestTime );
				sqlQuery->bindValue( ":lastItem", QString::fromStdWString( novel->lastItem ) );
				sqlQuery->bindValue( ":additionalData", QString::fromStdWString( novel->additionalData ) );
				sqlQuery->bindValue( ":url", QString::fromStdWString( novel->url ) );
				if( !depositoryShared->exec( sqlQuery.get( ) ) ) {
					instance_function::write_error_info_file( thisOStream, QUrl( " " ), outLogPath, "db_updateList", "db_error", "update", "db.log", instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__, "无法更新正确的小说内容", "无法更新正确的小说内容" );
					--result;
				}
			}
			if( transaction )
				depositoryShared->commit( );
			transaction = depositoryShared->transaction( );
			cmd = insterCmd.arg( tabName );
			sqlQuery.get( )->prepare( cmd );
			QStringList tabValues;
			sqlQuery->bindValue( ":rootUrl", rootQStringUrl );
			for( auto &novel : interList ) {
				sqlQuery->bindValue( ":novelName", QString::fromStdWString( novel->novelName ) );
				sqlQuery->bindValue( ":info", QString::fromStdWString( novel->info ) );
				sqlQuery->bindValue( ":updateTime", QString::fromStdWString( novel->updateTime ) );
				sqlQuery->bindValue( ":format", QString::fromStdWString( novel->format ) );
				sqlQuery->bindValue( ":lastRequestTime", requestTime );
				sqlQuery->bindValue( ":lastRequestTimeFormat", currentTimeForm );
				sqlQuery->bindValue( ":author", QString::fromStdWString( novel->author ) );
				sqlQuery->bindValue( ":url", QString::fromStdWString( novel->url ) );
				sqlQuery->bindValue( ":lastItem", QString::fromStdWString( novel->lastItem ) );
				sqlQuery->bindValue( ":additionalData", QString::fromStdWString( novel->additionalData ) );
				sqlQuery->bindValue( ":typePageUrl", QString::fromStdWString( novel->typePageUrl ) );
				sqlQuery->bindValue( ":typeName", QString::fromStdWString( novel->typeName ) );
				if( !depositoryShared->exec( sqlQuery.get( ) ) ) {
					instance_function::write_error_info_file( thisOStream, QUrl( " " ), outLogPath, "db_interList", "db_error", "inster", "db.log", instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__, "无法插入正确的小说内容", "无法插入正确的小说内容" );
					--result;
				}
			}

			if( transaction )
				depositoryShared->commit( );
			sqlQuery.reset( );
			depositoryShared->close( );
			break;
		}
	}
	return result;
}

inline NovelDBJob::NovelInfoVector converNovelBaseVector( cylDB::IResultInfo_Shared &allItem ) {
	NovelDBJob::NovelInfoVector vectorINovelInfoSPtr;
	allItem->resetColIndex( );
	auto currentRows = allItem->getCurrentRows( );
	while( currentRows->size( ) > 0 ) {
		auto novelBase = std::make_shared< interfacePlugsType::INovelInfo >( );
		novelBase->setRootUrl( currentRows->at( 0 )->toString( ).toStdWString( ) ); // rootUrl
		novelBase->setNovelName( currentRows->at( 1 )->toString( ).toStdWString( ) ); // novelName
		novelBase->setNovelInfo( currentRows->at( 2 )->toString( ).toStdWString( ) ); // info
		novelBase->setNovelUpdateTime( currentRows->at( 3 )->toString( ).toStdWString( ) ); // updateTime
		novelBase->setNovelUpdateTimeFormat( currentRows->at( 4 )->toString( ).toStdWString( ) ); // format
		novelBase->setNovelLastRequestGetTime( currentRows->at( 5 )->toString( ).toStdWString( ) ); // lastRequestTime
		novelBase->setNovelAuthor( currentRows->at( 7 )->toString( ).toStdWString( ) ); // author
		novelBase->setNovelUrl( currentRows->at( 8 )->toString( ).toStdWString( ) );  // url
		novelBase->setNovelLastItem( currentRows->at( 9 )->toString( ).toStdWString( ) );  // lastItem
		novelBase->setNovelUrlAtPageLocation( currentRows->at( 11 )->toString( ).toStdWString( ) );  // typePageUrl
		novelBase->setNovelTypeName( currentRows->at( 12 )->toString( ).toStdWString( ) );  // typeName
		vectorINovelInfoSPtr.emplace_back( novelBase );
		if( !allItem->nextCol( ) )
			break;
		currentRows = allItem->getCurrentRows( );
	}
	return vectorINovelInfoSPtr;
}

NovelDBJob::NovelInfoVector_Shared NovelDBJob::readDB( OStream *thisOStream, const QString &db_link, const QString &db_name ) {
	NovelInfoVector_Shared result( std::make_shared< NovelInfoVector >( ) );
	auto dbInterface = cylDB::DBTools::linkDB( db_link );
	if( dbInterface->link( ) ) {
		auto depositoryShared = dbInterface->openDepository( db_name );
		if( depositoryShared ) {
			QString tabName = db_name.mid( 0, db_name.length( ) - 3 );
			if( depositoryShared->open( ) ) {
				bool hasTab = depositoryShared->hasTab( tabName );
				if( hasTab ) {
					auto allItem = depositoryShared->findItems( tabName, tabFieldNames );
					if( allItem ) {
						auto vectorINovelInfoSPtr = converNovelBaseVector( allItem );
						vectorINovelInfoSPtr = NovelDBJob::identical( vectorINovelInfoSPtr ); // 剔除相同
						for( auto &novel : vectorINovelInfoSPtr )
							result->emplace_back( novel );
					}
				}
			} else {
				auto lastError = depositoryShared->getLastError( );
				OStream::anyStdCerr( lastError.text( ), instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __LINE__, __FUNCTION__, thisOStream );
			}

		}
	}
	if( result->size( ) )
		return result;
	return nullptr;
}

bool NovelDBJob::novelIsExpire( size_t expire_day, const interfacePlugsType::INovelInfoPtr &novel_info_ptr ) {
	expire_day = expire_day * 24 * 60 * 60 * 1000; // 获取天数到分钟的时间

	QString updateTime = QString::fromStdWString( novel_info_ptr->lastRequestTime );
	QDateTime dateTime = QDateTime::fromString( updateTime, currentTimeForm );
	auto milliseconds = currentTime - dateTime;
	if( milliseconds.count( ) > expire_day )
		return true;
	updateTime = QString::fromStdWString( novel_info_ptr->updateTime );
	QString updateTimeFormat = QString::fromStdWString( novel_info_ptr->format );
	dateTime = QDateTime::fromString( updateTime, updateTimeFormat );
	milliseconds = currentTime - dateTime;
	if( milliseconds.count( ) > expire_day )
		return true;
	return false;
}
interfacePlugsType::Vector_INovelInfoSPtr NovelDBJob::novelVectorIsExpire( const size_t &expire_day, const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_ptr ) {
	interfacePlugsType::Vector_INovelInfoSPtr result;
	cylHtmlTools::HtmlWorkThreadPool threadPool;
	for( auto &novel : novel_info_ptr )
		if( novelIsExpire( expire_day, novel.get( ) ) )
			result.emplace_back( novel );
	return result;
}
std::vector< interfacePlugsType::HtmlDocString > NovelDBJob::novelVectorGetNovleUrl( interfacePlugsType::Vector_INovelInfoSPtr &novel_info_ptr ) {
	std::vector< interfacePlugsType::HtmlDocString > result;
	for( auto &novel : novel_info_ptr )
		result.emplace_back( novel->url );
	return result;
}
interfacePlugsType::Vector_INovelInfoSPtr NovelDBJob::formVectorINovelInfoRemoveVectorINovelInfo( const interfacePlugsType::Vector_INovelInfoSPtr &src, const interfacePlugsType::Vector_INovelInfoSPtr &des ) {
	interfacePlugsType::Vector_INovelInfoSPtr result;

	auto srcIterator = src.begin( );
	auto srcEnd = src.end( );
	if( srcIterator == srcEnd )
		return result;

	auto desIterator = des.begin( );
	auto desEnd = des.end( );
	if( srcIterator == srcEnd )
		return src;
	interfacePlugsType::HtmlDocString desUrl;
	interfacePlugsType::HtmlDocString srcUrl;
	do {
		srcIterator->get( )->getNovelUrl( &srcUrl );
		do {
			desIterator->get( )->getNovelUrl( &desUrl );
			if( cylHtmlTools::HtmlStringTools::equRemoveSpaceOverHtmlString( srcUrl, desUrl ) )
				break;
			++desIterator;
		} while( desIterator != desEnd );
		if( desIterator == desEnd )
			result.emplace_back( *srcIterator );
		++srcIterator;
		desIterator = des.begin( );
		desEnd = des.end( );
	} while( srcIterator != srcEnd );

	return result;
}

NovelDBJob::NovelInfoVector_Shared NovelDBJob::removeExpireDB( OStream *thisOStream, const QString &db_link, const QString &db_name, const size_t &expire_day ) {
	NovelInfoVector_Shared result( std::make_shared< NovelInfoVector >( ) );
	auto dbInterface = cylDB::DBTools::linkDB( db_link );
	if( dbInterface->link( ) ) {
		auto depositoryShared = dbInterface->openDepository( db_name );
		if( depositoryShared ) {
			QString tabName = db_name.mid( 0, db_name.length( ) - 3 );
			if( depositoryShared ) {
				if( depositoryShared->open( ) ) {
					bool hasTab = depositoryShared->hasTab( tabName );
					if( hasTab ) {
						QString removeUrlWhere( " `url` = \"%1\"" );

						auto allItem = depositoryShared->findItems( tabName, tabFieldNames );
						if( allItem ) {
							bool transaction = depositoryShared->transaction( );
							auto vectorINovelInfoSPtr = converNovelBaseVector( allItem );
							interfacePlugsType::HtmlDocString novelUrl;
							for( auto &novel : vectorINovelInfoSPtr )
								if( novelIsExpire( expire_day, novel.get( ) ) ) {
									result->emplace_back( novel );
									novel->getNovelUrl( &novelUrl );
									depositoryShared->removeItem( tabName, removeUrlWhere.arg( QString::fromStdWString( novelUrl ) ) );
								}
							if( transaction )
								depositoryShared->commit( );
						}
					}
				} else {
					auto lastError = depositoryShared->getLastError( );
					OStream::anyStdCerr( lastError.text( ), instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __LINE__, __FUNCTION__, thisOStream );
				}

			}
		}

	}
	if( result->size( ) )  // 开始删除列表
		return result;
	return nullptr;
}
void NovelDBJob::removeNovelVectorDB( OStream *thisOStream, const QString &db_link, const QString &db_name, const std::vector< interfacePlugsType::HtmlDocString > &novel_url_vector, const QString &outPath ) {
	auto dbInterface = cylDB::DBTools::linkDB( db_link );
	if( dbInterface->link( ) ) {
		auto depositoryShared = dbInterface->openDepository( db_name );
		if( depositoryShared ) {
			QString outLogPath;
			bool logIsWrite = !outPath.isEmpty( );
			if( logIsWrite )
				outLogPath = outPath + QDir::separator( ) + u8"logs" + QDir::separator( );
			QString tabName = db_name.mid( 0, db_name.length( ) - 3 );
			if( depositoryShared ) {
				if( depositoryShared->open( ) ) {
					bool hasTab = depositoryShared->hasTab( tabName );
					if( hasTab ) {
						bool transaction = depositoryShared->transaction( );
						auto sqlQuery = depositoryShared->generateSqlQuery( );
						QSqlQuery *query = sqlQuery.get( );
						query->prepare( deleteCmd.arg( tabName ) );
						for( auto &novelUrl : novel_url_vector ) {
							query->bindValue( ":url", QString::fromStdWString( novelUrl ) );
							if( !depositoryShared->exec( query ) )
								if( logIsWrite ) // 写入
									instance_function::write_error_info_file( thisOStream, QUrl( " " ), outLogPath, "db_remove_list", "db_error", "remove", "db.log", instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__, "无法删除指定项目", "无法删除指定项目" );
								else // 纯输出
									instance_function::write_error_info_ostream_error( nullptr, instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__, "无法删除指定项目" );
						}
						if( transaction )
							depositoryShared->commit( );
					}
				}
			} else {
				auto lastError = depositoryShared->getLastError( );
				OStream::anyStdCerr( lastError.text( ), instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __LINE__, __FUNCTION__, thisOStream );
			}

		}
	}

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


NovelDBJob::NovelInfoVector NovelDBJob::removeSubName( const NovelInfoVector &infos, const std::unordered_map< size_t, std::shared_ptr< std::vector< interfacePlugsType::HtmlDocString > > > &remove_name_s ) {

	NovelDBJob::NovelInfoVector result;
	std::vector< size_t > mapLenKeyS; // 存储所有的 key
	for( auto it = remove_name_s.begin( ), en = remove_name_s.end( ); it != en; ++it )
		mapLenKeyS.emplace_back( it->first );
	std::sort( mapLenKeyS.begin( ), mapLenKeyS.end( ) );
	for( auto node : infos ) {
		interfacePlugsType::HtmlDocString name;
		if( !node->getNovelName( &name ) )
			continue;
		cylHtmlTools::HtmlStringTools::removeAllSpace( name );
		name = NovelDBJob::converStringToUpper( name );
		size_t length = name.length( );
		for( auto &key : mapLenKeyS ) {
			if( length < key )
				break;
			auto &&sharedPtr = remove_name_s.at( key );
			for( auto &str : *sharedPtr )
				if( cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &name, &str ) ) {
					node = nullptr;
					break;
				}
			if( node == nullptr )
				break;
		}
		if( node )
			result.emplace_back( node );
	}
	return result;
}
NovelDBJob::NovelInfoVector NovelDBJob::removeEquName( const NovelInfoVector &infos, const std::unordered_map< size_t, std::shared_ptr< std::vector< interfacePlugsType::HtmlDocString > > > &remove_name_s ) {
	NovelDBJob::NovelInfoVector result;
	std::vector< size_t > mapLenKeyS; // 存储所有的 key
	for( auto it = remove_name_s.begin( ), en = remove_name_s.end( ); it != en; ++it )
		mapLenKeyS.emplace_back( it->first );
	std::sort( mapLenKeyS.begin( ), mapLenKeyS.end( ) );
	for( auto node : infos ) {
		interfacePlugsType::HtmlDocString name;
		if( !node->getNovelName( &name ) )
			continue;
		cylHtmlTools::HtmlStringTools::removeAllSpace( name );
		name = NovelDBJob::converStringToUpper( name );
		size_t length = name.length( );
		for( auto &key : mapLenKeyS ) {
			if( length != key )
				continue;
			if( key > length )
				break;
			auto &&sharedPtr = remove_name_s.at( key );
			for( auto &str : *sharedPtr )
				if( cylHtmlTools::HtmlStringTools::equRemoveSpaceOverHtmlString( name, str ) ) {
					node = nullptr;
					break;
				}
			if( node == nullptr )
				break;
		}
		if( node )
			result.emplace_back( node );
	}
	return result;
}
NovelDBJob::NovelInfoVector NovelDBJob::findNovel( const NovelInfoVector &infos, const std::unordered_map< size_t, std::shared_ptr< std::vector< interfacePlugsType::HtmlDocString > > > &find_key, const std::function< void( ) > &call_function ) {
	std::mutex *writeMutex = new std::mutex; // 数组写入锁
	NovelDBJob::NovelInfoVector result;
	std::vector< size_t > mapLenKeyS; // 存储所有的 key
	for( auto it = find_key.begin( ), en = find_key.end( ); it != en; ++it )
		mapLenKeyS.emplace_back( it->first );
	std::sort( mapLenKeyS.begin( ), mapLenKeyS.end( ) );
	cylHtmlTools::HtmlWorkThreadPool threadPool; // 线程池
	for( auto &node : infos ) {
		threadPool.appendWork( [
					node,writeMutex
					,&mapLenKeyS,&find_key,&result
				]( cylHtmlTools::HtmlWorkThread * ) ->void {
					// todo : ......
					interfacePlugsType::HtmlDocString name;
					interfacePlugsType::HtmlDocString info;
					interfacePlugsType::HtmlDocString auth;
					interfacePlugsType::HtmlDocString lastItem;
					node->getNovelName( &name );
					node->getNovelInfo( &info );
					node->getNovelAuthor( &auth );
					node->getNovelLastItem( &lastItem );
					size_t nameLen = name.length( );
					size_t infoLen = info.length( );
					size_t authLen = auth.length( );
					size_t lastItemLen = lastItem.length( );
					for( auto &key : mapLenKeyS ) {
						if( nameLen < key && infoLen < key && authLen < key && lastItemLen < key )
							break;
						auto &&sharedPtr = find_key.at( key );
						for( auto &str : *sharedPtr ) {
							if( nameLen >= key ) {
								if( cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &name, &str ) ) {
									writeMutex->lock( );
									result.emplace_back( node );
									writeMutex->unlock( );
									break; // 小说当属匹配当前子字符串
								}
							}
							if( infoLen >= key ) {
								if( cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &info, &str ) ) {
									writeMutex->lock( );
									result.emplace_back( node );
									writeMutex->unlock( );
									break; // 小说当属匹配当前子字符串
								}
							}
							if( authLen >= key ) {
								if( cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &auth, &str ) ) {
									writeMutex->lock( );
									result.emplace_back( node );
									writeMutex->unlock( );
									break; // 小说当属匹配当前子字符串
								}
							}
							if( lastItemLen >= key ) {
								if( cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &lastItem, &str ) ) {
									writeMutex->lock( );
									result.emplace_back( node );
									writeMutex->unlock( );
									break; // 小说当属匹配当前子字符串
								}
							}
						}
					}
				} );
	}
	threadPool.start( 8,
			[&](
			cylHtmlTools::HtmlWorkThreadPool *html_work_thread_pool,
			const unsigned long long &i,
			const unsigned long long & ) {
				if( call_function )
					call_function( );
			} );
	threadPool.waiteOverJob( );
	return result;
}


bool NovelDBJob::findNovelKey( const interfacePlugsType::INovelInfo_Shared &novel_info_shared, const std::unordered_map< size_t, std::shared_ptr< std::vector< interfacePlugsType::HtmlDocString > > > &find_key ) {

	interfacePlugsType::HtmlDocString novelName, novelInfo, novelAuth, novelLastItem, novelUrl;
	size_t novelNameLen, novelInfoLen, novelAuthLen, novelLastItemLen, novelUrlLen;
	interfacePlugsType::INovelInfo *element = novel_info_shared.get( );
	novelNameLen = element->getNovelName( &novelName );
	novelInfoLen = element->getNovelInfo( &novelInfo );
	novelAuthLen = element->getNovelAuthor( &novelAuth );
	novelLastItemLen = element->getNovelLastItem( &novelLastItem );
	novelUrlLen = element->getNovelUrl( &novelUrl );
	converStringToUpper( novelName );
	converStringToUpper( novelInfo );
	converStringToUpper( novelAuth );
	converStringToUpper( novelLastItem );
	converStringToUpper( novelUrl );
	// 匹配名称
	auto iterator = find_key.begin( );
	auto end = find_key.end( );
	for( ; iterator != end; ++iterator ) {
		size_t len = iterator->first;
		auto second = iterator->second;
		for( auto &str : *second ) {
			if( novelNameLen > len && cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &novelName, &str ) )
				return true;
			if( novelInfoLen > len && cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &novelInfo, &str ) )
				return true;
			if( novelAuthLen > len && cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &novelAuth, &str ) )
				return true;
			if( novelLastItemLen > len && cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &novelLastItem, &str ) )
				return true;
			if( novelUrlLen > len && cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &novelUrl, &str ) )
				return true;
		}
	}
	return false;
}
std::vector< QString > NovelDBJob::getNovelNames( const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_vector ) {
	std::vector< QString > result;
	interfacePlugsType::HtmlDocString name;
	for( auto &novel : novel_info_vector )
		if( novel->getNovelName( &name ) )
			result.emplace_back( QString::fromStdWString( name ) );
	return result;
}
std::vector< QString > NovelDBJob::converNovelToStrings( const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_vector ) {
	std::vector< QString > result;
	auto iterator = novel_info_vector.begin( );
	auto end = novel_info_vector.end( );
	interfacePlugsType::HtmlDocString resultSerializableHtmlDocString;
	for( ; iterator != end; ++iterator )
		if( iterator->get( )->objToHtmlDocString( &resultSerializableHtmlDocString ) > 0 )
			result.emplace_back( QString::fromStdWString( resultSerializableHtmlDocString ) );
	return result;
}
QString NovelDBJob::jionNovels( const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_vector ) {
	QString result = QDateTime::currentDateTime( ).toString( "yyyy 年 MM 月 dd 日 hh 时 mm 分 ss 秒" );
	std::vector< QString > vector;
	interfacePlugsType::HtmlDocString name;
	for( auto &novel : novel_info_vector )
		if( novel->objToHtmlDocString( &name ) )
			vector.emplace_back( QString::fromStdWString( name ) );
	size_t count = vector.size( );
	auto countQString = " / " + QString::number( count ) + '\n';
	QString jon = "\n==================\t";
	for( size_t index = 0; index < count; ++index )
		result = result + jon + QString::number( index + 1 ) + countQString + vector.at( index ) + jon;
	return result;
}

/// <summary>
/// 获取文件的基本名称
/// </summary>
/// <param name="filePathInfo">文件路径信息</param>
/// <returns>文件的基本名称</returns>
QString getFileBaseName( const QFileInfo &filePathInfo ) {
	QString absoluteDirPath = filePathInfo.absoluteDir( ).absolutePath( );
	QString absoluteFilePath = filePathInfo.absoluteFilePath( );
	QString fileName = absoluteFilePath.remove( 0, absoluteDirPath.size( ) + 1 );
	qsizetype indexOf = fileName.lastIndexOf( "." );
	if( indexOf > 0 )
		return fileName.slice( 0, indexOf );
	return fileName;
}

/// <summary>
/// 拷贝小说信息到新对象
/// </summary>
/// <param name="novel_info">被拷贝的对象</param>
/// <returns>新的对象</returns>
interfacePlugsType::INovelInfo_Shared cloneNovel( interfacePlugsType::INovelInfo &novel_info ) {
	interfacePlugsType::INovelInfo_Shared newNovel = std::make_shared< NovelInfo >( );
	newNovel->novelName = novel_info.novelName; // 配置名称
	newNovel->url = novel_info.url; // 配置 url
	newNovel->typeName = novel_info.typeName; // 配置类型名称
	newNovel->author = novel_info.author; // 配置作者
	newNovel->lastItem = novel_info.lastItem; // 配置最后更新项
	newNovel->lastRequestTime = novel_info.lastRequestTime; // 配置最后获取时间
	newNovel->updateTime = novel_info.updateTime; // 配置最后更新时间
	newNovel->additionalData = novel_info.additionalData; // 追加附加信息
	return newNovel;
}
/// <summary>
/// 从 novel_keys_map 查找匹配的信息追加到  interfacePlugsType::INovelInfo::additionalData 当中
/// </summary>
/// <param name="append_novel_info">追加的末尾对象</param>
/// <param name="find_novel_info">novel_keys_map 查找匹配的对象信息</param>
/// <param name="write_file_path">匹配的路径</param>
/// <param name="novel_keys_map">信息存储对象</param>
void appendKeyMapToAdditionDataLast( interfacePlugsType::INovelInfo_Shared append_novel_info, interfacePlugsType::INovelInfo_Shared find_novel_info, const interfacePlugsType::HtmlDocString &write_file_path, const std::unordered_map< interfacePlugsType::HtmlDocString, std::unordered_map< interfacePlugsType::INovelInfo_Shared, std::vector< interfacePlugsType::HtmlDocString > > > &novel_keys_map ) {

	std::vector< cylHtmlTools::HtmlString > atts;
	for( auto &mapIterator : novel_keys_map ) {
		if( cylHtmlTools::HtmlStringTools::equHtmlString( write_file_path, mapIterator.first ) )
			for( auto &novelKeyMapIterator : mapIterator.second )
				if( novelKeyMapIterator.first == find_novel_info ) {
					for( auto &key : novelKeyMapIterator.second )
						atts.emplace_back( key );
					break;
				}
	}

	auto begin = atts.begin( );
	auto end = atts.end( );
	if( begin != end )
		do {
			append_novel_info->additionalData = append_novel_info->additionalData + *begin;
			++begin;
			if( begin == end )
				break;
			append_novel_info->additionalData = append_novel_info->additionalData + L',';
		} while( true );
}
/// <summary>
/// 获取文件的基本名称
/// </summary>
/// <param name="filePathInfo">文件路径信息</param>
/// <returns>文件的基本名称</returns>
QString getFileBaseName( const QString &filePathInfo ) {
	return getFileBaseName( QFileInfo( filePathInfo ) );
}
interfacePlugsType::Vector_INovelInfoSPtr_Shared NovelDBJob::inductionNovelsForNameAndAuthor( const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_vector, const std::unordered_map< interfacePlugsType::HtmlDocString, std::unordered_map< interfacePlugsType::INovelInfo_Shared, std::vector< interfacePlugsType::HtmlDocString > > > &novel_keys_map, const interfacePlugsType::HtmlDocString &write_path ) {
	interfacePlugsType::Vector_INovelInfoSPtr_Shared vectorINovelInfoSPtr =
			std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( ); // 临时存储未处理的小说列表
	interfacePlugsType::Vector_INovelInfoSPtr_Shared resultIndutionNovels =
			std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( ); // 合并后返回的小说列表
	interfacePlugsType::Vector_INovelInfoSPtr_Shared notProcessedNovelsBuff =
			std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( novel_info_vector.begin( ), novel_info_vector.end( ) ); // 遍历的小说列表
	auto inputNovelMax = notProcessedNovelsBuff->size( );
	auto wirtNormalPath = getFileBaseName( QString::fromStdWString( write_path ) ).toUpper( ).toStdWString( );
	while( inputNovelMax ) {
		auto inductionNovel = notProcessedNovelsBuff->at( 0 );
		interfacePlugsType::INovelInfo_Shared newNovel = cloneNovel( *inductionNovel );
		appendKeyMapToAdditionDataLast( newNovel, inductionNovel, wirtNormalPath, novel_keys_map );
		resultIndutionNovels->emplace_back( newNovel );
		// 从下一个开始比较
		for( size_t startIndex = 1; startIndex < inputNovelMax; ++startIndex ) {
			// 比较的小说对象
			auto compNovel = notProcessedNovelsBuff->at( startIndex );
			if( compNovel->novelName == inductionNovel->novelName && compNovel->author == inductionNovel->author ) { // 正确匹配
				if( compNovel->info.length( ) > inductionNovel->info.length( ) )
					newNovel->info = compNovel->info;
				else
					newNovel->info = inductionNovel->info;
				if( !cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &newNovel->url, &compNovel->url ) )
					newNovel->url = newNovel->url + L"\n\t\t" + compNovel->url; // 追加 url
				if( !cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &newNovel->typeName, &compNovel->typeName ) )
					newNovel->typeName = newNovel->typeName + L"," + compNovel->typeName; // 追加类型名称
				if( !compNovel->lastItem.empty( ) ) // 追加最后更新项
					if( !newNovel->lastItem.empty( ) ) { // 如果不为空，则同时分解
						std::vector< interfacePlugsType::HtmlDocString > compAddDataList;
						std::vector< interfacePlugsType::HtmlDocString > buff = cylHtmlTools::HtmlStringTools::split( compNovel->lastItem, L'\n' );
						// 追加到容器
						for( auto &appStr : buff )
							if( !cylHtmlTools::HtmlStringTools::removeAllSpace( appStr ).empty( ) ) {
								auto end = compAddDataList.end( );
								auto result = std::find_if( compAddDataList.begin( ),
										end,
										[&]( const interfacePlugsType::HtmlDocString &compStr ) ->bool {
											if( cylHtmlTools::HtmlStringTools::equHtmlString( compStr, appStr ) )
												return true;
											return false;
										} );
								if( result != end )
									compAddDataList.emplace_back( appStr );
							}
						buff = cylHtmlTools::HtmlStringTools::split( newNovel->lastItem, L'\n' );
						// 追加到容器
						for( auto &appStr : buff )
							if( !cylHtmlTools::HtmlStringTools::removeAllSpace( appStr ).empty( ) ) {
								auto end = compAddDataList.end( );
								auto result = std::find_if( compAddDataList.begin( ),
										end,
										[&]( const interfacePlugsType::HtmlDocString &compStr ) ->bool {
											if( cylHtmlTools::HtmlStringTools::equHtmlString( compStr, appStr ) )
												return true;
											return false;
										} );
								if( result != end )
									compAddDataList.emplace_back( appStr );
							}
						auto begin = compAddDataList.begin( );
						auto end = compAddDataList.end( );
						if( begin != end ) {
							newNovel->lastItem.clear( );
							do {
								newNovel->lastItem = newNovel->lastItem + *begin;
								++begin;
								if( begin == end )
									break;
								newNovel->lastItem = newNovel->lastItem + L"\n\t\t";
							} while( true );
						}
					} else // 如果为空，则直接赋值
						newNovel->lastItem = compNovel->lastItem;

				std::vector< cylHtmlTools::HtmlString > atts;
				for( auto &mapIterator : novel_keys_map ) {
					if( cylHtmlTools::HtmlStringTools::equHtmlString( wirtNormalPath, mapIterator.first ) )
						for( auto &novelKeyMapIterator : mapIterator.second )
							if( novelKeyMapIterator.first == inductionNovel ) {
								for( auto &key : novelKeyMapIterator.second )
									atts.emplace_back( key );
								break;
							}
				}
				auto begin = atts.begin( );
				auto end = atts.end( );
				if( begin != end ) {
					std::vector< cylHtmlTools::HtmlString > split;
					if( !compNovel->additionalData.empty( ) ) {
						split = cylHtmlTools::HtmlStringTools::split( compNovel->additionalData, L',' );
						// 追加到容器
						for( auto &appStr : split )
							if( !cylHtmlTools::HtmlStringTools::removeAllSpace( appStr ).empty( ) )
								atts.emplace_back( appStr );
					}
					if( !newNovel->additionalData.empty( ) ) {
						split = cylHtmlTools::HtmlStringTools::split( newNovel->additionalData, L',' );
						// 追加到容器
						for( auto &appStr : split )
							if( !cylHtmlTools::HtmlStringTools::removeAllSpace( appStr ).empty( ) )
								atts.emplace_back( appStr );
					}
					atts = cylHtmlTools::HtmlStringTools::vectorStrAdjustSubStr( atts );
					begin = atts.begin( );
					end = atts.end( );
					newNovel->additionalData.clear( );
					if( begin != end )
						do {
							newNovel->additionalData = newNovel->additionalData + *begin;
							++begin;
							if( begin == end )
								break;
							newNovel->additionalData = newNovel->additionalData + L',';
						} while( true );
				}
			} else
				vectorINovelInfoSPtr->emplace_back( compNovel );
		}
		notProcessedNovelsBuff = vectorINovelInfoSPtr;
		vectorINovelInfoSPtr = std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( );
		inputNovelMax = notProcessedNovelsBuff->size( );
	}


	return resultIndutionNovels;
}
interfacePlugsType::Vector_INovelInfoSPtr_Shared NovelDBJob::inductionNovelsForNameAndAuthor( const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_vector ) {

	interfacePlugsType::Vector_INovelInfoSPtr_Shared vectorINovelInfoSPtr =
			std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( ); // 临时存储未处理的小说列表
	interfacePlugsType::Vector_INovelInfoSPtr_Shared resultIndutionNovels =
			std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( ); // 合并后返回的小说列表
	interfacePlugsType::Vector_INovelInfoSPtr_Shared notProcessedNovelsBuff =
			std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( novel_info_vector.begin( ), novel_info_vector.end( ) ); // 遍历的小说列表
	auto inputNovelMax = notProcessedNovelsBuff->size( );
	while( inputNovelMax ) {
		auto inductionNovel = notProcessedNovelsBuff->at( 0 );
		interfacePlugsType::INovelInfo_Shared newNovel = cloneNovel( *inductionNovel );
		resultIndutionNovels->emplace_back( newNovel );
		// 从下一个开始比较
		for( size_t startIndex = 1; startIndex < inputNovelMax; ++startIndex ) {
			// 比较的小说对象
			auto compNovel = notProcessedNovelsBuff->at( startIndex );
			if( compNovel->novelName == inductionNovel->novelName && compNovel->author == inductionNovel->author ) { // 正确匹配
				if( compNovel->info.length( ) > inductionNovel->info.length( ) )
					newNovel->info = compNovel->info;
				else
					newNovel->info = inductionNovel->info;
				if( !cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &newNovel->url, &compNovel->url ) )
					newNovel->url = newNovel->url + L"\n\t\t" + compNovel->url; // 追加 url
				if( !cylHtmlTools::HtmlStringTools::findNextHtmlStringPotion( &newNovel->typeName, &compNovel->typeName ) )
					newNovel->typeName = newNovel->typeName + L"\n\t\t" + compNovel->typeName; // 追加类型名称
				if( !compNovel->additionalData.empty( ) )
					newNovel->additionalData = newNovel->additionalData + L"," + compNovel->additionalData;
			} else
				vectorINovelInfoSPtr->emplace_back( compNovel );
		}
		notProcessedNovelsBuff = vectorINovelInfoSPtr;
		vectorINovelInfoSPtr = std::make_shared< interfacePlugsType::Vector_INovelInfoSPtr >( );
		inputNovelMax = notProcessedNovelsBuff->size( );
	}
	return resultIndutionNovels;
}
