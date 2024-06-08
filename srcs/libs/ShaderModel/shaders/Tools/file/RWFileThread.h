#ifndef RWFILETHREAD_H_H_HEAD__FILE__
#define RWFILETHREAD_H_H_HEAD__FILE__

#pragma once
#include <qmutex.h>
#include <QObject>

class QFileInfo;
class FileThread;
class QFile;
class FileResult;
#include "../auto_generate_files/export/Tools_export.h"


class TOOLS_EXPORT  RWFileThread : public QObject {

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
	void await( long usleep = 20 );
	void stop( long usleep = 20 );
	FileResult *start( );
public:
	void setFilePath( const QString &filePath );

	QString getFilePath( );
	FileResult *getFileResult( );
public:
	bool isFinished( );
	void requestInterruption( );
};

#endif // RWFILETHREAD_H_H_HEAD__FILE__
