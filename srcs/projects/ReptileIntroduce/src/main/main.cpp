#include "../qt/group/ui/MainWidget.h"
#include "../qt/extend/core/Appliction.h"
#include "../qt/group/ui/CountEditWidget.h"

#include <QFileDialog>
#include <QPluginLoader>
#include <Request/IRequestNetInterface.h>

int main( int argc, char *argv[ ] ) {
	Appliction appliction( argc, argv );

	QString fileName = QFileDialog::getOpenFileName( nullptr, u8"选择一个插件", qApp->applicationFilePath( ), u8"动态库(*.dll)" );
	if( !fileName.isEmpty( ) ) {
		QPluginLoader loader( fileName );
		QObject *instance = loader.instance( );
		if( instance ) {
			auto cast = qobject_cast< QGenericPlugin * >( instance );
			if( cast ) {
				auto ptr = cast->create( "", "" );
				IRequestNetInterface *interface = qobject_cast< IRequestNetInterface * >( ptr );
				qDebug( ) << u8"100 + 200 = " << interface->add( 100, 200 );
			} else {
				qDebug( ) << u8"cast = " << cast << ", error = " << loader.errorString( );
			}
		}
	}

	//CountEditWidget countEditWidget;
	//countEditWidget.show(  );
	MainWidget qWidget;
	qWidget.show( );
	return appliction.exec( );
}
