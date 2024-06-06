#ifndef FILE_H_H_HEAD__FILE__
#define FILE_H_H_HEAD__FILE__
#pragma once

#include <QFileInfo>
#include <QString>

#include "../auto_generate_files/export/Path_Tool_export.h"
class Path;
class PATH_TOOL_EXPORT File {
public:
	friend class Path;
private:
	QString currentFilePtah;
public:
	File( const QString &currentFilePtah );
	virtual ~File( );
public:
	const QString & getCurrentFilePtah( ) const {
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
	friend bool operator==( const File &lhs, const File &rhs ) { return lhs.currentFilePtah == rhs.currentFilePtah; }
	friend bool operator!=( const File &lhs, const File &rhs ) { return !( lhs == rhs ); }
};

#endif // FILE_H_H_HEAD__FILE__
