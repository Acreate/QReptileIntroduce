#include "NovelNetJob.h"
#include <netWork/RequestConnect.h>
#include <netWork/Request.h>
#include <interface/IRequestNetInterfaceExtend.h>
#include <stream/OStream.h>

#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/HtmlNode/HtmlNode.h"
#include "thread/FileThread.h"

NovelNetJob::NovelNetJob( QObject *parent, OStream *o_stream, QObject *interface_obj_ptr, IRequestNetInterfaceExtend *interface_this_ptr ): QObject( parent ), interfaceObjPtr( interface_obj_ptr ), interfaceThisPtr( interface_this_ptr ), oStream( o_stream ) {

}

NovelNetJob::~NovelNetJob( ) {
	interfaceObjPtr->deleteLater( );
}
void NovelNetJob::start( ) {
	Request *request = new Request( this );
	RequestConnect *requestConnect = new RequestConnect( this );
	connect( requestConnect, &RequestConnect::networkReplyFinished, [=]( RequestConnect *request_connect )->void {
		QNetworkReply *networkReply = request_connect->getNetworkReply( );
		auto byteArray = networkReply->readAll( );
		if( byteArray.size( ) == 0 )
			return;
		*oStream << u8"请求完毕 : " << networkReply->url( ).host( ) << u8"(" << QThread::currentThread( )->currentThreadId( ) << u8")" << "\n";
		oStream->flush( );
		std::shared_ptr< std::wstring > stdWString( new std::wstring( QString( byteArray ).toStdWString( ) ) );
		size_t index = 0, end = stdWString->size( );
		auto htmlDoc = HtmlTools::HtmlDoc::parse( stdWString, end, index );
		if( !htmlDoc.get( ) )
			return;
		auto vectorHtmlNodeSPtrShared = htmlDoc->getNodes( [=]( const HtmlTools::HtmlNode_Shared &html_node ) ->bool {
			std::shared_ptr< std::wstring > nodeWsName = htmlDoc->getNodeWSName( html_node );
			if( *nodeWsName == L"div" ) {
				HtmlTools::StdWString_Shared stdWStringShared = htmlDoc->getPath( html_node );
				if( *stdWStringShared == L"/html/body/div/div/div/div" )
					return true;
			}
			oStream->flush( );
			return false;
		} );
		oStream->flush( );


		auto vectorIterator = vectorHtmlNodeSPtrShared->begin( );
		auto vectorEnd = vectorHtmlNodeSPtrShared->end( );
		for( ; vectorIterator != vectorEnd; ++vectorIterator ) {
			auto htmlNode = vectorIterator->get( );
			*oStream << u8"\t" << *htmlDoc->getWSNode( *vectorIterator ) << tr( u8"遍历属性" );
			oStream->flush( );
			auto unorderedMap = htmlNode->analysisAttribute( );
			size_t mapSize = unorderedMap->size( );
			*oStream << u8"\t" << mapSize << tr( u8"\n" );
			oStream->flush( );
			auto mapIterator = unorderedMap->begin( );
			auto mapEnd = unorderedMap->end( );
			for( ; mapIterator != mapEnd; ++mapIterator ) {
				auto key = mapIterator->first;
				auto value = mapIterator->second;
				*oStream << u8"\t\t" << key << u8"=" << value << "\n";
			}
		}

		request->deleteLater( );
		requestConnect->deleteLater( );
	} );
	std::string url;
	interfaceThisPtr->getUrl( &url );
	*oStream << u8"正在请求 : " << url << u8"(" << QThread::currentThread( )->currentThreadId( ) << u8")" << "\n";
	oStream->flush( );
	request->netGetWork( QString::fromStdString( url ), requestConnect );
}
QString NovelNetJob::getUrl( ) const {
	std::string resultUrl;
	size_t size = interfaceThisPtr->getUrl( &resultUrl );
	if( size )
		return QString::fromStdString( resultUrl );
	return "";
}
