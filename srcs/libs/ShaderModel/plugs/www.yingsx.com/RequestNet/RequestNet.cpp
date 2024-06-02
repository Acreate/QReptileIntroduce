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
#include <qbrush.h>

#include <DB/DBTools.h>
#include <DB/dbInterface/i_depositoryInterface/I_Depository.h>
#include <DB/dbInterface/i_resultInfo/I_ResultInfo.h>
#include <DB/dbInterface/i_tabInfo/I_TabInfo.h>
#include <DB/dbInterface/I_DB.h>
#include <QSqlQuery>

#include "../NovelInfo/NovelInfo.h"
#include "dateTime/DateTime.h"
#include "../instance_function/instance_function.h"
using namespace interfacePlugsType;
using namespace cylHtmlTools;

QString RequestNet::timeForm = QObject::tr( u8R"(yyyy-MM-dd hh:mm:ss)" );
QString RequestNet::orgCurrentFormToUpdateTimeForm = u8R"(yyyy-%1 hh:mm:ss)";
QString RequestNet::currentTimeForm = QObject::tr( u8R"(yyyy-MM-dd hh:mm:ss)" );
int RequestNet::expireDay = 2;
QDateTime RequestNet::currentTime;


RequestNet::RequestNet( QObject *parent ): QObject( parent ), rootUrl( GET_URL ), oStream( nullptr ), iStream( nullptr ), thisOStream( nullptr ), typeUrlMap( nullptr ) {
}

