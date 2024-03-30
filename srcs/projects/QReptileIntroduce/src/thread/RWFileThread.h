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
	FileThreadResult *threadFileThreadResult = nullptr;
	QMutex mutex;
public: // 线程函数
	FileThreadResult *readFile( );
	FileThreadResult *writeFile( const QString &content );
	FileThreadResult *writeFile( const QByteArray &byteData );
	bool await( );
	FileThreadResult *start( );
public:
	void setFilePath( const QString &filePath ) {
		QMutexLocker< QMutex > locker( &mutex );
		file->setFile( filePath );
	}

	QString getFilePath( ) {
		QMutexLocker< QMutex > locker( &mutex );
		return file->absoluteFilePath( );
	}
	FileThreadResult *getFileResult( );
public:
	bool isFinished( );
	void requestInterruption( );
};

#endif // RWFILETHREAD_H_H_HEAD__FILE__
