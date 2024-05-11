#include "RequestNet.h"
#include <string>

#include <QFile>
#include <QDir>
#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/htmlTools/XPath/XPath.h"
#include <htmls/htmlTools/HtmlWorkThread/HtmlWorkThread.h>
#include <qguiapplication.h>

#include "macro/cmake_to_c_cpp_header_env.h"
#include <stream/OStream.h>
using namespace interfacePlugsType;
using namespace cylHtmlTools;


RequestNet::RequestNet( QObject *parent ): QObject( parent )
, rootUrl( GET_URL )
, oStream( nullptr )
, iStream( nullptr )
, typeUrlMap( nullptr ) {
}

RequestNet::~RequestNet( ) {
	qDebug( ) << "RequestNet::~RequestNet( " << IRequestNetInterface_iid << ")";

}
void RequestNet::getData( void *resultAnyPtr ) {

}
void RequestNet::setRootUrl( const HtmlDocString &url ) {
	this->rootUrl.setUrl( QString::fromStdWString( url ) );
}
size_t RequestNet::getHost( HtmlDocString *outHost ) {
	QString host = this->rootUrl.host( );
	auto byteArray = host.toLocal8Bit( );
	*outHost = QString( byteArray ).toStdWString( );
	return byteArray.length( );
}
size_t RequestNet::getScheme( HtmlDocString *outScheme ) {

	QString host = this->rootUrl.scheme( );
	auto byteArray = host.toLocal8Bit( );
	*outScheme = QString( byteArray ).toStdWString( );
	return byteArray.length( );
}
size_t RequestNet::getRootUrl( HtmlDocString *outStr ) {
	QByteArray array = rootUrl.url( ).toLocal8Bit( );
	*outStr = QString( array ).toStdWString( );
	return array.length( );

}
void RequestNet::setHost( const HtmlDocString &host ) {
	rootUrl.setHost( QString::fromStdWString( host ) );
}
void RequestNet::setScheme( const HtmlDocString &scheme ) {
	rootUrl.setScheme( QString::fromStdWString( scheme ) );
}

bool RequestNet::setInterfaceParent( void *parent ) {
	QObject *object = reinterpret_cast< QObject * >( parent );
	QObject::setParent( object );
	return true;
}
void RequestNet::deleteMember( ) {
	deleteLater( );
}


