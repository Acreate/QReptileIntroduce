
#include "../auto_generate_files/macro/cmake_to_c_cpp_header_env.h"
#include "../qt/group/ui/MainWidget.h"
#include "../qt/extend/core/Application.h"
#include <path/Path.h>

int main( int argc, char *argv[ ] ) {
	Application application( argc, argv );
	MainWidget qWidget;
	qWidget.show( );
	auto *instance = qApp;
	return instance->exec( );
}
