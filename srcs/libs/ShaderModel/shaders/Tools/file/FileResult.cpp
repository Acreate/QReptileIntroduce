#include "FileResult.h"
#include "RWFileThread.h"
#include <DebugInfo.h>
FileResult::~FileResult( ) {
}
void FileResult::await( ) {
	fileThread->await( );
}
