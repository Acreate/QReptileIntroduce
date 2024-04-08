#include "Dir.h"

#include <QDir>
Dir::Dir( const QString &path ): currentPath( path ) {

}
Dir::~Dir( ) {
}
size_t Dir::updateInfo( ) {
	QDir currentDir( currentPath );
	QFileInfoList entryInfoList = currentDir.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot );
	for( auto &info : entryInfoList )
		if( info.isDir( ) )
			dirs.emplace_back( Dir( info.absoluteFilePath( ) ) );
		else
			files.emplace_back( File( info.absoluteFilePath( ) ) );
	return entryInfoList.count( );
}
