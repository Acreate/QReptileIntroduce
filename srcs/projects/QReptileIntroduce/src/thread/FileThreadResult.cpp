#include "FileThreadResult.h"
#include "RWFileThread.h"
bool FileThreadResult::await( ) {
	return fileThread->await( );
}
