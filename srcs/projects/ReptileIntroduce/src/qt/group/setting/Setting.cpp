#include "./Setting.h"

#include <qcoreapplication.h>
#include <qdir.h>
#include <QFileInfo>

Setting::Setting( const QString &filePath, QObject *parent = nullptr ):
	QObject( parent ) {
	setting = new QSettings( filePath, QSettings::IniFormat, this );
	instanceMutex = new QMutex;
}
Setting::Setting( QObject *parent = nullptr ) : QObject( parent ) {
	auto filePath = QString( u8"%1%2%3%2%4%2%5%6" ).arg( qApp->applicationDirPath( ) ).arg( QDir::separator( ) ).arg( u8"progress" ).arg( u8"ini" ).arg( qApp->applicationName( ) ).arg( u8"ini" );
	setting = new QSettings( filePath, QSettings::IniFormat, this );
	instanceMutex = new QMutex;
}
Setting::~Setting( ) {
	delete instanceMutex;
	if( setting ) {
		setting->sync( );
		setting->deleteLater( );
	}
}
QVariant Setting::getValue( const QAnyStringView &key ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	return setting->value( key );
}
QVariant Setting::getValue( const QAnyStringView &groupName, const QAnyStringView &key ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	setting->beginGroup( groupName );
	auto result = setting->value( key );
	setting->endGroup( );
	return result;
}
QVariant Setting::getValue( const QAnyStringView &key, const std::function< QVariant( const QVariant & ) > &ifFunction ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	return ifFunction( setting->value( key ) );
}
QVariant Setting::getValue( const QAnyStringView &groupName, const QAnyStringView &key, const std::function< QVariant( const QVariant & ) > &ifFunction ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	setting->beginGroup( groupName );
	auto result = setting->value( key );
	setting->endGroup( );
	return ifFunction( result );
}
void Setting::setValue( const QAnyStringView &key, const QVariant &value ) {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	setting->setValue( key, value );
}
void Setting::setValue( const QAnyStringView &groupName, const QAnyStringView &key, const QVariant &value ) {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	setting->beginGroup( groupName );
	setting->setValue( key, value );
	setting->endGroup( );
}
void Setting::setValue( const QMap< QAnyStringView, QVariant > &valuePair ) {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	auto iterator = valuePair.begin( );
	auto endIterator = valuePair.end( );
	for( ; iterator != endIterator; ++iterator )
		setting->setValue( iterator.key( ), iterator.value( ) );
}
void Setting::setValue( const QAnyStringView &groupName, const QMap< QAnyStringView, QVariant > &valuePair ) {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	auto iterator = valuePair.begin( );
	auto endIterator = valuePair.end( );
	setting->beginGroup( groupName );
	for( ; iterator != endIterator; ++iterator )
		setting->setValue( iterator.key( ), iterator.value( ) );
	setting->endGroup( );
}
QStringList Setting::getAllKey( ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	return setting->allKeys( );
}
QStringList Setting::getAllKey( const QAnyStringView &group ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	setting->beginGroup( group );
	auto allKey = setting->allKeys( );
	setting->endGroup( );
	return allKey;
}
QList< QVariant > Setting::getAllValue( ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	QList< QVariant > result;
	auto allKey = setting->allKeys( );
	for( auto &key : allKey )
		result.append( setting->value( key ) );
	return result;
}
QList< QVariant > Setting::getAllValue( const QAnyStringView &group ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	QList< QVariant > result;
	setting->beginGroup( group );
	auto allKey = setting->allKeys( );
	for( auto &key : allKey )
		result.append( setting->value( key ) );
	setting->endGroup( );
	return result;
}
QMap< QAnyStringView, QVariant > Setting::getAllInfo( ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	QMap< QAnyStringView, QVariant > result;
	auto allKey = setting->allKeys( );
	for( auto &key : allKey )
		result.insert( key, setting->value( key ) );
	return result;
}
QMap< QAnyStringView, QVariant > Setting::getAllInfo( const QAnyStringView &group ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	QMap< QAnyStringView, QVariant > result;
	setting->beginGroup( group );
	auto allKey = setting->allKeys( );
	for( auto &key : allKey )
		result.insert( key, setting->value( key ) );
	setting->endGroup( );
	return result;

}
void Setting::sync( ) {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	setting->sync( );
}
bool Setting::setFilePath( const QString &filePath ) {
	QFileInfo fileInfo( filePath );
	if( fileInfo.exists( ) ) {
		QString fileName = setting->fileName( );
		auto absoluteFilePath = fileInfo.absoluteFilePath( );
		fileInfo.setFile( fileName );
		if( absoluteFilePath == fileInfo.absoluteFilePath( ) )
			return false;
		QMutexLocker< QMutex > nstanceLock( instanceMutex );
		setting->sync( );
		setting->deleteLater( );
		setting = new QSettings( absoluteFilePath, QSettings::IniFormat, this );
		return true;
	}
	return false;
}
QString Setting::getFilePath( ) const {
	return QFileInfo( setting->fileName( ) ).absoluteFilePath( );
}
void Setting::clearGroup( ) {
	do {
		if( setting->group( ).isEmpty( ) )
			break;
		setting->endGroup( );
	} while( true );
}


//
//
//
//
//QVariant Setting::getValue( const QAnyStringView &key, bool isSync ) const {
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//	return getValue( key );
//}
//QVariant Setting::getValue( const QAnyStringView &groupName, const QAnyStringView &key, bool isSync ) const {
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//	return getValue( groupName, key );
//}
//QVariant Setting::getValue( const QAnyStringView &groupName, const QAnyStringView &key, const std::function< QVariant( const QVariant & ) > &ifFunction, bool isSync ) const {
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//	return getValue( groupName, key, ifFunction );
//}
//QVariant Setting::getValue( const QAnyStringView &key, const std::function< QVariant( const QVariant & ) > &ifFunction, bool isSync ) const {
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//	return getValue( key, ifFunction );
//}
//void Setting::setValue( const QAnyStringView &key, const QVariant &value, bool isSync ) {
//	setValue( key, value );
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//}
//void Setting::setValue( const QAnyStringView &groupName, const QAnyStringView &key, const QVariant &value, bool isSync ) {
//	setValue( groupName, key, value );
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//}
//void Setting::setValue( const QMap< QAnyStringView, QVariant > &valuePair, bool isSync ) {
//	setValue( valuePair );
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//}
//void Setting::setValue( const QAnyStringView &groupName, const QMap< QAnyStringView, QVariant > &valuePair, bool isSync ) {
//	setValue( groupName, valuePair );
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//}
//QStringList Setting::getAllKey( bool isSync ) const {
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//	return getAllKey( );
//}
//QStringList Setting::getAllKey( const QAnyStringView &group, bool isSync ) const {
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//	return getAllKey( group );
//}
//QList< QVariant > Setting::getAllValue( bool isSync ) const {
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//	return getAllValue( );
//}
//QList< QVariant > Setting::getAllValue( const QAnyStringView &group, bool isSync ) const {
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//	return getAllValue( group );
//}
//QMap< QAnyStringView, QVariant > Setting::getAllInfo( bool isSync ) const {
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//	return getAllInfo( );
//}
//QMap< QAnyStringView, QVariant > Setting::getAllInfo( const QAnyStringView &group, bool isSync ) const {
//	if( isSync ) {
//		QMutexLocker< QMutex > nstanceLock( instanceMutex );
//		setting->sync( );
//	}
//	return getAllInfo( group );
//}
