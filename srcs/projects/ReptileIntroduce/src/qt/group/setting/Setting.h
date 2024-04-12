#ifndef SETTING_H_H_HEAD__FILE__
#define SETTING_H_H_HEAD__FILE__
#pragma once

#include <QSettings>
#include <qmutex.h>
class Setting : public QObject {
	Q_OBJECT;
private:
	QSettings *setting;
	QMutex *instanceMutex;
public: // 构造函数
	Setting( const QString &filePath, QObject *parent );
	~Setting( ) override;
public: // 配置文件的值操作
	QVariant getValue( const QAnyStringView &key ) const;
	QVariant getValue( const QAnyStringView &groupName, const QAnyStringView &key ) const;
	QVariant getValue( const QAnyStringView &groupName, const QAnyStringView &key, const std::function< QVariant( const QVariant & ) > &ifFunction ) const;
	QVariant getValue( const QAnyStringView &key, const std::function< QVariant( const QVariant & ) > &ifFunction ) const;
	void setValue( const QAnyStringView &key, const QVariant &value );
	void setValue( const QAnyStringView &groupName, const QAnyStringView &key, const QVariant &value );
	void setValue( const QMap< QAnyStringView, QVariant > &valuePair );
	void setValue( const QAnyStringView &groupName, const QMap< QAnyStringView, QVariant > &valuePair );
	QStringList getAllKey( ) const;
	QStringList getAllKey( const QAnyStringView &group ) const;
	QList< QVariant > getAllValue( ) const;
	QList< QVariant > getAllValue( const QAnyStringView &group ) const;
	QMap< QAnyStringView, QVariant > getAllInfo( ) const;
	QMap< QAnyStringView, QVariant > getAllInfo( const QAnyStringView &group ) const;

	void sync( );
public:
	void setFilePath( const QString &filePath );
	QString getFilePath( ) const;
private:
	void clearGroup( );
};

#endif // SETTING_H_H_HEAD__FILE__
