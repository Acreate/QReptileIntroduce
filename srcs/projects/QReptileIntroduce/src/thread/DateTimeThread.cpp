#include "DateTimeThread.h"

#include <qdatetime.h>
DateTimeThread::DateTimeThread( QObject *parent ) : QThread( parent ) {
	dateTimeStrFormatMutex = new QMutex;
}
DateTimeThread::~DateTimeThread( ) {
	delete dateTimeStrFormatMutex;
}
void DateTimeThread::run( ) {
	qDebug( ) << "DateTimeThread : " << currentThreadId();
	while( !this->isInterruptionRequested( ) ) {
		dateTimeStrFormatMutex->lock( );
		QDateTime currentDateTime = QDateTime::currentDateTime( );
		currentDateTimeStrFormat = currentDateTime.toString( tr( u8"yyyy Year dd Month MM Day hh:mm:ss" ) );
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
