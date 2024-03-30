#include "FileResult.h"
#include "RWFileThread.h"
#include "../userHread/DebugInfo.h"
FileResult::~FileResult( ) {
	DEBUG_RUN( qDebug( ) << "~FileResult( ) : " << this );
}
bool FileResult::await( ) {
	return fileThread->await( );
}
