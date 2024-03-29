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
	QSharedPointer< FileThreadResult > fileThreadResult;
	QIODeviceBase::OpenMode openMode;
	QIODeviceBase::OpenMode runOpenMode;
public:
	FileThread( const QString &filePath, QIODeviceBase::OpenMode openMode, QSharedPointer< FileThreadResult > fileThreadResult );
public:
	QIODeviceBase::OpenMode resetOpenMode( const QIODeviceBase::OpenMode &newOpenMode );
	QSharedPointer< FileThreadResult > readFile( );
	QSharedPointer< FileThreadResult > writeFile( );
protected:
	void run( ) override;
public:
	QSharedPointer< FileThreadResult > getFileThreadResult( ) {
		return fileThreadResult;
	}
};

#endif // FILETHREAD_H_H_HEAD__FILE__
