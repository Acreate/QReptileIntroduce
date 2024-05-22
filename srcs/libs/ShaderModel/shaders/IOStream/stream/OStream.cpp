#include "OStream.h"
#include <QDebug>
#include <QFile>

void OStream::errorQDebugOut( const std::string &msg, const std::string &fileName, size_t line, const std::string &callFunName, const QString &writePath, const QString &writeContent ) {
	QStringList msgList;
	QString buff;
	msgList << u8"============================================";
	msgList << msg.c_str( );
	msgList << u8"--------------------------------------------";
	msgList << u8"--------------------------------------------";
	buff.append( fileName.c_str( ) ).append( u8"\n\t调用: " ).append( QString::fromStdString( callFunName ) ).append( u8"\n\t第 " ).append( QString( u8"%1" ).arg( line ) ).append( u8" 行" );
	msgList << buff;
	msgList << u8"============================================";
	buff = msgList.join( "\n" );
	qDebug( ) << buff.toStdString( ).c_str( );
	if( writePath.isEmpty( ) )
		return;
	QFile writeHtmlFile( writePath );
	if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
		buff = writeContent + u8"\n" + buff;
		writeHtmlFile.write( buff.toLocal8Bit( ) );
		writeHtmlFile.close( );
	}
}
