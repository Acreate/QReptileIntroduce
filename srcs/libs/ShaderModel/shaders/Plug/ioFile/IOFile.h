#ifndef IOFILE_H_H_HEAD__FILE__
#define IOFILE_H_H_HEAD__FILE__
#pragma once
#include <QString>
#include <QFile>

#include "../auto_generate_files/export/Plug_export.h"
#include "interface/INovelInfo.h"
#include "nameSpace/interfacePlugsType.h"
#include "path/Path.h"


class PLUG_EXPORT IOFile {
	QFile *qFile;
	interfacePlugsType::Vector_INovelInfoSPtr novelInfoSPtrShared;
public:
	IOFile( const QString &filePath, const interfacePlugsType::Vector_INovelInfoSPtr &novel_info_s_ptr_shared ) {
		novelInfoSPtrShared = novel_info_s_ptr_shared;
		qFile = new QFile( filePath );
	}
	virtual ~IOFile( ) { }
public:
	QStringList getNovelInfoList( ) {
		QStringList result;
		if( novelInfoSPtrShared.size( ) == 0 ) // 不存在，返回 空
			return result;
		auto iterator = novelInfoSPtrShared.begin( );
		auto end = novelInfoSPtrShared.end( );
		interfacePlugsType::HtmlDocString resultSerializableHtmlDocString;
		do {
			if( iterator->get( )->objToHtmlDocString( &resultSerializableHtmlDocString ) > 0 )
				result.append( QString::fromStdWString( resultSerializableHtmlDocString ) );
			++iterator;
			if( iterator == end )
				break;
		} while( true );
		return result;
	}
	/// <summary>
	/// 根据列表写入所有成员
	/// </summary>
	/// <param name="writeContent">写入列表</param>
	/// <returns>写入数量</returns>
	size_t writeQStringToFile( const QString &writeContent ) {
		qint64 qsizetype = writeContent.size( );
		if( qsizetype == 0 )
			return qsizetype;
		if( qFile->open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
			qsizetype = qFile->write( writeContent.toUtf8( ) );
			qFile->close( );
		}
		return qsizetype;
	}

	/// <summary>
	/// 根据列表写入所有成员
	/// </summary>
	/// <param name="writeContent">写入列表</param>
	/// <param name="jion_str">拼接字符串</param>
	/// <returns>写入数量</returns>
	size_t writeQStringListToFile( const QStringList &writeContent, const QString &jion_str ) {
		qint64 resultSize = writeContent.size( );
		if( resultSize == 0 )
			return resultSize;
		QString fileName = qFile->fileName( );
		Path::creatFilePath( fileName );
		if( qFile->open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
			QString writeContents = QDateTime::currentDateTime( ).toString( u8"yyyy 连 MM 月 dd 日 hh:mm:ss\n" );
			for( qsizetype index = 0; index < resultSize; ++index )
				writeContents.append( jion_str ).append( QString( "\t%1 / %2\n" ).arg( index + 1 ).arg( resultSize ) ).append( writeContent.at( index ) );
			qint64 write = qFile->write( writeContents.toUtf8( ) );
			qFile->close( );
			if( write == 0 )
				return 0;
		} else
			return 0;
		return resultSize;
	}

	/// <summary>
	/// 根据列表写入所有成员
	/// </summary>
	/// <param name="writeContent">写入列表</param>
	/// <returns>写入数量</returns>
	size_t writeQStringListToFile( const QStringList &writeContent ) {
		qint64 resultSize = writeContent.size( );
		if( resultSize == 0 )
			return resultSize;
		QString fileName = qFile->fileName( );
		Path::creatFilePath( fileName );
		if( qFile->open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
			QString writeContents = QDateTime::currentDateTime( ).toString( u8"yyyy 连 MM 月 dd 日 hh:mm:ss" );
			QString fort( "\n%2\t%3 / %1\n" );
			fort = fort.arg( resultSize );
			for( qsizetype index = 0; index < resultSize; ++index )
				writeContents.append( fort.arg( u8"============" ).arg( index + 1 ) ).append( writeContent.at( index ) ).append( "\n" );
			qint64 write = qFile->write( writeContents.toUtf8( ) );
			qFile->close( );
			if( write == 0 )
				return 0;
		} else
			return 0;
		return resultSize;
	}

	/// <summary>
	/// 使用小说列表写入路径
	/// </summary>
	/// <param name="jion_str">拼接字符串</param>
	/// <returns>写入数量</returns>
	size_t writeNoveInfoListToFile( const QString &jion_str ) {
		return writeQStringListToFile( getNovelInfoList( ), jion_str );
	}
	/// <summary>
	/// 使用小说列表写入路径
	/// </summary>
	/// <returns>写入数量</returns>
	size_t writeNoveInfoListToFile( ) {
		return writeQStringListToFile( getNovelInfoList( ) );
	}

};


#endif // IOFILE_H_H_HEAD__FILE__
