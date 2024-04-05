#ifndef IREQUESTNETINTERFACE_H_H_HEAD__FILE__
#define IREQUESTNETINTERFACE_H_H_HEAD__FILE__

#pragma once

#include <QGenericPlugin>
#include "../export/Www.121ds.cc_export.h"
WWW_121DS_CC_EXPORT class IRequestNetInterface {
public:
	explicit IRequestNetInterface( );
	virtual ~IRequestNetInterface( );
public:
	virtual int add( const int &left, const int &right ) = 0;
};
#define RequestNet_iid "www_121ds_cc.IRequestNetInterface"
Q_DECLARE_INTERFACE( IRequestNetInterface, QGenericPluginFactoryInterface_iid );

#endif // IREQUESTNETINTERFACE_H_H_HEAD__FILE__
