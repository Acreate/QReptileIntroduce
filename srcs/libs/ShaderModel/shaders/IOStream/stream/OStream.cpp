#include "OStream.h"
#include <QDebug>
#include <QFile>
OStream::UN_Map_Iterator_Template< OStream::QString_Shared, OStream::OStream_Ptr > OStream::defaultOStreamMap;
QStringList OStream::anyDebugOut( OStream *os, const QString &msg, const QString &fileName, size_t line, const QString &callFunName, const QString &writePath, const QString &writeContent ) {
	QStringList anyDebugOut = OStream::anyDebugOut( os, msg, fileName, line, callFunName );
	if( !writePath.isEmpty( ) ) {
		QFile writeHtmlFile( writePath );
		if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
			auto buff = anyDebugOut.join( "" );
			buff = writeContent + u8"\n" + buff;
			writeHtmlFile.write( buff.toLocal8Bit( ) );
			writeHtmlFile.close( );
		}
	}
	return anyDebugOut;
}
QStringList OStream::anyDebugOut( OStream *os, const QString &msg, const QString &fileName, size_t line, const QString &callFunName ) {
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
		*os << errorMessage << "\n";
		os->flush( );
	}
	return msgList;
}
QStringList OStream::errorQDebugOut( const QString &msg, const QString &fileName, size_t line, const QString &callFunName, const QString &writePath, const QString &writeContent ) {
	QStringList msgList = OStream::errorQDebugOut( msg, fileName, line, callFunName );
	if( !writePath.isEmpty( ) ) {
		QFile writeHtmlFile( writePath );
		if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
			auto buff = msgList.join( "" );
			buff = writeContent + u8"\n" + buff;
			writeHtmlFile.write( buff.toLocal8Bit( ) );
			writeHtmlFile.close( );
		}
	}
	return msgList;

}
QStringList OStream::errorQDebugOut( OStream *os, const QString &msg, const QString &fileName, size_t line, const QString &callFunName ) {
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
		*os << cStr << "\n";
		os->flush( );
		return msgList;
	} else
		return OStream::errorQDebugOut( msg, fileName, line, callFunName );

}
QStringList OStream::errorQDebugOut( const QString &msg, const QString &fileName, size_t line, const QString &callFunName ) {
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
	qDebug( ) << cStr;
	return msgList;
}
QStringList OStream::errorQDebugOut( OStream *os, const QString &msg, const QString &fileName, size_t line, const QString &callFunName, const QString &writePath, const QString &writeContent ) {
	QStringList msgList = OStream::errorQDebugOut( os, msg, fileName, line, callFunName );
	QFile writeHtmlFile( writePath );
	if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
		auto buff = msgList.join( "" );
		buff = writeContent + u8"\n" + buff;
		writeHtmlFile.write( buff.toLocal8Bit( ) );
		writeHtmlFile.close( );
	}
	return msgList;
}
void OStream::anyDebugOut( OStream *os, const QString &msg ) {
	if( os ) {
		*os << msg << "\n";
		os->flush( );
	}
	qDebug( ) << msg.toStdString(  ).c_str(  );
}

void OStream::errorQDebugOut( OStream *os, const QString &msg ) {
	if( os ) {
		*os << msg << "\n";
		os->flush( );
	} else
		OStream::errorQDebugOut( msg );
}
void OStream::errorQDebugOut( const QString &msg ) {
	qDebug( ) << msg.toStdString(  ).c_str(  );
}
