#ifndef FILETHREADRESULT_H_H_HEAD__FILE__
#define FILETHREADRESULT_H_H_HEAD__FILE__

#pragma once
#include <qbytearray.h>
#include "RWFileThread.h"
class FileThreadResult : public QObject {
	Q_OBJECT;
public:
	friend class RWFileThread;
	friend class FileThread;
private:
	RWFileThread *fileThread;
	QByteArray data;
	bool isFinish;
public:
	FileThreadResult( RWFileThread *fileThread ): fileThread( fileThread ) {
		isFinish = true;
	}
	FileThreadResult( RWFileThread *fileThread, const QByteArray &data ): fileThread( fileThread ), data( data ) {
		isFinish = true;
	}
	FileThreadResult( RWFileThread *fileThread, const QString &str ): fileThread( fileThread ), data( str.toLocal8Bit( ) ) {
		isFinish = true;
	}
	~FileThreadResult( ) override {

	}
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
	const QByteArray &getData( ) const {
		return data;
	}
	bool await( );
Q_SIGNALS:
	void finish( );
	void interruptionRequested( );
};

#endif // FILETHREADRESULT_H_H_HEAD__FILE__
