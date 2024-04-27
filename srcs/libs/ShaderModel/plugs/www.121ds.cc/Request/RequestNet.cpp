#include "RequestNet.h"
#include <string>

#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
using namespace interfacePlugsType;
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
	auto htmlDoc = HtmlTools::HtmlDoc::parse( stdWString, end, index );
	if( !htmlDoc.get( ) )
		return nullptr;
	auto vectorHtmlNodeSPtrShared = htmlDoc->getNodes( [=]( const HtmlTools::HtmlNode_Shared &html_node ) ->bool {
		std::shared_ptr< std::wstring > nodeWsName = htmlDoc->getNodeWSName( html_node );
		if( *nodeWsName == L"div" ) {
			HtmlTools::StdWString_Shared stdWStringShared = htmlDoc->getPath( html_node );
			if( *stdWStringShared == L"/html/body/div/div/div/div" )
				return true;
		}
		return false;
	} );
	size_t size = vectorHtmlNodeSPtrShared->size( );
	if( size == 0 )
		return nullptr;
	auto vectorIterator = vectorHtmlNodeSPtrShared->begin( );
	auto vectorEnd = vectorHtmlNodeSPtrShared->end( );
	for( ; vectorIterator != vectorEnd; ++vectorIterator ) {
		auto htmlNode = vectorIterator->get( );

		auto unorderedMap = htmlNode->analysisAttribute( );
		size_t mapSize = unorderedMap->size( );
		if( mapSize == 0 )
			continue;
		auto mapIterator = unorderedMap->begin( );
		auto mapEnd = unorderedMap->end( );
		for( ; mapIterator != mapEnd; ++mapIterator ) {
			auto key = mapIterator->first;
			if( key != L"class" )
				continue;
			auto value = mapIterator->second;
			HtmlDocString classVlaueName = L"hd";
			size_t find = value.find( classVlaueName );
			if( find < value.size( ) ) { // 找到了
				HtmlDocString substr = value.substr( find, classVlaueName.size( ) );
				auto msg = key + L":" + value;
				*oStream << htmlNode << u8"\t找到属性\t" << QString::fromStdWString( msg ) << '\n';
				qDebug( ) << *htmlNode->getWSNode( );
			}
		}
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
