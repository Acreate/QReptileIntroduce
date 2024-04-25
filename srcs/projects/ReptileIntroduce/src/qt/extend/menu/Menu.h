#ifndef MENU_H_H_HEAD__FILE__
#define MENU_H_H_HEAD__FILE__
#pragma once
#include <QMenu>
class Action;
class Menu : public QMenu {
	Q_OBJECT;
public: // 友元
	friend class Action;
	friend class MenuBar;
private:
	QObject *parentMenu;
public:
	Menu( QWidget *parent = nullptr );
	~Menu( ) override;
public:
	void addAction( Action *action );
	void addMenu( Menu *menu );
public:
	QObject * getParentMenu( ) const {
		return parentMenu;
	}
Q_SIGNALS:
	/// <summary>
	/// 点击
	/// </summary>
	void click( Action *clickAction );
};

#endif // MENU_H_H_HEAD__FILE__
