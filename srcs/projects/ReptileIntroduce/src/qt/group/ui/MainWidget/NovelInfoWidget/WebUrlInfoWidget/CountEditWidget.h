#ifndef COUNTEDITWIDGET_H_H_HEAD__FILE__
#define COUNTEDITWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
class VLayoutBox;
class HLayoutBox;
class Button;
class EditLine;
class CountEditWidget : public QWidget {
	Q_OBJECT;
public:
	CountEditWidget( QWidget *parent, Qt::WindowFlags flags = Qt::WindowFlags( ) );
	CountEditWidget( );
public:
	long long getValue( );
	void enClickSub( );
	void enClickAdd( );
	bool setValue( long long value );
private: // 布局
	VLayoutBox *btnLayout;
	HLayoutBox *mainLayout;
private: // 组件
	EditLine *allCount; // 输入
	QWidget *btnWidget; // 按钮容器
	Button *addBtn; // 添加计数
	Button *subBtn; // 减去计数
private: // 正则匹配
	QRegularExpression *exp;
private:
	static const QString defaultCount; // 默认的值
	static const QString addBtnText; // 加
	static const QString subBtnText; // 减
private:
	/// <summary>
	/// 初始化组件对象
	/// </summary>
	void initComponents( );
	/// <summary>
	/// 初始化组件属性
	/// </summary>
	void initComponentPropertys( );
	/// <summary>
	/// 组件布局初始化
	/// </summary>
	void initComponentLayouts( );
	/// <summary>
	/// 初始化组件显示文本
	/// </summary>
	void initComponentTexts( );
	/// <summary>
	/// 初始化该实例对象
	/// </summary>
	void initInstance( );
	/// <summary>
	/// 初始化信号
	/// </summary>
	void initComponentConnect( );
Q_SIGNALS: // 信号
	/// <summary>
	/// 编辑完成发送信号
	/// </summary>
	/// <param name="value">文本内容</param>
	void changeValueFinish( const long long &value );
	/// <summary>
	/// 正在改变
	/// </summary>
	/// <param name="value">文本内容</param>
	void changeingValue( const long long &value );
	/// <summary>
	/// 点击了减号
	/// </summary>
	void clickSubBtn( );
	/// <summary>
	/// 点击了加号
	/// </summary>
	void clickAddBtn( );
};

#endif // COUNTEDITWIDGET_H_H_HEAD__FILE__
