#ifndef REQUESTNET_H_H_HEAD__FILE__
#define REQUESTNET_H_H_HEAD__FILE__
#pragma once
#include <QGenericPlugin>

#include "IRequestNetInterface.h"
WWW_121DS_CC_EXPORT class RequestNet : public QObject, public IRequestNetInterface {
	Q_OBJECT;
	Q_INTERFACES( IRequestNetInterface )
public:
	RequestNet( QObject *parent = nullptr ): QObject( parent ) {
	}
	int add( const int &left, const int &right ) override;
};

#endif // REQUESTNET_H_H_HEAD__FILE__
