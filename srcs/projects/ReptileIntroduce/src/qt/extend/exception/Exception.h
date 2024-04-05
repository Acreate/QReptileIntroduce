﻿#ifndef EXCEPTION_H_H_HEAD__FILE__
#define EXCEPTION_H_H_HEAD__FILE__
#pragma once
#include <QException>
class Exception : public QException {
private:
	QString msg;
public:
	Exception( ) {
	}
	Exception( const QString &msg ): msg( msg ) {

	}
	void raise( ) const override;
	QException *clone( ) const override;
	const QString &getMsg( ) const {
		return msg;
	}
	void setMsg(const QString& newMsg) {
		msg = newMsg;
	}
};

#endif // EXCEPTION_H_H_HEAD__FILE__