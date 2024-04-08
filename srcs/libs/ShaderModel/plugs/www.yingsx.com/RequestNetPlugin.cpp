#include "RequestNetPlugin.h"

#include "Request/RequestNet.h"
RequestNetPlugin::RequestNetPlugin( QObject *parent ): QGenericPlugin( parent ) {
}
RequestNetPlugin::~RequestNetPlugin( ) {
	qDebug( ) << u8"RequestNetPlugin::~RequestNetPlugin";
}

QObject *RequestNetPlugin::create( const QString &name, const QString &spec ) {
	qDebug( ) << u8"RequestNetPlugin::create( const QString &name, const QString &spec ):\n\tname:\n\t\t" << name << "\nspec:\n\t\t" << spec;
	return new RequestNet( this );
}
