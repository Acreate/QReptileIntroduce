#include "OStream.h"
#include <QDebug>
#include <QFile>

void OStream::errorQDebugOut( const std::string &msg, const std::string &fileName, size_t line, const QString &writePath, const QString &writeContent ) {
	qDebug( ) << u8"============================================";
	qDebug( ) << msg.c_str( );
	qDebug( ) << fileName.c_str( ) << u8"\n\t第 " << line << u8" 行";
	qDebug( ) << u8"============================================";
	if( writePath.isEmpty( ) || writeContent.isEmpty( ) )
		return;
	QFile writeHtmlFile( writePath );
	if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
		writeHtmlFile.write( writeContent.toLocal8Bit( ) );
		writeHtmlFile.close( );
	}

}
