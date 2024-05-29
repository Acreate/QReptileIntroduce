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
public: // - set

	void setNovelName( const std::shared_ptr<QString> &novel_name ) { novelName = novel_name; }
	void setInfo( const std::shared_ptr<QString> &info ) { this->info = info; }
	void setUpdateTime( const std::shared_ptr<QString> &update_time ) { updateTime = update_time; }
	void setFormat( const std::shared_ptr<QString> &format ) { this->format = format; }
	void setLastRequestTime( const std::shared_ptr<QString> &last_request_time ) { lastRequestTime = last_request_time; }
	void setLastRequestTimeFormat( const std::shared_ptr<QString> &last_request_time_format ) { lastRequestTimeFormat = last_request_time_format; }
	void setAuthor( const std::shared_ptr<QString> &author ) { this->author = author; }
	void setUrl( const std::shared_ptr<QString> &url ) { this->url = url; }
	void setLastItem( const std::shared_ptr<QString> &last_item ) { lastItem = last_item; }
	void setAdditionalData( const std::shared_ptr<QString> &additional_data ) { additionalData = additional_data; }
	void setTypePageUrl( const std::shared_ptr<QString> &type_page_url ) { typePageUrl = type_page_url; }
	void setTypeName( const std::shared_ptr<QString> &type_name ) { typeName = type_name; }
	std::shared_ptr< QString > getQStringSPtrName( ) const { return novelName; }
	std::shared_ptr< QString > getQStringSPtrInfo( ) const { return info; }
	std::shared_ptr< QString > getQStringSPtrUpdateTime( ) const { return updateTime; }
	std::shared_ptr< QString > getQStringSPtrFormat( ) const { return format; }
	std::shared_ptr< QString > getQStringSPtrLastRequestTime( ) const { return lastRequestTime; }
	std::shared_ptr< QString > getQStringSPtrLastRequestTimeFormat( ) const { return lastRequestTimeFormat; }
	std::shared_ptr< QString > getQStringSPtrAuthor( ) const { return author; }
	std::shared_ptr< QString > getQStringSPtrUrl( ) const { return url; }
	std::shared_ptr< QString > getQStringSPtrLastItem( ) const { return lastItem; }
	std::shared_ptr< QString > getQStringSPtrAdditionalData( ) const { return additionalData; }
	std::shared_ptr< QString > getQStringSPtrTypePageUrl( ) const { return typePageUrl; }
	std::shared_ptr< QString > getQStringSPtrTypeName( ) const { return typeName; }
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
	size_t getNovelTypeName( interfacePlugsType::HtmlDocString *result_type_name ) override;
	size_t objToHtmlDocString( interfacePlugsType::HtmlDocString *result_serializable_html_doc_string ) override;
	void clear( );
};


#endif // NOVELINFO_H_H_HEAD__FILE__
