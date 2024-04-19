#include <QFileDialog>
#include <QGenericPlugin>
#include <QPluginLoader>

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

#include <htmls/html/HtmlDoc.h>

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
			wchar_t *cStr = ( wchar_t * )stdWString.c_str( );
			size_t cStrLen = stdWString.size( );
			size_t index = 0;
			auto htmlDoc = HtmlDoc::parse( cStr, cStrLen, &index );
			htmlDoc.getNodeFromName( [&]( const std::wstring &name, HtmlDoc::HtmlNodeCharPair::Html_Node_Type type ) {
				qDebug( ) << QString::fromStdWString( name ).toLocal8Bit( ).toStdString( ).c_str( );
				return false;
			} );
			readFile.close( );
		}
	}

	//MainWidget qWidget;
	//qWidget.show( );
	return appliction.exec( );
}
