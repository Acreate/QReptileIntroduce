#ifndef NOVELINFO_H_H_HEAD__FILE__
#define NOVELINFO_H_H_HEAD__FILE__
#pragma once

#include <QString>

#include "interface/INovelInfo.h"
class QString;
class NovelInfo : public interfacePlugsType::INovelInfo {
public:
	friend class RequestNet;
private:
	void *additionalData;
	interfacePlugsType::HtmlDocString author;
	interfacePlugsType::HtmlDocString format;
	interfacePlugsType::HtmlDocString info;
	interfacePlugsType::HtmlDocString lastItem;
	interfacePlugsType::HtmlDocString lastRequestTime;
	interfacePlugsType::HtmlDocString lastRequestTimeFormat;
	interfacePlugsType::HtmlDocString name;
	interfacePlugsType::HtmlDocString rootUrl;
	interfacePlugsType::HtmlDocString typeName;
	interfacePlugsType::HtmlDocString typePageUrl;
	interfacePlugsType::HtmlDocString updateTime;
	interfacePlugsType::HtmlDocString url;
public:
	NovelInfo( );
	~NovelInfo( ) override;
public: // - get
	void * getAdditionalData( ) const { return additionalData; }
	const interfacePlugsType::HtmlDocString & getAuthor( ) const { return author; }
	const interfacePlugsType::HtmlDocString & getFormat( ) const { return format; }
	const interfacePlugsType::HtmlDocString & getInfo( ) const { return info; }
	const interfacePlugsType::HtmlDocString & getLastItem( ) const { return lastItem; }
	const interfacePlugsType::HtmlDocString & getLastRequestTime( ) const { return lastRequestTime; }
	const interfacePlugsType::HtmlDocString & getLastRequestTimeFormat( ) const { return lastRequestTimeFormat; }
	const interfacePlugsType::HtmlDocString & getName( ) const { return name; }
	const interfacePlugsType::HtmlDocString & getRootUrl( ) const { return rootUrl; }
	const interfacePlugsType::HtmlDocString & getTypeName( ) const { return typeName; }
	const interfacePlugsType::HtmlDocString & getTypePageUrl( ) const { return typePageUrl; }
	const interfacePlugsType::HtmlDocString & getUpdateTime( ) const { return updateTime; }
	const interfacePlugsType::HtmlDocString & getUrl( ) const { return url; }

	const QString & getQStringAuthor( ) const { return QString::fromStdWString( author ); }
	const QString & getQStringFormat( ) const { return QString::fromStdWString( format ); }
	const QString & getQStringInfo( ) const { return QString::fromStdWString( info ); }
	const QString & getQStringLastItem( ) const { return QString::fromStdWString( lastItem ); }
	const QString & getQStringLastRequestTime( ) const { return QString::fromStdWString( lastRequestTime ); }
	const QString & getQStringLastRequestTimeFormat( ) const { return QString::fromStdWString( lastRequestTimeFormat ); }
	const QString & getQStringName( ) const { return QString::fromStdWString( name ); }
	const QString & getQStringRootUrl( ) const { return QString::fromStdWString( rootUrl ); }
	const QString & getQStringTypeName( ) const { return QString::fromStdWString( typeName ); }
	const QString & getQStringTypePageUrl( ) const { return QString::fromStdWString( typePageUrl ); }
	const QString & getQStringUpdateTime( ) const { return QString::fromStdWString( updateTime ); }
	const QString & getQStringUrl( ) const { return QString::fromStdWString( url ); }
public: // - set
	void setAdditionalData( void *const additional_data ) { additionalData = additional_data; }
	void setAuthor( const interfacePlugsType::HtmlDocString &author ) { this->author = author; }
	void setFormat( const interfacePlugsType::HtmlDocString &format ) { this->format = format; }
	void setInfo( const interfacePlugsType::HtmlDocString &info ) { this->info = info; }
	void setLastItem( const interfacePlugsType::HtmlDocString &last_item ) { lastItem = last_item; }
	void setLastRequestTime( const interfacePlugsType::HtmlDocString &last_request_time ) { lastRequestTime = last_request_time; }
	void setLastRequestTimeFormat( const interfacePlugsType::HtmlDocString &last_request_time_format ) { lastRequestTimeFormat = last_request_time_format; }
	void setNovelName( const interfacePlugsType::HtmlDocString &novel_name ) { name = novel_name; }
	void setRootUrl( const interfacePlugsType::HtmlDocString &root_url ) { rootUrl = root_url; }
	void setTypeName( const interfacePlugsType::HtmlDocString &type_name ) { typeName = type_name; }
	void setTypePageUrl( const interfacePlugsType::HtmlDocString &type_page_url ) { typePageUrl = type_page_url; }
	void setUpdateTime( const interfacePlugsType::HtmlDocString &update_time ) { updateTime = update_time; }
	void setUrl( const interfacePlugsType::HtmlDocString &url ) { this->url = url; }

	void setAuthor( const QString &author ) { this->author = author.toStdWString( ); }
	void setFormat( const QString &format ) { this->format = format.toStdWString( ); }
	void setInfo( const QString &info ) { this->info = info.toStdWString( ); }
	void setLastItem( const QString &last_item ) { lastItem = last_item.toStdWString( ); }
	void setLastRequestTime( const QString &last_request_time ) { lastRequestTime = last_request_time.toStdWString( ); }
	void setLastRequestTimeFormat( const QString &last_request_time_format ) { lastRequestTimeFormat = last_request_time_format.toStdWString( ); }
	void setNovelName( const QString &novel_name ) { name = novel_name.toStdWString( ); }
	void setRootUrl( const QString &root_url ) { rootUrl = root_url.toStdWString( ); }
	void setTypeName( const QString &type_name ) { typeName = type_name.toStdWString( ); }
	void setTypePageUrl( const QString &type_page_url ) { typePageUrl = type_page_url.toStdWString( ); }
	void setUpdateTime( const QString &update_time ) { updateTime = update_time.toStdWString( ); }
	void setUrl( const QString &url ) { this->url = url.toStdWString( ); }
public:	// - 继承
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
