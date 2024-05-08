#include "NetworkRequest.h"
#include <QObject>

NetworkRequest::NetworkRequest( ) {
}
NetworkRequest::~NetworkRequest( ) {
}

QStringList NetworkRequest::userAgentHeaders = {
	QObject::tr( u8"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.5735.289 Safari/537.36 QIHU 360SE" )
	};

QStringList NetworkRequest::getUserAgentHeaders( ) {
	return userAgentHeaders;
}
QString NetworkRequest::getRandomUserAgentHeader( ) {
	srand( time( 0 ) );
	qint64 size = abs( userAgentHeaders.size( ) - 1 );
	if( size > 0 )
		return userAgentHeaders[ rand( ) % size + 0.5 ];
	return userAgentHeaders[ 0 ];
}
