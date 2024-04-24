#include "LoadPlug.h"

#include <interface/IRequestNetInterfaceExtend.h>
#include <QPluginLoader>

#include "path/Path.h"

LoadPlug::LoadPlug( const QString &path, QObject *parent ): QObject( parent ), plugPath( path ) {
}
LoadPlug::~LoadPlug( ) {
}
bool LoadPlug::findLib( const QString &path, std::pair< QObject *, IRequestNetInterfaceExtend * > *result ) {
	QFileInfo fileInfo( path );
	QString absolutePath = fileInfo.absolutePath( );
	if( lib.contains( absolutePath ) ) {
		*result = lib[ absolutePath ];
		return true;
	}
	return false;
}
QMap< QObject *, IRequestNetInterfaceExtend * > LoadPlug::loadPlugs( ) {
	QMap< QObject *, IRequestNetInterfaceExtend * > result;
	auto pathInfo = Path::getPathInfo( plugPath );
	auto files = pathInfo.second;
	if( files.size( ) == 0 )
		return result;
	for( auto &filePath : files ) {
		if( lib.contains( filePath ) ) {
			auto interface = lib[ filePath ];
			result.insert( interface.first, interface.second );
		} else {
			auto interface = LoadPlug::getIRequestNetInterface( filePath.getCurrentFilePtah( ), name, spec, loadClassName, methodName );
			if( interface.second != nullptr ) {
				result.insert( interface.first, interface.second );
				lib.insert( filePath, interface );
			}
		}

	}
	return result;
}


IRequestNetInterfaceExtend * LoadPlug::metaGetResult(
	QObject *outObj, const QString &loadClassName, const QString &methodName
) {
	const QMetaObject *metaObject = outObj->metaObject( );
	if( metaObject->className( ) == loadClassName ) {
		IRequestNetInterfaceExtend *res = nullptr;
		bool invokeMethod = metaObject->invokeMethod( outObj, methodName, Qt::DirectConnection, Q_RETURN_ARG( IRequestNetInterfaceExtend *, res ) );
		if( invokeMethod )
			return res;
	}
	return nullptr;
}
std::pair< QObject *, IRequestNetInterfaceExtend * > LoadPlug::getIRequestNetInterface(
	const QString &plugFilePath, const QString &name, const QString &spec, const QString &loadClassName, const QString &methodName
) {
	QPluginLoader loader( plugFilePath );
	if( loader.load( ) ) {
		QObject *instance = loader.instance( );
		QGenericPlugin *genericPlugin = qobject_cast< QGenericPlugin * >( instance );
		if( genericPlugin ) {
			QObject *object = genericPlugin->create( name, spec );
			IRequestNetInterfaceExtend *requestNetInterface = metaGetResult( object, loadClassName, methodName );
			if( requestNetInterface ) {
				requestNetInterface->setInterfaceParent( nullptr );
				return { object, requestNetInterface };
			}
		}
	}
	return { };
}
