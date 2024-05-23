#include "NovelInfo.h"
#include <QString>
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
	if( time ) {
		*result_time = time->toStdWString( );
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
size_t NovelInfo::getNovelUrlAtName( interfacePlugsType::HtmlDocString *result_type_name ) {
	if( typeName ) {
		*result_type_name = typeName->toStdWString( );
		return result_type_name->size( );
	}
	return 0;
}

void NovelInfo::clear( ) {
	novelName = info = time = format =
		lastRequestTime = lastRequestTimeFormat =
		author = url = lastItem = additionalData =
		typePageUrl = nullptr;
}
