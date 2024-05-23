#include "OStream.h"
#include <QDebug>
#include <QFile>
OStream::UN_Map_Iterator_Template< OStream::QString_Shared, OStream::OStream_Ptr > OStream::defaultOStreamMap;
void OStream::anyDebugOut( OStream *os, const std::string &msg, const std::string &fileName, size_t line, const std::string &callFunName, const QString &writePath, const QString &writeContent ) {
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
void OStream::errorQDebugOut( OStream *os, const std::string &msg, const std::string &fileName, size_t line, const std::string &callFunName, const QString &writePath, const QString &writeContent ) {
	if( os ) {
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
