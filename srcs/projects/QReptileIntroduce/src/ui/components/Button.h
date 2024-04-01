#ifndef BUTTON_H_H_HEAD__FILE__
#define BUTTON_H_H_HEAD__FILE__
#pragma once
#include <QPushButton>
class Button : public QPushButton {
	Q_OBJECT;
public:
	Button( const QString &text, QWidget *parent = nullptr );
	Button( const QIcon &icon, const QString &text, QWidget *parent );
	Button( QWidget *parent = nullptr );
	~Button( ) override;
};

#endif // BUTTON_H_H_HEAD__FILE__
