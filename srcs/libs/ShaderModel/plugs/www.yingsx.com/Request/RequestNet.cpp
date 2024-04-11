#include "RequestNet.h"

RequestNet::~RequestNet( ) {
	qDebug( ) << "RequestNet::~RequestNet( " << IRequestNetInterfaceExtend_iid << ")";
}

QUrl RequestNet::getUrl( ) {
	return QUrl( GET_URL );
}
