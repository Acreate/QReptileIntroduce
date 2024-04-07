#include "FileResult.h"
#include "RWFileThread.h"
#include <DebugInfo.h>
FileResult::~FileResult( ) {
	DEBUG_RUN( qDebug( ) << "~FileResult( ) : " << this );
}
void FileResult::await( ) {
	fileThread->await( );
}
