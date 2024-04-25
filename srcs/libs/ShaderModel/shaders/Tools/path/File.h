#ifndef FILE_H_H_HEAD__FILE__
#define FILE_H_H_HEAD__FILE__
#pragma once

#include <QFileInfo>
#include <QString>

#include "../export/Tools_export.h"
class TOOLS_EXPORT File {
private:
	QString currentFilePtah;
public:
	File( const QString &currentFilePtah );
	virtual ~File( );
public:
	QString getCurrentFilePtah( ) const {
		return currentFilePtah;
	}
public:
	bool operator==( const QString &path ) {
		return currentFilePtah == QFileInfo( path ).absolutePath( );
	}
	friend bool operator==( const File &file, const QString &path ) {
		return file.currentFilePtah == QFileInfo( path ).absolutePath( );
	}
	bool operator==( const QFileInfo &path ) {
		return currentFilePtah == path.absolutePath( );
	}
	friend bool operator==( const File &file, const QFileInfo &path ) {
		return file.currentFilePtah == path.absolutePath( );
	}
};

#endif // FILE_H_H_HEAD__FILE__
