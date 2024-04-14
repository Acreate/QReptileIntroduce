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

void test( IRequestNetInterfaceExtend *requestNetInterfaceExtend ) {

	std::string host;
	qDebug( ) << requestNetInterfaceExtend->getHost( &host ) << " : " << host;
}

int main( int argc, char *argv[ ] ) {
	Appliction appliction( argc, argv );
	auto fileInfo = Path::getPathInfo( QString( u8"%1%2%3" ).arg( Project_Plug_bin ).arg( QDir::separator( ) ).arg( u8"www.121ds.cc" ) );
	std::vector< File > files = fileInfo.second;
	for( auto &file : files ) {
		QString currentFilePtah = file.getCurrentFilePtah( );
		auto interface = NovelInfoWidget::getIRequestNetInterface( currentFilePtah, "", "" );
		if( interface.second ) {
			auto requestNetInterfaceExtend = interface.second;
			test( requestNetInterfaceExtend );
			QObject::connect( interface.first, &QObject::destroyed,qApp, &QCoreApplication::quit );
			requestNetInterfaceExtend->deleteMember( );
		}
	}

	//MainWidget qWidget;
	//qWidget.show( );
	return appliction.exec( );
}
