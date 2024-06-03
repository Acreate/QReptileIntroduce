#include "NovelInfo.h"
#include <QString>

#include "dateTime/DateTime.h"
NovelInfo::NovelInfo( ) {
	clear( );
}
NovelInfo::~NovelInfo( ) {
}
size_t NovelInfo::getNovelName( interfacePlugsType::HtmlDocString *result_name ) {
	*result_name = name;
	return result_name->size( );
}
size_t NovelInfo::getNovelInfo( interfacePlugsType::HtmlDocString *result_info ) {
	*result_info = info;
	return result_info->size( );
}
size_t NovelInfo::getNovelUpdateTime( interfacePlugsType::HtmlDocString *result_time ) {
	*result_time = updateTime;
	return result_time->size( );
}
size_t NovelInfo::getNovelUpdateTimeFormat( interfacePlugsType::HtmlDocString *result_format ) {
	*result_format = format;
	return result_format->size( );
}
size_t NovelInfo::getNovelLastRequestGetTime( interfacePlugsType::HtmlDocString *result_last_request_time ) {
	*result_last_request_time = lastRequestTime;
	return result_last_request_time->size( );
}
size_t NovelInfo::getNovelLastRequestGetTimeFormat( interfacePlugsType::HtmlDocString *result_last_request_time_format ) {
	*result_last_request_time_format = lastRequestTimeFormat;
	return result_last_request_time_format->size( );
}
size_t NovelInfo::getNovelAuthor( interfacePlugsType::HtmlDocString *result_author ) {
	*result_author = author;
	return result_author->size( );
}
size_t NovelInfo::getNovelUrl( interfacePlugsType::HtmlDocString *result_url ) {
	*result_url = url;
	return result_url->size( );
}
size_t NovelInfo::getNovelLastItem( interfacePlugsType::HtmlDocString *result_last_item ) {
	*result_last_item = lastItem;
	return result_last_item->size( );
}
size_t NovelInfo::getNovelAttach( void *&result_additional_data ) {
	result_additional_data = additionalData;
	return result_additional_data != nullptr;
}
size_t NovelInfo::getNovelUrlAtPageLocation( interfacePlugsType::HtmlDocString *result_type_page_url ) {
	*result_type_page_url = typePageUrl;
	return result_type_page_url->size( );
}
size_t NovelInfo::getNovelTypeName( interfacePlugsType::HtmlDocString *result_type_name ) {
	*result_type_name = typeName;
	return result_type_name->size( );
}
#define SHARED_PTR_TO_RESULT( shared_ptr , default) ((shared_ptr.empty(  ))? ( QString::fromStdWString(shared_ptr  )):(default))
size_t NovelInfo::objToHtmlDocString( interfacePlugsType::HtmlDocString *result_serializable_html_doc_string ) {

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
		result.arg( SHARED_PTR_TO_RESULT( name, "" ) )
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


void NovelInfo::clear( ) {
	name = info = updateTime = format =
		lastRequestTime = lastRequestTimeFormat =
		author = url = lastItem = typePageUrl = L"";
	additionalData = nullptr;
}
