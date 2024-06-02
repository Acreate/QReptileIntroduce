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

QString RequestNet::timeForm = QObject::tr( u8R"(yyyy-MM-dd hh:mm)" );
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
		if( result->size( ) > 0 )
			typeUrlMap = result;
	} while( false );
	if( typeUrlMap == nullptr || typeUrlMap->size( ) == 0 )
		return nullptr;
	return typeUrlMap.get( );
}

Vector_INovelInfoSPtr RequestNet::formHtmlGetTypePageNovels( const interfacePlugsType::HtmlDocString &type_name, const interfacePlugsType::HtmlDocString &request_url, const HtmlDocString &htmlText, const Vector_INovelInfoSPtr &saveNovelInfos, void *appendDataPtr ) { // todo : 类型页面获取小说信息
	Vector_INovelInfoSPtr result;
	auto htmlDoc = cylHtmlTools::HtmlDoc::parse( htmlText );
	if( !htmlDoc ) {
		auto typeNme = QString::fromStdWString( type_name );
		auto msg = QString( "%1 : %2 : %3" ).arg( typeNme ).arg( request_url ).arg( QString( u8" HtmlDoc::parse 异常，登出" ) );
		QUrl url( QString::fromStdWString( request_url ) );
		write_error_info_file( oStream, url, "parse", typeNme, ".html", __FILE__, __FUNCTION__, __LINE__, msg, msg );
		return result;
	}
	htmlDoc->analysisBrotherNode( );
	auto xpath = cylHtmlTools::XPath( instance_function::NovelNodeXPathInfo::novels_type_get_novels_node_xpath );
	auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
	Vector_HtmlNodeSPtr_Shared vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
	instance_function::NovelNodeXPathInfo novelNodeXPathInfo;
	QString currentYearStart = currentTime.toString( "yyyy-" );
	if( vectorHtmlNodeSPtrShared ) { // 首次的页面算法
		novelNodeXPathInfo.formTypePageGetNovelNameXpath = QString( tr( u8R"(./span[@class="s2"]/a)" ) ).toStdWString( );
		novelNodeXPathInfo.formTypePageGetNovelUpdateTimeXpath = QString( tr( u8R"(.//span[@class="s5"])" ) ).toStdWString( );
		novelNodeXPathInfo.formTypePageGetNovelAuthorXpath = QString( tr( u8R"(./span[@class="s4"])" ) ).toStdWString( );
		novelNodeXPathInfo.formTypePageGetNovelLastUpdateItemXpath = QString( tr( u8R"(./span[@class="s3"]/a)" ) ).toStdWString( );
		novelNodeXPathInfo.normal_update_time_function = [&]( HtmlString_Shared &html_string_shared )->QString {
			return currentYearStart + QString::fromStdWString( *html_string_shared );
		};
		novelNodeXPathInfo.check_html_is_first_type_url = [&]( HtmlNode *html_node )->bool {
			return false;
		};
	} else { // 次要的页面算法
		xpath = cylHtmlTools::XPath( QString( tr( u8R"(div[@class="novelslist2"]/ul/li/)" ) ).toStdWString( ) );
		htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
		vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
		if( vectorHtmlNodeSPtrShared ) {
			novelNodeXPathInfo.formTypePageGetNovelNameXpath = QString( tr( u8R"(./span[@class="s2"]/a)" ) ).toStdWString( );
			novelNodeXPathInfo.formTypePageGetNovelUpdateTimeXpath = QString( tr( u8R"(.//span[@class="s7"])" ) ).toStdWString( );
			novelNodeXPathInfo.formTypePageGetNovelAuthorXpath = QString( tr( u8R"(./span[@class="s4"]/a)" ) ).toStdWString( );
			novelNodeXPathInfo.formTypePageGetNovelLastUpdateItemXpath = QString( tr( u8R"(./span[@class="s3"]/a)" ) ).toStdWString( );
			HtmlString continueName = L"小说分类";
			HtmlString jionKey = QString( tr( u8R"(./span[@class="s1"]/b)" ) ).toStdWString( );
			novelNodeXPathInfo.check_html_is_first_type_url = [jionKey,continueName]( HtmlNode *html_node )->bool {
				auto xpath = cylHtmlTools::XPath( jionKey );
				auto htmlNodes = html_node->xpath( xpath );
				if( htmlNodes ) {
					auto buff = htmlNodes->begin( )->get( )->getNodeIncludeContentText( );
					// 跳过奇特页面
					if( HtmlStringTools::findNextHtmlStringPotion( buff.get( ), 0, &continueName ) )
						return true;
				}
				return false;
			};
			novelNodeXPathInfo.normal_update_time_function = [&]( HtmlString_Shared &html_string_shared )->QString {
				return QString::fromStdWString( *html_string_shared );
			};
		} else {
			auto msg = QString( "%1 : %2" ).arg( QString::fromStdWString( url ) ).arg( QString( u8" HtmlDoc::parse 异常，登出" ) );
			OStream::anyDebugOut( thisOStream, msg, __FILE__, __LINE__, __FUNCTION__ );
			return result;
		}
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
		HtmlString_Shared buff;
		auto novelInfoBuffPtr = std::make_shared< NovelInfo >( );
		size_t novelCount = saveNovelInfos.size( );
		for( ; vectorIterator != vectorEnd; ++vectorIterator ) {
			quitMsg = None;
			element = vectorIterator->get( );
			if( novelNodeXPathInfo.check_html_is_first_type_url( element ) )
				continue;
			do {
				element = vectorIterator->get( ); // 重置查找到的 node
				//////////// url xpath
				xpath = cylHtmlTools::XPath( novelNodeXPathInfo.formTypePageGetNovelNameXpath );
				htmlNodes = element->xpath( xpath );
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
				QString urlLastStr = QString::fromStdWString( newSecond );
				novelInfoBuffPtr->url = std::make_shared< QString >( QString( u8"%1%2" ).arg( rootUrl ).arg( urlLastStr ) );
				//////////// 名称 xpath
				auto contentText = element->getNodeIncludeContentText( );
				if( contentText )
					novelInfoBuffPtr->novelName = std::make_shared< QString >( QString::fromStdWString( *contentText ) );
				else
					novelInfoBuffPtr->novelName = std::make_shared< QString >( QString::fromStdWString( L"" ) );

				//////////// 更新时间 xpath
				element = vectorIterator->get( ); // 重置查找到的 node
				xpath = cylHtmlTools::XPath( novelNodeXPathInfo.formTypePageGetNovelUpdateTimeXpath );
				htmlNodes = element->xpath( xpath );
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
				element = vectorIterator->get( ); // 重置查找到的 node
				xpath = cylHtmlTools::XPath( novelNodeXPathInfo.formTypePageGetNovelAuthorXpath );
				htmlNodes = element->xpath( xpath );
				if( htmlNodes ) {
					auto author = htmlNodes->at( 0 ).get( )->getNodeIncludeContentText( );
					if( author )
						novelInfoBuffPtr->author = std::make_shared< QString >( QString::fromStdWString( *author ) );
				}
				//////////// 最后更新项目 xpath
				element = vectorIterator->get( ); // 重置查找到的 node
				xpath = cylHtmlTools::XPath( novelNodeXPathInfo.formTypePageGetNovelLastUpdateItemXpath );
				htmlNodes = element->xpath( xpath );
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
			if( !novelInfoPtr ) { // 为空，说明没有被赋值，也就是异常
				instance_function::out_debug( quitMsg, element, novelInfoBuffPtr.get( ), type_name, request_url, xpath, thisOStream, htmlText );
				novelInfoBuffPtr->clear( ); // 重置
			} else {
				result.emplace_back( novelInfoPtr ); // 加入列表
				// 成功获取即可输出
				QString outMsg( u8"[%1]小说(%2):url(%3) -> 解析成功 [%4]" );
				++novelCount;
				outMsg = outMsg.arg( *novelInfoBuffPtr->typeName ).arg( *novelInfoBuffPtr->novelName ).arg( *novelInfoBuffPtr->url ).arg( novelCount );
				OStream::anyDebugOut( thisOStream, outMsg );
				novelInfoBuffPtr = std::make_shared< NovelInfo >( );
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
	size_t quitMsg = 0;
	do {
		auto htmlDoc = cylHtmlTools::HtmlDoc::parse( htmlText );
		if( !htmlDoc )
			break;

		auto xpath = cylHtmlTools::XPath( instance_function::NovelNodeXPathInfo::novels_type_get_type_next_xpath );
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
				QString cmd = R"(UPDATE `)" + tabName + R"(` SET `updateTime`=:updateTime, `format`=:format, `lastRequestTime`=:lastRequestTime, `additionalData`=:additionalData, `lastItem`=:lastItem  WHERE `url`=:url;)";
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
				auto close = depositoryShared->close( );
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
