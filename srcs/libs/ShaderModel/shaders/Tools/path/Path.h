#ifndef PATH_H_H_HEAD__FILE__
#define PATH_H_H_HEAD__FILE__
#pragma once
#include <qcontainerfwd.h>

#include "Dir.h"

#include "../export/Tools_export.h"
class TOOLS_EXPORT Path {
public:
	using FileList = std::vector< File >;
	using DirList = std::vector< Dir >;
	static std::pair< DirList, FileList > getPathInfo( const QString &path );
	static std::pair< DirList, FileList > getDirInfo( const QString &path );
	static std::pair< DirList, FileList > getFileInfo( const QString &path );
};

#endif // PATH_H_H_HEAD__FILE__
