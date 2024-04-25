#ifndef MENUBAR_H_H_HEAD__FILE__
#define MENUBAR_H_H_HEAD__FILE__
#pragma once
#include <QMenuBar>
class Action;
class Menu;
class MenuBar : public QMenuBar {
	Q_OBJECT;
public: // 友元
	friend class Action;
	friend class Menu;
private:
	QObject *parentMenu;
public:
	MenuBar( QWidget *parent );
	~MenuBar( ) override;
public:
	void addAction( Action *action );
	void addMenu( Menu *menu );
public:
	QObject * getParentMenu( ) {
		return parentMenu;
	}
Q_SIGNALS:
	/// <summary>
	/// 点击
	/// </summary>
	void click( Action *clickAction );
};


#endif // MENUBAR_H_H_HEAD__FILE__
