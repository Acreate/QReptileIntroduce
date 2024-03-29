#ifndef RWFILETHREAD_H_H_HEAD__FILE__
#define RWFILETHREAD_H_H_HEAD__FILE__

#pragma once
#include <QThread>
#include <qfile.h>
#include <qmutex.h>
#include <QDebug>
#include <QSharedPointer>

class QFile;
class FileThreadResult;
class RWFileThread : public QObject {

public:
	Q_OBJECT;
public:
	RWFileThread( QObject *parent = nullptr );
	RWFileThread( const QString &fileName );
	RWFileThread( QObject *parent, const QString &fileName );
	~RWFileThread( ) override;
private:
	QFile *file;
	QThread *currentThread = nullptr;
	QMutex mutex;
public: // 线程函数
	QSharedPointer< FileThreadResult > readFile( );
	QSharedPointer< FileThreadResult > writeFile( const QString &content );
	bool await( ) {
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
public:
	void setFilePath( const QString &filePath ) {
		QMutexLocker< QMutex > locker( &mutex );
		file->setFileName( filePath );
	}
	QString getFilePath( ) {
		QMutexLocker< QMutex > locker( &mutex );
		return file->fileName( );
	}
public:
	bool isFinished( ) {
		QMutexLocker< QMutex > locker( &mutex );
		if( currentThread != nullptr )
			return currentThread->isFinished( );
		return true;
	}
	void requestInterruption( ) {
		QMutexLocker< QMutex > locker( &mutex );
		if( currentThread != nullptr && !currentThread->isFinished( ) )
			currentThread->requestInterruption( );
	}
};

#endif // RWFILETHREAD_H_H_HEAD__FILE__
