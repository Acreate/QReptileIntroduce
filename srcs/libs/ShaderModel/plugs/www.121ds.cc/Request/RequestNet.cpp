#include "RequestNet.h"
#include <string>

#include <QFile>
#include <QDir>
#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/htmlTools/XPath/XPath.h"
#include <htmls/htmlTools/HtmlWorkThread/HtmlWorkThread.h>
#include <qguiapplication.h>

#include "auto_generate_files/macro/cmake_to_c_cpp_header_env.h"
#include <stream/OStream.h>
#include <HttpNetWork/NetworkRequest.h>
#include <HttpNetWork/Request.h>
#include <qbrush.h>

#include <DB/DBTools.h>
#include <DB/dbInterface/i_depositoryInterface/I_Depository.h>
#include <DB/dbInterface/i_resultInfo/I_ResultInfo.h>
#include <DB/dbInterface/i_tabInfo/I_TabInfo.h>
#include <DB/dbInterface/I_DB.h>
#include <QSqlQuery>

#include "../NovelInfo/NovelInfo.h"
#include "dateTime/DateTime.h"
using namespace interfacePlugsType;
using namespace cylHtmlTools;
QString RequestNet::timeForm = QObject::tr( u8R"(yyyy-MM-dd hh:mm:ss)" );
QString RequestNet::currentTimeForm = QObject::tr( u8R"(yyyy-MM-dd hh:mm:ss)" );
int RequestNet::expireDay = 2;
QDateTime RequestNet::currentTime;


