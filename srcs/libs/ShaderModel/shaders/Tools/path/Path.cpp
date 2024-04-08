#include "Path.h"

#include "../../../../../projects/ReptileIntroduce/src/qt/extend/thread/FileThread.h"
std::pair< Path::DirList, Path::FileList > Path::getPathInfo( const QString &path ) {

	QFileInfo info( path );
	if( info.isDir( ) )
		return getDirInfo( path );
	return getFileInfo( path );
}
std::pair< Path::DirList, Path::FileList > Path::getDirInfo( const QString &path ) {
	std::pair< DirList, FileList > result;
	Dir topDir( path );
	topDir.updateInfo( );
	auto dirs = topDir.getDirs( );
	for( auto &dir : dirs ) {
		result.first.emplace_back( dir );
		auto dirInfo = getDirInfo( dir.getCurrentPath( ) );
		auto subDirs = dirInfo.first;
		for( auto &subDir : subDirs )
			result.first.emplace_back( subDir );
		auto subFiles = dirInfo.second;
		for( auto &subFile : subFiles )
			result.second.emplace_back( subFile );
	}
	auto files = topDir.getFiles( );
	for( auto &file : files )
		result.second.emplace_back( file );
	return result;
}
std::pair< Path::DirList, Path::FileList > Path::getFileInfo( const QString &path ) {
	return std::pair< DirList, FileList >( { }, { File( QFileInfo( path ).absoluteFilePath( ) ) } );
}
