#include "FileThread.h"
#include "../file/FileResult.h"
#include  "../userHread/DebugInfo.h"
#include <QDir>
FileThread::FileThread( const QString &filePath,
	QIODeviceBase::OpenMode openMode,
	FileResult *fileThreadResult ):
file( filePath ),
fileThreadResult( fileThreadResult ),
openMode( openMode ) {
	runOpenMode = openMode;
	fileThreadResult->setParent( this );
}
FileThread::~FileThread( ) {

}
QIODeviceBase::OpenMode FileThread::resetOpenMode( const QIODeviceBase::OpenMode &newOpenMode ) {
	auto oldOpenmo = openMode;
	openMode = newOpenMode;
	return oldOpenmo;
}
FileResult *FileThread::readFile( ) {
	runOpenMode = QIODeviceBase::ReadOnly;
	if( !file.exists( ) )
		return nullptr;

	DEBUG_RUN( qDebug( ) << "FileThread::readFile( " << file.fileName( ) << " ) openMode = " << openMode );
	if( runOpenMode & openMode && file.open( openMode ) )
		return fileThreadResult;
	emit fileThreadResult->error( 1, file.error( ), QFileDevice::NoError );
	return nullptr;
}
FileResult *FileThread::writeFile( ) {
	runOpenMode = QIODeviceBase::WriteOnly;
	QFileInfo fileInfo( file.fileName( ) );
	if( fileInfo.exists( ) )
		openMode = openMode | QIODeviceBase::ExistingOnly | QIODeviceBase::Truncate;
	else {
		openMode = openMode | QIODeviceBase::NewOnly;
		QDir absoluteDir = fileInfo.absoluteDir( );
		if( !absoluteDir.exists( ) ) {
			QString absolutePath = absoluteDir.absolutePath( );
			if( !absoluteDir.mkdir( absolutePath ) ) {
				DEBUG_RUN( qDebug( ) << "mkdir( " <<absolutePath << " ) error :" << __FILE__ << " : " << __LINE__ );
				emit fileThreadResult->error( 2, QFileDevice::NoError, QFileDevice::PermissionsError );
				return nullptr;
			}
		}
	}

	DEBUG_RUN( qDebug( ) << "FileThread::writeFile( " << file.fileName( ) << " ) openMode = " << openMode );
	QIODeviceBase::OpenMode flags = runOpenMode & openMode;
	if( flags && file.open( openMode ) )
		return fileThreadResult;

	emit fileThreadResult->error( 1, file.error( ), QFileDevice::NoError );
	return nullptr;
}
void FileThread::run( ) {
	DEBUG_RUN( qDebug( ) << "状态" << runOpenMode << " (" << file.fileName( ) << " )" );
	if( runOpenMode == QIODeviceBase::ReadOnly ) /*读取*/ {
		DEBUG_RUN( qDebug( ) << "FileThread::run : QIODeviceBase::ReadOnly( " << file.fileName( ) << " )" );
		fileThreadResult->setFinish( false );
		fileThreadResult->data.clear( );
		qsizetype readSize = 0;
		auto currentBuffSize = buffSize;
		auto buff = new char[ currentBuffSize ];
		qint64 readCount = 0;
		do {
			readCount = file.read( buff, currentBuffSize );
			if( readCount == -1 || readCount == 0 || isInterruptionRequested( ) )
				break;
			fileThreadResult->data.append( buff, readCount );
			readSize += readCount;
		} while( true );

		file.close( );
		delete[] buff;
		if( readCount != 0 ) {
			// 被请求中断
			emit fileThreadResult->interruptionRequested( );
			return;
		}
		fileThreadResult->setFinish( true );
		emit fileThreadResult->finish( );
		return;
	} else if( runOpenMode == QIODeviceBase::WriteOnly ) /*写入*/ {
		DEBUG_RUN( qDebug( ) << "FileThread::run : QIODeviceBase::WriteOnly( " << file.fileName( ) << " )" );
		fileThreadResult->setFinish( false );
		auto currentBuffSize = buffSize;
		auto buff = fileThreadResult->data.data( );
		auto writeMaxSize = fileThreadResult->data.length( );
		qint64 writeCount = 0;
		if( writeMaxSize < buffSize ) {
			writeCount += file.write( buff, writeMaxSize );
		} else {
			qint64 writeIndex = 0;
			do {
				writeCount += file.write( buff + writeIndex, currentBuffSize );
				writeIndex += currentBuffSize;
				if( writeCount >= writeMaxSize || isInterruptionRequested( ) )
					break;
				if( writeIndex + currentBuffSize >= writeMaxSize ) {
					writeCount += file.write( buff + writeIndex, writeMaxSize - writeIndex );
					break;
				}
			} while( true );

		}

		file.close( );
		if( writeCount != writeMaxSize ) {
			// 被请求中断
			emit fileThreadResult->interruptionRequested( );
			return;
		}
		fileThreadResult->setFinish( true );
		emit fileThreadResult->finish( );
		return;
	}

	DEBUG_RUN( qDebug( ) << "!! FileThread::run : error ( " << file.fileName( ) << " ) !!" );
}
