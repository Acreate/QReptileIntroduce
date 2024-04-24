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
	setting = nullptr;
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
	if( setting )
		return setting->value( key );
	return QVariant( );
}
QVariant Setting::getValue( const QAnyStringView &groupName, const QAnyStringView &key ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	if( setting ) {
		setting->beginGroup( groupName );
		auto result = setting->value( key );
		setting->endGroup( );
		return result;
	}
	return QVariant( );

}
QVariant Setting::getValue( const QAnyStringView &key, const std::function< QVariant( const QVariant & ) > &ifFunction ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	if( setting )
		return ifFunction( setting->value( key ) );
	return QVariant( );
}
QVariant Setting::getValue( const QAnyStringView &groupName, const QAnyStringView &key, const std::function< QVariant( const QVariant & ) > &ifFunction ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	if( setting ) {
		setting->beginGroup( groupName );
		auto result = setting->value( key );
		setting->endGroup( );
		return ifFunction( result );
	}
	return QVariant( );

}
bool Setting::setValue( const QAnyStringView &key, const QVariant &value ) {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	if( setting ) {
		setting->setValue( key, value );
		return true;
	}
	return false;
}
bool Setting::setValue( const QAnyStringView &groupName, const QAnyStringView &key, const QVariant &value ) {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	if( setting ) {
		setting->beginGroup( groupName );
		setting->setValue( key, value );
		setting->endGroup( );
		return true;
	}
	return false;

}
bool Setting::setValue( const QMap< QAnyStringView, QVariant > &valuePair ) {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	if( setting ) {
		auto iterator = valuePair.begin( );
		auto endIterator = valuePair.end( );
		for( ; iterator != endIterator; ++iterator )
			setting->setValue( iterator.key( ), iterator.value( ) );
		return true;
	}
	return false;
}
bool Setting::setValue( const QAnyStringView &groupName, const QMap< QAnyStringView, QVariant > &valuePair ) {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	if( setting ) {
		auto iterator = valuePair.begin( );
		auto endIterator = valuePair.end( );
		setting->beginGroup( groupName );
		for( ; iterator != endIterator; ++iterator )
			setting->setValue( iterator.key( ), iterator.value( ) );
		setting->endGroup( );
		return true;
	}
	return false;
}
QStringList Setting::getAllKey( ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	if( setting )
		return setting->allKeys( );
	return QStringList( );
}
QStringList Setting::getAllKey( const QAnyStringView &group ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	if( setting ) {
		setting->beginGroup( group );
		auto allKey = setting->allKeys( );
		setting->endGroup( );
		return allKey;
	}
	return QStringList( );

}
QList< QVariant > Setting::getAllValue( ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	QList< QVariant > result;
	if( setting ) {
		auto allKey = setting->allKeys( );
		for( auto &key : allKey )
			result.append( setting->value( key ) );
	}
	return result;
}
QList< QVariant > Setting::getAllValue( const QAnyStringView &group ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	QList< QVariant > result;
	if( setting ) {
		setting->beginGroup( group );
		auto allKey = setting->allKeys( );
		for( auto &key : allKey )
			result.append( setting->value( key ) );
		setting->endGroup( );
	}
	return result;
}
QMap< QAnyStringView, QVariant > Setting::getAllInfo( ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	QMap< QAnyStringView, QVariant > result;
	if( setting ) {
		auto allKey = setting->allKeys( );
		for( auto &key : allKey )
			result.insert( key, setting->value( key ) );
	}
	return result;
}
QMap< QAnyStringView, QVariant > Setting::getAllInfo( const QAnyStringView &group ) const {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	QMap< QAnyStringView, QVariant > result;
	if( setting ) {
		setting->beginGroup( group );
		auto allKey = setting->allKeys( );
		for( auto &key : allKey )
			result.insert( key, setting->value( key ) );
		setting->endGroup( );
	}
	return result;

}
bool Setting::sync( ) {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	if( setting ) {
		setting->sync( );
		return true;
	}
	return false;
}
bool Setting::setFilePath( const QString &filePath ) {
	QMutexLocker< QMutex > nstanceLock( instanceMutex );
	QFileInfo fileInfo( filePath );
	if( fileInfo.exists( ) ) {
		if( setting ) {
			QString fileName = setting->fileName( );
			auto absoluteFilePath = fileInfo.absoluteFilePath( );
			fileInfo.setFile( fileName );
			if( absoluteFilePath == fileInfo.absoluteFilePath( ) )
				return false;
			setting->sync( );
			setting->deleteLater( );
			setting = new QSettings( absoluteFilePath, QSettings::IniFormat, this );
		} else
			setting = new QSettings( filePath, QSettings::IniFormat );
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
