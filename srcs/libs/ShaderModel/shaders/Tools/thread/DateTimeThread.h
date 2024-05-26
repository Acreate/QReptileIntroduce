#ifndef DATETIMETHREAD_H_H_HEAD__FILE__
#define DATETIMETHREAD_H_H_HEAD__FILE__
#pragma once
#include <QThread>
#include <QMutexLocker>
#include <QObject>
#include "../auto_generate_files/export/Tools_export.h"

class TOOLS_EXPORT DateTimeThread : public QThread {
	Q_OBJECT;
public:
	DateTimeThread( QObject *parent = nullptr );
	~DateTimeThread( ) override;
protected:
	void run( ) override;
public:
	QString getTimeStr( ) const;
private:
	QString currentDateTimeStrFormat;
	QMutex *dateTimeStrFormatMutex;

Q_SIGNALS:
	void updateDateTimeStr( const QString &time );
	void updateDateTime( const QDateTime &dateTime );
};

#endif // TIMETHREAD_H_H_HEAD__FILE__
