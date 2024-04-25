#include "RequestNet.h"
#include <string>
RequestNet::RequestNet( QObject *parent ): QObject( parent ), url( GET_URL ) {
	qDebug( ) << u8"RequestNet::RequestNet( QObject *parent ) : " << url;
}

RequestNet::~RequestNet( ) {
	qDebug( ) << "RequestNet::~RequestNet( " << IRequestNetInterfaceExtend_iid << ")";

}
void RequestNet::getData( void *resultAnyPtr ) {

}
void RequestNet::setUrl( const StdString &url ) {
	this->url.setUrl( QString::fromStdString( url ) );
}
size_t RequestNet::getHost( StdString *outHost ) {
	QString host = this->url.host( );
	auto byteArray = host.toLocal8Bit( );
	*outHost = byteArray;
	return byteArray.length( );
}
size_t RequestNet::getScheme( StdString *outScheme ) {

	QString host = this->url.scheme( );
	auto byteArray = host.toLocal8Bit( );
	*outScheme = byteArray;
	return byteArray.length( );
}
size_t RequestNet::getUrl( StdString *outStr ) {
	QByteArray array = url.url( ).toLocal8Bit( );
	*outStr = array.toStdString( );
	qDebug( ) << array.data( ) << " RequestNet::getUrl( )[byteArray.data( )]";
	return array.length( );

}
void RequestNet::setHost( const StdString &host ) {
	url.setHost( QString::fromStdString( host ) );
}
void RequestNet::setScheme( const StdString &scheme ) {
	url.setScheme( QString::fromStdString( scheme ) );
}

bool RequestNet::setInterfaceParent( void *parent ) {
	qDebug( ) << "RequestNet::setInterfaceParent( void *" << parent << ")";
	QObject *object = reinterpret_cast< QObject * >( parent );
	QObject::setParent( object );
	return true;
}
void RequestNet::deleteMember( ) {
	deleteLater( );
}


IRequestNetInterface::un_ordered_map *RequestNet::formHtmlGetTypeTheUrls( const StdString &htmlText ) {
	return IRequestNetInterfaceExtend::formHtmlGetTypeTheUrls( htmlText );
}
IRequestNetInterface::NovelPtrList RequestNet::formHtmlGetTypePageNovels( const StdString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) {
	return IRequestNetInterfaceExtend::formHtmlGetTypePageNovels( htmlText, saveNovelInfos, appendDataPtr );
}
IRequestNetInterface::INovelInfoSharedPtr RequestNet::formHtmlGetUrlNovelInfo( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) {
	return IRequestNetInterfaceExtend::formHtmlGetUrlNovelInfo( htmlText, saveNovelInfos, networkReplayNovel );
}
IRequestNetInterface::StdString RequestNet::formHtmlGetNext( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) {
	return IRequestNetInterfaceExtend::formHtmlGetNext( htmlText, saveNovelInfos, lastNovelInfos );
}
