#include "OStream.h"
#include <QDebug>
#include <QFile>

void OStream::anyDebugOut( OStream *os, const QString &msg, const QString &fileName, size_t line, const QString &callFunName, const QString &writePath, const QString &writeContent ) {
	QStringList msgList;
	QString buff;
	msgList << u8"============================================";
	msgList << msg;
	msgList << u8"--------------------------------------------";
	msgList << u8"--------------------------------------------";
	buff.append( fileName ).append( u8"\n\t调用: " ).append( callFunName ).append( u8"\n\t第 " ).append( QString( u8"%1" ).arg( line ) ).append( u8" 行" );
	msgList << buff;
	msgList << u8"============================================";
	buff = msgList.join( "\n" );
	auto errorMessage = buff.toStdString( );
	qDebug( ) << errorMessage.c_str( );
	if( os ) {
		*os << errorMessage;
		os->flush( );
	}
	if( writePath.isEmpty( ) )
		return;
	QFile writeHtmlFile( writePath );
	if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
		buff = writeContent + u8"\n" + buff;
		writeHtmlFile.write( buff.toLocal8Bit( ) );
		writeHtmlFile.close( );
	}
}
void OStream::errorQDebugOut( const QString &msg, const QString &fileName, size_t line, const QString &callFunName, const QString &writePath, const QString &writeContent ) {
	QStringList msgList;
	QString buff;
	msgList << u8"============================================";
	msgList << msg;
	msgList << u8"--------------------------------------------";
	msgList << u8"--------------------------------------------";
	buff.append( fileName ).append( u8"\n\t调用: " ).append( callFunName ).append( u8"\n\t第 " ).append( QString( u8"%1" ).arg( line ) ).append( u8" 行" );
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
void OStream::errorQDebugOut( OStream *os, const QString &msg, const QString &fileName, size_t line, const QString &callFunName, const QString &writePath, const QString &writeContent ) {
	if( os ) {
		QStringList msgList;
		QString buff;
		msgList << u8"============================================";
		msgList << msg;
		msgList << u8"--------------------------------------------";
		msgList << u8"--------------------------------------------";
		buff.append( fileName ).append( u8"\n\t调用: " ).append( callFunName ).append( u8"\n\t第 " ).append( QString( u8"%1" ).arg( line ) ).append( u8" 行" );
		msgList << buff;
		msgList << u8"============================================";
		buff = msgList.join( "\n" );
		std::string string = buff.toStdString( );
		const char *cStr = string.c_str( );
		*os << cStr;
		if( writePath.isEmpty( ) )
			return;
		QFile writeHtmlFile( writePath );
		if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
			buff = writeContent + u8"\n" + buff;
			writeHtmlFile.write( buff.toLocal8Bit( ) );
			writeHtmlFile.close( );
		}
	} else
		OStream::errorQDebugOut( msg, fileName, line, callFunName, writePath, writeContent );
}
void OStream::anyDebugOut( OStream *os, const QString &msg ) {

	if( os )
		*os << msg;
	qDebug( ) << msg;
}

void OStream::errorQDebugOut( OStream *os, const QString &msg ) {
	if( os )
		*os << msg;
	else
		OStream::errorQDebugOut( msg );
}
void OStream::errorQDebugOut( const QString &msg ) {
	qDebug( ) << msg;
}
