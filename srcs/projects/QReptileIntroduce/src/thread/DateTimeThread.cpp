#include "DateTimeThread.h"

#include <qdatetime.h>
#include <qmutex.h>

#include "../userHread/DebugInfo.h"
DateTimeThread::DateTimeThread( QObject *parent ) : QThread( parent ) {
	dateTimeStrFormatMutex = new QMutex;
}
DateTimeThread::~DateTimeThread( ) {
	delete dateTimeStrFormatMutex;
}
void DateTimeThread::run( ) {
	DEBUG_RUN( qDebug( ) << "DateTimeThread : " << currentThreadId() );
	QString format = tr( u8"%1 Year %2 Month %3 Day %4:%5:%6" );
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
const QString &DateTimeThread::getTimeStr( ) const {
	dateTimeStrFormatMutex->lock( );
	auto resultStrTime = currentDateTimeStrFormat;
	dateTimeStrFormatMutex->unlock( );
	return resultStrTime;
}
