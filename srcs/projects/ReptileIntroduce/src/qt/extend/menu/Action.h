#ifndef ACTION_H_H_HEAD__FILE__
#define ACTION_H_H_HEAD__FILE__
#pragma once
#include <QAction>
class MenuBar;
class Menu;
class Action : public QAction {
	Q_OBJECT;
public: // 友元
	friend class Menu;
	friend class MenuBar;
private:
	QObject *parentMenu;
public:
	Action( QObject *parent = nullptr );
	~Action( ) override;
public:
	QObject * getParentMenu( ) const {
		return parentMenu;
	}
	QSharedPointer< QString > getActionXPath( ) const;

public: // 静态函数

	/// <summary>
	/// 转换到菜单栏
	/// </summary>
	/// <param name="converObj">转换对象</param>
	/// <returns>失败返回 nullptr</returns>
	static MenuBar * converToMenuBar( QObject *converObj );
	/// <summary>
	/// 转化到菜单
	/// </summary>
	/// <param name="converObj">转换对象</param>
	/// <returns></returns>
	static Menu * converToMenu( QObject *converObj );
};

#endif // ACTION_H_H_HEAD__FILE__
