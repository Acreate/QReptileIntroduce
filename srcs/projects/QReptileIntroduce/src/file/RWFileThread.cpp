#include "RWFileThread.h"
#include "FileResult.h"
#include "../thread/FileThread.h"
#include <QFile>
#include  "../userHread/DebugInfo.h"

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
	if( currentThread ) {
		currentThread->requestInterruption( );
		while( currentThread->isRunning( ) && !currentThread->isFinished( ) )
			QThread::usleep( 20 );
		delete currentThread;
	}

}
FileResult *RWFileThread::readFile( ) {
	if( !file->exists( ) )
		return nullptr;
	DEBUG_RUN( qDebug( ) << "RWFileThread::readFile : QIODeviceBase::ReadOnly( " << file->absoluteFilePath( ) << " )" );
	if( !currentThread ) {
		threadFileThreadResult = new FileResult( this );
		currentThread = new FileThread( file->absoluteFilePath( ), QIODeviceBase::ReadOnly, threadFileThreadResult );
	} else {
		while( currentThread->isRunning( ) && !currentThread->isFinished( ) )
			currentThread->usleep( 50 );
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
	} else {
		while( currentThread->isRunning( ) && !currentThread->isFinished( ) )
			currentThread->usleep( 50 );
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
	} else {
		while( currentThread->isRunning( ) && !currentThread->isFinished( ) )
			currentThread->usleep( 50 );
		threadFileThreadResult->data = byteData;
		currentThread->setFilePath( file->absoluteFilePath( ) );
		currentThread->setOpenMode( QIODeviceBase::WriteOnly );
	}
	return currentThread->writeFile( );
}
bool RWFileThread::await( ) {
	DEBUG_RUN_IF_NOT_EQU_PTR( this->currentThread, nullptr, qDebug( ) << "await currentThread id = " << this->currentThread );

	while( this->currentThread && this->currentThread->isRunning( ) ) {
		QMutexLocker< QMutex > locker( &mutex );
		if( this->currentThread->isRunning( ) ) {
			bool finished = this->currentThread->isFinished( );
			if( finished )
				break;
			QThread::currentThread( )->usleep( 20 );
		}
	}

	return true;
}
FileResult *RWFileThread::start( ) {
	if( currentThread && threadFileThreadResult ) {
		currentThread->start( );
		return threadFileThreadResult;
	}
	return nullptr;
}
FileResult *RWFileThread::getFileResult( ) {
	if( threadFileThreadResult )
		return threadFileThreadResult;
	threadFileThreadResult = new FileResult( this );
	currentThread = new FileThread( file->absoluteFilePath( ), QIODeviceBase::NotOpen, threadFileThreadResult );
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
