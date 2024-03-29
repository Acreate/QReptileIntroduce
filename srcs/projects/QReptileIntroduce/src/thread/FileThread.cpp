#include "FileThread.h"
void FileThread::run( ) {
	if( !file.isOpen( ) ) {
		if( !file.open( file.openMode( ) ) )
			return;
	}
	switch( file.openMode( ) ) {
		case QIODeviceBase::ReadOnly : /*读取*/ {
			fileThreadResult->data.clear( );
			constexpr qsizetype readBuffSize = 1024;
			qsizetype readSize = 0;
			auto buff = new char[ readBuffSize ];
			qint64 readCount = 0;
			do {
				readCount = file.read( buff, readBuffSize );
				if( readCount == 0 || isInterruptionRequested( ) )
					break;
				fileThreadResult->data.append( buff, readCount );
				readSize += readCount;
			} while( true );

			file.close( );
			delete[] buff;
			if( readCount != 0 ) {
				// 被请求中断
				emit fileThreadResult->interruptionRequested( );
				return;
			}
			fileThreadResult->setFinish( true );
			emit fileThreadResult->finish( );
		}
		break;
		case QIODeviceBase::WriteOnly : /*写入*/ {
		}
		break;
	}

}
