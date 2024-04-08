#include "RequestNet.h"

RequestNet::~RequestNet( ) {
}
std::shared_ptr< void > RequestNet::getData( ) {
	void *string = new QString( u8"RequestNet::getData" );
	std::shared_ptr< void > result( string, [=]( void *p ) {
		QString *msg = ( QString * )p;
		qDebug( ) << "delete = " << this << ", info(" << msg << "):" << *msg;
		delete msg;
	} );
	return result;
}
QUrl RequestNet::getUrl( ) {
	return QUrl( "https://www.ishuquge.net/" );
}
QMap< QString, QUrl > RequestNet::getTypeUrls( const QNetworkReply &&networkReply ) {
	return { };
}
IRequestNetInterface::NovelPtrList RequestNet::getTypePageNovels( const QNetworkReply &&networkReply, const NovelPtrList &&saveNovelInfos, void *appendDataPtr ) {
	return { };
}
IRequestNetInterface::INovelInfoSharedPtr RequestNet::getUrlNovelInfo( const QNetworkReply &&networkReply, const NovelPtrList &&saveNovelInfos, const INovelInfoSharedPtr &&networkReplayNovel ) {
	return { };
}

QUrl RequestNet::getNext( const QNetworkReply &&networkReply, const NovelPtrList &&saveNovelInfos, const NovelPtrList &&lastNovelInfos ) {
	return { };
}
void RequestNet::novelTypeEnd( const NovelPtrList &&saveNovelInfos ) {
}
void RequestNet::endHost( const NovelPtrList &&saveNovelInfos ) {
}
