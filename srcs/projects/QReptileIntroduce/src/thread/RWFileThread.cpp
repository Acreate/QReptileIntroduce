#include "RWFileThread.h"
#include "FileThreadResult.h"
#include <QFile>

RWFileThread::RWFileThread( QObject *parent ): QObject( parent ), file( new QFile ) {

}
RWFileThread::RWFileThread( const QString &fileName ) {
	file = new QFile( fileName );
}
RWFileThread::RWFileThread( QObject *parent, const QString &fileName ) : QObject( parent ) {
	file = new QFile( fileName );
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
	if( !file->exists( ) || !file->open( QIODeviceBase::ReadOnly ) )
		return QSharedPointer< FileThreadResult >( nullptr );
	if( !currentThread )
		currentThread = new QThread;
	else
		while( !currentThread->isFinished( ) )
			currentThread->usleep( 50 );

	QMutexLocker< QMutex > locker( &mutex );
	auto result = new FileThreadResult( this );
	result->setFinish( false );
	this->moveToThread( this->currentThread );

	qDebug( ) << "readFile currentThread id = " << this->currentThread;
	connect( this->currentThread, &QThread::started, [this,result]( ) {
		
		emit result->finish( );
	} );
	connect( currentThread, &QThread::finished, [this, result]( ) {
	} );
	currentThread->start( );

	return QSharedPointer< FileThreadResult >( result );
}
QSharedPointer< FileThreadResult > RWFileThread::writeFile( const QString &content ) {
	if( !file->exists( ) || !file->open( QIODeviceBase::WriteOnly ) )
		return QSharedPointer< FileThreadResult >( nullptr );
	if( !currentThread )
		currentThread = new QThread;
	else
		while( !currentThread->isFinished( ) )
			currentThread->usleep( 50 );
	QMutexLocker< QMutex > locker( &mutex );

	auto result = new FileThreadResult( this );
	result->setFinish( false );
	connect( currentThread, &QThread::started, [this,&result,content]( ) {
		qint64 writeCount = file->write( content.toLocal8Bit( ) );
		result->setData( QByteArray::number( writeCount ) );
	} );
	connect( currentThread, &QThread::finished, [this,&result]( ) {
		result->setFinish( true );
	} );
	currentThread->start( );
	return QSharedPointer< FileThreadResult >( result );
}
