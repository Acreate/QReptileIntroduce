#include "RWFileThread.h"
#include "FileThreadResult.h"
#include "FileThread.h"
#include <QFile>

RWFileThread::RWFileThread( QObject *parent ): QObject( parent ), file( new QFileInfo ) {
}
RWFileThread::RWFileThread( const QString &fileName ) {
	file = new QFileInfo( fileName );
}
RWFileThread::RWFileThread( QObject *parent, const QString &fileName ) : QObject( parent ) {
	file = new QFileInfo( fileName );
}
RWFileThread::~RWFileThread( ) {
	QMutexLocker< QMutex > locker( &mutex );
	if( currentThread && currentThread->isFinished( ) )
		currentThread->requestInterruption( );
	delete file;
	while( !currentThread->isFinished( ) )
		QThread::usleep( 20 );
}
QSharedPointer< FileThreadResult > RWFileThread::readFile( ) {
	if( !file->exists( ) )
		return QSharedPointer< FileThreadResult >( nullptr );
	qDebug( ) << "RWFileThread::readFile : QIODeviceBase::ReadOnly( " << file->absoluteFilePath( ) << " )";
	threadFileThreadResult = QSharedPointer< FileThreadResult >( new FileThreadResult( this ) );
	if( !currentThread )
		currentThread = new FileThread( file->absoluteFilePath( ), QIODeviceBase::ReadOnly, threadFileThreadResult );
	else {
		while( !currentThread->isFinished( ) )
			currentThread->usleep( 50 );
	}
	return currentThread->readFile( );
}
QSharedPointer< FileThreadResult > RWFileThread::writeFile( const QString &content ) {
	if( !file->makeAbsolute( ) )
		return QSharedPointer< FileThreadResult >( nullptr );
	qDebug( ) << "RWFileThread::writeFile : QIODeviceBase::ReadOnly( " << file->absoluteFilePath( ) << " )";
	threadFileThreadResult = QSharedPointer< FileThreadResult >( new FileThreadResult( this, content ) );
	if( !currentThread )
		currentThread = new FileThread( file->absoluteFilePath( ), QIODeviceBase::WriteOnly, threadFileThreadResult );
	else {
		while( !currentThread->isFinished( ) )
			currentThread->usleep( 50 );
	}
	return currentThread->writeFile( );
}
bool RWFileThread::await( ) {
	if( this->currentThread )
		qDebug( ) << "await currentThread id = " << this->currentThread;
	while( this->currentThread && this->currentThread->isRunning( ) ) {
		QMutexLocker< QMutex > locker( &mutex );
		bool finished = this->currentThread->isFinished( );
		if( finished )
			break;
		QThread::currentThread( )->usleep( 20 );
	}

	return true;
}
QSharedPointer< FileThreadResult > RWFileThread::start( ) {
	if( currentThread && threadFileThreadResult ) {
		currentThread->start( );
		return threadFileThreadResult;
	}
	return nullptr;
}
bool RWFileThread::isFinished( ) {
	QMutexLocker< QMutex > locker( &mutex );
	if( currentThread != nullptr )
		return currentThread->isFinished( );
	return true;
}
void RWFileThread::requestInterruption( ) {
	QMutexLocker< QMutex > locker( &mutex );
	if( currentThread != nullptr && !currentThread->isFinished( ) )
		currentThread->requestInterruption( );
}
