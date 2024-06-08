#ifndef FILETHREADRESULT_H_H_HEAD__FILE__
#define FILETHREADRESULT_H_H_HEAD__FILE__

#pragma once
#include <QFileDevice>
#include <qbytearray.h>
#include <QObject>
class RWFileThread;
class FileThread;
#include "../auto_generate_files/export/Tools_export.h"

class TOOLS_EXPORT FileResult : public QObject {
	Q_OBJECT;
public:
	friend class RWFileThread;
	friend class FileThread;
private:
	RWFileThread *fileThread;
	QByteArray data;
	bool isFinish;
public:
	FileResult( RWFileThread *fileThread ): fileThread( fileThread ) {
		isFinish = true;
	}
	FileResult( RWFileThread *fileThread, const QByteArray &data ): fileThread( fileThread ), data( data ) {
		isFinish = true;
	}
	FileResult( RWFileThread *fileThread, const QString &str ): fileThread( fileThread ), data( str.toUtf8( ) ) {
		isFinish = true;
	}
	~FileResult( ) override;
private:
	void setData( const QByteArray &data ) {
		this->data.clear( );
		const char *saveDataPtr = data.data( );
		auto saveContentSize = data.length( );
		this->data.resize( saveContentSize );
		for( decltype(saveContentSize) index = 0 ; index < saveContentSize ; ++index )
			this->data[ index ] = saveDataPtr[ index ];
	}
	void setFinish( const bool flag ) {
		isFinish = flag;
	}
public:
	bool runFinish( ) const {
		return isFinish;
	}
	QByteArray &getData( ) {
		return data;
	}
	void await( );
Q_SIGNALS:
	void finish( );
	void interruptionRequested( );
	/// <summary>
	/// 错误的槽函数
	/// </summary>
	/// <param name="errorType">错误类型，3 表示同时错误，1 表示文件错误，2 表示目录错误</param>
	/// <param name="errorCode">文件错误信息</param>
	/// <param name="dirError">目录错误信息</param>
	void error( int errorType, QFileDevice::FileError errorCode, QFileDevice::FileError dirError );
};

#endif // FILETHREADRESULT_H_H_HEAD__FILE__
