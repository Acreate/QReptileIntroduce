#ifndef IREQUESTNETINTERFACE_H_H_HEAD__FILE__
#define IREQUESTNETINTERFACE_H_H_HEAD__FILE__

#pragma once

#include <QGenericPlugin>

#include "../export/Www.121ds.cc_export.h"
class WWW_121DS_CC_EXPORT IRequestNetInterface {
public:
	explicit IRequestNetInterface( );
	virtual ~IRequestNetInterface( );
public:
	virtual QUrl getUrl( ) = 0;
	virtual QMap< QString, QUrl > getTypeUrls( ) = 0;
};
#define RequestNet_iid "www.121ds.cc.json.IRequestNetInterface"
Q_DECLARE_INTERFACE( IRequestNetInterface, QGenericPluginFactoryInterface_iid );

#endif // IREQUESTNETINTERFACE_H_H_HEAD__FILE__
