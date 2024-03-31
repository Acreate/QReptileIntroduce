#include "../ui/MainWidget.h"
#include "../ui/NovelInfoWidget.h"
#include "../core/Appliction.h"

#include "../file/FileResult.h"

int main( int argc, char *argv[ ] ) {
	Appliction appliction( argc, argv );
	MainWidget qWidget;
	appliction.appendNotifyByObjPtr( &qWidget );
	appliction.appendNotifyByObjPtr( qWidget.getCompoents( ) );
	appliction.appendNotifyByName( qWidget.metaObject( )->className( ) );
	qWidget.show( );
	return appliction.exec( );
}
