#include "DateTimeThread.h"

#include <qdatetime.h>
#include <qmutex.h>

#include "../../userHread/DebugInfo.h"
DateTimeThread::DateTimeThread( QObject *parent ) : QThread( parent ) {
	dateTimeStrFormatMutex = new QMutex;
}
DateTimeThread::~DateTimeThread( ) {
	delete dateTimeStrFormatMutex;
}
void DateTimeThread::run( ) {
	DEBUG_RUN( qDebug( ) << "DateTimeThread 运行，当前线程 id: " << currentThreadId() );
	QString format = tr( u8"%1 年 %2 月 %3 日 %4:%5:%6" );
	while( !this->isInterruptionRequested( ) ) {
		dateTimeStrFormatMutex->lock( );
		QDateTime currentDateTime = QDateTime::currentDateTime( );
		auto date = currentDateTime.date( );
		auto time = currentDateTime.time( );
		currentDateTimeStrFormat = format.arg( date.year( ) ).arg( date.month( ) ).arg( date.day( ) ).arg( time.hour( ) ).arg( time.minute( ) ).arg( time.second( ) );
		auto resultStrTime = currentDateTimeStrFormat;
		dateTimeStrFormatMutex->unlock( );
		emit updateDateTimeStr( resultStrTime );
		emit updateDateTime( currentDateTime );
		usleep( 500 );
	}
}
 QString DateTimeThread::getTimeStr( ) const {
	dateTimeStrFormatMutex->lock( );
	auto resultStrTime = currentDateTimeStrFormat;
	dateTimeStrFormatMutex->unlock( );
	return resultStrTime;
}
