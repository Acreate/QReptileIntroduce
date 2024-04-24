#include "Path.h"

#include <QDir>

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
		QString currentPath = dir.getCurrentPath( );
		auto dirInfo = getDirInfo( currentPath );
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

	QFileInfo fileInfo( path );
	if( fileInfo.isFile( ) )
		return std::pair< DirList, FileList >( { }, { File( fileInfo.absoluteFilePath( ) ) } );
	return { };
}
bool Path::creatFilePath( const QString &path ) {
	QFileInfo info( path );
	if( info.exists( ) )
		return true;
	auto isCreateDirPath = creatDirPath( info.dir( ).absolutePath( ) );
	if( isCreateDirPath ) {
		QFile file( info.absoluteFilePath( ) );
		if( file.open( QIODeviceBase::NewOnly | QIODevice::WriteOnly ) ) {
			file.write( "", 0 );
			file.close( );
			return true;
		}
	}
	return false;
}
bool Path::creatDirPath( const QString &path ) {
	QFileInfo info( path );
	if( info.exists( ) )
		return true;

	return info.dir( ).mkdir( path );
}
bool Path::removePath( const QString &path ) {
	QFileInfo fileInfo( path );
	if( !fileInfo.exists( ) )
		return true;
	if( fileInfo.isFile( ) )
		return removeFilePath( path );
	return removeDirPath( path );
}
bool Path::removeFilePath( const QString &path ) {
	QFileInfo fileInfo( path );
	if( !fileInfo.exists( ) )
		return true;
	return QFile::remove( path );
}
bool Path::removeDirPath( const QString &path ) {
	QFileInfo fileInfo( path );
	if( !fileInfo.exists( ) )
		return true;
	QDir absoluteDir = fileInfo.absoluteDir( );
	return absoluteDir.removeRecursively( );
}
