#include "RequestNet.h"
#include <string>

#include <QFile>
#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/htmlTools/XPath.h"
#include "macro/cmake_to_c_cpp_header_env.h"
using namespace interfacePlugsType;
using namespace cylHtmlTools;
RequestNet::RequestNet( QObject *parent ): QObject( parent ), url( GET_URL ), oStream( nullptr ), iStream( nullptr ), typeUrlMap( nullptr ) {
}

RequestNet::~RequestNet( ) {
	qDebug( ) << "RequestNet::~RequestNet( " << IRequestNetInterface_iid << ")";

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


Map_HtmlStrK_HtmlStrV * RequestNet::formHtmlGetTypeTheUrls( const HtmlDocString &htmlText ) {
	auto removeBothSpaceHtmlText = htmlText;
	HtmlStringTools::removeBothSpace( removeBothSpaceHtmlText );
	if( removeBothSpaceHtmlText.size( ) > 0 ) {
		auto result = std::make_shared< Map_HtmlStrK_HtmlStrV >( );
		auto stdWString( std::make_shared< HtmlString >( removeBothSpaceHtmlText ) );
		size_t index = 0, end = stdWString->size( );
		auto htmlDoc = cylHtmlTools::HtmlDoc::parse( stdWString, end, index );
		if( !htmlDoc.get( ) )
			return nullptr;

		htmlDoc->analysisBrotherNode( );
		auto xpath = cylHtmlTools::XPath( QString( u8"div[@class='hd']/ul/li/a" ).toStdWString( ) );

		auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
		auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
		if( !vectorHtmlNodeSPtrShared )
			return nullptr;
		auto vectorIterator = vectorHtmlNodeSPtrShared->begin( );
		auto vectorEnd = vectorHtmlNodeSPtrShared->end( );

		for( ; vectorIterator != vectorEnd; ++vectorIterator ) {

			qDebug( ) << QString::fromStdWString( *vectorIterator->get( )->getNodeContent( ) );

			auto element = vectorIterator->get( );
			QString url;
			auto unorderedMap = element->findAttribute( [&]( const HtmlString &first, const HtmlString &scen ) ->bool {
				if( HtmlStringTools::equRemoveSpaceOverHtmlString( first, L"href" ) )
					return true;
				return false;
			} );
			if( unorderedMap ) {
				auto key = *element->getNodeContentText( );
				auto value = unorderedMap->at( L"href" );
				QString qulr = QString( u8"%1%2" ).arg( GET_URL ).arg( value.substr( 1, value.size( ) - 2 ) );
				value = qulr.toStdWString( );
				result->emplace( key, value );
			}
		}
		if( result->size( ) > 0 ) {
			typeUrlMap = result;
			return typeUrlMap.get( );
		}
	}
	if( typeUrlMap == nullptr || typeUrlMap->size( ) == 0 )
		return nullptr;
	return typeUrlMap.get( );
}
Vector_NovelSPtr RequestNet::formHtmlGetTypePageNovels( const HtmlDocString &htmlText, const Vector_NovelSPtr &saveNovelInfos, void *appendDataPtr ) {
	Vector_NovelSPtr result;
	auto stdWString( std::make_shared< HtmlString >( htmlText ) );
	size_t index = 0, end = stdWString->size( );
	auto htmlDoc = cylHtmlTools::HtmlDoc::parse( stdWString, end, index );
	if( htmlDoc ) {

		htmlDoc->analysisBrotherNode( );
		auto xpath = cylHtmlTools::XPath( QString( u8"div[@class='cf' @id='sitebox']/dl" ).toStdWString( ) );

		auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
		auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
		if( !vectorHtmlNodeSPtrShared )
			return result;
		auto vectorIterator = vectorHtmlNodeSPtrShared->begin( );
		auto vectorEnd = vectorHtmlNodeSPtrShared->end( );

		for( ; vectorIterator != vectorEnd; ++vectorIterator ) {
			qDebug( ) << QString::fromStdWString( *vectorIterator->get( )->getIncludeNodeContent( ) );
		}
	}
	return result;
}
INovelInfo_Shared RequestNet::formHtmlGetUrlNovelInfo( const HtmlDocString &htmlText, const Vector_NovelSPtr &saveNovelInfos, const INovelInfo_Shared &networkReplayNovel ) {
	INovelInfo_Shared result = nullptr;
	auto stdWString( std::make_shared< HtmlString >( htmlText ) );
	size_t index = 0, end = stdWString->size( );
	auto htmlDoc = cylHtmlTools::HtmlDoc::parse( stdWString, end, index );
	if( htmlDoc ) {

	}

	return result;
}
HtmlDocString RequestNet::formHtmlGetNext( const HtmlDocString &htmlText, const Vector_NovelSPtr &saveNovelInfos, const Vector_NovelSPtr &lastNovelInfos ) {
	return { };
}
bool RequestNet::isRequestNovelInfoUrl( const interfacePlugsType::INovelInfoPtr &novel_info_ptr ) {
	return false;
}
void RequestNet::novelTypeEnd( const interfacePlugsType::Vector_NovelSPtr &saveNovelInfos ) {
}
void RequestNet::endHost( const interfacePlugsType::Vector_NovelSPtr &saveNovelInfos ) {
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
