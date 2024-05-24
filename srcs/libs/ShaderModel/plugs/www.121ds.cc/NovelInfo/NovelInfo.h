#ifndef NOVELINFO_H_H_HEAD__FILE__
#define NOVELINFO_H_H_HEAD__FILE__
#pragma once

#include "interface/INovelInfo.h"
class QString;
class NovelInfo : public interfacePlugsType::INovelInfo {
public:
	friend class RequestNet;
private:
	std::shared_ptr< QString >
		novelName,
		info,
		updateTime,
		format,
		lastRequestTime,
		lastRequestTimeFormat,
		author,
		url,
		lastItem,
		additionalData,
		typePageUrl,
		typeName;
public:
	NovelInfo( );
	~NovelInfo( ) override;
public: // - 继承
	size_t getNovelName( interfacePlugsType::HtmlDocString *result_name ) override;
	size_t getNovelInfo( interfacePlugsType::HtmlDocString *result_info ) override;
	size_t getNovelUpdateTime( interfacePlugsType::HtmlDocString *result_time ) override;
	size_t getNovelUpdateTimeFormat( interfacePlugsType::HtmlDocString *result_format ) override;
	size_t getNovelLastRequestGetTime( interfacePlugsType::HtmlDocString *result_last_request_time ) override;
	size_t getNovelLastRequestGetTimeFormat( interfacePlugsType::HtmlDocString *result_last_request_time_format ) override;
	size_t getNovelAuthor( interfacePlugsType::HtmlDocString *result_author ) override;
	size_t getNovelUrl( interfacePlugsType::HtmlDocString *result_url ) override;
	size_t getNovelLastItem( interfacePlugsType::HtmlDocString *result_last_item ) override;
	size_t getNovelAttach( void *&result_additional_data ) override;
	size_t getNovelUrlAtPageLocation( interfacePlugsType::HtmlDocString *result_type_page_url ) override;
	size_t getNovelUrlAtName( interfacePlugsType::HtmlDocString *result_type_name ) override;
	size_t objToHtmlDocString( interfacePlugsType::HtmlDocString *result_serializable_html_doc_string ) override;
	void clear( );
};


#endif // NOVELINFO_H_H_HEAD__FILE__
