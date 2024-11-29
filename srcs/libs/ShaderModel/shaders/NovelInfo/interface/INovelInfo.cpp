#include "INovelInfo.h"
#include <QDateTime>
#include <QString>
using namespace interfacePlugsType;

size_t INovelInfo::getRootUrl( HtmlDocString *result_root_url ) {
	*result_root_url = rootUrl;
	return rootUrl.size( );
}
size_t INovelInfo::getNovelName( interfacePlugsType::HtmlDocString *result_name ) {
	*result_name = novelName;
	return result_name->size( );
}
size_t INovelInfo::getNovelInfo( interfacePlugsType::HtmlDocString *result_info ) {
	*result_info = info;
	return result_info->size( );
}
size_t INovelInfo::getNovelUpdateTime( interfacePlugsType::HtmlDocString *result_time ) {
	*result_time = this->updateTime;
	return result_time->size( );
}
size_t INovelInfo::getNovelUpdateTimeFormat( interfacePlugsType::HtmlDocString *result_format ) {
	*result_format = this->format;
	return result_format->size( );
}
size_t INovelInfo::getNovelLastRequestGetTime( interfacePlugsType::HtmlDocString *result_last_request_time ) {
	*result_last_request_time = this->lastRequestTime;
	return result_last_request_time->size( );
}
size_t INovelInfo::getNovelLastRequestGetTimeFormat( interfacePlugsType::HtmlDocString *result_last_request_time_format ) {
	*result_last_request_time_format = this->lastRequestTimeFormat;
	return result_last_request_time_format->size( );
}
size_t INovelInfo::getNovelAuthor( interfacePlugsType::HtmlDocString *result_author ) {
	*result_author = this->author;
	return result_author->size( );
}
size_t INovelInfo::getNovelUrl( interfacePlugsType::HtmlDocString *result_url ) {
	*result_url = this->url;
	return result_url->size( );
}
size_t INovelInfo::getNovelLastItem( interfacePlugsType::HtmlDocString *result_last_item ) {
	*result_last_item = this->lastItem;
	return result_last_item->size( );
}
size_t INovelInfo::getNovelAttach( interfacePlugsType::HtmlDocString *result_additional_data ) {
	*result_additional_data = this->additionalData;
	return result_additional_data->size( );
}
size_t INovelInfo::getNovelUrlAtPageLocation( interfacePlugsType::HtmlDocString *result_type_page_url ) {
	*result_type_page_url = this->typePageUrl;
	return result_type_page_url->size( );
}
size_t INovelInfo::getNovelTypeName( interfacePlugsType::HtmlDocString *result_type_name ) {
	*result_type_name = this->typeName;
	return result_type_name->size( );
}
void INovelInfo::setRootUrl( const HtmlDocString &result_root_url ) {
	this->rootUrl = result_root_url;
}
void INovelInfo::setNovelName( const HtmlDocString &name ) {
	this->novelName = name;
}
void INovelInfo::setNovelInfo( const HtmlDocString &info ) {
	this->info = info;
}
void INovelInfo::setNovelUpdateTime( const HtmlDocString &time ) {
	this->updateTime = time;
}
void INovelInfo::setNovelUpdateTimeFormat( const HtmlDocString &format ) {
	this->format = format;
}
void INovelInfo::setNovelLastRequestGetTime( const HtmlDocString &last_request_time ) {
	this->lastRequestTime = last_request_time;
}
void INovelInfo::setNovelLastRequestGetTimeFormat( const HtmlDocString &last_request_time_format ) {
	this->lastRequestTimeFormat = last_request_time_format;
}
void INovelInfo::setNovelAuthor( const HtmlDocString &author ) {
	this->author = author;
}
void INovelInfo::setNovelUrl( const HtmlDocString &url ) {
	this->url = url;
}
void INovelInfo::setNovelLastItem( const HtmlDocString &last_item ) {
	this->lastItem = last_item;
}
void INovelInfo::setNovelAttach( interfacePlugsType::HtmlDocString &result_additional_data ) {
	this->additionalData = result_additional_data;
}
void INovelInfo::setNovelUrlAtPageLocation( const HtmlDocString &type_page_url ) {
	this->typePageUrl = type_page_url;
}
void INovelInfo::setNovelTypeName( const HtmlDocString &type_name ) {
	this->typeName = type_name;
}

#define SHARED_PTR_TO_RESULT( shared_ptr , default) (((shared_ptr).empty(  ))? (default):( QString::fromStdWString(shared_ptr  )))
size_t INovelInfo::objToHtmlDocString( interfacePlugsType::HtmlDocString *result_serializable_html_doc_string ) {

	QString result(
		u8"\t名称: %1\n"
		u8"\t类型: %2\n"
		u8"\t作者: %3\n"
		u8"\t最后更新项: %4\n"
		u8"\t更新日期: %5\n"
		u8"\t获取日期: %6\n"
		u8"\t地址: %7\n"
		u8"\t详情: %8"
	);
	*result_serializable_html_doc_string =
		result.arg( SHARED_PTR_TO_RESULT( novelName, "" ) )
			.arg( SHARED_PTR_TO_RESULT( typeName, "" ) )
			.arg( SHARED_PTR_TO_RESULT( author, "" ) )
			.arg( SHARED_PTR_TO_RESULT( lastItem, "" ) )
			.arg( SHARED_PTR_TO_RESULT( updateTime, "" ) )
			.arg( SHARED_PTR_TO_RESULT( lastRequestTime, QDateTime::currentDateTime( ).toString( "yyyy-MM-dd hh:mm:ss" ) ) )
			.arg( SHARED_PTR_TO_RESULT( url, "" ) )
			.arg( SHARED_PTR_TO_RESULT( info, "" ) )
			.toStdWString( );

	return result_serializable_html_doc_string->size( );
}
