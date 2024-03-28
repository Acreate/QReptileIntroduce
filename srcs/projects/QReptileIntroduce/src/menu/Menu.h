#ifndef MENU_H_H_HEAD__FILE__
#define MENU_H_H_HEAD__FILE__
#pragma once
#include <QMenu>
#include "Action.h"
class Menu : public QMenu {
public:
	Menu( QWidget *parent = nullptr );
	~Menu() override;
public:
	void addAction(Action* action);
};

#endif // MENU_H_H_HEAD__FILE__
