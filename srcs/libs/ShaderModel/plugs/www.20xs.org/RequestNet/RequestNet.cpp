﻿#include "RequestNet.h"
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
QDateTime RequestNet::currentTime;
int RequestNet::expireDay = 2;

RequestNet::RequestNet( QObject *parent ): QObject( parent ), rootUrl( GET_URL ), oStream( nullptr ), iStream( nullptr ), thisOStream( nullptr ), typeUrlMap( nullptr ) {
	outPath = Cache_Path_Dir;
}

RequestNet::~RequestNet( ) {
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
	*outHost = this->rootUrl.host( ).toStdWString( );
	return outHost->length( );
}
size_t RequestNet::getScheme( HtmlDocString *outScheme ) {
	*outScheme = this->rootUrl.scheme( ).toStdWString( );
	return outScheme->length( );
}
size_t RequestNet::getRootUrl( HtmlDocString *outStr ) {
	*outStr = rootUrl.url( ).toStdWString( );
	return outStr->length( );
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
		HtmlWorkThread thread;
		auto stdWString( std::make_shared< HtmlString >( removeBothSpaceHtmlText ) );
		auto result = std::make_shared< Map_HtmlStrK_HtmlStrV >( );
		thread.setCurrentThreadRun( [&result,&stdWString, this,&url]( ) {
			auto htmlDoc = cylHtmlTools::HtmlDoc::parse( stdWString );
			if( !htmlDoc.get( ) ) {
				auto msg = QString( "%1 : %2" ).arg( QString::fromStdWString( url ) ).arg( QString( u8" HtmlDoc::parse 异常，登出" ) );
				OStream::anyStdCerr( msg, __FILE__, __LINE__, __FUNCTION__, thisOStream );
				return;
			}

			htmlDoc->analysisBrotherNode( );
			auto xpath = cylHtmlTools::XPath( instance_function::NovelNodeXPathInfo::novels_root_get_type_xpath );

			auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
			auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
			if( !vectorHtmlNodeSPtrShared ) {
				auto msg = QString( "%1 : %2 " ).arg( QString::fromStdWString( url ) ).arg( QString( u8" xpath 异常，登出" ) );
				OStream::anyStdCerr( msg, __FILE__, __LINE__, __FUNCTION__, thisOStream );
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
		instance_function::write_error_info_file( oStream, url, outPath, "formHtmlGetTypePageNovels", "parse", typeNme, ".html", __FILE__, __FUNCTION__, __LINE__, msg, msg );
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
			auto typeNme = QString::fromStdWString( type_name );
			auto msg = QString( "%1 : %2 : %3" ).arg( typeNme ).arg( request_url ).arg( QString( u8" xpath 异常，登出" ) );
			QUrl url( QString::fromStdWString( request_url ) );
			instance_function::write_error_info_file( oStream, url, outPath, "xpath", "parse", typeNme, ".html", __FILE__, __FUNCTION__, __LINE__, msg, msg );
			return result;
		}
	}

	std::function< void( ) > currentThreadRun = [&vectorHtmlNodeSPtrShared,&saveNovelInfos,&result, &type_name,&xpath,&request_url,&htmlText,&novelNodeXPathInfo,this]( ) {
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
				novelInfoBuffPtr->url = QString( u8"%1%2" ).arg( rootUrl ).arg( urlLastStr ).toStdWString( );
				//////////// 名称 xpath
				auto contentText = element->getNodeIncludeContentText( );
				if( contentText )
					novelInfoBuffPtr->name = *contentText;

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
				novelInfoBuffPtr->updateTime = fromStdWString.toStdWString( );
				//////////// 作者 xpath
				element = vectorIterator->get( ); // 重置查找到的 node
				xpath = cylHtmlTools::XPath( novelNodeXPathInfo.formTypePageGetNovelAuthorXpath );
				htmlNodes = element->xpath( xpath );
				if( htmlNodes ) {
					auto author = htmlNodes->at( 0 ).get( )->getNodeIncludeContentText( );
					if( author )
						novelInfoBuffPtr->author = *author;
				}
				//////////// 最后更新项目 xpath
				element = vectorIterator->get( ); // 重置查找到的 node
				xpath = cylHtmlTools::XPath( novelNodeXPathInfo.formTypePageGetNovelLastUpdateItemXpath );
				htmlNodes = element->xpath( xpath );
				if( htmlNodes ) {
					auto lastItem = htmlNodes->at( 0 ).get( )->getNodeIncludeContentText( );
					if( lastItem )
						novelInfoBuffPtr->lastItem = *lastItem;
				}
				if( novelInfoBuffPtr->lastItem.empty( ) ) {
					auto novelUrl = QString::fromStdWString( novelInfoBuffPtr->url );
					novelUrl.append( u8" => 找不到最后选项" );
					auto typeNme = QString::fromStdWString( type_name );
					auto msg = QString( "%1 : %2 : %3" ).arg( typeNme ).arg( request_url ).arg( novelUrl );
					QUrl url( QString::fromStdWString( request_url ) );
					instance_function::write_error_info_file( oStream, url, outPath, "novel_listItem_error", "none", typeNme, ".html", __FILE__, __FUNCTION__, __LINE__, msg, QString::fromStdWString( htmlText ) );
				}

				novelInfoBuffPtr->format = RequestNet::timeForm.toStdWString( );
				novelInfoBuffPtr->typePageUrl = request_url;
				novelInfoBuffPtr->typeName = type_name;
				// 成功则赋值
				novelInfoPtr = novelInfoBuffPtr;
			} while( false );
			if( novelInfoPtr ) { // 为空，说明没有被赋值，也就是异常
				result.emplace_back( novelInfoPtr ); // 加入列表
				// 成功获取即可输出
				QString outMsg( u8"[%1]小说(%2):url(%3) -> 解析成功 [%4]" );
				++novelCount;
				outMsg = outMsg.arg( novelInfoBuffPtr->getQStringTypeName( ) ).arg( novelInfoBuffPtr->getQStringName( ) ).arg( novelInfoBuffPtr->getQStringUrl( ) ).arg( novelCount );
				OStream::anyStdCOut( outMsg, thisOStream );
				novelInfoBuffPtr = std::make_shared< NovelInfo >( );
			} else {
				if( DateTime_Error_Xpath < quitMsg ) {
					QString errorInfo = instance_function::get_error_info( quitMsg, QString::fromStdWString( xpath.getHtmlString( ) ) );
					auto typeNme = QString::fromStdWString( type_name );
					auto msg = QString( "%1 : %2 : %3" ).arg( typeNme ).arg( request_url ).arg( errorInfo );
					QUrl url( QString::fromStdWString( request_url ) );
					instance_function::write_error_info_file( oStream, url, outPath, "novel_error", "parse", typeNme, ".html", __FILE__, __FUNCTION__, __LINE__, msg, msg );
				}
				novelInfoBuffPtr->clear( ); // 重置
			}
			novelInfoPtr.reset( ); // 重置
		}
	};
	bool has = false;
	cylHtmlTools::HtmlWorkThread thread( nullptr, currentThreadRun, nullptr );
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
