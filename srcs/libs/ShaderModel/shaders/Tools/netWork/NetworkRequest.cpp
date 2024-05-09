#include "NetworkRequest.h"
#include <QObject>

NetworkRequest::NetworkRequest( ) {
}
NetworkRequest::~NetworkRequest( ) {
}

QStringList NetworkRequest::userAgentHeaders = {
	QObject::tr( u8"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.5735.289 Safari/537.36 QIHU 360SE" )
	, QObject::tr( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/68.0.3440.106 Safari/537.36" )
	, QObject::tr( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/67.0.3396.99 Safari/537.36" )
	, QObject::tr( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.186 Safari/537.36" )
	, QObject::tr( "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.62 Safari/537.36" )
	, QObject::tr( "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.101 Safari/537.36" )
	, QObject::tr( "Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.0)" )
	, QObject::tr( "Mozilla/5.0 (Macintosh; U; PPC Mac OS X 10.5; en-US; rv:1.9.2.15) Gecko/20110303 Firefox/3.6.15" )
	, QObject::tr( "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.108 Safari/537.36" )
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
