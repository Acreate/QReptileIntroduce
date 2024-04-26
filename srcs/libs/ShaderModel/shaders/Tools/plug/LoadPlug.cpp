#include "LoadPlug.h"

#include <interface/IRequestNetInterfaceExtend.h>
#include <QPluginLoader>

#include "../path/Path.h"


LoadPlug::LoadPlug( const QString &path, QObject *parent )
	: QObject( parent ), plugPath( QFileInfo( path ).absoluteFilePath( ) ) {
}
LoadPlug::~LoadPlug( ) {
	auto iterator = lib.begin( );
	auto end = lib.end( );
	for( ; iterator != end; ++iterator ) {
		QObject *object = iterator.value( ).first;
		if( object->parent( ) == nullptr )
			object->deleteLater( );
	}
}
bool LoadPlug::findLib( const QString &path, std::pair< QObject *, IRequestNetInterfaceExtend * > *result ) {
	QFileInfo fileInfo( path );
	QString absolutePath = fileInfo.absoluteFilePath( );
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
		auto absolutePath = filePath.getCurrentFilePtah( );
		if( lib.contains( absolutePath ) ) {
			auto pair = lib[ absolutePath ];
			result.insert( pair.first, pair.second );
		} else {
			auto interface = LoadPlug::getIRequestNetInterface( absolutePath, name, spec, loadClassName, methodName );
			if( interface.second != nullptr ) {
				result.insert( interface.first, interface.second );
				lib.insert( absolutePath, interface );
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
		QByteArray local8Bit = methodName.toLocal8Bit( );
		std::string stdString = local8Bit.toStdString( );
		const char *cStr = stdString.c_str( );
		bool invokeMethod = metaObject->invokeMethod( outObj, cStr, Qt::DirectConnection, Q_RETURN_ARG( IRequestNetInterfaceExtend *, res ) );
		if( invokeMethod )
			return res;
	}
	return nullptr;
}
std::pair< QObject *, IRequestNetInterfaceExtend * > LoadPlug::getIRequestNetInterface(
	const QString &plugFilePath, const QString &name, const QString &spec, const QString &loadClassName, const QString &methodName
) {
	QPluginLoader loader( plugFilePath );
	QObject *instance = loader.instance( );
	if( instance ) {
		QGenericPlugin *genericPlugin = qobject_cast< QGenericPlugin * >( instance );
		if( genericPlugin ) {
			QObject *object = genericPlugin->create( name, spec );
			IRequestNetInterfaceExtend *requestNetInterface = metaGetResult( object, loadClassName, methodName );
			if( requestNetInterface ) {
				requestNetInterface->setInterfaceParent( nullptr );
				return { object, requestNetInterface };
			}
		}
		instance->deleteLater( );
		loader.unload( );
	}
	return { };
}
