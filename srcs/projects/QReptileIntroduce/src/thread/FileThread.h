#ifndef FILETHREAD_H_H_HEAD__FILE__
#define FILETHREAD_H_H_HEAD__FILE__
#pragma once
#include <QThread>
#include <qfileinfo.h>
#include <qobject.h>
#include <QFile>
#include <QIODeviceBase>
#include <QSharedPointer>

class FileThreadResult;
class FileThread : public QThread {
	Q_OBJECT;
private:
	QFile file;
	FileThreadResult *fileThreadResult;
	QIODeviceBase::OpenMode openMode;
	QIODeviceBase::OpenMode runOpenMode;
public:
	FileThread( const QString &filePath, QIODeviceBase::OpenMode openMode, FileThreadResult *fileThreadResult );
public:
	QIODeviceBase::OpenMode resetOpenMode( const QIODeviceBase::OpenMode &newOpenMode );
	FileThreadResult *readFile( );
	FileThreadResult *writeFile( );
protected:
	void run( ) override;
public:
	FileThreadResult *getFileThreadResult( ) {
		return fileThreadResult;
	}
	QIODeviceBase::OpenMode getOpenMode( ) const {
		return openMode;
	}
	void setOpenMode( const QIODeviceBase::OpenMode &newOpenMode ) {
		openMode = newOpenMode;
	}

	QString getFileName( ) const {
		return file.fileName( );
	}
	void setFilePath( const QString &newPath ) {
		file.setFileName( newPath );
	}
};

#endif // FILETHREAD_H_H_HEAD__FILE__
