#ifndef RWFILETHREAD_H_H_HEAD__FILE__
#define RWFILETHREAD_H_H_HEAD__FILE__

#pragma once
#include <QThread>
#include <qfile.h>
#include <qmutex.h>
#include <QDebug>
#include <QSharedPointer>

#include "FileThread.h"

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
	QFileInfo *file;
	FileThread *currentThread = nullptr;
	QSharedPointer< FileThreadResult > threadFileThreadResult = nullptr;
	QMutex mutex;
public: // 线程函数
	QSharedPointer< FileThreadResult > readFile( );
	QSharedPointer< FileThreadResult > writeFile( const QString &content );
	bool await( );
	QSharedPointer< FileThreadResult > start( );
public:
	void setFilePath( const QString &filePath ) {
		QMutexLocker< QMutex > locker( &mutex );
		file->setFile( filePath );
	}
	QString getFilePath( ) {
		QMutexLocker< QMutex > locker( &mutex );
		return file->absoluteFilePath( );
	}
public:
	bool isFinished( );
	void requestInterruption( );
};

#endif // RWFILETHREAD_H_H_HEAD__FILE__
