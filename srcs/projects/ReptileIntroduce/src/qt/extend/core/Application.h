#ifndef APPLICTION_H_H_HEAD__FILE__
#define APPLICTION_H_H_HEAD__FILE__
#pragma once
#include <QApplication>

class Application : public QApplication {
	Q_OBJECT;
private:
	QStringList catchEventAtClassNameList;
	QVector< QObject * > catchEventAtObjectPtrList;
public:
	Application( int &argc, char **argv, int i = ApplicationFlags );
	~Application( ) override;
public:
	bool notify( QObject *, QEvent * ) override;
public:
	void appendNotifyByObjPtr( QObject *object ) {
		if( !catchEventAtObjectPtrList.contains( object ) )
			catchEventAtObjectPtrList.append( object );
	}
	void appendNotifyByObjPtr( const QVector< QObject * > &objects ) {
		for( auto &obj : objects )
			if( !catchEventAtObjectPtrList.contains( obj ) )
				catchEventAtObjectPtrList.append( obj );
	}
	void appendNotifyByName( const char *c_className ) {
		QString className( c_className );
		if( !catchEventAtClassNameList.contains( className ) )
			catchEventAtClassNameList.append( className );
	}
	void appendNotifyByName( const QString &className ) {
		if( !catchEventAtClassNameList.contains( className ) )
			catchEventAtClassNameList.append( className );
	}
	void appendNotifyByName( const QVector< QString > &classNames ) {
		for( auto &className : classNames )
			if( !catchEventAtClassNameList.contains( className ) )
				catchEventAtClassNameList.append( className );
	}

};

#endif // APPLICTION_H_H_HEAD__FILE__
