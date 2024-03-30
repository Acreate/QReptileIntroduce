#ifndef RWFILETHREAD_H_H_HEAD__FILE__
#define RWFILETHREAD_H_H_HEAD__FILE__

#pragma once
#include <qmutex.h>

#include "../thread/FileThread.h"

class QFileInfo;
class FileThread;
class QFile;
class FileResult;
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
	FileResult *threadFileThreadResult = nullptr;
	QMutex mutex;
public: // 线程函数
	FileResult *readFile( );
	FileResult *writeFile( const QString &content );
	FileResult *writeFile( const QByteArray &byteData );
	bool await( );
	FileResult *start( );
public:
	void setFilePath( const QString &filePath ) {
		QMutexLocker< QMutex > locker( &mutex );
		file->setFile( filePath );
	}

	QString getFilePath( ) {
		QMutexLocker< QMutex > locker( &mutex );
		return file->absoluteFilePath( );
	}
	FileResult *getFileResult( );
public:
	bool isFinished( );
	void requestInterruption( );
};

#endif // RWFILETHREAD_H_H_HEAD__FILE__
