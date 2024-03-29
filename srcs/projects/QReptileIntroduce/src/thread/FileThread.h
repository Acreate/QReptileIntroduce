#ifndef FILETHREAD_H_H_HEAD__FILE__
#define FILETHREAD_H_H_HEAD__FILE__
#pragma once
#include <QThread>
#include <qfileinfo.h>
#include <qobject.h>
#include <QFile>
#include <QIODeviceBase>

#include "FileThreadResult.h"
class FileThread : public QThread {
	Q_OBJECT;
private:
	QFile file;
	FileThreadResult *fileThreadResult;
public:
	FileThread( const QString &filePath, FileThreadResult *fileThreadResult ) : file( filePath ), fileThreadResult( fileThreadResult ) {

	}

	FileThreadResult *readFile( ) {
		if( !file.exists( ) )
			return nullptr;
		if (file.open( QIODeviceBase::ReadOnly )) {
			
		}
	}
protected:
	void run( ) override;
};

#endif // FILETHREAD_H_H_HEAD__FILE__
