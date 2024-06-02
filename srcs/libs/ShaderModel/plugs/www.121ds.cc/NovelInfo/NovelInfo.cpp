#include "NovelInfo.h"
#include <QString>

#include "dateTime/DateTime.h"
NovelInfo::NovelInfo( ) {
	clear( );
}
NovelInfo::~NovelInfo( ) {
}
size_t NovelInfo::getNovelName( interfacePlugsType::HtmlDocString *result_name ) {
	if( novelName ) {
		*result_name = novelName->toStdWString( );
		return result_name->size( );
	}
	return 0;
}
size_t NovelInfo::getNovelInfo( interfacePlugsType::HtmlDocString *result_info ) {
	if( info ) {
		*result_info = info->toStdWString( );
		return result_info->size( );
	}
	return 0;
}
size_t NovelInfo::getNovelUpdateTime( interfacePlugsType::HtmlDocString *result_time ) {
	if( updateTime ) {
		*result_time = updateTime->toStdWString( );
		return result_time->size( );
	}
	return 0;
}
size_t NovelInfo::getNovelUpdateTimeFormat( interfacePlugsType::HtmlDocString *result_format ) {
	if( format ) {
		*result_format = format->toStdWString( );
		return result_format->size( );
	}
	return 0;
}
size_t NovelInfo::getNovelLastRequestGetTime( interfacePlugsType::HtmlDocString *result_last_request_time ) {
	if( lastRequestTime ) {
		*result_last_request_time = lastRequestTime->toStdWString( );
		return result_last_request_time->size( );
	}
	return 0;
}
size_t NovelInfo::getNovelLastRequestGetTimeFormat( interfacePlugsType::HtmlDocString *result_last_request_time_format ) {
	if( lastRequestTimeFormat ) {
		*result_last_request_time_format = lastRequestTimeFormat->toStdWString( );
		return result_last_request_time_format->size( );
	}
	return 0;
}
size_t NovelInfo::getNovelAuthor( interfacePlugsType::HtmlDocString *result_author ) {
	if( author ) {
		*result_author = author->toStdWString( );
		return result_author->size( );
	}
	return 0;
}
size_t NovelInfo::getNovelUrl( interfacePlugsType::HtmlDocString *result_url ) {
	if( url ) {
		*result_url = url->toStdWString( );
		return result_url->size( );
	}
	return 0;
}
size_t NovelInfo::getNovelLastItem( interfacePlugsType::HtmlDocString *result_last_item ) {
	if( lastItem ) {
		*result_last_item = lastItem->toStdWString( );
		return result_last_item->size( );
	}
	return 0;
}
size_t NovelInfo::getNovelAttach( void *&result_additional_data ) {
	if( additionalData ) {
		result_additional_data = additionalData.get( );
		return additionalData->size( );
	}
	return 0;
}
size_t NovelInfo::getNovelUrlAtPageLocation( interfacePlugsType::HtmlDocString *result_type_page_url ) {
	if( typePageUrl ) {
		*result_type_page_url = typePageUrl->toStdWString( );
		return result_type_page_url->size( );
	}
	return 0;
}
size_t NovelInfo::getNovelTypeName( interfacePlugsType::HtmlDocString *result_type_name ) {
	if( typeName ) {
		*result_type_name = typeName->toStdWString( );
		return result_type_name->size( );
	}
	return 0;
}
#define SHARED_PTR_TO_RESULT( shared_ptr , default) ((shared_ptr)? *(shared_ptr):(default))
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

void NovelInfo::clear( ) {
	novelName = info = updateTime = format =
		lastRequestTime = lastRequestTimeFormat =
		author = url = lastItem = additionalData =
		typePageUrl = nullptr;
}
