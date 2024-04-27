#include "NovelNetJob.h"
#include <netWork/RequestConnect.h>
#include <netWork/Request.h>
#include <interface/IRequestNetInterfaceExtend.h>
#include <stream/OStream.h>

#include "htmls/htmlDoc/HtmlDoc.h"
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
				if( *stdWStringShared == L"/html/body/div/div/div/div" ) {
					*oStream << '\t' << *htmlDoc->getWSNode( html_node ) << '\t' << tr( u8"遍历节点属性->" ) << "\n";
					htmlDoc->findAttribute( html_node, [=]( HtmlTools::WStringPairUnorderMap_Shared map ) ->bool {
						if( map->size( ) == 0 )
							return false;
						auto mapIterator = map->begin( );
						auto mapEnd = map->end( );
						for( ; mapIterator != mapEnd; ++mapIterator ) {
							std::wstring first = mapIterator->first;
							std::wstring second = mapIterator->second;
							*oStream << "\t\t" << first << '=' << second << "\n";
							if( first == L"class" && second == L"hd" ) {
								*oStream << "\t\t" << tr( u8"找到节点" ) << "\n";
								return true;
							}
						}
						return false;
					} );
					return true;
				}
			}
			oStream->flush( );
			return false;
		} );
		oStream->flush( );
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
