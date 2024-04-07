#include "RWFileThread.h"
#include <QFile>
#include <DebugInfo.h>
#include "FileResult.h"
#include "../../extend/thread//FileThread.h"
#include "../../extend/thread/FileThread.h"

RWFileThread::RWFileThread( QObject *parent ): QObject( parent ), file( new QFileInfo ) {
}
RWFileThread::RWFileThread( const QString &fileName ) {
	file = new QFileInfo( fileName );
}
RWFileThread::RWFileThread( QObject *parent, const QString &fileName ) : QObject( parent ) {
	file = new QFileInfo( fileName );
}
RWFileThread::~RWFileThread( ) {
	delete file;
	if( currentThread )
		await( );
}
FileResult *RWFileThread::readFile( ) {
	if( !file->exists( ) )
		return nullptr;
	DEBUG_RUN( qDebug( ) << "RWFileThread::readFile : QIODeviceBase::ReadOnly( " << file->absoluteFilePath( ) << " )" );
	if( !currentThread ) {
		threadFileThreadResult = new FileResult( this );
		currentThread = new FileThread( file->absoluteFilePath( ), QIODeviceBase::ReadOnly, threadFileThreadResult );
		currentThread->setParent( this );
	} else {
		await( );
		currentThread->setFilePath( file->absoluteFilePath( ) );
		currentThread->setOpenMode( QIODeviceBase::ReadOnly );
	}
	return currentThread->readFile( );
}
FileResult *RWFileThread::writeFile( const QString &content ) {
	if( !file->makeAbsolute( ) )
		return nullptr;
	DEBUG_RUN( qDebug( ) << "RWFileThread::writeFile : QIODeviceBase::ReadOnly( " << file->absoluteFilePath( ) << " )" );

	if( !currentThread ) {
		threadFileThreadResult = new FileResult( this, content );
		currentThread = new FileThread( file->absoluteFilePath( ), QIODeviceBase::WriteOnly, threadFileThreadResult );
		currentThread->setParent( this );
	} else {
		await( );
		threadFileThreadResult->data = content.toUtf8( );
		currentThread->setFilePath( file->absoluteFilePath( ) );
		currentThread->setOpenMode( QIODeviceBase::WriteOnly );
	}
	return currentThread->writeFile( );
}
FileResult *RWFileThread::writeFile( const QByteArray &byteData ) {
	DEBUG_RUN( qDebug( ) << "RWFileThread::writeFile : QIODeviceBase::WriteOnly( " << file->absoluteFilePath( ) << " )" );
	if( !currentThread ) {
		threadFileThreadResult = new FileResult( this, byteData );
		currentThread = new FileThread( file->absoluteFilePath( ), QIODeviceBase::WriteOnly, threadFileThreadResult );
		currentThread->setParent( this );
	} else {
		await( );
		threadFileThreadResult->data = byteData;
		currentThread->setFilePath( file->absoluteFilePath( ) );
		currentThread->setOpenMode( QIODeviceBase::WriteOnly );
	}
	return currentThread->writeFile( );
}
void RWFileThread::await( long usleep ) {
	DEBUG_RUN_IF_NOT_EQU_PTR( this->currentThread, nullptr, qDebug( ) << "await currentThread id = " << this->currentThread );

	bool isRunning = this->currentThread->isRunning( );

	while( this->currentThread && isRunning ) {
		mutex.lock( );
		this->currentThread->requestInterruption( );
		if( this->currentThread->isFinished( ) ) {
			mutex.unlock( );
			break;
		}
		mutex.unlock( );
		QThread::currentThread( )->usleep( usleep );
	}

}
FileResult *RWFileThread::start( ) {
	if( currentThread && threadFileThreadResult ) {
		await( );
		currentThread->start( );
		return threadFileThreadResult;
	}
	return nullptr;
}
void RWFileThread::setFilePath( const QString &filePath ) {
	QMutexLocker< QMutex > locker( &mutex );
	file->setFile( filePath );
}
QString RWFileThread::getFilePath( ) {
	QMutexLocker< QMutex > locker( &mutex );
	return file->absoluteFilePath( );
}
FileResult *RWFileThread::getFileResult( ) {
	if( threadFileThreadResult )
		return threadFileThreadResult;
	threadFileThreadResult = new FileResult( this );
	currentThread = new FileThread( file->absoluteFilePath( ), QIODeviceBase::NotOpen, threadFileThreadResult );
	currentThread->setParent( this );
	return threadFileThreadResult;
}
bool RWFileThread::isFinished( ) {
	QMutexLocker< QMutex > locker( &mutex );
	if( currentThread != nullptr ) {
		bool cond = !currentThread->isRunning( );
		if( cond )
			return true;
		return currentThread->isFinished( );
	}
	return true;
}
void RWFileThread::requestInterruption( ) {
	QMutexLocker< QMutex > locker( &mutex );
	if( currentThread != nullptr && currentThread->isRunning( ) && !currentThread->isFinished( ) )
		currentThread->requestInterruption( );
}
