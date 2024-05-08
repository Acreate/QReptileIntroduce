#ifndef NETWORKACCESSMANAGER_H_H_HEAD__FILE__
#define NETWORKACCESSMANAGER_H_H_HEAD__FILE__
#pragma once
#include "../export/Tools_export.h"
#include <QNetworkAccessManager>

class TOOLS_EXPORT NetworkAccessManager : public QNetworkAccessManager {
	Q_OBJECT;
public:
	NetworkAccessManager( QObject *parent = nullptr );
	~NetworkAccessManager( ) override;
};


#endif // NETWORKACCESSMANAGER_H_H_HEAD__FILE__
