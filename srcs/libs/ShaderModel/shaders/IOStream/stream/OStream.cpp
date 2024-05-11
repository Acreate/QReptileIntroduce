#include "OStream.h"
#include <QDebug>
void OStream::errorQDebugOut( const std::string &msg, const std::string &fileName, size_t line ) {
	qDebug( ) << u8"============================================";
	qDebug( ) << msg.c_str( );
	qDebug( ) << fileName.c_str( ) << u8"\n\t第 " << line << u8" 行";
	qDebug( ) << u8"============================================";
}
