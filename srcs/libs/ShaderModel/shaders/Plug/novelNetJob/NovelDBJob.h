﻿#ifndef NOVELDBJOB_H_H_HEAD__FILE__
#define NOVELDBJOB_H_H_HEAD__FILE__
#pragma once
#include <QDateTime>

#include <cwctype>

#include "nameSpace/interfacePlugsType.h"
#include "../auto_generate_files/export/Plug_export.h"
#include "stream/OStream.h"

class PLUG_EXPORT NovelDBJob {
public: // 新类型
	using NovelTypeNameType = QString;
	using NovelUrlHostType = QString;
	using NovelInfoVector = interfacePlugsType::Vector_INovelInfoSPtr;
	using NovelInfoVector_Shared = std::shared_ptr< NovelInfoVector >;
	using NovelTypePair = std::pair< NovelTypeNameType, NovelInfoVector_Shared >;
	using NovelTypePairVector = std::vector< NovelTypePair >;
	using NovelTypePairVector_Shared = std::shared_ptr< NovelTypePairVector >;
	using NovelHostMap = std::unordered_map< NovelUrlHostType, NovelTypePairVector_Shared >;
	using NovelHostMap_Shared = std::shared_ptr< NovelHostMap >;
private: // - 静态成员变量
	static QDateTime currentTime; // 请求时间
	static QString currentTimeForm; // 当前请求时间的格式
	static QStringList tabFieldNames; // 表字体信息
	static QString updateCmd; // 更新命令
	static QString insterCmd; // 插入命令
	static QString deleteCmd; // 删除命令

public:// - 静态方法
	static QString getCurrentTimeForm( ) {
		return currentTimeForm;
	}
	/// @brief 移除所有空格
	/// @param remove_obj 移除对象
	static void removeAllSpace( QString &remove_obj ) {
		if( remove_obj.isEmpty( ) )
			return;
		size_t leftLen = remove_obj.length( );
		if( leftLen == 0 )
			return;
		QString::value_type *buff = new QString::value_type [ leftLen ];
		size_t index = 0, buffIndex = 0;
		for( ; index < leftLen; ++index ) {
			auto value = remove_obj.at( index );
			if( value.isSpace( ) || !value.isPrint( ) )
				continue;
			buff[ buffIndex ] = value;
			++buffIndex;
		}
		if( buffIndex == 0 )
			remove_obj = "";
		else
			remove_obj = QString( buff, buffIndex );
		delete[] buff;
	}
	/// <summary>
	/// 对小说列表进行排序，并且返回结果
	/// </summary>
	/// <param name="infos">排序列表</param>
	/// <returns>排序结果</returns>
	static NovelInfoVector sort( const NovelInfoVector &infos );
	/// <summary>
	/// 剔除相同
	/// </summary>
	/// <param name="infos">剔除列表</param>
	/// <returns>剔除完毕的列表</returns>
	static NovelInfoVector identical( const NovelInfoVector &infos );
	/// <summary>
	/// 分解列表<br/>
	///	网站域名为 key - 网页类型与小说列表组成的 pair 成为 value
	/// </summary>
	/// <param name="infos">分解列表</param>
	/// <param name="call_back">当进行工作时候回调该函数</param>
	/// <returns>分解返回</returns>
	static NovelHostMap decompose( const NovelInfoVector &infos, const std::function< void( ) > &call_back );
	/// <summary>
	/// 写入数据库
	/// </summary>
	/// <param name="thisOStream">可选信息输出流</param>
	/// <param name="outPath">目标根目录</param>
	/// <param name="saveNovelInfos">写入的小说</param>
	/// <param name="run">线程工作当中时，会调用该函数</param>
	/// <returns>写入个数</returns>
	static size_t writeDB( OStream *thisOStream, const QString &outPath, const NovelInfoVector &saveNovelInfos, const std::function< bool( const std::chrono::system_clock::time_point::duration & ) > &run );
	/// <summary>
	/// 读取数据库
	/// </summary>
	/// <param name="thisOStream">可选信息输出流</param>
	/// <param name="db_link">链接对象</param>
	/// <param name="db_name">数据库名称</param>
	/// <returns>写入个数</returns>
	static NovelInfoVector_Shared readDB( OStream *thisOStream, const QString &db_link, const QString &db_name );
	/// <summary>
	/// 删除过期选项
	/// </summary>
	/// <param name="thisOStream">可选信息输出流</param>
	/// <param name="db_link">链接对象</param>
	/// <param name="db_name">数据库名称</param>
	/// <param name="expire_day">超过日期</param>
	/// <returns>删除元素</returns>
	static NovelInfoVector_Shared removeExpireDB( OStream *thisOStream, const QString &db_link, const QString &db_name, const size_t &expire_day );
	/// <summary>
	/// 删除指定 url
	/// </summary>
	/// <param name="thisOStream">可选信息输出流</param>
	/// <param name="db_link">链接对象</param>
	/// <param name="db_name">数据库名称</param>
	/// <param name="novel_url_vector">删除的列表</param>
	/// <param name="outPath">日志根路径，字符串为空时，纯输出，不写入日志</param>
	/// <returns>删除元素</returns>
	static void removeNovelVectorDB( OStream *thisOStream, const QString &db_link, const QString &db_name, const std::vector< interfacePlugsType::HtmlDocString > &novel_url_vector, const QString &outPath );
	/// <summary>
	/// 校验小说是否过期-单对象
	/// </summary>
	/// <param name="expire_day">过期天数</param>
	/// <param name="novel_info_ptr">校验小说指针对象</param>
	/// <returns>true 表示过期</returns>
	static bool novelIsExpire( size_t expire_day, const interfacePlugsType::INovelInfoPtr &novel_info_ptr );
	/// <summary>
	/// 校验小说是否过期-列表
	/// </summary>
	/// <param name="expire_day">过期天数</param>
	/// <param name="novel_info_ptr">校验小说列表</param>
	/// <returns>过期的 url</returns>
	static interfacePlugsType::Vector_INovelInfoSPtr novelVectorIsExpire( const size_t &expire_day, const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_ptr );
	/// <summary>
	/// 获取小说列表当中的 url
	/// </summary>
	/// <param name="novel_info_ptr">获取小说</param>
	/// <returns>url 列表</returns>
	static std::vector< interfacePlugsType::HtmlDocString > novelVectorGetNovleUrl( interfacePlugsType::Vector_INovelInfoSPtr &novel_info_ptr );
	/// <summary>
	/// src 排除 des 所有成员
	/// </summary>
	/// <param name="src">源</param>
	/// <param name="des">目标</param>
	/// <returns>返回 src 排除 des 后的列表</returns>
	static interfacePlugsType::Vector_INovelInfoSPtr formVectorINovelInfoRemoveVectorINovelInfo( const interfacePlugsType::Vector_INovelInfoSPtr &src, const interfacePlugsType::Vector_INovelInfoSPtr &des );