Map_HtmlStrK_HtmlStrV * RequestNet::formHtmlGetTypeTheUrls( const interfacePlugsType::HtmlDocString &url, const HtmlDocString &htmlText ) {
	do {
		auto removeBothSpaceHtmlText = htmlText;
		HtmlStringTools::removeBothSpace( removeBothSpaceHtmlText );
		if( removeBothSpaceHtmlText.size( ) == 0 )
			break;
		HtmlWorkThread< std::shared_ptr< HtmlString > > thread;
		auto stdWString( std::make_shared< HtmlString >( removeBothSpaceHtmlText ) );
		auto result = std::make_shared< Map_HtmlStrK_HtmlStrV >( );
		thread.setData( stdWString );
		thread.setCurrentThreadRun( [&result,&stdWString, this,&url](
			const HtmlWorkThread< std::shared_ptr< HtmlString > > *html_work_thread,
			const std::thread *run_std_cpp_thread, std::mutex *html_work_thread_mutex,
			std::mutex *std_cpp_thread_mutex, std::shared_ptr< HtmlString > &data,
			const time_t *startTime ) {
				size_t index = 0, end = stdWString->size( );
				auto htmlDoc = cylHtmlTools::HtmlDoc::parse( stdWString, end, index );
				if( !htmlDoc.get( ) ) {
					auto msg = QString( "%1 : %2" ).arg( QString::fromStdWString( url ) ).arg( QString( u8" HtmlDoc::parse 异常，登出" ) ).toStdString( );
					OStream::errorQDebugOut( msg, __FILE__, __LINE__ );
					return;
				}

				htmlDoc->analysisBrotherNode( );
				auto xpath = cylHtmlTools::XPath( QString( tr( u8"div[@class='hd']/ul/li/a" ) ).toStdWString( ) );

				auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
				auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
				if( !vectorHtmlNodeSPtrShared ) {
					auto msg = QString( "%1 : %2 " ).arg( QString::fromStdWString( url ) ).arg( QString( u8" xpath 异常，登出" ) ).toStdString( );
					OStream::errorQDebugOut( msg, __FILE__, __LINE__ );
					return;
				}
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
			} );
		thread.start( );
		while( !thread.isFinish( ) )
			qApp->processEvents( );
		if( result->size( ) > 0 )
			typeUrlMap = result;
	} while( false );
	if( typeUrlMap == nullptr || typeUrlMap->size( ) == 0 )
		return nullptr;
	return typeUrlMap.get( );
}
Vector_NovelSPtr RequestNet::formHtmlGetTypePageNovels( const interfacePlugsType::HtmlDocString &type_name, const interfacePlugsType::HtmlDocString &request_url, const HtmlDocString &htmlText, const Vector_NovelSPtr &saveNovelInfos, void *appendDataPtr ) {
	Vector_NovelSPtr result;
	do {
		auto stdWString( std::make_shared< HtmlString >( htmlText ) );
		size_t index = 0, end = stdWString->size( );
		auto htmlDoc = cylHtmlTools::HtmlDoc::parse( stdWString, end, index );
		if( !htmlDoc ) {
			auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( QString( u8" HtmlDoc::parse 异常，登出" ) );
			auto path = QString( Cache_Path_Dir ).append( QDir::separator( ) ).append( type_name ).append( u8".html" );
			OStream::errorQDebugOut( msg.toStdString( ), __FILE__, __LINE__, path, QString::fromStdWString( htmlText ).toLocal8Bit( ) );
			break;
		}
		htmlDoc->analysisBrotherNode( );
		auto xpath = cylHtmlTools::XPath( QString( tr( u8"div[@class='cf']" ) ).toStdWString( ) );
		auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
		auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
		if( !vectorHtmlNodeSPtrShared ) {
			auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( QString( u8" xpath 异常，登出" ) );
			auto path = QString( Cache_Path_Dir ).append( QDir::separator( ) ).append( type_name ).append( u8".html" );
			OStream::errorQDebugOut( msg.toStdString( ), __FILE__, __LINE__, path, QString::fromStdWString( htmlText ).toLocal8Bit( ) );
			break;
		}
		auto vectorIterator = vectorHtmlNodeSPtrShared->begin( );
		auto vectorEnd = vectorHtmlNodeSPtrShared->end( );
		for( ; vectorIterator != vectorEnd; ++vectorIterator )
			qDebug( ) << QString::fromStdWString( *vectorIterator->get( )->getIncludeNodeContent( ) );
	} while( false );
	return result;
}
INovelInfo_Shared RequestNet::formHtmlGetUrlNovelInfo( const interfacePlugsType::HtmlDocString &url, const HtmlDocString &htmlText, const Vector_NovelSPtr &saveNovelInfos, const INovelInfo_Shared &networkReplayNovel ) {
	INovelInfo_Shared result = nullptr;
	do {
		auto stdWString( std::make_shared< HtmlString >( htmlText ) );
		size_t index = 0, end = stdWString->size( );
		auto htmlDoc = cylHtmlTools::HtmlDoc::parse( stdWString, end, index );
		if( htmlDoc )
			break;
	} while( false );

	return result;
}
HtmlDocString RequestNet::formHtmlGetNext( const interfacePlugsType::HtmlDocString &type_name, const interfacePlugsType::HtmlDocString &url, const HtmlDocString &htmlText, const Vector_NovelSPtr &saveNovelInfos, const Vector_NovelSPtr &lastNovelInfos ) {
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
