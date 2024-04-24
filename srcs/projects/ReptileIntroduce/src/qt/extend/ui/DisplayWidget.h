﻿#ifndef DISPLAYWIDGET_H_H_HEAD__FILE__
#define DISPLAYWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
class QMenuBar;
class MenuBar;
class Action;
class Menu;
class DisplayContentWidget;
class Button;
class HLayoutBox;
class VLayoutBox;
class DisplayWidget : public QWidget {

	Q_OBJECT;
public:
	enum Display_Type {
		NORMALE,
		TEXT,
		DB_SQLITE,
		FILE_BIN
	};
private: // 用户界面布局
	HLayoutBox *menuHLayout; // 顶级横向菜单布局
	VLayoutBox *mainVLayout; // 顶级纵向主要布局
private:
	MenuBar *topMenuBar; // 菜单显示栏
	Menu *topMenu; // 菜单
	Action *startGet; // 开始请求
private: // 类成员变量
	QImage *backImage; // 要绘制的内容
	Display_Type currentDisplayType; // 当前绘制类型
private: // 运算参考
	/// <summary>
	/// 顶部大小<br/>
	/// 绘制时，会根据该值来确定开始的 y 轴。
	/// </summary>
	size_t topHeight;
public:
	DisplayWidget( QWidget *parent, Qt::WindowFlags flags = Qt::WindowFlags( ) );
	~DisplayWidget( ) override;
private:
	void initComponent( ); // 初始化对象
	void initProperty( ); // 初始化属性
	void initComponentLayout( ); // 初始化组件布局
	void initConnect( ); // 初始化信号
public:
	Display_Type getDisplay( ) const {
		return currentDisplayType;
	}
protected:
	void paintEvent( QPaintEvent *event ) override;
	void mousePressEvent( QMouseEvent *event ) override;
	void mouseReleaseEvent( QMouseEvent *event ) override;
	void mouseDoubleClickEvent( QMouseEvent *event ) override;
	void mouseMoveEvent( QMouseEvent *event ) override;
	void resizeEvent( QResizeEvent *event ) override;
Q_SIGNALS:
	/// <summary>
	/// 设置显示类型
	/// </summary>
	/// <param name="type">类型</param>
	void setType( Display_Type type );
	/// <summary>
	/// 显示数据
	/// </summary>
	/// <param name="data">数据</param>
	void display( QObject *data );
	/// <summary>
	/// 数据显示完毕
	/// </summary>
	void displayLater( );
	/// <summary>
	/// 数据显示之前
	/// </summary>
	void displayBefore( );
	/// <summary>
	/// 菜单或者选项被点击时发生
	/// </summary>
	/// <param name="send">信号发生者</param>
	/// <param name="menuBar">所在QMenuBar组件</param>
	void menuActionClick( QAction *send, QMenuBar *menuBar );
protected slots: // 响应自身信号的槽
	void native_slots_display( QObject *data );
	void native_slots_setType( Display_Type type );
};


#endif // DISPLAYWIDGET_H_H_HEAD__FILE__
