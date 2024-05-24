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
#include <qbrush.h>

#include "../NovelInfo/NovelInfo.h"
#include "dateTime/DateTime.h"
using namespace interfacePlugsType;
using namespace cylHtmlTools;
QString RequestNet::timeForm = QObject::tr( u8R"(yyyy-MM-dd hh:mm:ss)" );
QString RequestNet::currentTimeForm = QObject::tr( u8R"(yyyy-MM-dd hh:mm:ss)" );
int RequestNet::expireDay = 2;
QDateTime RequestNet::currentTime;


RequestNet::RequestNet( QObject *parent ): QObject( parent )
, rootUrl( GET_URL )
, oStream( nullptr )
, iStream( nullptr )
, thisOStream( nullptr )
, typeUrlMap( nullptr ) {
	cylHttpNetWork::NetworkRequest::initTools( );
}

RequestNet::~RequestNet( ) {
	qDebug( ) << "RequestNet::~RequestNet( " << IRequestNetInterface_iid << ")";

}
int RequestNet::initAfter( ) {
	thisOStream = OStream::getDefaultOStream( rootUrl.toString( ) );
	currentTime = QDateTime::currentDateTime( );
	return 0;
}
int RequestNet::initBefore( ) {
	return 0;
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
					OStream::anyDebugOut( thisOStream, msg, __FILE__, __LINE__, __FUNCTION__ );
					return;
				}

				htmlDoc->analysisBrotherNode( );
				auto xpath = cylHtmlTools::XPath( QString( tr( u8"div[@class='hd']/ul/li/a" ) ).toStdWString( ) );

				auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
				auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
				if( !vectorHtmlNodeSPtrShared ) {
					auto msg = QString( "%1 : %2 " ).arg( QString::fromStdWString( url ) ).arg( QString( u8" xpath 异常，登出" ) ).toStdString( );
					OStream::anyDebugOut( thisOStream, msg, __FILE__, __LINE__, __FUNCTION__ );
					return;
				}
				auto vectorIterator = vectorHtmlNodeSPtrShared->begin( );
				auto vectorEnd = vectorHtmlNodeSPtrShared->end( );
				HtmlDocString hrefKey = L"href";
				for( ; vectorIterator != vectorEnd; ++vectorIterator ) {
					auto element = vectorIterator->get( );
					auto unorderedMap = element->findAttribute( [&]( const HtmlString &first, const HtmlString &scen ) ->bool {
						if( HtmlStringTools::equRemoveSpaceOverHtmlString( first, hrefKey ) )
							return true;
						return false;
					} );
					if( unorderedMap ) {
						auto key = *element->getNodeContentText( );
						auto value = unorderedMap->at( hrefKey );
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
Vector_INovelInfoSPtr RequestNet::formHtmlGetTypePageNovels( const interfacePlugsType::HtmlDocString &type_name, const interfacePlugsType::HtmlDocString &request_url, const HtmlDocString &htmlText, const Vector_INovelInfoSPtr &saveNovelInfos, void *appendDataPtr ) {
	Vector_INovelInfoSPtr result;
	do {
		auto stdWString( std::make_shared< HtmlString >( htmlText ) );
		size_t index = 0, end = stdWString->size( );
		auto htmlDoc = cylHtmlTools::HtmlDoc::parse( stdWString, end, index );
		if( !htmlDoc ) {
			auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( QString( u8" HtmlDoc::parse 异常，登出" ) );
			auto path = QString( Cache_Path_Dir ).append( QDir::separator( ) ).append( type_name ).append( u8".html" );
			OStream::anyDebugOut( thisOStream, msg, __FILE__, __LINE__, __FUNCTION__, path, QString::fromStdWString( htmlText ) );
			break;
		}
		htmlDoc->analysisBrotherNode( );
		auto xpath = cylHtmlTools::XPath( QString( tr( u8"div[@class='cf' @id='sitebox' ]/dl" ) ).toStdWString( ) );
		auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
		auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
		if( !vectorHtmlNodeSPtrShared ) {
			auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( QString( u8" xpath 异常，登出" ) );
			auto path = QString( Cache_Path_Dir ).append( QDir::separator( ) ).append( type_name ).append( u8".html" );
			OStream::anyDebugOut( thisOStream, msg, __FILE__, __LINE__, __FUNCTION__, path, QString::fromStdWString( htmlText ) );
			break;
		}
		auto vectorIterator = vectorHtmlNodeSPtrShared->begin( );
		auto vectorEnd = vectorHtmlNodeSPtrShared->end( );
		std::shared_ptr< INovelInfo > novelInfoPtr = nullptr;
		Vector_HtmlNodeSPtr_Shared htmlNodes = nullptr;
		HtmlString_Shared content = nullptr;
		QString rootUrl = GET_URL;
		Novel_Xpath_Analysis_Error quitMsg;
		auto novelInfoBuffPtr = std::make_shared< NovelInfo >( );
		std::vector< std::shared_ptr< HtmlNode > >::iterator findResultEnd;
		std::vector< std::shared_ptr< HtmlNode > >::iterator findResultIterator;
		for( ; vectorIterator != vectorEnd; ++vectorIterator ) {
			quitMsg = None;
			do {
				//////////// 名称 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8"./dd/h3/a" ) ).toStdWString( ) );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( !htmlNodes ) {
					quitMsg = Name_Error_Xpath; // xpath 异常：名称
					break;
				}
				findResultIterator = htmlNodes->begin( );
				findResultEnd = htmlNodes->end( );
				if( findResultIterator == findResultEnd ) {
					quitMsg = Name_Error_None; // xpath 找不到：名称
					break;
				}
				HtmlNode *element = findResultIterator->get( );
				auto contentText = element->getNodeContentText( );
				if( contentText && !contentText->empty( ) )
					novelInfoBuffPtr->novelName = std::make_shared< QString >( QString::fromStdWString( *contentText ) );
				else {
					quitMsg = Name_Error_None; // xpath 找不到：名称
					break;
				}
				//////////// url xpath
				auto unorderedMapAttribute = element->findAttribute( []( const HtmlString &attributeName, const HtmlString &attributeValue )->bool {
					if( HtmlStringTools::equRemoveSpaceOverHtmlString( attributeName, L"href" ) )
						return true;
					return false;
				} );
				if( unorderedMapAttribute->size( ) == 0 ) {
					quitMsg = Url_Error_None; // xpath 异常 : url 找不到
					break;
				}
				auto begin = unorderedMapAttribute->begin( );
				auto second = begin->second;
				auto newSecond = second.substr( 1, second.length( ) - 2 );
				QString urlLastStr = QString::fromStdWString( newSecond );
				novelInfoBuffPtr->url = std::make_shared< QString >( QString( u8"%1%2" ).arg( rootUrl ).arg( urlLastStr ) );
				//////////// 更新时间 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8"./dd/h3/span[@class='uptime']" ) ).toStdWString( ) );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( !htmlNodes ) {
					quitMsg = DateTime_Error_Xpath; // xpath 异常 : 更新时间
					break;
				}
				findResultIterator = htmlNodes->begin( );
				findResultEnd = htmlNodes->end( );
				if( findResultIterator == findResultEnd ) {
					quitMsg = DateTime_Error_None; // xpath 异常 : 更新时间 找不到
					break;
				}
				auto lastTime = findResultIterator->get( )->getNodeContentText( );
				if( !lastTime ) {
					quitMsg = DateTime_Error_None; // xpath 异常 : 更新时间 找不到
					break;
				}
				QString fromStdWString = QString::fromStdWString( *lastTime );
				QDateTime currentTime = QDateTime::currentDateTime( );
				QDateTime novelTime = QDateTime::fromString( fromStdWString, timeForm );

				auto compareDateTime = DateTime::compareDateTime( currentTime, novelTime );
				int16_t timeToDay = DateTime::getTimeToDay( compareDateTime );
				if( abs( timeToDay ) > RequestNet::expireDay ) {
					quitMsg = DateTime_Error_Expire; // xpath 异常 : 更新时间 找不到
					break;
				}
				novelInfoBuffPtr->time = std::make_shared< QString >( fromStdWString );
				//////////// 作者 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8"./dd/span" ) ).toStdWString( ) );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( htmlNodes ) {
					findResultIterator = htmlNodes->begin( );
					findResultEnd = htmlNodes->end( );
					if( findResultIterator != findResultEnd ) {
						auto author = findResultIterator->get( )->getNodeContentText( );
						if( author )
							novelInfoBuffPtr->author = std::make_shared< QString >( QString::fromStdWString( *author ) );
					}
				}
				//////////// 最后更新项目 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8R"(./dd[@class="book_other"]/a)" ) ).toStdWString( ) );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( htmlNodes ) {
					findResultIterator = htmlNodes->begin( );
					findResultEnd = htmlNodes->end( );
					if( findResultIterator != findResultEnd ) {
						auto lastItem = findResultIterator->get( )->getNodeContentText( );
						if( lastItem )
							novelInfoBuffPtr->lastItem = std::make_shared< QString >( QString::fromStdWString( *lastItem ) );
					}
				}
				//////////// 详情 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8"./dd[@class='book_des']" ) ).toStdWString( ) );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( htmlNodes ) {
					findResultIterator = htmlNodes->begin( );
					findResultEnd = htmlNodes->end( );
					if( findResultIterator != findResultEnd ) {
						auto info = findResultIterator->get( )->getNodeContentText( );
						if( info )
							novelInfoBuffPtr->info = std::make_shared< QString >( QString::fromStdWString( *info ) );
					}
				}
				novelInfoBuffPtr->format = std::make_shared< QString >( RequestNet::timeForm );
				novelInfoBuffPtr->lastRequestTimeFormat = std::make_shared< QString >( currentTimeForm );
				QString fromStdString = RequestNet::currentTime.toString( currentTimeForm );
				novelInfoBuffPtr->lastRequestTime = std::make_shared< QString >( fromStdString );
				novelInfoBuffPtr->typePageUrl = std::make_shared< QString >( QString::fromStdWString( request_url ) );
				novelInfoBuffPtr->typeName = std::make_shared< QString >( QString::fromStdWString( type_name ) );
				// 成功则赋值
				novelInfoPtr = novelInfoBuffPtr;
			} while( false );
			if( !novelInfoPtr ) { // 为空，说明没有被赋值，也就是异常
				QString includeNodeContent( QString::fromStdWString( *vectorIterator.operator*( )->getIncludeNodeContent( ) ) );
				includeNodeContent = u8"\n===========================\n" + includeNodeContent + u8"\n===========================\n";
				switch( quitMsg ) {
					case DateTime_Error_Expire : {
						QString errorMsg( u8" DateTime_Error_Expire 异常(过期)，登出:\n\turl (%1) -> 小说名称 (%2) => 退出代码(%3)" );
						errorMsg = errorMsg.arg( *novelInfoBuffPtr->url ).arg( *novelInfoBuffPtr->novelName ).arg( quitMsg );
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg.toStdString( ) );
						break;
					}
					case DateTime_Error_None : {
						QString errorMsg( u8" DateTime_Error_None 异常(日期找不到)，登出:\n\turl (%1) -> 小说名称 (%2) => 退出代码(%3)" );
						errorMsg = errorMsg.arg( *novelInfoBuffPtr->url ).arg( *novelInfoBuffPtr->novelName ).arg( quitMsg ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg.toStdString( ) );
						break;
					}
					case DateTime_Error_Xpath : {
						QString errorMsg( u8" DateTime_Error_Xpath 异常(日期 xpath 错误)，登出:\n\turl (%1) -> 小说名称 (%2) => 退出代码(%3)" );
						errorMsg = errorMsg.arg( *novelInfoBuffPtr->url ).arg( *novelInfoBuffPtr->novelName ).arg( quitMsg ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg.toStdString( ) );
						break;
					}
					case Name_Error_None : {
						QString errorMsg( u8" Name_Error_None 异常，登出:\n\turl (%1) -> 小说名称 (none) => 退出代码(%2)" );
						errorMsg = errorMsg.arg( *novelInfoBuffPtr->url ).arg( quitMsg ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg.toStdString( ) );
						break;
					}
					case Name_Error_Xpath : {
						QString errorMsg( u8" Name_Error_Xpath 异常，登出:\n\turl (%1) -> 小说名称 (none) => 退出代码(%2)" );
						errorMsg = errorMsg.arg( *novelInfoBuffPtr->url ).arg( quitMsg ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg.toStdString( ) );
						break;
					}
					case Url_Error_Xpath : {
						QString errorMsg( u8" Url_Error_Xpath 异常，登出 => 退出代码(%1)" );
						errorMsg = errorMsg.arg( quitMsg ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg.toStdString( ) );
						break;
					}
					case Url_Error_None : {
						QString errorMsg( u8" Url_Error_None 异常，登出 => 退出代码(%1)" );
						errorMsg = errorMsg.arg( quitMsg ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg.toStdString( ) );
						break;
					}
					case None :
					default : {
						QString errorMsg( u8" 未知异常，登出->\n=================\n%1\n=========\n" );
						HtmlString_Shared content = vectorIterator->get( )->getIncludeNodeContent( );
						errorMsg = errorMsg.arg( QString::fromStdWString( *content ) ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						auto path = QString( Cache_Path_Dir ).append( QDir::separator( ) ).append( type_name ).append( u8".html" );
						OStream::anyDebugOut( thisOStream, msg, __FILE__, __LINE__, __FUNCTION__, path, QString::fromStdWString( htmlText ) );
					}
				}
				novelInfoBuffPtr->clear( ); // 重置
				novelInfoPtr.reset( ); // 重置
			} else {
				result.emplace_back( novelInfoPtr ); // 加入列表
				// 成功获取即可输出
				QString outMsg( u8"[%1]小说(%2):url(%3) -> 解析成功" );
				outMsg = outMsg.arg( *novelInfoBuffPtr->typeName ).arg( *novelInfoBuffPtr->novelName ).arg( *novelInfoBuffPtr->url );
				OStream::anyDebugOut( oStream, outMsg );
				novelInfoPtr.reset( ); // 重置
				novelInfoBuffPtr = std::make_shared< NovelInfo >( );
			}
		}
	} while( false );
	return result;
}
INovelInfo_Shared RequestNet::formHtmlGetUrlNovelInfo( const interfacePlugsType::HtmlDocString &url, const HtmlDocString &htmlText, const Vector_INovelInfoSPtr &saveNovelInfos, const INovelInfo_Shared &networkReplayNovel ) {
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
HtmlDocString RequestNet::formHtmlGetNext( const interfacePlugsType::HtmlDocString &type_name, const interfacePlugsType::HtmlDocString &url, const HtmlDocString &htmlText, const Vector_INovelInfoSPtr &saveNovelInfos, const Vector_INovelInfoSPtr &lastNovelInfos ) {
	HtmlDocString result;
	if( lastNovelInfos.size( ) == 0 ) // 当前页面没有获取到小说时候，直接返回
		return result;
	size_t quitMsg = 0;
	do {
		auto stdWString( std::make_shared< HtmlString >( htmlText ) );
		size_t index = 0, end = stdWString->size( );
		auto htmlDoc = cylHtmlTools::HtmlDoc::parse( stdWString, end, index );
		if( !htmlDoc )
			break;

		auto xpath = cylHtmlTools::XPath( QString( tr( u8R"(div[@class="pages"]/ul/li/a[@class="none"])" ) ).toStdWString( ) );
		auto htmlNodes = htmlDoc->xpathRootNodes( xpath );
		if( !htmlNodes ) {
			quitMsg = 1;
			break;
		}
		auto iterator = htmlNodes->begin( );
		auto endIterator = htmlNodes->end( );
		HtmlString wstrNextPageKey = L"下一页";
		for( ; iterator != endIterator; ++iterator ) {
			HtmlNode *element = iterator->get( );
			HtmlString_Shared contentText = element->getNodeContentText( );
			if( HtmlStringTools::findNextHtmlStringPotion( contentText.get( ), 0, &wstrNextPageKey ) ) {
				auto findAttribute = element->findAttribute( []( const HtmlString &attributeName, const HtmlString &attributeValue ) {
					if( HtmlStringTools::equRemoveSpaceOverHtmlString( attributeName, L"href" ) )
						return true;
					return false;
				} );
				auto pair = findAttribute->begin( );
				if( pair == findAttribute->end( ) )
					continue;
				result = ( rootUrl.scheme( ) + u8"://" + rootUrl.host( ) + QString::fromStdWString( pair->second.substr( 1, pair->second.length( ) - 2 ) ) ).toStdWString( );
				break;
			}

		}
	} while( false );
	if( result.empty( ) ) { // 异常， 输出
		if( quitMsg < 2 ) { // 找不到节点
			QString errorMsg( u8" xpath 异常，登出 => 退出代码(%1)" );
			errorMsg = errorMsg.arg( quitMsg );
			auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( url ).arg( errorMsg );
			auto path = QString( Cache_Path_Dir ).append( QDir::separator( ) ).append( type_name ).append( u8".html" );
			OStream::anyDebugOut( thisOStream, msg, __FILE__, __LINE__, __FUNCTION__, path, QString::fromStdWString( htmlText ) );
		}
	}
	return result;
}
bool RequestNet::isRequestNovelInfoUrl( const interfacePlugsType::INovelInfoPtr &novel_info_ptr ) {
	return false;
}
void RequestNet::novelTypeEnd( const HtmlDocString &root_url, const HtmlDocString &type_name, const HtmlDocString &url, const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos ) {
}
void RequestNet::endHost( const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos ) {
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
