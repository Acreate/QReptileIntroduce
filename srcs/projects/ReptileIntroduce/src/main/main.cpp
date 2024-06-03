#include "../qt/group/ui/MainWidget.h"
#include "../qt/extend/core/Application.h"
#include <locale>
#include <iostream>
int main( int argc, char *argv[ ] ) {
	//std::locale locale( "zh_CN.UTF8" );
	//std::locale::global( locale );
	//std::cout.imbue( locale );
	//std::wcout.imbue( locale );
	Application application( argc, argv );
	MainWidget qWidget;
	qWidget.show( );
	auto *instance = qApp;
	return instance->exec( );
}
