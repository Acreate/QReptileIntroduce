﻿#ifndef DIR_H_H_HEAD__FILE__
#define DIR_H_H_HEAD__FILE__
#pragma once

#include <QDir>
#include <QString>

#include "File.h"

#include "../auto_generate_files/export/Path_Tool_export.h"


class Path;
class PATH_TOOL_EXPORT Dir {
public:
	friend class Path;
private:
	QString currentPath;
	std::vector< File > files;
	std::vector< Dir > dirs;
public:
	Dir( const QString &path );
	virtual ~Dir( );
public: // 动作

	std::vector< File > getFiles( ) const {
		return files;
	}
	std::vector< Dir > getDirs( ) const {
		return dirs;
	}
	const QString & getCurrentPath( ) const {
		return currentPath;
	}
public:
	bool operator==( const QString &path ) {
		return currentPath == QFileInfo( path ).absolutePath( );
	}
	bool operator==( const QFileInfo &path ) {
		return currentPath == path.absolutePath( );
	}
	bool operator==( const QDir &path ) {
		return currentPath == path.absolutePath( );
	}
	friend bool operator==( const Dir &dir, const QString &path ) {
		return dir.currentPath == QFileInfo( path ).absolutePath( );
	}
	friend bool operator==( const Dir &dir, const QFileInfo &path ) {
		return dir.currentPath == path.absolutePath( );
	}
	friend bool operator==( const Dir &dir, const QDir &path ) {
		return dir.currentPath == path.absolutePath( );
	}
	friend bool operator==( const Dir &lhs, const Dir &rhs ) { return lhs.currentPath == rhs.currentPath; }
	friend bool operator!=( const Dir &lhs, const Dir &rhs ) { return !( lhs == rhs ); }
};

#endif // DIR_H_H_HEAD__FILE__
