#include "FileThread.h"
#include "FileThreadResult.h"
FileThread::FileThread( const QString &filePath, QIODeviceBase::OpenMode openMode, QSharedPointer<FileThreadResult> fileThreadResult ): file( filePath ), openMode( openMode ),
	fileThreadResult( fileThreadResult ) {
	runOpenMode = QIODeviceBase::NotOpen;
}
QIODeviceBase::OpenMode FileThread::resetOpenMode( const QIODeviceBase::OpenMode &newOpenMode ) {
	auto oldOpenmo = openMode;
	openMode = newOpenMode;
	return oldOpenmo;
}
QSharedPointer<FileThreadResult> FileThread::readFile( ) {
	runOpenMode = QIODeviceBase::ReadOnly;
	if( !file.exists( ) )
		return nullptr;
	if( runOpenMode & openMode && file.open( openMode ) ) {
		emit start( );
		return fileThreadResult;
	}
	return nullptr;
}
QSharedPointer<FileThreadResult> FileThread::writeFile( ) {
	runOpenMode = QIODeviceBase::WriteOnly;
	if( !file.exists( ) )
		return nullptr;
	if( runOpenMode & openMode && file.open( openMode ) ) {
		emit start( );
		return fileThreadResult;
	}
	return nullptr;
}
void FileThread::run( ) {
	if( runOpenMode == QIODeviceBase::ReadOnly ) /*读取*/ {
		fileThreadResult->setFinish( false );
		fileThreadResult->data.clear( );
		constexpr qsizetype readBuffSize = 1024;
		qsizetype readSize = 0;
		auto buff = new char[ readBuffSize ];
		qint64 readCount = 0;
		do {
			readCount = file.read( buff, readBuffSize );
			if( readCount == 0 || isInterruptionRequested( ) )
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
		fileThreadResult->setFinish( false );
		constexpr qsizetype readBuffSize = 1024;
		qsizetype readSize = 0;
		auto buff = fileThreadResult->data.data( );
		auto writeMaxSize = fileThreadResult->data.length( );
		qint64 writeIndex = 0;
		qint64 writeCount = 0;
		do {
			writeCount += file.write( buff + writeIndex, readBuffSize );
			writeIndex += readBuffSize;
			if( writeIndex >= writeMaxSize )
				writeCount += file.write( buff + writeIndex, readBuffSize );
			if( writeCount >= writeMaxSize || isInterruptionRequested( ) )
				break;
		} while( true );

		file.close( );
		if( writeCount < writeMaxSize ) {
			// 被请求中断
			emit fileThreadResult->interruptionRequested( );
			return;
		}
		fileThreadResult->setFinish( true );
		emit fileThreadResult->finish( );
	}

}