	/// <summary>
	/// 写入小说列表
	/// </summary>
	/// <param name="writeFilePath">写入路径</param>
	/// <param name="infos">待写入列表</param>
	/// <returns>写入列表-完成排序</returns>
	static NovelInfoVector writeFile( const QString &writeFilePath, const NovelInfoVector &infos );

	/// <summary>
	/// 写入小说
	/// </summary>
	/// <param name="root_path">根路径</param>
	/// <param name="novel_host">网站域名-子目录</param>
	/// <param name="infos">写入小说</param>
	/// <returns>已经排序的小说</returns>
	static NovelInfoVector writeFile( const QString &root_path, const QString &novel_host, const NovelInfoVector &infos );
	/// <summary>
	/// 写入小说
	/// </summary>
	/// <param name="root_path">根路径</param>
	/// <param name="novel_host">网站域名-次目录</param>
	/// <param name="novel_type">小说类型-次目录下的文件名</param>
	/// <param name="infos">写入小说</param>
	/// <returns>已经排序的小说</returns>
	static NovelInfoVector writeFile( const QString &root_path, const QString &novel_host, const QString &novel_type, const NovelInfoVector &infos );

	/// <summary>
	/// 删除列表当中指定子名称的列表( aa 与 关键字 a， 结果为删除 aa 名称小说) 
	/// </summary>
	/// <param name="infos">校验列表</param>
	/// <param name="remove_name_s">提供关键字的列表</param>
	/// <returns>删除完毕的列表</returns>
	static NovelInfoVector removeSubName( const NovelInfoVector &infos, const std::unordered_map< size_t, std::shared_ptr< std::vector< interfacePlugsType::HtmlDocString > > > &remove_name_s );
	/// <summary>
	/// 删除列表当中指定名称的列表( aa 与 关键字 a， 保留 aa， 如果存在 a 名称， 则删除 a) 
	/// </summary>
	/// <param name="infos">校验列表</param>
	/// <param name="remove_name_s">提供关键字的列表</param>
	/// <returns>删除完毕的列表</returns>
	static NovelInfoVector removeEquName( const NovelInfoVector &infos, const std::unordered_map< size_t, std::shared_ptr< std::vector< interfacePlugsType::HtmlDocString > > > &remove_name_s );