RequestNet::~RequestNet( ) {
	qDebug( ) << "RequestNet::~RequestNet( " << IRequestNetInterface_iid << ")";

}
size_t RequestNet::getRequestInterval( ) {
	return 1000;
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


Map_HtmlStrK_HtmlStrV * RequestNet::formHtmlGetTypeTheUrls( const interfacePlugsType::HtmlDocString &url, const HtmlDocString &htmlText ) { // todo : 从首页获取类型页面
	auto removeBothSpaceHtmlText = htmlText;
	HtmlStringTools::removeBothSpace( removeBothSpaceHtmlText );
	if( removeBothSpaceHtmlText.size( ) == 0 )
		return nullptr;
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
			auto xpath = cylHtmlTools::XPath( instance_function::NovelNodeXPathInfo::novels_root_get_type_xpath );

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
	if( result->size( ) > 0 ) {
		typeUrlMap = result;
		return typeUrlMap.get( );
	}
	return nullptr;
}
Vector_INovelInfoSPtr RequestNet::formHtmlGetTypePageNovels( const interfacePlugsType::HtmlDocString &type_name, const interfacePlugsType::HtmlDocString &request_url, const HtmlDocString &htmlText, const Vector_INovelInfoSPtr &saveNovelInfos, void *appendDataPtr ) { // todo :从类型页面中解析小说信息
	Vector_INovelInfoSPtr result;

	auto htmlDoc = cylHtmlTools::HtmlDoc::parse( htmlText );

	if( !htmlDoc ) {
		auto typeNme = QString::fromStdWString( type_name );
		auto msg = QString( "%1 : %2 : %3" ).arg( typeNme ).arg( request_url ).arg( QString( u8" HtmlDoc::parse 异常，登出" ) );
		QUrl url( QString::fromStdWString( request_url ) );
		instance_function::write_error_info_file( oStream, url, "parse", typeNme, ".html", __FILE__, __FUNCTION__, __LINE__, msg, msg );
		return result;
	}
	htmlDoc->analysisBrotherNode( );
	auto xpath = cylHtmlTools::XPath( instance_function::NovelNodeXPathInfo::novels_type_get_novels_node_xpath );
	auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
	auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );

	instance_function::NovelNodeXPathInfo novelNodeXPathInfo;
	auto timeArgForm = currentTime.toString( orgCurrentFormToUpdateTimeForm );

	if( vectorHtmlNodeSPtrShared ) { // 首次的页面算法
		novelNodeXPathInfo.formTypePageGetNovelNameXpath = QString( tr( u8R"(./span[@class="s2"]/a)" ) ).toStdWString( );
		novelNodeXPathInfo.formTypePageGetNovelUpdateTimeXpath = QString( tr( u8R"(./span[@class="s3"])" ) ).toStdWString( );
		novelNodeXPathInfo.formTypePageGetNovelAuthorXpath = QString( tr( u8R"(./span[@class="s5"])" ) ).toStdWString( );
		novelNodeXPathInfo.formTypePageGetNovelLastUpdateItemXpath = QString( tr( u8R"(./span[@class="s3"]/a)" ) ).toStdWString( );
		//novelNodeXPathInfo.formTypePageGetNovelInfoXpath = QString( tr( u8R"(./dd[@class='book_des'])" ) ).toStdWString( );
		novelNodeXPathInfo.normal_update_time_function = [&]( HtmlString_Shared &html_string_shared )->QString {
			size_t start = html_string_shared->find( L"(" );
			*html_string_shared = html_string_shared->substr( start + 1 );
			start = html_string_shared->find( L")" );
			*html_string_shared = html_string_shared->substr( 0, start );
			QString fromStdWString = QString::fromStdWString( *html_string_shared );
			return timeArgForm.arg( fromStdWString );
		};
	} else {
		auto typeNme = QString::fromStdWString( type_name );
		auto msg = QString( "%1 : %2 : %3" ).arg( typeNme ).arg( request_url ).arg( QString( u8" xpath 异常，登出" ) );
		QUrl url( QString::fromStdWString( request_url ) );
		instance_function::write_error_info_file( oStream, url, "parse", typeNme, ".html", __FILE__, __FUNCTION__, __LINE__, msg, msg );
		return result;
	}
	cylHtmlTools::HtmlWorkThread< bool * >::Current_Thread_Run currentThreadRun = [&vectorHtmlNodeSPtrShared,&saveNovelInfos,&result, &type_name,&xpath,&request_url,&htmlText,&novelNodeXPathInfo,this]( const cylHtmlTools::HtmlWorkThread< bool * > *html_work_thread, const std::thread *run_std_cpp_thread, std::mutex *html_work_thread_mutex, std::mutex *std_cpp_thread_mutex, bool *data, const time_t *startTime ) {
		auto vectorIterator = vectorHtmlNodeSPtrShared->begin( );
		auto vectorEnd = vectorHtmlNodeSPtrShared->end( );
		std::shared_ptr< INovelInfo > novelInfoPtr = nullptr;
		Vector_HtmlNodeSPtr_Shared htmlNodes = nullptr;
		HtmlString_Shared content = nullptr;
		QString rootUrl = GET_URL;
		Novel_Xpath_Analysis_Error quitMsg;
		HtmlNode *element;
		auto novelInfoBuffPtr = std::make_shared< NovelInfo >( );
		size_t novelCount = saveNovelInfos.size( );
		for( ; vectorIterator != vectorEnd; ++vectorIterator ) {
			quitMsg = None;
			do {
				//////////// url xpath
				xpath = cylHtmlTools::XPath( novelNodeXPathInfo.formTypePageGetNovelNameXpath );
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
				auto begin = unorderedMapAttribute->begin( );
				auto second = begin->second;
				auto newSecond = second.substr( 1, second.length( ) - 2 );
				novelInfoBuffPtr->url = std::make_shared< QString >( QString::fromStdWString( newSecond ) );
				//////////// 名称 xpath
				auto contentText = element->getNodeIncludeContentText( );
				if( contentText )
					novelInfoBuffPtr->novelName = std::make_shared< QString >( QString::fromStdWString( *contentText ) );
				else
					novelInfoBuffPtr->novelName = std::make_shared< QString >( QString::fromStdWString( L"" ) );

				//////////// 更新时间 xpath
				xpath = cylHtmlTools::XPath( novelNodeXPathInfo.formTypePageGetNovelUpdateTimeXpath );
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
				QString fromStdWString = novelNodeXPathInfo.normal_update_time_function( lastTime );
				QDateTime novelTime = QDateTime::fromString( fromStdWString, timeForm );
				auto compareDateTime = DateTime::compareDateTime( currentTime, novelTime );
				int16_t timeToDay = DateTime::getTimeToDay( compareDateTime );
				if( abs( timeToDay ) > RequestNet::expireDay ) {
					quitMsg = DateTime_Error_Expire; // xpath 异常 : 更新时间 找不到
					break;
				}
				novelInfoBuffPtr->updateTime = std::make_shared< QString >( fromStdWString );
				//////////// 作者 xpath
				xpath = cylHtmlTools::XPath( novelNodeXPathInfo.formTypePageGetNovelAuthorXpath );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( htmlNodes ) {
					auto author = htmlNodes->at( 0 ).get( )->getNodeIncludeContentText( );
					if( author )
						novelInfoBuffPtr->author = std::make_shared< QString >( QString::fromStdWString( *author ) );
				}
				//////////// 最后更新项目 xpath
				xpath = cylHtmlTools::XPath( novelNodeXPathInfo.formTypePageGetNovelLastUpdateItemXpath );
				htmlNodes = vectorIterator->get( )->xpath( xpath );
				if( htmlNodes ) {
					auto lastItem = htmlNodes->at( 0 ).get( )->getNodeIncludeContentText( );
					if( lastItem )
						novelInfoBuffPtr->lastItem = std::make_shared< QString >( QString::fromStdWString( *lastItem ) );
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
			if( novelInfoPtr ) { // 为空，说明没有被赋值，也就是异常
				result.emplace_back( novelInfoPtr ); // 加入列表
				// 成功获取即可输出
				QString outMsg( u8"[%1]小说(%2):url(%3) -> 解析成功 [%4]" );
				++novelCount;
				outMsg = outMsg.arg( *novelInfoBuffPtr->typeName ).arg( *novelInfoBuffPtr->novelName ).arg( *novelInfoBuffPtr->url ).arg( novelCount );
				OStream::anyDebugOut( thisOStream, outMsg );
				novelInfoBuffPtr = std::make_shared< NovelInfo >( );
			} else {
				QString errorInfo = instance_function::get_error_info( quitMsg, QString::fromStdWString( xpath.getHtmlString( ) ) );
				auto typeNme = QString::fromStdWString( type_name );
				auto msg = QString( "%1 : %2 : %3" ).arg( typeNme ).arg( request_url ).arg( errorInfo );
				QUrl url( QString::fromStdWString( request_url ) );
				instance_function::write_error_info_file( oStream, url, "parse", typeNme, ".html", __FILE__, __FUNCTION__, __LINE__, msg, msg );
				novelInfoBuffPtr->clear( ); // 重置
			}
			novelInfoPtr.reset( ); // 重置
		}
	};

	bool has = false;
	cylHtmlTools::HtmlWorkThread< bool * > thread( nullptr, currentThreadRun, nullptr, &has );
	thread.start( );
	while( thread.isRun( ) )
		qApp->processEvents( );
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
	auto htmlDoc = cylHtmlTools::HtmlDoc::parse( htmlText );
	if( htmlDoc ) {
		auto xpath = cylHtmlTools::XPath( instance_function::NovelNodeXPathInfo::novels_type_get_type_next_xpath );
		auto htmlNodes = htmlDoc->xpathRootNodes( xpath );
		if( htmlNodes ) {
			auto iterator = htmlNodes->begin( );
			auto endIterator = htmlNodes->end( );
			HtmlString wstrNextPageKey = L"&gt;";
			for( ; iterator != endIterator; ++iterator ) {
				HtmlNode *element = iterator->get( );
				HtmlString_Shared contentText = element->getNodeIncludeContentText( );
				if( HtmlStringTools::equRemoveSpaceOverHtmlString( contentText.get( ), &wstrNextPageKey ) ) {
					auto findAttribute = element->findAttribute( []( const HtmlString &attributeName, const HtmlString &attributeValue ) {
						if( HtmlStringTools::equRemoveSpaceOverHtmlString( attributeName, L"href" ) )
							return true;
						return false;
					} );
					auto pair = findAttribute->begin( );
					if( pair == findAttribute->end( ) )
						continue;
					result = QString::fromStdWString( pair->second.substr( 1, pair->second.length( ) - 2 ) ).toStdWString( );
					break;
				}

			}
		}
	}

	return result;
}
bool RequestNet::isRequestNovelInfoUrl( const interfacePlugsType::INovelInfoPtr &novel_info_ptr ) {
	return false;
}
void RequestNet::novelTypeEnd( const HtmlDocString &root_url, const HtmlDocString &type_name, const HtmlDocString &url, const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos ) {
}
void RequestNet::endHost( const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos, const std::function< bool( const std::chrono::system_clock::time_point::duration & ) > &run ) {

	QString linkPath( u8"%1%2" );
	linkPath = linkPath.arg( Cache_Path_Dir ).arg( "dbs/" );
	if( !QDir( linkPath ).mkpath( linkPath ) )
		linkPath = Cache_Path_Dir;
	auto dbInterface = cylDB::DBTools::linkDB( linkPath );

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
					if( !instance_function::generate_db_tab( dbInterface, depositoryShared, tabName, thisOStream ) )
						return;

				QStringList tabFieldNames = { "rootUrl", "novelName", "info", "updateTime", "format", "lastRequestTime", "lastRequestTimeFormat", "author", "url", "lastItem", "additionalData", "typePageUrl", "typeName" };
				auto allItem = depositoryShared->findItems( tabName, tabFieldNames );

				interfacePlugsType::Vector_INovelInfoSPtr updateList; // 更新列表
				interfacePlugsType::Vector_INovelInfoSPtr interList; // 插入列表
				// 分解-插入/更新 列表
				if( allItem ) {
					instance_function::separate_list( saveNovelInfos, allItem, updateList, interList );
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
				QString cmd = R"(UPDATE `)" + tabName + R"(` SET `updateTime`=:updateTime, `lastRequestTime`=:lastRequestTime, `additionalData`=:additionalData, `lastItem`=:lastItem , `format`=:format  WHERE `url`=:url;)";
				bool transaction = depositoryShared->transaction( );
				std::shared_ptr< QSqlQuery > sqlQuery = depositoryShared->generateSqlQuery( );
				sqlQuery.get( )->prepare( cmd );
				for( auto &novel : updateList ) {
					novel->getNovelUrl( &novelUrl );
					novel->getNovelUpdateTime( &novelUpdateTime );
					novel->getNovelUpdateTimeFormat( &novelFormat );
					novel->getNovelLastItem( &novelLastItem );
					novel->getNovelLastRequestGetTime( &novelLastRequestTime );
					void *ptr = &novelAuthor;
					novel->getNovelAttach( ptr );
					sqlQuery->bindValue( ":updateTime", QString::fromStdWString( novelUpdateTime ) );
					sqlQuery->bindValue( ":format", QString::fromStdWString( novelFormat ) );
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
				depositoryShared->close( );
			}
		};

		bool has = true;
		cylHtmlTools::HtmlWorkThread< bool * > thread( nullptr, currentThreadRun, nullptr, &has );
		thread.start( );

		auto currentTime = std::chrono::system_clock::now( ).time_since_epoch( );
		while( thread.isRun( ) ) {
			auto epoch = std::chrono::system_clock::now( ).time_since_epoch( );
			auto duration = currentTime - epoch;
			run( duration );
			currentTime = epoch;
			qApp->processEvents( );
		}
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
