﻿#ifndef REQUESTNETPLUGIN_H_H_HEAD__FILE__
#define REQUESTNETPLUGIN_H_H_HEAD__FILE__
#pragma once

#include <QGenericPlugin>
#include "Request/RequestNet.h"

#include "export/Www.121ds.cc_export.h"
WWW_121DS_CC_EXPORT class RequestNetPlugin : public QGenericPlugin{
	Q_OBJECT;
	Q_PLUGIN_METADATA( IID QGenericPluginFactoryInterface_iid FILE "www_121ds_cc.json" );
public:
	explicit RequestNetPlugin( QObject *parent = nullptr );
	~RequestNetPlugin( ) override;
private:
	QObject *create( const QString &name, const QString &spec ) override;
};

#endif // REQUESTNETPLUGIN_H_H_HEAD__FILE__
