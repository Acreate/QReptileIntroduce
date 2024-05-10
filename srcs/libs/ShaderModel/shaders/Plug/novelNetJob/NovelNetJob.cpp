#include "NovelNetJob.h"
#include <HttpNetWork/RequestConnect.h>
#include <HttpNetWork/Request.h>
#include <HttpNetWork/NetworkRequest.h>
#include <stream/OStream.h>

#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/htmlTools/XPath.h"
#include "htmls/HtmlNode/HtmlNode.h"
#include "thread/FileThread.h"
#include <HttpNetWork/NetworkAccessManager.h>
#include <qguiapplication.h>

#include "interface/INovelInfo.h"
using namespace interfacePlugsType;
NovelNetJob::NovelNetJob( OStream *o_stream, QObject *interface_obj_ptr, IRequestNetInterface *interface_this_ptr ): interfaceObjPtr( interface_obj_ptr ), interfaceThisPtr( interface_this_ptr ), oStream( o_stream ) {
	interface_this_ptr->setOStream( oStream );
	initObj( );
}

NovelNetJob::~NovelNetJob( ) {
	interfaceObjPtr->deleteLater( );
}
void NovelNetJob::initObj( ) {
}
#define Q_CONNECT(sing_ptr, sing_type, rev_ptr, rev_type, connect_name , connectType) \
	connect(sing_ptr,&sing_type::connect_name ,rev_ptr,&rev_type::connect_name,connectType)
#define Q_CONNECT_THIS(  send_name , rev_name) \
	connect(this,&NovelNetJob::send_name ,this,&NovelNetJob::rev_name)


void NovelNetJob::initConnect( ) {
	
}
bool NovelNetJob::start( ) {
	HtmlDocString resultUrl;
	size_t size = interfaceThisPtr->getUrl( &resultUrl );
	if( size == 0 )
		return false;
	return true;
}
QString NovelNetJob::getUrl( ) const {
	HtmlDocString resultUrl;
	size_t size = interfaceThisPtr->getUrl( &resultUrl );
	if( size )
		return QString::fromStdWString( resultUrl );
	return "";
}
void NovelNetJob::slots_requesting_get_root_page_signals( const QUrl &url ) {
	qDebug( ) << u8"slots_requesting_get_root_page_signals" << url;
}
void NovelNetJob::slots_requesting_get_type_page_url_signals( const QString &root_url, const QString &type, const QUrl &url ) {
}
void NovelNetJob::slots_requesting_get_next_type_page_url_signals( const QString &root_url, const QString &type, const QUrl &old_url, const QUrl &url, size_t old_page_index, size_t current_page_index, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared ) {
}
void NovelNetJob::slots_requesting_get_novel_page_url_signals( const QString &root_url, const QString &type, const QString &type_page_url, const QString &novelName, const QUrl &url, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared ) {
}
void NovelNetJob::slots_requested_get_novel_page_url_signals( const QString &root_url, const QString &type, const QString &type_page_url, const QString &novelName, const QUrl &url, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared, const interfacePlugsType::INovelInfo_Shared novel_info_shared ) {
}
void NovelNetJob::slots_requested_get_type_page_url_end( const QString &root_url, const QString &type, const QUrl &url, size_t current_page_index, const interfacePlugsType::Vector_NovelSPtr_Shared novel_s_ptr_shared ) {
}
void NovelNetJob::slots_requested_get_web_page_signals_end( const QUrl &url ) {
}
