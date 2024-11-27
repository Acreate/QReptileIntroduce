#ifndef INOVELINFO_H_H_HEAD__FILE__
#define INOVELINFO_H_H_HEAD__FILE__
#pragma once

#include "../nameSpace/interfacePlugsType.h"


namespace interfacePlugsType {
	class NOVELINFO_EXPORT INovelInfo {
		void *additionalData = nullptr;
		interfacePlugsType::HtmlDocString author;
		interfacePlugsType::HtmlDocString format;
		interfacePlugsType::HtmlDocString info;
		interfacePlugsType::HtmlDocString lastItem;
		interfacePlugsType::HtmlDocString lastRequestTime;
		interfacePlugsType::HtmlDocString lastRequestTimeFormat;
		interfacePlugsType::HtmlDocString novelName;
		interfacePlugsType::HtmlDocString rootUrl;
		interfacePlugsType::HtmlDocString typeName;
		interfacePlugsType::HtmlDocString typePageUrl;
		interfacePlugsType::HtmlDocString updateTime;
		interfacePlugsType::HtmlDocString url;
	public:
		INovelInfo( ) = default;
		virtual ~INovelInfo( ) {

		}
	public:
		/// <summary>
		/// 获取网址首页地址
		/// </summary>
		/// <param name="result_root_url">返回网址首页名称</param>
		virtual size_t getRootUrl( HtmlDocString *result_root_url );
		/// <summary>
		/// 获取小数名称
		/// </summary>
		/// <param name="result_name">返回名称</param>
		/// <returns>名称大小</returns>
		virtual size_t getNovelName( HtmlDocString *result_name );
		/// <summary>
		/// 获取小说详细信息
		/// </summary>
		/// <param name="result_info">返回信息</param>
		/// <returns>信息大小</returns>
		virtual size_t getNovelInfo( HtmlDocString *result_info );
		/// <summary>
		/// 获取小说最后更新
		/// </summary>
		/// <param name="result_time">返回最后时间</param>
		/// <returns>时间长度</returns>
		virtual size_t getNovelUpdateTime( HtmlDocString *result_time );
		/// <summary>
		/// 获取小说最后更新时间的格式
		/// </summary>
		/// <param name="result_format">返回最后时间的格式</param>
		/// <returns>格式长度</returns>
		virtual size_t getNovelUpdateTimeFormat( HtmlDocString *result_format );
		/// <summary>
		/// 获取最后请求时间
		/// </summary>
		/// <param name="result_last_request_time">返回的最后请求时间</param>
		/// <returns>返回长度</returns>
		virtual size_t getNovelLastRequestGetTime( HtmlDocString *result_last_request_time );
		/// <summary>
		/// 获取最后请求时间的格式
		/// </summary>
		/// <param name="result_last_request_time_format">返回最后请求时间的格式</param>
		/// <returns>格式长度</returns>
		virtual size_t getNovelLastRequestGetTimeFormat( HtmlDocString *result_last_request_time_format );
		/// <summary>
		/// 获取小说的作者
		/// </summary>
		/// <param name="result_author">返回小说作者</param>
		/// <returns>小说作者信息的长度</returns>
		virtual size_t getNovelAuthor( HtmlDocString *result_author );
		/// <summary>
		/// 获取小说 url
		/// </summary>
		/// <param name="result_url">返回小说 url</param>
		/// <returns>小说 url 长度</returns>
		virtual size_t getNovelUrl( HtmlDocString *result_url );
		/// <summary>
		/// 获取小说最后更新项
		/// </summary>
		/// <param name="result_last_item">返回小说最后更新项</param>
		/// <returns>返回项的长度</returns>
		virtual size_t getNovelLastItem( HtmlDocString *result_last_item );
		/// <summary>
		/// 获取小说附加信息
		/// </summary>
		/// <param name="result_additional_data">直线附加信息的指针，如果为 nullptr 则不存在</param>
		/// <returns>小说附加信息的长度，如果为 0， 并且 result_additional_data 不等于 nullptr。则指向的内容存在，但为空</returns>
		virtual size_t getNovelAttach( void *&result_additional_data );
		/// <summary>
		/// 获取小说解析的页面所在地址
		/// </summary>
		/// <param name="result_type_page_url">返回小说获取的页面，该页面并非小说详情页面而是从那获取小说详情页面的地址</param>
		/// <returns>地址长度</returns>
		virtual size_t getNovelUrlAtPageLocation( HtmlDocString *result_type_page_url );
		/// <summary>
		/// 获取小说的类型
		/// </summary>
		/// <param name="result_type_name">返回小说类型</param>
		/// <returns>类型名称</returns>
		virtual size_t getNovelTypeName( HtmlDocString *result_type_name );


		/// <summary>
		/// 设置网址首页地址
		/// </summary>
		virtual void setRootUrl( const HtmlDocString &result_root_url );
		/// <summary>
		/// 设置小数名称
		/// </summary>
		virtual void setNovelName( const HtmlDocString &name );
		/// <summary>
		/// 设置小说详细信息
		/// </summary>
		virtual void setNovelInfo( const HtmlDocString &info );
		/// <summary>
		/// 设置小说最后更新
		/// </summary>
		virtual void setNovelUpdateTime( const HtmlDocString &time );
		/// <summary>
		/// 设置小说最后更新时间的格式
		/// </summary>
		virtual void setNovelUpdateTimeFormat( const HtmlDocString &format );
		/// <summary>
		/// 设置最后请求时间
		/// </summary>
		virtual void setNovelLastRequestGetTime( const HtmlDocString &last_request_time );
		/// <summary>
		/// 设置最后请求时间的格式
		/// </summary>
		virtual void setNovelLastRequestGetTimeFormat( const HtmlDocString &last_request_time_format );
		/// <summary>
		/// 设置小说的作者
		/// </summary>
		virtual void setNovelAuthor( const HtmlDocString &author );
		/// <summary>
		/// 设置小说 url
		/// </summary>
		virtual void setNovelUrl( const HtmlDocString &url );
		/// <summary>
		/// 设置小说最后更新项
		/// </summary>
		virtual void setNovelLastItem( const HtmlDocString &last_item );
		/// <summary>
		/// 设置小说附加信息
		/// </summary>
		virtual void setNovelAttach( void *result_additional_data );
		/// <summary>
		/// 设置小说解析的页面所在地址
		/// </summary>
		virtual void setNovelUrlAtPageLocation( const HtmlDocString &type_page_url );
		/// <summary>
		/// 设置小说的类型
		/// </summary>
		virtual void setNovelTypeName( const HtmlDocString &type_name );


		/// <summary>
		/// 对象转换到字符串
		/// </summary>
		/// <param name="result_serializable_html_doc_string">字符串返回</param>
		/// <returns>字符串大小</returns>
		virtual size_t objToHtmlDocString( HtmlDocString *result_serializable_html_doc_string );

	};
}


#endif // INOVELINFO_H_H_HEAD__FILE__
