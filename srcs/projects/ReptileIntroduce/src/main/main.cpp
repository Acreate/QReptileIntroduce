#include <QFileDialog>
#include <QGenericPlugin>
#include <QDateTime>
#include <QPluginLoader>
#include <stack>

#include "../qt/group/ui/MainWidget.h"
#include "../qt/extend/core/Appliction.h"
#include "../qt/extend/layout/HLayoutBox.h"
#include "../qt/extend/layout/VLayoutBox.h"
#include "../qt/group/ui/MainWidget/NovelInfoWidget.h"

#include "interface/IRequestNetInterfaceExtend.h"
#include "../macro/cmake_to_c_cpp_header_env.h"

#include "path/Path.h"
#include "../qt/extend/netWork/Request.h"
#include "../qt/extend/netWork/RequestConnect.h"

#include <htmls/htmlDoc/HtmlDoc.h>
#include <wstr/WStrTools.h>
#include "htmls/HtmlNode/HtmlNode.h"

void test( QObject *plugQobjectInstance, IRequestNetInterfaceExtend *requestNetInterfaceExtend ) {

	std::string plugUrl;
	qDebug( ) << requestNetInterfaceExtend->getUrl( &plugUrl ) << " : " << plugUrl;
	Request *request = new Request( plugQobjectInstance );
	RequestConnect *connect = new RequestConnect( request );
	auto cPlugUrl = plugUrl.c_str( );
	// &RequestConnect::networkAccessManagerFinished, [=]( QNetworkReply *reply )
	connect->connect( connect, &RequestConnect::networkAccessManagerFinished, [=]( QNetworkReply *networkReply ) {
		QUrl url = networkReply->url( );
		std::string host;
		requestNetInterfaceExtend->getHost( &host );
		QString comtStr = url.host( );
		if( comtStr == host.c_str( ) ) {
			QByteArray array = networkReply->readAll( );
			QString writeFile = QString( u8"%1%2%3%2%4%5" ).arg( Project_Run_bin ).arg( QDir::separator( ) ).arg( "wirtFile" ).arg( comtStr ).arg( u8".html" );
			QFileInfo fileInfo( writeFile );
			QFile file( writeFile );
			if( !fileInfo.exists( ) ) {
				auto result = Path::creatFilePath( fileInfo.absoluteFilePath( ) );
			}
			if( file.open( QIODevice::WriteOnly | QIODevice::Text | QIODeviceBase::ExistingOnly ) )
				file.write( array );
			QString downLoad( array );
			auto pairs = requestNetInterfaceExtend->formHtmlGetTypeTheUrls( downLoad.toLocal8Bit( ).toStdString( ) );
		}
		request->deleteLater( );
	} );
	request->netGetWork( cPlugUrl, connect );
}

int main( int argc, char *argv[ ] ) {
	Appliction appliction( argc, argv );

	QString cecheFilePath = QString( u8"%1%2writeFile%2%3" ).arg( Project_Run_bin ).arg( QDir::separator( ) ).arg( u8"www.121ds.cc.html" );
	QFileInfo readHtmlFileInfo( cecheFilePath );
	if( readHtmlFileInfo.exists( ) ) {
		QFile readFile( readHtmlFileInfo.absoluteFilePath( ) );
		if( readFile.open( QIODeviceBase::ReadOnly | QIODeviceBase::Text ) ) {
			QString fileContent = readFile.readAll( );
			auto stdWString = fileContent.toStdWString( );
			std::shared_ptr< std::wstring > stdcwStrshared( new std::wstring( stdWString.c_str( ), stdWString.length( ) ) );
			size_t cStrLen = stdWString.size( );
			size_t index = 0;
			auto htmlDoc = HtmlTools::HtmlDoc::parse( stdcwStrshared, cStrLen, index );
			auto vectorHtmlNodeSPtrShared = htmlDoc.analysisBrotherNode( );
			QString writeCachePath( u8"%1write_cache%2" );
			writeCachePath = writeCachePath.arg( Project_Run_bin ).arg( QDir::separator( ) );
			bool isRemovePath = Path::removePath( writeCachePath );
			htmlDoc.getNodes( [&]( const HtmlTools::HtmlNode_Shared &node ) {
				qDebug( ) << QString::fromStdWString( *node->getNodeWSName( ) ).toLocal8Bit( ).toStdString( ).c_str( );
				QString writeContent;
				writeContent
					.append( QString::number( node->getPtrOffset( ) ) )
					.append( u8", " )
					.append( QString::number( node->getPtrCWtrLen( ) ) )
					.append( u8" = " )
					.append( QString::number( node->nodeSize( ) ) )
					.append( "\n" u8".......!!!!........" "\n" )
					.append( QString::fromStdWString( node->getPath( )->c_str( ) ) )
					.append( "\n" u8".......====........" "\n" )
					.append( QString::fromStdWString( node->getContent( )->c_str( ) ) )
					.append( "\n" u8".......====........" );
				auto writeFilePath = writeCachePath + QTime::currentTime( ).toString( u8"hh_mm_ss_zzz" );
				QFile writeFile( writeFilePath );
				if( Path::creatFilePath( writeFilePath ) )
					if( writeFile.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
						auto data = writeContent.toUtf8( ).toStdString( );
						writeFile.write( data.c_str( ), data.length( ) );
						writeFile.close( );
					}
				return false;
			} );
			auto htmlNode = htmlDoc.getNodeFromName( [&]( const std::wstring &name, HtmlTools::Html_Node_Type type ) {
				qDebug( ) << QString::fromStdWString( name ).toLocal8Bit( ).toStdString( ).c_str( );
				if( name == L"html" )
					return true;
				return false;
			} );

			if( !htmlNode ) return 1;
			auto node = htmlNode.get( )->getType( );
			if( node == HtmlTools::Html_Node_Type::DoubleNode ) {
				auto name = htmlNode->getStartNode( )->getNodeWSName( );
				qDebug( ) << u8"htmlNode->getStartNode( )->getNodeWSName( ):" << QString::fromStdWString( *name ).toLocal8Bit( ).toStdString( ).c_str( );
				name = htmlNode->getEndNode( )->getNodeWSName( );
				qDebug( ) << u8"htmlNode->getEndNode( )->getNodeWSName( ):" << QString::fromStdWString( *name ).toLocal8Bit( ).toStdString( ).c_str( );

			}
			readFile.close( );
		}
	}

	//MainWidget qWidget;
	//qWidget.show( );
	return appliction.exec( );
}
