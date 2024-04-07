#include "IRequestNetInterface.h"
IRequestNetInterface::IRequestNetInterface( QObject *parent ) : QObject( parent ) {
}
IRequestNetInterface::~IRequestNetInterface( ) {
	qDebug( ) << u8"IRequestNetInterface::~IRequestNetInterface( )";
}
