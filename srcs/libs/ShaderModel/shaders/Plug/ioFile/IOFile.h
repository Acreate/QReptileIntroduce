#ifndef IOFILE_H_H_HEAD__FILE__
#define IOFILE_H_H_HEAD__FILE__
#pragma once
#include <QString>
#include <QFile>

#include "../export/Plug_export.h"
#include "interface/INovelInfo.h"
#include "nameSpace/interfacePlugsType.h"

class PLUG_EXPORT IOFile {
	QFile *qFile;
	interfacePlugsType::Vector_INovelInfoSPtr_Shared novelInfoSPtrShared;
public:
	IOFile( const QString &filePath, interfacePlugsType::Vector_INovelInfoSPtr_Shared novel_info_s_ptr_shared ) {
		novelInfoSPtrShared = novel_info_s_ptr_shared;
		qFile = new QFile( filePath );
	}
	virtual ~IOFile( ) { }
public:
	QStringList getNovelInfoList( ) {
		QStringList result;
		auto element = novelInfoSPtrShared.get( );
		if( !element || element->size( ) ) // 不存在，返回 空
			return result;
		auto iterator = element->begin( );
		auto end = element->end( );
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
	/// <param name="jion_str">拼接字符串</param>
	/// <returns>写入数量</returns>
	size_t writeQStringListToFile( const QStringList &writeContent, const QString &jion_str ) {
		qint64 qsizetype = writeContent.size( );
		if( qsizetype == 0 )
			return qsizetype;
		if( qFile->open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate ) ) {
			QString writeContents = writeContent.join( jion_str );
			qFile->write( writeContents.toLocal8Bit( ) );
			qFile->close( );
		}
		return qsizetype;
	}

	/// <summary>
	/// 使用小说列表写入路径
	/// </summary>
	/// <param name="jion_str">拼接字符串</param>
	/// <returns>写入数量</returns>
	size_t writeQStringListToFile( const QString &jion_str ) {
		return writeQStringListToFile( getNovelInfoList( ), jion_str );
	}

};


#endif // IOFILE_H_H_HEAD__FILE__
