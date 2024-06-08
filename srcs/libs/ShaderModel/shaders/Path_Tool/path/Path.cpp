#include "Path.h"

#include <QDir>
#include "Dir.h"
#include "File.h"
std::pair< Path::DirList, Path::FileList > Path::getPathInfo( const QString &path ) {
	QFileInfo info( path );
	if( info.isDir( ) )
		return getDirInfo( path );
	return getFileInfo( path );
}
std::pair< Path::DirList, Path::FileList > Path::getDirInfo( const QString &path ) {
	std::pair< DirList, FileList > result;
	Dir topDir( path );
	Path::updateInfo( topDir );
	auto dirs = topDir.getDirs( );
	bool isExis = false; // 防止重复
	for( auto &dir : dirs ) {
		std::vector< Dir > &dirVector = result.first;
		for( auto &exisDir : dirVector )
			if( exisDir.currentPath == dir.currentPath ) {
				isExis = true;
				break;
			}
		if( isExis ) {
			isExis = false;
			continue;
		}
		dirVector.emplace_back( dir );
		QString currentPath = dir.currentPath;
		auto dirInfo = getDirInfo( dir.currentPath );
		auto subDirs = dirInfo.first;
		for( auto &subDir : subDirs )
			dirVector.emplace_back( subDir );
		auto subFiles = dirInfo.second;
		for( auto &subFile : subFiles )
			result.second.emplace_back( subFile );
	}
	auto files = topDir.getFiles( );
	for( auto &file : files ) {
		std::vector< File > &fileVector = result.second;
		for( auto &exisFile : fileVector )
			if( exisFile.currentFilePtah == file.currentFilePtah ) {
				isExis = true;
				break;
			}
		if( isExis ) {
			isExis = false;
			continue;
		}
		result.second.emplace_back( file );
	}
	return result;
}
std::pair< Path::DirList, Path::FileList > Path::getFileInfo( const QString &path ) {

	QFileInfo fileInfo( path );
	if( fileInfo.isFile( ) )
		return std::pair< DirList, FileList >( { }, { File( fileInfo.absoluteFilePath( ) ) } );
	return { };
}
QStringList Path::splitPath( const QString &path ) {
	qsizetype length = path.length( );
	QStringList result;
	if( length == 0 )
		return result;
	QChar *buff = new QChar[ length ];
	qsizetype buffIndex = 0, index = 0;
	QChar currentQChar;
	for( ; index < length; ++index ) {
		currentQChar = path[ index ];
		if( currentQChar == '\\' || currentQChar == '/' )
			continue;
		buff[ buffIndex ] = currentQChar;
		for( ++index, ++buffIndex; index < length; ++index, ++buffIndex ) {
			currentQChar = path[ index ];
			if( currentQChar == '\\' || currentQChar == '/' ) {
				if( buffIndex > 0 ) {
					result.append( QString( buff, buffIndex ) );
					buffIndex = 0;
				}
				break;
			}
			buff[ buffIndex ] = currentQChar;
		}
	}
	if( buffIndex > 0 )
		result.append( QString( buff, buffIndex ) );
	delete[] buff;
	return result;

}
bool Path::creatFilePath( const QString &path ) {
	QFileInfo info( path );
	if( info.exists( ) )
		if( info.isFile( ) )
			return true;
		else
			return false;
	QDir targetDir = info.dir( );
	QString dirPath = targetDir.absolutePath( );
	if( !Path::creatDirPath( dirPath ) )
		return false;
	QString absoluteFilePath = info.absoluteFilePath( );
	QFile file( absoluteFilePath );
	if( file.open( QIODeviceBase::NewOnly | QIODevice::WriteOnly ) ) {
		file.write( "", 0 );
		file.close( );
		return true;
	}
	return false;
}
bool Path::creatDirPath( const QString &path ) {
	QFileInfo info( path );
	if( info.exists( ) )
		if( info.isDir( ) )
			return true;
		else
			return false;
	return QDir( ).mkpath( path );
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

size_t Path::updateInfo( Dir &dir ) {
	QDir currentDir( dir.currentPath );
	auto entryInfoList = currentDir.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot );
	for( auto &info : entryInfoList ) {
		auto absolutePath = info.absoluteFilePath( );
		if( info.isDir( ) )
			dir.dirs.emplace_back( Dir( absolutePath ) );
		else
			dir.files.emplace_back( File( absolutePath ) );
	}
	return dir.dirs.size( ) + dir.files.size( );
}
size_t Path::copyDirInfoToOtherDir( Dir &src, Dir &des ) {
	size_t orgSize = src.files.size( ) + des.dirs.size( );
	size_t srcSize = src.dirs.size( );
	for( size_t index = 0; index < srcSize; ++index )
		des.dirs.emplace_back( src.dirs[ index ] );
	srcSize = src.files.size( );
	for( size_t index = 0; index < srcSize; ++index )
		des.files.emplace_back( src.files[ index ] );
	return src.files.size( ) + des.dirs.size( ) - orgSize;
}
