#ifndef FILETHREAD_H_H_HEAD__FILE__
#define FILETHREAD_H_H_HEAD__FILE__
#pragma once
#include <QThread>
#include <qfileinfo.h>
#include <QFile>
#include <QIODeviceBase>

class FileResult;
class FileThread : public QThread {
	Q_OBJECT;
private:
	qsizetype buffSize = 1024;
	QFile file;
	FileResult *fileThreadResult;
	QIODeviceBase::OpenMode openMode;
	QIODeviceBase::OpenMode runOpenMode;
public:
	FileThread( const QString &filePath, QIODeviceBase::OpenMode openMode, FileResult *fileThreadResult );
	~FileThread( ) override;
public:
	QIODeviceBase::OpenMode resetOpenMode( const QIODeviceBase::OpenMode &newOpenMode );
	FileResult *readFile( );
	FileResult *writeFile( );
protected:
	void run( ) override;
public:
	FileResult *getFileThreadResult( ) {
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
	qsizetype getBuffSize( ) const {
		return buffSize;
	}
	void setBuffSize( const qsizetype buffSize ) {
		this->buffSize = buffSize;
	}
};

#endif // FILETHREAD_H_H_HEAD__FILE__
