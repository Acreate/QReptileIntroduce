#include "../ui/MainWidget.h"
#include "../ui/NovelInfoWidget.h"
#include "../core/Appliction.h"


int main( int argc, char *argv[ ] ) {
	Appliction appliction( argc, argv );
	
	MainWidget qWidget;
	qWidget.show( );
	return appliction.exec( );
}
