#include <QFileDialog>
#include <QGenericPlugin>
#include <QPluginLoader>

#include "../qt/group/ui/MainWidget.h"
#include "../qt/extend/core/Appliction.h"
#include "../qt/extend/layout/HLayoutBox.h"
#include "../qt/extend/layout/VLayoutBox.h"

#include "interface/IRequestNetInterface.h"

int main( int argc, char *argv[ ] ) {
	Appliction appliction( argc, argv );
	MainWidget qWidget;
	qWidget.show( );
	return appliction.exec( );
}
