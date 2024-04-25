#include "RequestNetPlugin.h"

#include "../Request/RequestNet.h"
RequestNetPlugin::RequestNetPlugin( QObject *parent ): QGenericPlugin( parent ) {
}
RequestNetPlugin::~RequestNetPlugin( ) {
	qDebug( ) << u8"RequestNetPlugin::~RequestNetPlugin";
}

QObject *RequestNetPlugin::create( const QString &name, const QString &spec ) {
	qDebug( ) << u8"RequestNetPlugin::create( &" << name << ", &" << spec << ")";
	return new RequestNet( this );
}