	/// <summary>
	/// 查找小说
	/// </summary>
	/// <param name="infos">查找列表</param>
	/// <param name="find_key">长度关键字</param>
	/// <param name="call_function">工作时候会调用该函数</param>
	/// <returns>匹配的小说列表</returns>
	static NovelInfoVector findNovel( const NovelInfoVector &infos, const std::unordered_map< size_t, std::shared_ptr< std::vector< interfacePlugsType::HtmlDocString > > > &find_key, const std::function< void( ) > &call_function );
	/// <summary>
	/// 字符串转换到大写
	/// </summary>
	/// <param name="str">转换的字符串</param>
	static std::wstring & converStringToUpper( std::wstring &str ) {
		std::transform( str.begin( ), str.end( ), str.begin( ), []( std::wstring::value_type c ) { return std::towupper( c ); } );
		return str;
	}
	/// <summary>
	/// 字符串转换到小写
	/// </summary>
	/// <param name="str">转换的字符串</param>
	static std::wstring & converStringToLower( std::wstring &str ) {
		std::transform( str.begin( ), str.end( ), str.begin( ), []( std::wstring::value_type c ) { return std::towlower( c ); } );
		return str;
	}
	/// <summary>
	/// 字符串转换到大写
	/// </summary>
	/// <param name="str">转换的字符串</param>
	static std::string & converStringToUpper( std::string &str ) {
		std::transform( str.begin( ), str.end( ), str.begin( ), []( std::string::value_type c ) { return std::toupper( c ); } );
		return str;
	}
	/// <summary>
	/// 字符串转换到小写
	/// </summary>
	/// <param name="str">转换的字符串</param>
	static std::string & converStringToLower( std::string &str ) {
		std::transform( str.begin( ), str.end( ), str.begin( ), []( std::string::value_type c ) { return std::tolower( c ); } );
		return str;
	}
	/// <summary>
	/// 匹配小说，小说匹配 LenMap，返回 true，不配，返回 false
	/// </summary>
	/// <param name="novel_info_shared">小说</param>
	/// <param name="find_key">匹配映射</param>
	/// <returns>小说当中找到关键字，则返回 true</returns>
	static bool findNovelKey( const interfacePlugsType::INovelInfo_Shared &novel_info_shared, const std::unordered_map< size_t, std::shared_ptr< std::vector< interfacePlugsType::HtmlDocString > > > &find_key );
	/// <summary>
	/// 获取所有小说的名称
	/// </summary>
	/// <param name="novel_info_vector">获取列表</param>
	/// <returns>小说名称列表</returns>
	static std::vector< QString > getNovelNames( const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_vector );
	/// <summary>
	/// 把小说列表转换到字符串列表
	/// </summary>
	/// <param name="novel_info_vector">小说列表</param>
	/// <returns>字符串列表</returns>
	static std::vector< QString > converNovelToStrings( const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_vector );

	/// <summary>
	/// 输出拼接小说
	/// </summary>
	/// <param name="novel_info_vector"></param>
	/// <returns></returns>
	static QString jionNovels( const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_vector );

	/// <summary>
	/// 整理列表到小说信息，当名称与作者一致时，合并到新小说当中，并且返回合并后的小说内容(小说)
	/// </summary>
	/// <param name="novel_info_vector">检查合并的小说列表</param>
	/// <param name="novel_keys_map">追加的内容附加信息</param>
	/// <param name="write_path">写入路径-匹配 novel_keys_map 路径key</param>
	/// <returns>合并后的小说列表</returns>
	static interfacePlugsType::Vector_INovelInfoSPtr_Shared inductionNovelsForNameAndAuthor( const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_vector, const std::unordered_map< interfacePlugsType::HtmlDocString, std::unordered_map< interfacePlugsType::INovelInfo_Shared, std::vector< interfacePlugsType::HtmlDocString > > > &novel_keys_map, const interfacePlugsType::HtmlDocString &write_path );

	/// <summary>
	/// 整理列表到小说信息，当名称与作者一致时，合并到新小说当中，并且返回合并后的小说内容(小说)
	/// </summary>
	/// <param name="novel_info_vector">检查合并的小说列表</param>
	/// <returns>合并后的小说列表</returns>
	static interfacePlugsType::Vector_INovelInfoSPtr_Shared inductionNovelsForNameAndAuthor( const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_vector );
};


#endif // NOVELDBJOB_H_H_HEAD__FILE__
