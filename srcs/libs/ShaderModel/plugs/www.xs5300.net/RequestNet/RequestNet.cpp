﻿#include "RequestNet.h"
#include <string>

#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/htmlTools/XPath/XPath.h"
#include <qguiapplication.h>

#include <stream/OStream.h>
#include <qbrush.h>
#include <QDir>
#include <DB/DBTools.h>
#include "dateTime/DateTime.h"
#include <interface/instance_function.h>
#include "../instance_function/NovelNodeXPathInfo.h"
#include "interface/INovelInfo.h"
using namespace interfacePlugsType;
using namespace cylHtmlTools;
using namespace instance_function;

QString RequestNet::currentTimeForm = QObject::tr( u8R"(yyyy-MM-dd hh:mm:ss)" );
QDateTime RequestNet::currentTime;

RequestNet::RequestNet( QObject *parent ): QObject( parent ), rootUrl( GET_URL ), oStream( nullptr ), iStream( nullptr ), thisOStream( nullptr ), typeUrlMap( nullptr ) {
	outPath = "Cache_Path_Dir";
	expireDay = NovelInfo::getExpireDay( );
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


Map_HtmlStrK_HtmlStrV * RequestNet::formHtmlGetTypeTheUrls( const interfacePlugsType::HtmlDocString &url, const HtmlDocString &htmlText ) {	// todo : 首页获取类型
	auto removeBothSpaceHtmlText = htmlText;
	HtmlStringTools::removeBothSpace( removeBothSpaceHtmlText );
	
	
	auto outLogPath = outPath + QDir::separator( ) + u8"logs" + QDir::separator( );
	if( removeBothSpaceHtmlText.size( ) == 0 ) {
		instance_function::error::root::htmlDoc::write_error_info_file( QString::fromStdWString( url ), outLogPath, instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__, oStream );
		return nullptr;
	}
	auto stdWString( std::make_shared< HtmlString >( removeBothSpaceHtmlText ) );
	auto result = std::make_shared< Map_HtmlStrK_HtmlStrV >( );
	auto htmlDoc = cylHtmlTools::HtmlDoc::parse( stdWString );
	if( !htmlDoc.get( ) ) {
		instance_function::error::root::xpath::write_error_info_file( QString::fromStdWString( url ), outLogPath, QString::fromStdWString( removeBothSpaceHtmlText ), instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__, oStream );
		return nullptr;
	}

	htmlDoc->analysisBrotherNode( );
	auto xpath = cylHtmlTools::XPath( instance_function::NovelNodeXPathInfo::novels_root_get_type_xpath );

	auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
	auto vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
	if( !vectorHtmlNodeSPtrShared ) {

		instance_function::error::root::xpath::write_error_info_file( QString::fromStdWString( url ), outLogPath, QString::fromStdWString( removeBothSpaceHtmlText ), instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__, oStream );
		return nullptr;
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
	if( result->size( ) > 0 ) {
		typeUrlMap = result;
		return typeUrlMap.get( );
	}
	return nullptr;
}

Vector_INovelInfoSPtr RequestNet::formHtmlGetTypePageNovels( const interfacePlugsType::HtmlDocString &type_name, const interfacePlugsType::HtmlDocString &request_url, const HtmlDocString &htmlText, const Vector_INovelInfoSPtr &saveNovelInfos, void *appendDataPtr ) { // todo : 类型页面获取小说信息
	Vector_INovelInfoSPtr result;
	auto htmlDoc = cylHtmlTools::HtmlDoc::parse( htmlText );
	auto outLogPath = outPath + QDir::separator( ) + u8"logs" + QDir::separator( );
	if( !htmlDoc ) {
		instance_function::error::type::htmlDoc::write_error_info_file( QString::fromStdWString( type_name ), QString::fromStdWString( request_url ), outLogPath, QString::fromStdWString( htmlText ), instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__, oStream );
		return result;
	}
	htmlDoc->analysisBrotherNode( );
	auto xpath = cylHtmlTools::XPath( instance_function::NovelNodeXPathInfo::novels_type_get_novels_node_xpath );
	auto htmlNodeSPtrShared = htmlDoc->getHtmlNodeRoots( );
	Vector_HtmlNodeSPtr_Shared vectorHtmlNodeSPtrShared = xpath.buider( htmlNodeSPtrShared );
	instance_function::NovelNodeXPathInfo novelNodeXPathInfo;
	QString currentYearStart = currentTime.toString( "yyyy-" );
	QString currentHMS = currentTime.toString( " hh:mm:ss" );
	if( vectorHtmlNodeSPtrShared ) { // 首次的页面算法
		novelNodeXPathInfo.formTypePageGetNovelNameXpath = QString( tr( u8R"(./span[@class="s2"]/a)" ) ).toStdWString( );
		novelNodeXPathInfo.formTypePageGetNovelUpdateTimeXpath = QString( tr( u8R"(.//span[@class="s3"])" ) ).toStdWString( );
		novelNodeXPathInfo.formTypePageGetNovelAuthorXpath = QString( tr( u8R"(./span[@class="s5"])" ) ).toStdWString( );
		novelNodeXPathInfo.formTypePageGetNovelLastUpdateItemXpath = QString( tr( u8R"(./span[@class="s3"]/a)" ) ).toStdWString( );
		novelNodeXPathInfo.normal_update_time_function = [&]( HtmlString_Shared &html_string_shared )->QString {
			QString resultQString = QString::fromStdWString( *html_string_shared );
			qsizetype last_index_of = resultQString.lastIndexOf( '(' );
			if( last_index_of == -1 )
				return "";
			last_index_of += 1;
			qint64 index_of = resultQString.lastIndexOf( ')' );
			resultQString = resultQString.mid( last_index_of, index_of - last_index_of );
			return currentYearStart + resultQString + currentHMS;
		};
		novelNodeXPathInfo.check_html_is_first_type_url = [&]( HtmlNode *html_node )->bool {
			return false;
		};
	}
	if( !vectorHtmlNodeSPtrShared ) {
		instance_function::error::type::xpath::write_error_info_file( QString::fromStdWString( type_name ), QString::fromStdWString( request_url ), outLogPath, QString::fromStdWString( htmlText ), instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__, oStream );
		return result;
	}
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
		size_t line = __LINE__;
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
				line = __LINE__;
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
				line = __LINE__;
				break;
			}
			auto begin = unorderedMapAttribute->begin( );
			auto second = begin->second;
			auto newSecond = second.substr( 1, second.length( ) - 2 );
			novelInfoBuffPtr->url = newSecond;
			//////////// 名称 xpath
			auto contentText = element->getNodeIncludeContentText( );
			if( contentText )
				novelInfoBuffPtr->novelName = *contentText;

			//////////// 更新时间 xpath
			element = vectorIterator->get( ); // 重置查找到的 node
			xpath = cylHtmlTools::XPath( novelNodeXPathInfo.formTypePageGetNovelUpdateTimeXpath );
			htmlNodes = element->xpath( xpath );
			if( !htmlNodes ) {
				quitMsg = DateTime_Error_Xpath; // xpath 异常 : 更新时间
				line = __LINE__;
				break;
			}
			auto lastTime = htmlNodes->at( 0 ).get( )->getNodeIncludeContentText( );
			if( !lastTime ) {
				quitMsg = DateTime_Error_None; // xpath 异常 : 更新时间 找不到
				line = __LINE__;
				break;
			}
			QString fromStdWString = novelNodeXPathInfo.normal_update_time_function( lastTime );
			if( fromStdWString.length( ) == 0 ) {
				quitMsg = DateTime_Error_None; // xpath 异常 : 更新时间 找不到
				line = __LINE__;
				break;
			}
			QDateTime novelTime = QDateTime::fromString( fromStdWString, currentTimeForm );
			auto compareDateTime = DateTime::compareDateTime( currentTime, novelTime );
			int16_t timeToDay = DateTime::getMillisecondsTimeToDay( compareDateTime );
			if( abs( timeToDay ) > expireDay ) {
				quitMsg = DateTime_Error_Expire; // xpath 异常 : 更新时间 找不到
				line = __LINE__;
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
				instance_function::write_error_info_file( oStream, url, outLogPath, "novel_listItem_error", "none", typeNme, ".html", instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, __LINE__, msg, QString::fromStdWString( htmlText ) );
			}

			novelInfoBuffPtr->format = RequestNet::currentTimeForm.toStdWString( );
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
			QString typeName = QString::fromStdWString( novelInfoBuffPtr->typeName );
			QString novelName = QString::fromStdWString( novelInfoBuffPtr->novelName );
			QString url = QString::fromStdWString( novelInfoBuffPtr->url );
			outMsg = outMsg.arg( typeName ).arg( novelName ).arg( url ).arg( novelCount );
			OStream::anyStdCOut( outMsg, thisOStream );
			novelInfoBuffPtr = std::make_shared< NovelInfo >( );
		} else {
			QString errorInfo = instance_function::get_error_info( quitMsg, QString::fromStdWString( xpath.getHtmlString( ) ) );
			auto typeNme = QString::fromStdWString( type_name );
			auto msg = QString( "%1 : %2 : %3" ).arg( typeNme ).arg( request_url ).arg( errorInfo );
			if( DateTime_Error_Xpath > quitMsg ) {
				QUrl url( QString::fromStdWString( request_url ) );
				instance_function::write_error_info_file( oStream, url, outLogPath, "novel_error", "parse", typeNme, ".html", instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), __FUNCTION__, line, msg, msg );
			} else
				OStream::anyStdCerr( msg, instance_function::getCmakeRootPathBuilderFilePath( __FILE__ ), line, __FUNCTION__, oStream );
			novelInfoBuffPtr->clear( ); // 重置
		}
		novelInfoPtr.reset( ); // 重置
	}
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
				if( HtmlStringTools::findNextHtmlStringPotion( contentText.get( ), &wstrNextPageKey ) ) {
					auto findAttribute = element->findAttribute( []( const HtmlString &attributeName, const HtmlString &attributeValue ) {
						if( HtmlStringTools::equRemoveSpaceOverHtmlString( attributeName, L"href" ) )
							return true;
						return false;
					} );
					auto pair = findAttribute->begin( );
					if( pair == findAttribute->end( ) )
						continue;
					result = pair->second.substr( 1, pair->second.length( ) - 2 );
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
