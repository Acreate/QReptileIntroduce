#ifndef REQUESTNET_H_H_HEAD__FILE__
#define REQUESTNET_H_H_HEAD__FILE__
#pragma once
#include <QGenericPlugin>

#include "IRequestNetInterface.h"
class WWW_121DS_CC_EXPORT RequestNet : public QObject, public IRequestNetInterface {
	Q_OBJECT;
	Q_INTERFACES( IRequestNetInterface )
public:
	RequestNet( QObject *parent = nullptr ): QObject( parent ) {
	}
	QUrl getUrl( ) override;
	QMap< QString, QUrl > getTypeUrls( ) override;
};

#endif // REQUESTNET_H_H_HEAD__FILE__
