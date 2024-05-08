#include "RequestNet.h"
#include <string>

#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/htmlTools/XPath.h"
using namespace interfacePlugsType;
using namespace cylHtmlTools;
RequestNet::RequestNet( QObject *parent ): QObject( parent ), url( GET_URL ), oStream( nullptr ), iStream( nullptr ) {
}

RequestNet::~RequestNet( ) {
	qDebug( ) << "RequestNet::~RequestNet( " << IRequestNetInterfaceExtend_iid << ")";

}
void RequestNet::getData( void *resultAnyPtr ) {

}
void RequestNet::setUrl( const HtmlDocString &url ) {
	this->url.setUrl( QString::fromStdWString( url ) );
}
size_t RequestNet::getHost( HtmlDocString *outHost ) {
	QString host = this->url.host( );
	auto byteArray = host.toLocal8Bit( );
	*outHost = QString( byteArray ).toStdWString( );
	return byteArray.length( );
}
size_t RequestNet::getScheme( HtmlDocString *outScheme ) {

	QString host = this->url.scheme( );
	auto byteArray = host.toLocal8Bit( );
	*outScheme = QString( byteArray ).toStdWString( );
	return byteArray.length( );
}
size_t RequestNet::getUrl( HtmlDocString *outStr ) {
	QByteArray array = url.url( ).toLocal8Bit( );
	*outStr = QString( array ).toStdWString( );
	return array.length( );

}
void RequestNet::setHost( const HtmlDocString &host ) {
	url.setHost( QString::fromStdWString( host ) );
}
void RequestNet::setScheme( const HtmlDocString &scheme ) {
	url.setScheme( QString::fromStdWString( scheme ) );
}

bool RequestNet::setInterfaceParent( void *parent ) {
	QObject *object = reinterpret_cast< QObject * >( parent );
	QObject::setParent( object );
	return true;
}
void RequestNet::deleteMember( ) {
	deleteLater( );
}


un_ordered_map * RequestNet::formHtmlGetTypeTheUrls( const HtmlDocString &htmlText ) {

	std::shared_ptr< std::wstring > stdWString( new std::wstring( htmlText ) );
	size_t index = 0, end = stdWString->size( );
	auto htmlDoc = cylHtmlTools::HtmlDoc::parse( stdWString, end, index );
	if( !htmlDoc.get( ) )
		return nullptr;
	auto xpath = cylHtmlTools::XPath( QString( u8"div[@class='hd']" ).toStdWString( ) );
	htmlDoc->analysisBrotherNode( );
	auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
	auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
	if( !vectorHtmlNodeSPtrShared )
		return nullptr;
	size_t size = vectorHtmlNodeSPtrShared->size( );
	if( size == 0 )
		return nullptr;
	auto vectorIterator = vectorHtmlNodeSPtrShared->begin( );
	auto vectorEnd = vectorHtmlNodeSPtrShared->end( );
	for( ; vectorIterator != vectorEnd; ++vectorIterator ) {
		qDebug( ) << QString::fromStdWString( *vectorIterator->get( )->getNodeContent( ) );
	}
	oStream->flush( );
	return nullptr;
}
NovelPtrList RequestNet::formHtmlGetTypePageNovels( const HtmlDocString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) {
	return IRequestNetInterfaceExtend::formHtmlGetTypePageNovels( htmlText, saveNovelInfos, appendDataPtr );
}
INovelInfoSharedPtr RequestNet::formHtmlGetUrlNovelInfo( const HtmlDocString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) {
	return IRequestNetInterfaceExtend::formHtmlGetUrlNovelInfo( htmlText, saveNovelInfos, networkReplayNovel );
}
HtmlDocString RequestNet::formHtmlGetNext( const HtmlDocString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) {
	return IRequestNetInterfaceExtend::formHtmlGetNext( htmlText, saveNovelInfos, lastNovelInfos );
}
OStream * RequestNet::setOStream( OStream *o_stream ) {
	auto oldOStream = this->oStream;
	this->oStream = o_stream;
	return oldOStream;
}
IStream * RequestNet::setIStream( IStream *i_stream ) {
	auto oldIStream = this->iStream;
	this->iStream = i_stream;
	return oldIStream;
}
