#include "../qt/group/ui/MainWidget.h"
#include "../qt/extend/core/Application.h"
#include "dateTime/DateTime.h"

int main( int argc, char *argv[ ] ) {
	Application application( argc, argv );
	MainWidget qWidget;
	qWidget.show( );
	auto *instance = qApp;
	return instance->exec( );
}
