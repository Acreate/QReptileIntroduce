#ifndef INSTANCE_FUNCTION_H_H_HEAD__FILE__
#define INSTANCE_FUNCTION_H_H_HEAD__FILE__
#pragma once

#include "./NovelNodeXPathInfo.h"
#include "../auto_generate_files/macro/cmake_to_c_cpp_header_env.h"
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
	/// <summary>
	/// 获取错误内容
	/// </summary>
	/// <param name="quitMsg">错误代码</param>
	/// <param name="xpath">错误 xpath</param>
	/// <returns>错误内容</returns>
	inline QString get_error_info( RequestNet::Novel_Xpath_Analysis_Error quitMsg, const QString &xpath ) {
		switch( quitMsg ) {
			case RequestNet::Novel_Xpath_Analysis_Error::DateTime_Error_Expire :
				return u8" DateTime_Error_Expire 异常 ( 过期 )";
			case RequestNet::Novel_Xpath_Analysis_Error::DateTime_Error_None :
				return u8" DateTime_Error_None 异常 ( 日期找不到 xpath : \"" + xpath + "\")";
			case RequestNet::Novel_Xpath_Analysis_Error::DateTime_Error_Xpath :
				return u8" DateTime_Error_Xpath 异常 ( 日期 xpath 错误,xpath : \"" + xpath + "\" )";
			case RequestNet::Novel_Xpath_Analysis_Error::Name_Error_None :
				return u8" Name_Error_None 异常 ( 名称错误, xpath : \"" + xpath + "\" )";
			case RequestNet::Novel_Xpath_Analysis_Error::Name_Error_Xpath :
				return u8" Name_Error_Xpath 异常 ( 名称 xpath 错误, xpath : \"" + xpath + "\" )";
			case RequestNet::Novel_Xpath_Analysis_Error::Url_Error_Xpath :
				return u8" Url_Error_Xpath 异常 ( url xpath 错误, xpath : \"" + xpath + "\" )";
			case RequestNet::Novel_Xpath_Analysis_Error::Url_Error_None :
				return u8" Url_Error_Xpath 异常 ( url 错误, xpath : \"" + xpath + "\" )";
			case RequestNet::Novel_Xpath_Analysis_Error::None :
				return "";
			default :
				return "none";
		}
	}
	/// <summary>
	/// 输出错误信息到指定文件
	/// </summary>
	/// <param name="os">流，可选为nullptr</param>
	/// <param name="url">错误的 url - 节选 host 为路径名</param>
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
	inline bool write_error_info_file( OStream *os, const QUrl &url, const QString &dir_name, const QString &error_type, const QString &error_novel_type_name, const QString &error_file_suffix, const QString &error_call_path_file_name, const QString &error_file_call_function_name, const size_t error_file_call_function_line, const QString &error_info_text, const QString &error_write_info_content ) {
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
		auto path = QString( Cache_Path_Dir ).append( QDir::separator( ) )
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


#endif // INSTANCE_FUNCTION_H_H_HEAD__FILE__
