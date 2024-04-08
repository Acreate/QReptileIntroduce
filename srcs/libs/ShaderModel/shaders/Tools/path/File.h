#ifndef FILE_H_H_HEAD__FILE__
#define FILE_H_H_HEAD__FILE__
#pragma once

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
};

#endif // FILE_H_H_HEAD__FILE__
