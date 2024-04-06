#include "../qt/group/ui/MainWidget.h"
#include "../qt/extend/core/Appliction.h"

int main( int argc, char *argv[ ] ) {
	Appliction appliction( argc, argv );
	MainWidget qWidget;
	qWidget.show( );
	return appliction.exec( );
}