RequestNet::RequestNet( QObject *parent ): QObject( parent ), rootUrl( GET_URL ), oStream( nullptr ), iStream( nullptr ), thisOStream( nullptr ), typeUrlMap( nullptr ) {
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
			const HtmlWorkThread< std::shared_ptr< HtmlString > > *html_work_thread
			, const std::thread *run_std_cpp_thread
			, std::mutex *html_work_thread_mutex
			, std::mutex *std_cpp_thread_mutex
			, std::shared_ptr< HtmlString > &data
			, const time_t *startTime ) {
				auto htmlDoc = cylHtmlTools::HtmlDoc::parse( stdWString );
				if( !htmlDoc.get( ) ) {
					auto msg = QString( "%1 : %2" ).arg( QString::fromStdWString( url ) ).arg( QString( u8" HtmlDoc::parse 异常，登出" ) );
					OStream::anyDebugOut( thisOStream, msg, __FILE__, __LINE__, __FUNCTION__ );
					return;
				}

				htmlDoc->analysisBrotherNode( );
				auto xpath = cylHtmlTools::XPath( QString( tr( u8"div[@class='hd']/ul/li/a" ) ).toStdWString( ) );

				auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
				auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
				if( !vectorHtmlNodeSPtrShared ) {
					auto msg = QString( "%1 : %2 " ).arg( QString::fromStdWString( url ) ).arg( QString( u8" xpath 异常，登出" ) );
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
						auto key = *element->getNodeIncludeContentText( );
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
		auto htmlDoc = cylHtmlTools::HtmlDoc::parse( htmlText );
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
		HtmlNode *element;
		auto novelInfoBuffPtr = std::make_shared< NovelInfo >( );
		for( ; vectorIterator != vectorEnd; ++vectorIterator ) {
			quitMsg = None;
			do {
				//////////// url xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8"./dd/h3/a" ) ).toStdWString( ) );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( !htmlNodes ) {
					quitMsg = Url_Error_None; // xpath 异常 : url 找不到
					break;
				}
				element = htmlNodes->at( 0 ).get( );
				auto unorderedMapAttribute = element->findAttribute( []( const HtmlString &attributeName, const HtmlString &attributeValue )->bool {
					if( HtmlStringTools::equRemoveSpaceOverHtmlString( attributeName, L"href" ) )
						return true;
					return false;
				} );
				if( unorderedMapAttribute->size( ) == 0 ) {
					quitMsg = Url_Error_None; // xpath 异常 : url 找不到
					break;
				}

				//////////// 名称 xpath
				auto contentText = element->getNodeIncludeContentText( );
				if( contentText )
					novelInfoBuffPtr->novelName = std::make_shared< QString >( QString::fromStdWString( *contentText ) );
				else
					novelInfoBuffPtr->novelName = std::make_shared< QString >( QString::fromStdWString( L"" ) );

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
				auto lastTime = htmlNodes->at( 0 ).get( )->getNodeIncludeContentText( );
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
				novelInfoBuffPtr->updateTime = std::make_shared< QString >( fromStdWString );
				//////////// 作者 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8"./dd/span" ) ).toStdWString( ) );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( htmlNodes ) {
					auto author = htmlNodes->at( 0 ).get( )->getNodeIncludeContentText( );
					if( author )
						novelInfoBuffPtr->author = std::make_shared< QString >( QString::fromStdWString( *author ) );
				}
				//////////// 最后更新项目 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8R"(./dd[@class="book_other"]/a)" ) ).toStdWString( ) );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( htmlNodes ) {
					auto lastItem = htmlNodes->at( 0 ).get( )->getNodeIncludeContentText( );
					if( lastItem )
						novelInfoBuffPtr->lastItem = std::make_shared< QString >( QString::fromStdWString( *lastItem ) );
				}
				//////////// 详情 xpath
				xpath = cylHtmlTools::XPath( QString( tr( u8"./dd[@class='book_des']" ) ).toStdWString( ) );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( htmlNodes ) {
					auto info = htmlNodes->at( 0 ).get( )->getNodeIncludeContentText( );
					if( info )
						novelInfoBuffPtr->info = std::make_shared< QString >( QString::fromStdWString( *info ) );
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
						OStream::anyDebugOut( thisOStream, msg );
						break;
					}
					case DateTime_Error_None : {
						QString errorMsg( u8" DateTime_Error_None 异常(日期找不到)，登出:\n\turl (%1) -> 小说名称 (%2) => 退出代码(%3)" );
						errorMsg = errorMsg.arg( *novelInfoBuffPtr->url ).arg( *novelInfoBuffPtr->novelName ).arg( quitMsg ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg );
						break;
					}
					case DateTime_Error_Xpath : {
						QString errorMsg( u8" DateTime_Error_Xpath 异常(日期 xpath 错误)，登出:\n\turl (%1) -> 小说名称 (%2) => 退出代码(%3)" );
						errorMsg = errorMsg.arg( *novelInfoBuffPtr->url ).arg( *novelInfoBuffPtr->novelName ).arg( quitMsg ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg );
						break;
					}
					case Name_Error_None : {
						QString errorMsg( u8" Name_Error_None 异常，登出:\n\turl (%1) -> 小说名称 (none) => 退出代码(%2)" );
						errorMsg = errorMsg.arg( *novelInfoBuffPtr->url ).arg( quitMsg ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg );
						break;
					}
					case Name_Error_Xpath : {
						QString errorMsg( u8" Name_Error_Xpath 异常，登出:\n\turl (%1) -> 小说名称 (none) => 退出代码(%2)" );
						errorMsg = errorMsg.arg( *novelInfoBuffPtr->url ).arg( quitMsg ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg );
						break;
					}
					case Url_Error_Xpath : {
						QString errorMsg( u8" Url_Error_Xpath 异常，登出 => 退出代码(%1)" );
						errorMsg = errorMsg.arg( quitMsg ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg );
						break;
					}
					case Url_Error_None : {
						QString errorMsg( u8" Url_Error_None 异常，登出 => 退出代码(%1)" );
						errorMsg = errorMsg.arg( quitMsg ) + includeNodeContent;
						auto msg = QString( "%1 : %2 : %3" ).arg( type_name ).arg( request_url ).arg( errorMsg );
						OStream::anyDebugOut( thisOStream, msg );
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
				OStream::anyDebugOut( thisOStream, outMsg );
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
		auto htmlDoc = cylHtmlTools::HtmlDoc::parse( htmlText );
		if( htmlDoc )
			break;
	} while( false );

	return result;
}
HtmlDocString RequestNet::formHtmlGetNext( const interfacePlugsType::HtmlDocString &type_name, const interfacePlugsType::HtmlDocString &url, const HtmlDocString &htmlText, const Vector_INovelInfoSPtr &saveNovelInfos, const Vector_INovelInfoSPtr &lastNovelInfos ) {
	HtmlDocString result; // todo : 下一页
	if( lastNovelInfos.size( ) == 0 ) // 当前页面没有获取到小说时候，直接返回
		return result;
	size_t quitMsg = 0;
	do {
		auto htmlDoc = cylHtmlTools::HtmlDoc::parse( htmlText );
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
			HtmlString_Shared contentText = element->getNodeIncludeContentText( );
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
	if( result.empty( ) ) { // 输出
		QString errorMsg;
		QString fileBaseName;
		QString errorType;
		QString dirName = QUrl( GET_URL ).host( );
		if( quitMsg == 0 ) {
			errorMsg = u8" 找不到下一页，登出 => 退出代码(%1)";
			fileBaseName = QString::fromStdWString( type_name );
			errorType = u8"normal quit";
		} else if( quitMsg == 1 ) {// 找不到节点
			errorMsg = u8" xpath 异常，登出 => 退出代码(%1)";
			errorType = u8"xpath 异常";
			fileBaseName = QString::fromStdWString( type_name );
		}
		errorMsg = errorMsg.arg( quitMsg );
		auto errorFile = __FILE__;
		auto errorLine = __LINE__;
		auto errorFunction = __FUNCTION__;
		auto msg = QString( "%1 : %2 : %3\n文件:\n\t%4\n行号:\n\t%5 -> %6" ).arg( errorMsg ).arg( type_name ).arg( url ).arg( errorFile ).arg( errorLine ).arg( errorFunction );
		QString formMsg( u8"<!--  \n%1\n -->\n%2" );
		auto content = formMsg.arg( msg ).arg( QString::fromStdWString( htmlText ) );
		OStream::anyDebugOut( thisOStream, msg, errorFile, errorLine, errorFunction );
		OStream::outDebugLogFile( Cache_Path_Dir, content.toLatin1( ), fileBaseName, errorType, dirName, ".html" );
	}
	return result;
}
bool RequestNet::isRequestNovelInfoUrl( const interfacePlugsType::INovelInfoPtr &novel_info_ptr ) {
	return false;
}
void RequestNet::novelTypeEnd( const HtmlDocString &root_url, const HtmlDocString &type_name, const HtmlDocString &url, const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos ) {
}
/// <summary>
/// 分解列表数据<br/>
///	根据游离列表 src_vector 与永久列表 db_result 进行匹配解析<br/>
/// updateList : src_vector 与 db_result 交集<br/>
/// interList : src_vector 与 db_result 的不同部分<br/>
/// </summary>
/// <param name="src_vector">游离列表</param>
/// <param name="db_result">磁盘列表</param>
/// <param name="updateList">更新列表</param>
/// <param name="interList">插入列表</param>
inline void separate_list( const interfacePlugsType::Vector_INovelInfoSPtr &src_vector, cylDB::IResultInfo_Shared &db_result, interfacePlugsType::Vector_INovelInfoSPtr &updateList, interfacePlugsType::Vector_INovelInfoSPtr &interList ) {
	db_result->resetColIndex( );
	auto currentRows = db_result->getCurrentRows( );
	QStringList msg;
	HtmlDocString url;
	HtmlDocString compUrl;
	auto srcVectorEnd = src_vector.end( );
	while( currentRows->size( ) > 0 ) {
		url = currentRows->at( 8 )->toString( ).toStdWString( );

		auto iterator = src_vector.begin( );

		for( ; iterator != srcVectorEnd; ++iterator )
			if( iterator->get( )->getNovelUrl( &compUrl ) && url == compUrl ) {
				auto end = updateList.end( );
				if( std::find_if( updateList.begin( )
					, end
					, [&]( INovelInfo_Shared &info_shared ) ->bool {
						if( info_shared->getNovelUrl( &url ) && url == compUrl )
							return true;
						return false;
					} ) == end )
					updateList.emplace_back( *iterator );
				break;
			}
		if( !db_result->nextCol( ) )
			break;
		currentRows = db_result->getCurrentRows( );
		msg.clear( );
	}

	if( updateList.size( ) == 0 ) {
		interList = src_vector;
		return;
	}

	// 查找 src_vector 所有元素，并且把不在 updateList 当中的元素存储到 interList
	auto iterator = src_vector.begin( );

	auto end = updateList.end( );
	for( ; iterator != srcVectorEnd; ++iterator )
		if( iterator->get( )->getNovelUrl( &compUrl ) )
			if( std::find_if( updateList.begin( )
				, end
				, [&]( INovelInfo_Shared &info_shared ) ->bool {
					if( info_shared->getNovelUrl( &url ) && url == compUrl )
						return true;
					return false;
				} ) == end )
				interList.emplace_back( *iterator );

}
/// <summary>
/// 对字符串使用双引号 -> str => "str"
/// </summary>
/// <param name="updateMap">进化的列表</param>
/// <returns>进化完成的对象列表</returns>
inline QVariantMap conver_str_sql_text( const QVariantMap &updateMap ) {
	QVariantMap result;
	auto iterator = updateMap.begin( );
	auto end = updateMap.end( );
	QString form( R"("%1")" );
	for( ; iterator != end; ++iterator )
		result.insert( iterator.key( ), QVariant( form.arg( iterator.value( ).toString( ) ) ) );
	return result;

}/// <summary>
/// 对字符串使用双引号 -> str => "str"
/// </summary>
/// <param name="tabValues">进化的列表</param>
/// <returns>进化完成的对象列表</returns>
inline QStringList conver_str_sql_text( const QStringList &tabValues ) {
	QStringList result;
	auto iterator = tabValues.begin( );
	auto end = tabValues.end( );
	QString form( R"("%1")" );
	for( ; iterator != end; ++iterator )
		result << form.arg( *iterator );
	return result;
}
inline bool generate_db_tab( const cylDB::DB_Shared &dbInterface, const cylDB::Depository_Shared &depositoryShared, const QString &tab_name, OStream *thisOStream ) {
	auto supportType = dbInterface->getSupportType( );
	QString dbTextType = supportType.find( typeid( QString ).name( ) )->toString( );
	bool hasTab = false;
	if( dbTextType.isEmpty( ) )
		dbTextType = "TEXT";
	hasTab = depositoryShared->createTab( tab_name
		, { { "rootUrl", dbTextType }
			, { "novelName", dbTextType }
			, { "info", dbTextType }
			, { "updateTime", dbTextType }
			, { "format", dbTextType }
			, { "lastRequestTime", dbTextType }
			, { "lastRequestTimeFormat", dbTextType }
			, { "author", dbTextType }
			, { "url", dbTextType }
			, { "lastItem", dbTextType }
			, { "additionalData", dbTextType }
			, { "typePageUrl", dbTextType }
			, { "typeName", dbTextType }
		} );
	if( !hasTab )
		OStream::anyDebugOut( thisOStream, "无法创建正确的 db 文件", __FILE__, __LINE__, __FUNCTION__ );
	return hasTab;
}
/// <summary>
/// 更新命令拼接
/// </summary>
/// <param name="cmd">起始拼接对象</param>
/// <param name="append_map">拼接对象映射</param>
/// <returns>拼接个数</returns>
inline QString append_map_update( const QString &cmd, const QVariantMap &append_map ) {
	QString result;
	auto iterator = append_map.begin( );
	auto end = append_map.end( );
	if( iterator == end )
		return result;
	result = cmd;
	do {
		result.append( " `" );
		result.append( iterator.key( ) );
		result.append( "` =" );
		result.append( iterator.value( ).toString( ) );
		++iterator;
		if( iterator == end )
			break;
		result.append( ", " );
	} while( true );

	return result;
}
void RequestNet::endHost( const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos ) {

	auto dbInterface = cylDB::DBTools::linkDB( Cache_Path_Dir );
	if( dbInterface->link( ) ) {
		cylHtmlTools::HtmlWorkThread< bool * >::Current_Thread_Run currentThreadRun = [dbInterface,&saveNovelInfos,this]( const cylHtmlTools::HtmlWorkThread< bool * > *html_work_thread, const std::thread *run_std_cpp_thread, std::mutex *html_work_thread_mutex, std::mutex *std_cpp_thread_mutex, bool *data, const time_t *startTime ) {
			QString dbName = this->rootUrl.host( );
			QString tabName = dbName;
			dbName.append( ".db" );
			auto depositoryShared = dbInterface->openDepository( dbName );
			if( depositoryShared ) {
				if( !depositoryShared->open( ) ) {
					auto lastError = depositoryShared->getLastError( );
					OStream::anyDebugOut( thisOStream, lastError.text( ), __FILE__, __LINE__, __FUNCTION__ );
					return;
				}
				bool hasTab = depositoryShared->hasTab( tabName );

				if( !hasTab )
					if( generate_db_tab( dbInterface, depositoryShared, tabName, thisOStream ) )
						return;

				QStringList tabFieldNames = { "rootUrl", "novelName", "info", "updateTime", "format", "lastRequestTime", "lastRequestTimeFormat", "author", "url", "lastItem", "additionalData", "typePageUrl", "typeName" };
				auto allItem = depositoryShared->findItems( tabName, tabFieldNames );

				interfacePlugsType::Vector_INovelInfoSPtr updateList; // 更新列表
				interfacePlugsType::Vector_INovelInfoSPtr interList; // 插入列表
				// 分解-插入/更新 列表
				if( allItem ) {
					separate_list( saveNovelInfos, allItem, updateList, interList );
				} else
					interList = saveNovelInfos; // 数据库不存在数据的时候，全部拷贝到插入列表

				// 开始更新
				HtmlDocString rootUrl = this->rootUrl.toString( ).toStdWString( ),
					novelName,
					novelInfo,
					novelUpdateTime,
					novelFormat,
					novelLastRequestTime,
					novelLastRequestTimeFormat,
					novelAuthor,
					novelUrl,
					novelLastItem,
					novelAdditionalData,
					novelTypePageUrl,
					novelTypeName;
				QString cmd = R"(UPDATE `)" + tabName + R"(` SET `updateTime`=:updateTime, `lastRequestTime`=:lastRequestTime, `additionalData`=:additionalData, `lastItem`=:lastItem  WHERE `url`=:url;)";
				bool transaction = depositoryShared->transaction( );
				std::shared_ptr< QSqlQuery > sqlQuery = depositoryShared->generateSqlQuery( );
				sqlQuery.get( )->prepare( cmd );
				for( auto &novel : updateList ) {
					novel->getNovelUrl( &novelUrl );
					novel->getNovelUpdateTime( &novelUpdateTime );
					novel->getNovelLastItem( &novelLastItem );
					novel->getNovelLastRequestGetTime( &novelLastRequestTime );
					void *ptr = &novelAuthor;
					novel->getNovelAttach( ptr );
					sqlQuery->bindValue( ":updateTime", QString::fromStdWString( novelUpdateTime ) );
					sqlQuery->bindValue( ":lastRequestTime", QString::fromStdWString( novelLastRequestTime ) );
					sqlQuery->bindValue( ":lastItem", QString::fromStdWString( novelLastItem ) );
					sqlQuery->bindValue( ":additionalData", QString::fromStdWString( novelAuthor ) );
					sqlQuery->bindValue( ":url", QString::fromStdWString( novelUrl ) );
					if( !depositoryShared->exec( sqlQuery.get( ) ) )
						OStream::anyDebugOut( thisOStream, "无法更新正确的小说内容", __FILE__, __LINE__, __FUNCTION__ );
				}
				if( transaction )
					depositoryShared->commit( );
				transaction = depositoryShared->transaction( );
				auto rootQStringUrl = QString::fromStdWString( rootUrl );
				cmd = R"(INSERT INTO `)" + tabName + R"(`( `rootUrl`, `novelName`, `info`, `updateTime`, `format`, `lastRequestTime`, `lastRequestTimeFormat`, `author`, `url`, `lastItem`, `additionalData`, `typePageUrl`, `typeName`  ) VALUES ( :rootUrl,:novelName,:info,:updateTime,:format,:lastRequestTime,:lastRequestTimeFormat,:author,:url,:lastItem,:additionalData,:typePageUrl,:typeName  );)";
				sqlQuery.get( )->prepare( cmd );
				QStringList tabValues;
				sqlQuery->bindValue( ":rootUrl", rootQStringUrl );
				for( auto &novel : interList ) {
					novel->getNovelName( &novelName );
					novel->getNovelInfo( &novelInfo );
					novel->getNovelUpdateTime( &novelUpdateTime );
					novel->getNovelUpdateTimeFormat( &novelFormat );
					novel->getNovelLastRequestGetTime( &novelLastRequestTime );
					novel->getNovelLastRequestGetTimeFormat( &novelLastRequestTimeFormat );
					novel->getNovelAuthor( &novelAuthor );
					novel->getNovelUrl( &novelUrl );
					novel->getNovelLastItem( &novelLastItem );
					void *ptr = &novelAuthor;
					novel->getNovelAttach( ptr );
					novel->getNovelUrlAtPageLocation( &novelTypePageUrl );
					novel->getNovelTypeName( &novelTypeName );

					sqlQuery->bindValue( ":novelName", QString::fromStdWString( novelName ) );
					sqlQuery->bindValue( ":info", QString::fromStdWString( novelInfo ) );
					sqlQuery->bindValue( ":updateTime", QString::fromStdWString( novelUpdateTime ) );
					sqlQuery->bindValue( ":format", QString::fromStdWString( novelFormat ) );
					sqlQuery->bindValue( ":lastRequestTime", QString::fromStdWString( novelLastRequestTime ) );
					sqlQuery->bindValue( ":lastRequestTimeFormat", QString::fromStdWString( novelLastRequestTimeFormat ) );
					sqlQuery->bindValue( ":author", QString::fromStdWString( novelAuthor ) );
					sqlQuery->bindValue( ":url", QString::fromStdWString( novelUrl ) );
					sqlQuery->bindValue( ":lastItem", QString::fromStdWString( novelLastItem ) );
					sqlQuery->bindValue( ":additionalData", QString::fromStdWString( novelAdditionalData ) );
					sqlQuery->bindValue( ":typePageUrl", QString::fromStdWString( novelTypePageUrl ) );
					sqlQuery->bindValue( ":typeName", QString::fromStdWString( novelTypeName ) );
					if( !depositoryShared->exec( sqlQuery.get( ) ) )
						OStream::anyDebugOut( thisOStream, "无法插入正确的小说内容", __FILE__, __LINE__, __FUNCTION__ );
				}

				if( transaction )
					depositoryShared->commit( );
				sqlQuery.reset( );
				auto close = depositoryShared->close( );
			}

		};
		bool has = true;
		cylHtmlTools::HtmlWorkThread< bool * > thread( nullptr, currentThreadRun, nullptr, &has );
		thread.start( );
		while( thread.isRun( ) )
			qApp->processEvents( );
	}

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
