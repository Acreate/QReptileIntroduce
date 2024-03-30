#include "FileThread.h"
#include "FileThreadResult.h"
FileThread::FileThread( const QString &filePath,
	QIODeviceBase::OpenMode openMode,
	FileThreadResult *fileThreadResult ):
file( filePath ),
fileThreadResult( fileThreadResult ),
openMode( openMode ) {
	runOpenMode = openMode;
	fileThreadResult->setParent( this );
}
QIODeviceBase::OpenMode FileThread::resetOpenMode( const QIODeviceBase::OpenMode &newOpenMode ) {
	auto oldOpenmo = openMode;
	openMode = newOpenMode;
	return oldOpenmo;
}
FileThreadResult *FileThread::readFile( ) {
	runOpenMode = QIODeviceBase::ReadOnly;
	if( !file.exists( ) )
		return nullptr;
	if( runOpenMode & openMode && file.open( openMode ) ) {
		return fileThreadResult;
	}
	emit fileThreadResult->error( -1 );
	return nullptr;
}
FileThreadResult *FileThread::writeFile( ) {
	runOpenMode = QIODeviceBase::WriteOnly;
	if( QFileInfo::exists( file.fileName( ) ) )
		openMode = openMode | QIODeviceBase::ExistingOnly | QIODeviceBase::Truncate;
	else
		openMode = openMode | QIODeviceBase::NewOnly;
	QIODeviceBase::OpenMode flags = runOpenMode & openMode;
	if( flags && file.open( openMode ) ) {
		return fileThreadResult;
	}
	emit fileThreadResult->error( -2 );
	return nullptr;
}
void FileThread::run( ) {
	//qDebug( ) << "状态" << runOpenMode << " (" << file.fileName( ) << " )";
	if( runOpenMode == QIODeviceBase::ReadOnly ) /*读取*/ {
		//qDebug( ) << "FileThread::run : QIODeviceBase::ReadOnly( " << file.fileName( ) << " )";
		fileThreadResult->setFinish( false );
		fileThreadResult->data.clear( );
		constexpr qsizetype readBuffSize = 1024;
		qsizetype readSize = 0;
		auto buff = new char[ readBuffSize ];
		qint64 readCount = 0;
		do {
			readCount = file.read( buff, readBuffSize );
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
	} else if( runOpenMode == QIODeviceBase::WriteOnly ) /*写入*/ {
		//qDebug( ) << "FileThread::run : QIODeviceBase::WriteOnly( " << file.fileName( ) << " )";
		fileThreadResult->setFinish( false );
		constexpr qsizetype readBuffSize = 1024;
		auto buff = fileThreadResult->data.data( );
		auto writeMaxSize = fileThreadResult->data.length( );
		qint64 writeIndex = 0;
		qint64 writeCount = 0;
		do {
			writeCount += file.write( buff + writeIndex, readBuffSize );
			writeIndex += readBuffSize;
			if( writeCount >= writeMaxSize || isInterruptionRequested( ) )
				break;
			if( writeIndex + readBuffSize >= writeMaxSize ) {
				writeCount += file.write( buff + writeIndex, writeMaxSize - writeIndex );
				break;
			}

		} while( true );

		file.close( );
		if( writeCount != writeMaxSize ) {
			// 被请求中断
			emit fileThreadResult->interruptionRequested( );
			return;
		}
		fileThreadResult->setFinish( true );
		emit fileThreadResult->finish( );
	}

}
