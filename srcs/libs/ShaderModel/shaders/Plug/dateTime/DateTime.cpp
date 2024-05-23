#include "DateTime.h"

int16_t DateTime::getTimeToSecond( const std::chrono::milliseconds &milliseconds ) {
	auto count = milliseconds.count( );
	int64_t result = count / 1000 % 60;
	return result;
}
int16_t DateTime::getTimeToMinute( const std::chrono::milliseconds &milliseconds ) {
	auto count = milliseconds.count( );
	int64_t result = count / 1000 / 60 % 60;
	return result;
}
int16_t DateTime::getTimeToHour( const std::chrono::milliseconds &milliseconds ) {
	auto count = milliseconds.count( );
	int64_t result = count / 1000 / 60 / 60 % 24;
	return result;
}
int16_t DateTime::getTimeToDay( const std::chrono::milliseconds &milliseconds ) {
	auto count = milliseconds.count( );
	int64_t result = count / 1000 / 60 / 60 / 24;
	return result;
}
std::chrono::milliseconds DateTime::compare( const QDateTime &left_date_time, const QDateTime &right_date_time ) {
	std::chrono::milliseconds stdMilliseconds = left_date_time - right_date_time;
	return stdMilliseconds;
}
std::chrono::milliseconds DateTime::compareDateTime( const QString &left_date_time_qstring, const QString &left_date_time_qstring_format, const QString &right_date_time_qstring, const QString &right_date_time_qstring_format ) {
	QDateTime left_date_time = QDateTime::fromString( left_date_time_qstring, left_date_time_qstring_format );
	QDateTime right_date_time = QDateTime::fromString( right_date_time_qstring, right_date_time_qstring_format );
	return DateTime::compare( left_date_time, right_date_time );
}
