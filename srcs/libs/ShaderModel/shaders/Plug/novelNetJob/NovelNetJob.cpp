#include "NovelNetJob.h"
#include <netWork/RequestConnect.h>
#include <netWork/Request.h>
NovelNetJob::NovelNetJob( QObject *parent, QObject *interface_obj_ptr, IRequestNetInterface *interface_this_ptr ): QObject( parent ), interfaceObjPtr( interface_obj_ptr ), interfaceThisPtr( interface_this_ptr ) {

}

NovelNetJob::~NovelNetJob( ) {
}
void NovelNetJob::getData( void *resultAnyPtr ) {
	interfaceThisPtr->getData( resultAnyPtr );
}
size_t NovelNetJob::getUrl( StdString *outStr ) {
	return interfaceThisPtr->getUrl( outStr );
}
size_t NovelNetJob::getHost( StdString *outHost ) {
	return interfaceThisPtr->getHost( outHost );
}
size_t NovelNetJob::getScheme( StdString *outScheme ) {
	return interfaceThisPtr->getScheme( outScheme );
}
void NovelNetJob::setUrl( const StdString &url ) {
	interfaceThisPtr->setUrl( url );
}
void NovelNetJob::setHost( const StdString &host ) {
	interfaceThisPtr->setHost( host );
}
void NovelNetJob::setScheme( const StdString &scheme ) {
	interfaceThisPtr->setScheme( scheme );
}
IRequestNetInterface::un_ordered_map * NovelNetJob::formHtmlGetTypeTheUrls( const StdString &htmlText ) {
	return interfaceThisPtr->formHtmlGetTypeTheUrls( htmlText );
}
IRequestNetInterface::NovelPtrList NovelNetJob::formHtmlGetTypePageNovels( const StdString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) {
	return interfaceThisPtr->formHtmlGetTypePageNovels( htmlText, saveNovelInfos, appendDataPtr );
}
IRequestNetInterface::INovelInfoSharedPtr NovelNetJob::formHtmlGetUrlNovelInfo( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) {
	return interfaceThisPtr->formHtmlGetUrlNovelInfo( htmlText, saveNovelInfos, networkReplayNovel );
}
IRequestNetInterface::StdString NovelNetJob::formHtmlGetNext( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) {
	return interfaceThisPtr->formHtmlGetNext( htmlText, saveNovelInfos, lastNovelInfos );
}
void NovelNetJob::novelTypeEnd( const NovelPtrList &saveNovelInfos ) {
	interfaceThisPtr->novelTypeEnd( saveNovelInfos );
}
void NovelNetJob::endHost( const NovelPtrList &saveNovelInfos ) {
	interfaceThisPtr->endHost( saveNovelInfos );
}
bool NovelNetJob::setInterfaceParent( void *parent ) {
	return interfaceThisPtr->setInterfaceParent( parent );
}
void NovelNetJob::deleteMember( ) {
	interfaceObjPtr->deleteLater( );
	this->deleteLater( );
}
QNetworkReply * NovelNetJob::start( RequestConnect *request_connect ) {
	Request *request = new Request( this );
	std::string url;
	getUrl( &url );
	return request->netGetWork( QString::fromStdString( url ), request_connect );
}
