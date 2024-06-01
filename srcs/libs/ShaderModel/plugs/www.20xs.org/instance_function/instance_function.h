#ifndef INSTANCE_FUNCTION_H_H_HEAD__FILE__
#define INSTANCE_FUNCTION_H_H_HEAD__FILE__
#pragma once

#include "./NovelNodeXPathInfo.h"

namespace instance_function {
	struct NovelNodeXPathInfo;
	/// <summary>
	/// 根据错误信息输出异常
	/// </summary>
	/// <param name="quitMsg">异常标识</param>
	/// <param name="extent">节点</param>
	/// <param name="novelInfoBuffPtr">小说指针</param>
	/// <param name="type_name">小说类型</param>
	/// <param name="request_url">请求页面</param>
	/// <param name="thisOStream">附加输出流</param>
	/// <param name="htmlText">html 页面</param>
	inline void out_debug( RequestNet::Novel_Xpath_Analysis_Error quitMsg, cylHtmlTools::HtmlNode *extent, NovelInfo *novelInfoBuffPtr, const interfacePlugsType::HtmlDocString &type_name, const interfacePlugsType::HtmlDocString &request_url, cylHtmlTools::XPath &xpath, OStream *thisOStream, const cylHtmlTools::HtmlString &htmlText ) {
		QString includeNodeContent( QString::fromStdWString( *extent->getIncludeNodeContent( ) ) );
		includeNodeContent = u8"\n===========================\n" + includeNodeContent + u8"\n===========================\n";
		switch( quitMsg ) {
			case RequestNet::Novel_Xpath_Analysis_Error::DateTime_Error_Expire : {
				QString errorMsg( u8" DateTime_Error_Expire 异常(过期)，登出:\n\turl (%1) -> 小说名称 (%2) => 退出代码(%3)" );
				errorMsg = errorMsg.arg( *novelInfoBuffPtr->getQStringSPtrUrl( ) ).arg( *novelInfoBuffPtr->getQStringSPtrName( ) ).arg( quitMsg );
				auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
				OStream::anyDebugOut( thisOStream, msg );
				break;
			}
			case RequestNet::Novel_Xpath_Analysis_Error::DateTime_Error_None : {
				QString errorMsg( u8" DateTime_Error_None 异常(日期找不到 xpath : \"%4\" )，登出:\n\turl (%1) -> 小说名称 (%2) => 退出代码(%3)" );
				errorMsg = errorMsg.arg( *novelInfoBuffPtr->getQStringSPtrUrl( ) ).arg( *novelInfoBuffPtr->getQStringSPtrName( ) ).arg( quitMsg ).arg( xpath.getHtmlString( ) ) + includeNodeContent;
				auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
				OStream::anyDebugOut( thisOStream, msg );
				break;
			}
			case RequestNet::Novel_Xpath_Analysis_Error::DateTime_Error_Xpath : {
				QString errorMsg( u8" DateTime_Error_Xpath 异常(日期 xpath 错误,xpath : \"%4\" )，登出:\n\turl (%1) -> 小说名称 (%2) => 退出代码(%3)" );
				errorMsg = errorMsg.arg( *novelInfoBuffPtr->getQStringSPtrUrl( ) ).arg( *novelInfoBuffPtr->getQStringSPtrName( ) ).arg( quitMsg ).arg( xpath.getHtmlString( ) ) + includeNodeContent;
				auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
				OStream::anyDebugOut( thisOStream, msg );
				break;
			}
			case RequestNet::Novel_Xpath_Analysis_Error::Name_Error_None : {
				QString errorMsg( u8" Name_Error_None 异常 xpath : \"%3\" ，登出:\n\turl (%1) -> 小说名称 (none) => 退出代码(%2)" );
				errorMsg = errorMsg.arg( *novelInfoBuffPtr->getQStringSPtrUrl( ) ).arg( quitMsg ).arg( xpath.getHtmlString( ) ) + includeNodeContent;
				auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
				OStream::anyDebugOut( thisOStream, msg );
				break;
			}
			case RequestNet::Novel_Xpath_Analysis_Error::Name_Error_Xpath : {
				QString errorMsg( u8" Name_Error_Xpath 异常 xpath : \"%3\" ，登出:\n\turl (%1) -> 小说名称 (none) => 退出代码(%2)" );
				errorMsg = errorMsg.arg( *novelInfoBuffPtr->getQStringSPtrUrl( ) ).arg( quitMsg ).arg( xpath.getHtmlString( ) ) + includeNodeContent;
				auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
				OStream::anyDebugOut( thisOStream, msg );
				break;
			}
			case RequestNet::Novel_Xpath_Analysis_Error::Url_Error_Xpath : {
				QString errorMsg( u8" Url_Error_Xpath 异常 xpath : \"%2\" ，登出 => 退出代码(%1)" );
				errorMsg = errorMsg.arg( quitMsg ).arg( xpath.getHtmlString( ) ) + includeNodeContent;
				auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
				OStream::anyDebugOut( thisOStream, msg );
				break;
			}
			case RequestNet::Novel_Xpath_Analysis_Error::Url_Error_None : {
				QString errorMsg( u8" Url_Error_None 异常 xpath : \"%2\" ，登出 => 退出代码(%1)" );
				errorMsg = errorMsg.arg( quitMsg ).arg( xpath.getHtmlString( ) ) + includeNodeContent;
				auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
				OStream::anyDebugOut( thisOStream, msg );
				break;
			}
			case RequestNet::Novel_Xpath_Analysis_Error::None :
				return;
			default : {
				QString errorMsg( u8" 未知异常 xpath : \"%2\" ，登出->\n=================\n%1\n=========\n" );
				cylHtmlTools::HtmlNode *element = extent;
				errorMsg = errorMsg.arg( QString::fromStdWString( *element->getIncludeNodeContent( ) ) ).arg( xpath.getHtmlString( ) ) + includeNodeContent;
				auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
				auto path = QString( Cache_Path_Dir ).append( QDir::separator( ) ).append( type_name ).append( u8".html" );
				OStream::anyDebugOut( thisOStream, msg, __FILE__, __LINE__, __FUNCTION__, path, QString::fromStdWString( htmlText ) );
			}
		}
	}
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
	inline void separate_list( const interfacePlugsType::Vector_INovelInfoSPtr &src_vector, cylDB::IResultInfo_Shared &db_result, interfacePlugsType::Vector_INovelInfoSPtr &updateList, interfacePlugsType::Vector_INovelInfoSPtr &interList ) {
		db_result->resetColIndex( );
		auto currentRows = db_result->getCurrentRows( );
		interfacePlugsType::HtmlDocString url;
		interfacePlugsType::HtmlDocString lastItem;
		interfacePlugsType::HtmlDocString compUrl;
		interfacePlugsType::HtmlDocString compLastItem;
		interfacePlugsType::Vector_INovelInfoSPtr removeVectorINovelInfoS;
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
}


#endif // INSTANCE_FUNCTION_H_H_HEAD__FILE__
