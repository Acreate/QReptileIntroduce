#include "OStream.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <iostream>
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
QStringList OStream::anyStdCerr( const QString &msg, const QString &fileName, size_t line, const QString &call_fun_name, const QString &write_path, const QString &write_content, OStream *os ) {
	QStringList msgList = OStream::anyStdCerr( msg, fileName, line, call_fun_name );
	if( !write_path.isEmpty( ) ) {
		if( Path::creatFilePath( write_path ) ) {
			QFile writeHtmlFile( write_path );
			if( writeHtmlFile.open( QIODeviceBase::WriteOnly | QIODeviceBase::Text | QIODeviceBase::Truncate ) ) {
				writeHtmlFile.write( write_content.toUtf8( ) );
				writeHtmlFile.close( );
			}
		}
	}
	return msgList;
}
QStringList OStream::anyStdCerr( const QString &msg, const QString &fileName, size_t line, const QString &call_fun_name, OStream *o_stream ) {
	QStringList msgList;
	QString buff;
	msgList << u8"============================================";
	msgList << msg;
	msgList << u8"--------------------------------------------";
	msgList << u8"--------------------------------------------";
	buff.append( u8"\n\t文件名:" ).append( fileName ).append( u8"\n\t调用: " ).append( call_fun_name ).append( u8"\n\t第 " ).append( QString( u8"%1" ).arg( line ) ).append( u8" 行" );
	msgList << buff;
	msgList << u8"============================================";
	buff = msgList.join( "\n" );
	if( o_stream ) {
		*o_stream << buff;
		o_stream->flush( );
	}
	std::string string = buff.toStdString( );
	const char *cStr = string.c_str( );
	std::cerr << cStr << std::endl;
	return msgList;
}
void OStream::anyStdCOut( const QString &msg, OStream *os ) {
	if( os ) {
		*os << msg << "\n";
		os->flush( );
	}
	std::cout << msg.toStdString( ).c_str( ) << std::endl;
}

void OStream::anyStdCerr( const QString &msg, OStream *os ) {
	if( os ) {
		*os << msg << "\n";
		os->flush( );
	}
	std::cout << msg.toStdString( ).c_str( ) << std::endl;
}
