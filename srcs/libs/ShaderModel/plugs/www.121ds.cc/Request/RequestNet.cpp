#include "RequestNet.h"
#include <string>
RequestNet::RequestNet( QObject *parent ): QObject( parent ), url( GET_URL ) {
	qDebug( ) << u8"RequestNet::RequestNet( QObject *parent ) : " << url;
}

RequestNet::~RequestNet( ) {
	qDebug( ) << "RequestNet::~RequestNet( " << IRequestNetInterfaceExtend_iid << ")";

}
size_t RequestNet::getUrl( std::string *outStr ) {
	QByteArray array = url.toLocal8Bit( );
	*outStr = array.toStdString( );
	qDebug( ) << array.data( ) << " RequestNet::getUrl( )[byteArray.data( )]";

	return array.length( );

}

bool RequestNet::setInterfaceParent( void *parent ) {
	qDebug( ) << "RequestNet::setInterfaceParent( void *" << parent << ")";
	QObject *object = reinterpret_cast< QObject * >( parent );
	QObject::setParent( object );
	return true;
}
void RequestNet::deleteMember( ) {
}
