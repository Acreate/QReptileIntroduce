#ifndef REQUESTNET_H_H_HEAD__FILE__
#define REQUESTNET_H_H_HEAD__FILE__
#pragma once
#include <QGenericPlugin>
#include <interface/IRequestNetInterfaceExtend.h>
#include "../macro/cmake_to_c_cpp_header_macro.h"
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
