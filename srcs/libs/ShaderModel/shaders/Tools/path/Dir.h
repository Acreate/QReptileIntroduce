#ifndef DIR_H_H_HEAD__FILE__
#define DIR_H_H_HEAD__FILE__
#pragma once

#include <QString>

#include "File.h"

#include "../export/Tools_export.h"
class TOOLS_EXPORT Dir {
private:
	QString currentPath;
	std::vector< File > files;
	std::vector< Dir > dirs;
public:
	Dir( const QString &path );
	virtual ~Dir( );
public: // 动作
	size_t updateInfo( );
	std::vector< File > getFiles( ) const {
		return files;
	}
	std::vector< Dir > getDirs( ) const {
		return dirs;
	}
	QString getCurrentPath( ) const {
		return currentPath;
	}
};

#endif // DIR_H_H_HEAD__FILE__
