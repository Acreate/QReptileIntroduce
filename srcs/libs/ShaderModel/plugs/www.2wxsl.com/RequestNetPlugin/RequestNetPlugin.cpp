#include "RequestNetPlugin.h"

#include "../RequestNet/RequestNet.h"
RequestNetPlugin::RequestNetPlugin( QObject *parent ): QGenericPlugin( parent ) {
}
RequestNetPlugin::~RequestNetPlugin( ) {
}

QObject *RequestNetPlugin::create( const QString &name, const QString &spec ) {
	return new RequestNet( this );
}