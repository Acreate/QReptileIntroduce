#ifndef NOVELBASE_H_H_HEAD__FILE__
#define NOVELBASE_H_H_HEAD__FILE__
#pragma once
#include "../auto_generate_files/export/Plug_export.h"
#include "interface/INovelInfo.h"
#include <QString>
class PLUG_EXPORT NovelBase : public interfacePlugsType::INovelInfo {
public:
	void *additionalData;
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
	size_t getNovelTypeName( interfacePlugsType::HtmlDocString *result_type_name ) override;
	size_t objToHtmlDocString( interfacePlugsType::HtmlDocString *result_serializable_html_doc_string ) override;
};


#endif // NOVELBASE_H_H_HEAD__FILE__
