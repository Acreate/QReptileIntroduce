#include "FileThreadResult.h"
#include "RWFileThread.h"
#include "../userHread/DebugInfo.h"
FileThreadResult::~FileThreadResult( ) {
	DEBUG_RUN( qDebug( ) << "~FileThreadResult( ) : " << this );
}
bool FileThreadResult::await( ) {
	return fileThread->await( );
}
