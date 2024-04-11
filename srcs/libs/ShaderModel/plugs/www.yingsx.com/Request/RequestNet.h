#ifndef REQUESTNET_H_H_HEAD__FILE__
#define REQUESTNET_H_H_HEAD__FILE__
#pragma once
#include <QGenericPlugin>
#include <interface/IRequestNetInterfaceExtend.h>
#include "../macro/cmake_to_c_cpp_header_macro.h"
Q_DECLARE_INTERFACE( IRequestNetInterfaceExtend, IRequestNetInterfaceExtend_iid );
class RequestNet : public QObject, public IRequestNetInterfaceExtend {
	Q_OBJECT;
	Q_INTERFACES( IRequestNetInterfaceExtend )
private:
	QString url;
public:
	RequestNet( QObject *parent = nullptr );
	~RequestNet( ) override;
public: // 实现虚函数
	size_t getUrl( std::string *outStr ) override;

	bool setInterfaceParent( void *parent ) override;
	void deleteMember( ) override;
public slots:
	IRequestNetInterfaceExtend *getRequestNetInterfaceExtendPatr( ) {
		return this;
	}
};

#endif // REQUESTNET_H_H_HEAD__FILE__
