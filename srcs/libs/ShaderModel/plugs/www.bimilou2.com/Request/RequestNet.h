#ifndef REQUESTNET_H_H_HEAD__FILE__
#define REQUESTNET_H_H_HEAD__FILE__
#pragma once
#include <QGenericPlugin>
#include <interface/IRequestNetInterfaceExtend.h>

#define IRequestNetInterfaceExtend_iid "www.bimilou2.com.json.IRequestNetInterfaceExtend"
Q_DECLARE_INTERFACE( IRequestNetInterfaceExtend, IRequestNetInterfaceExtend_iid );

class RequestNet : public IRequestNetInterfaceExtend {
	Q_OBJECT;
	Q_INTERFACES( IRequestNetInterfaceExtend )
public:
	RequestNet( QObject *parent = nullptr ): IRequestNetInterfaceExtend( parent ) {
	}
	~RequestNet( ) override;
public: // 实现虚函数
	QUrl getUrl( ) override;

};

#endif // REQUESTNET_H_H_HEAD__FILE__
