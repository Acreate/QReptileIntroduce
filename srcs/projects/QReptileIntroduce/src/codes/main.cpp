#include <QWidget>
#include <QOpenGLWidget>
#include "../ui/MainWidget.h"
#include "../core/Appliction.h"

int main( int argc, char *argv[ ] ) {
	Appliction appliction( argc, argv );
	MainWidget qWidget;
	appliction.appendNotifyByObjPtr( &qWidget );
	appliction.appendNotifyByObjPtr( qWidget.getCompoents( ) );
	appliction.appendNotifyByName( qWidget.metaObject( )->className( ) );
	qWidget.show( );
	return appliction.exec( );
}
