#include "OStream.h"
#include <QDebug>
#include <QDir>
#include <QFile>

#include "path/Dir.h"
#include "path/Path.h"
OStream::UN_Map_Iterator_Template< OStream::QString_Shared, OStream::OStream_Ptr > OStream::defaultOStreamMap;
qint64 OStream::outDebugLogFile( const QString &write_path, const QByteArray &write_content, QString file_base_name, QString error_type, QString dir_name, QString file_suffix ) {
	qint64 result = 0;
	QChar separator = QDir::separator( );
	QString timeFormString = separator + QDateTime::currentDateTime( ).toString( "yyyy_MM_dd=hh-mm-ss" );;
	if( error_type.isEmpty( ) )
		error_type = error_type.append( separator ).append( "normal" );
	else
		error_type = separator + error_type;
	if( !dir_name.isEmpty( ) )
		dir_name = separator + dir_name;
	if( file_suffix.isEmpty( ) )
		file_suffix = u8".log";
	if( file_base_name.isEmpty( ) )
		file_base_name = timeFormString + file_suffix;
	else {
		qsizetype index = 0;
		QChar currentChar = file_base_name[ index ];

		if( currentChar == '\\' || currentChar == '/' ) {
			qsizetype length = file_base_name.length( );
			QChar *buff = new QChar[ length ];
			qsizetype buffIndex = 0;
			for( ++index; index < length; ++index ) {
				currentChar = file_base_name[ index ];
				if( currentChar == '\\' || currentChar == '/' ) // 跳过开始的 目录分隔符
					continue;
				for( ++index; index < length; ++index, ++buffIndex )
					buff[ buffIndex ] = currentChar;
				break;
			}
			file_base_name = QString( buff, buffIndex );
		}
		file_base_name = timeFormString + "_" + file_base_name + file_suffix;
	}
	QString wirteFileAbsPath = write_path + dir_name + error_type + file_base_name;
	bool isCreateFile = Path::creatFilePath( wirteFileAbsPath );
	if( isCreateFile ) {
		QFile writeFile( wirteFileAbsPath );
		if( writeFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
			result = writeFile.write( write_content );
			writeFile.close( );
		}
	}
	return result;
}
QStringList OStream::anyDebugOut( OStream *os, const QString &msg, const QString &file_name, size_t line, const QString &call_fun_name, const QString &write_path, const QString &write_content ) {
	QStringList anyDebugOut = OStream::anyDebugOut( os, msg, file_name, line, call_fun_name );
	if( !write_path.isEmpty( ) ) {
		QFile writeHtmlFile( write_path );
		if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
			auto buff = anyDebugOut.join( "" );
			buff = write_content + u8"\n" + buff;
			writeHtmlFile.write( buff.toLocal8Bit( ) );
			writeHtmlFile.close( );
		}
	}
	return anyDebugOut;
}
QStringList OStream::anyDebugOut( OStream *os, const QString &msg, const QString &file_name, size_t line, const QString &call_fun_name ) {
	QStringList msgList;
	QString buff;
	msgList << u8"============================================";
	msgList << msg;
	msgList << u8"--------------------------------------------";
	msgList << u8"--------------------------------------------";
	buff.append( file_name ).append( u8"\n\t调用: " ).append( call_fun_name ).append( u8"\n\t第 " ).append( QString( u8"%1" ).arg( line ) ).append( u8" 行" );
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
QStringList OStream::errorQDebugOut( const QString &msg, const QString &fileName, size_t line, const QString &call_fun_name, const QString &write_path, const QString &write_content ) {
	QStringList msgList = OStream::errorQDebugOut( msg, fileName, line, call_fun_name );
	if( !write_path.isEmpty( ) ) {
		QFile writeHtmlFile( write_path );
		if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
			auto buff = msgList.join( "" );
			buff = write_content + u8"\n" + buff;
			writeHtmlFile.write( buff.toLocal8Bit( ) );
			writeHtmlFile.close( );
		}
	}
	return msgList;

}
QStringList OStream::errorQDebugOut( OStream *os, const QString &msg, const QString &fileName, size_t line, const QString &call_fun_name ) {
	if( os ) {
		QStringList msgList;
		QString buff;
		msgList << u8"============================================";
		msgList << msg;
		msgList << u8"--------------------------------------------";
		msgList << u8"--------------------------------------------";
		buff.append( fileName ).append( u8"\n\t调用: " ).append( call_fun_name ).append( u8"\n\t第 " ).append( QString( u8"%1" ).arg( line ) ).append( u8" 行" );
		msgList << buff;
		msgList << u8"============================================";
		buff = msgList.join( "\n" );
		std::string string = buff.toStdString( );
		const char *cStr = string.c_str( );
		*os << cStr << "\n";
		os->flush( );
		return msgList;
	} else
		return OStream::errorQDebugOut( msg, fileName, line, call_fun_name );

}
QStringList OStream::errorQDebugOut( const QString &msg, const QString &fileName, size_t line, const QString &call_fun_name ) {
	QStringList msgList;
	QString buff;
	msgList << u8"============================================";
	msgList << msg;
	msgList << u8"--------------------------------------------";
	msgList << u8"--------------------------------------------";
	buff.append( fileName ).append( u8"\n\t调用: " ).append( call_fun_name ).append( u8"\n\t第 " ).append( QString( u8"%1" ).arg( line ) ).append( u8" 行" );
	msgList << buff;
	msgList << u8"============================================";
	buff = msgList.join( "\n" );
	std::string string = buff.toStdString( );
	const char *cStr = string.c_str( );
	qDebug( ) << cStr;
	return msgList;
}
QStringList OStream::errorQDebugOut( OStream *os, const QString &msg, const QString &fileName, size_t line, const QString &call_fun_name, const QString &write_path, const QString &write_content ) {
	QStringList msgList = OStream::errorQDebugOut( os, msg, fileName, line, call_fun_name );
	QFile writeHtmlFile( write_path );
	if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
		auto buff = msgList.join( "" );
		buff = write_content + u8"\n" + buff;
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
	qDebug( ) << msg.toStdString( ).c_str( );
}

void OStream::errorQDebugOut( OStream *os, const QString &msg ) {
	if( os ) {
		*os << msg << "\n";
		os->flush( );
	} else
		OStream::errorQDebugOut( msg );
}
void OStream::errorQDebugOut( const QString &msg ) {
	qDebug( ) << msg.toStdString( ).c_str( );
}
