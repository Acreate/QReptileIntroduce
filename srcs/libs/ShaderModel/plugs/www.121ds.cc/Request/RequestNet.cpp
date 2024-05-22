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
#include <HttpNetWork/NetworkRequest.h>
#include <HttpNetWork/Request.h>

#include "../NovelInfo/NovelInfo.h"
using namespace interfacePlugsType;
using namespace cylHtmlTools;


RequestNet::RequestNet( QObject *parent ): QObject( parent )
, rootUrl( GET_URL )
, oStream( nullptr )
, iStream( nullptr )
, typeUrlMap( nullptr ) {
	cylHttpNetWork::NetworkRequest::initTools( );
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
	QObject *object = static_cast< QObject * >( parent );
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
					OStream::errorQDebugOut( msg, __FILE__, __LINE__, __FUNCTION__ );
					return;
				}

				htmlDoc->analysisBrotherNode( );
				auto xpath = cylHtmlTools::XPath( QString( tr( u8"div[@class='hd']/ul/li/a" ) ).toStdWString( ) );

				auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
				auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
				if( !vectorHtmlNodeSPtrShared ) {
					auto msg = QString( "%1 : %2 " ).arg( QString::fromStdWString( url ) ).arg( QString( u8" xpath 异常，登出" ) ).toStdString( );
					OStream::errorQDebugOut( msg, __FILE__, __LINE__, __FUNCTION__ );
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
			OStream::errorQDebugOut( msg.toStdString( ), __FILE__, __LINE__, __FUNCTION__, path, QString::fromStdWString( htmlText ) );
			break;
		}
		htmlDoc->analysisBrotherNode( );
		auto xpath = cylHtmlTools::XPath( QString( tr( u8"div[@class='cf' @id='sitebox' ]/dl" ) ).toStdWString( ) );
		auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
		auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
		if( !vectorHtmlNodeSPtrShared ) {
			auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( QString( u8" xpath 异常，登出" ) );
			auto path = QString( Cache_Path_Dir ).append( QDir::separator( ) ).append( type_name ).append( u8".html" );
			OStream::errorQDebugOut( msg.toStdString( ), __FILE__, __LINE__, __FUNCTION__, path, QString::fromStdWString( htmlText ) );
			break;
		}
		auto vectorIterator = vectorHtmlNodeSPtrShared->begin( );
		auto vectorEnd = vectorHtmlNodeSPtrShared->end( );
		INovelInfoPtr novelInfoPtr = nullptr;
		Vector_HtmlNodeSPtr_Shared htmlNodes = nullptr;
		HtmlString_Shared content = nullptr;
		QString rootUrl = GET_URL;
		auto novelInfoBuffPtr = new NovelInfo;
		std::vector< std::shared_ptr< HtmlNode > >::iterator findResultEnd;
		std::vector< std::shared_ptr< HtmlNode > >::iterator findResultIterator;
		for( ; vectorIterator != vectorEnd; ++vectorIterator ) {
			do {
				//////////// 名称 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8"./dd/h3/a" ) ).toStdWString( ) );
				content = vectorIterator->get( )->getIncludeNodeContent( );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( !htmlNodes )
					break;
				findResultIterator = htmlNodes->begin( );
				findResultEnd = htmlNodes->end( );
				if( findResultIterator == findResultEnd )
					break;
				HtmlNode *element = findResultIterator->get( );
				auto contentText = element->getNodeContentText( );
				if( contentText && !contentText->empty( ) )
					novelInfoBuffPtr->novelName = std::make_shared< QString >( QString::fromStdWString( *contentText ) );
				//////////// url xpath
				auto unorderedMapAttribute = element->findAttribute( []( const HtmlString &attributeName, const HtmlString &attributeValue )->bool {
					if( HtmlStringTools::equRemoveSpaceOverHtmlString( attributeName, L"href" ) )
						return true;
					return false;
				} );
				if( unorderedMapAttribute->size( ) == 0 )
					break;
				auto begin = unorderedMapAttribute->begin( );

				auto second = begin->second;
				auto newSecond = second.substr( 1, second.length( ) - 2 );
				QString urlLastStr = QString::fromStdWString( newSecond );
				novelInfoBuffPtr->url = std::make_shared< QString >( QString( u8"%1%2" ).arg( rootUrl ).arg( urlLastStr ) );
				//////////// 更新时间 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8"./dd/h3/span[@class='uptime']" ) ).toStdWString( ) );
				content = vectorIterator->get( )->getIncludeNodeContent( );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( !htmlNodes )
					break;
				findResultIterator = htmlNodes->begin( );
				findResultEnd = htmlNodes->end( );
				if( findResultIterator == findResultEnd )
					break;
				auto lastTime = findResultIterator->get( )->getNodeContentText( );
				if( !lastTime )
					break;
				novelInfoBuffPtr->time = std::make_shared< QString >( QString::fromStdWString( *lastTime ) );
				//////////// 作者 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8"./dd/span" ) ).toStdWString( ) );
				content = vectorIterator->get( )->getIncludeNodeContent( );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( !htmlNodes )
					break;
				findResultIterator = htmlNodes->begin( );
				findResultEnd = htmlNodes->end( );
				if( findResultIterator == findResultEnd )
					break;
				auto author = findResultIterator->get( )->getNodeContentText( );
				if( !author )
					break;
				novelInfoBuffPtr->author = std::make_shared< QString >( QString::fromStdWString( *author ) );
				//////////// 最后更新项目 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8R"(./dd[@class="book_other"]/a)" ) ).toStdWString( ) );
				content = vectorIterator->get( )->getIncludeNodeContent( );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( !htmlNodes )
					break;
				findResultIterator = htmlNodes->begin( );
				findResultEnd = htmlNodes->end( );
				if( findResultIterator == findResultEnd )
					break;
				auto lastItem = findResultIterator->get( )->getNodeContentText( );
				if( !lastItem )
					break;
				novelInfoBuffPtr->lastItem = std::make_shared< QString >( QString::fromStdWString( *lastItem ) );
				//////////// 详情 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8"./dd[@class='book_des']" ) ).toStdWString( ) );
				content = vectorIterator->get( )->getIncludeNodeContent( );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( !htmlNodes )
					break;
				findResultIterator = htmlNodes->begin( );
				findResultEnd = htmlNodes->end( );
				if( findResultIterator == findResultEnd )
					break;
				auto info = findResultIterator->get( )->getNodeContentText( );
				if( !info )
					break;
				novelInfoBuffPtr->info = std::make_shared< QString >( QString::fromStdWString( *info ) );
				// 成功则赋值
				novelInfoPtr = novelInfoBuffPtr;
			} while( false );
			if( novelInfoPtr ) { // 为空，说明没有被赋值，也就是异常
				auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( QString( u8" xpath 异常，登出" ) );
				auto path = QString( Cache_Path_Dir ).append( QDir::separator( ) ).append( type_name ).append( u8".html" );
				OStream::errorQDebugOut( msg.toStdString( ), __FILE__, __LINE__, __FUNCTION__, path, QString::fromStdWString( htmlText ) );
				delete novelInfoBuffPtr; // 释放
				novelInfoBuffPtr = nullptr; // 置 0
			} else {
				result.emplace_back( novelInfoPtr ); // 加入列表
				novelInfoPtr = nullptr; // 重置
			}
		}
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
void RequestNet::novelTypeEnd( const HtmlDocString &root_url, const HtmlDocString &type_name, const HtmlDocString &url, const interfacePlugsType::Vector_NovelSPtr &saveNovelInfos ) {
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
