#ifndef JOBWIDGET_H_H_HEAD__FILE__
#define JOBWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
#include <QProgressBar>
class QTextEdit;
class VLayoutBox;
class Button;
class HLayoutBox;
class JobWidget : public QWidget {
	Q_OBJECT;
public:
	JobWidget( QWidget *parent );
	~JobWidget( ) override;

protected:
	/// <summary>
	/// 在内存中初始化组件
	/// </summary>
	void initCompoentObjMemory( );
	/// <summary>
	/// 设置组件属性
	/// </summary>
	void initComponentProperty( );
	/// <summary>
	/// 设置组件的布局
	/// </summary>
	void initComponentLayout( );
	/// <summary>
	/// 设置组件信号槽
	/// </summary>
	void initComponentConnect( );
	/// <summary>
	/// 组件设置完成
	/// </summary>
	void initComponentOver( );
public: // 布局
	VLayoutBox *mainLayout;
	HLayoutBox *subLayout;
	Button *button;
	QProgressBar *progressBar;
	QTextEdit *edit;
Q_SIGNALS:
	/// <summary>
	/// 点击了按钮
	/// </summary>
	void click( bool flage );
	/// <summary>
	/// 设置值
	/// </summary>
	/// <param name="value"></param>
	void setProgressValue( int value );
};

#endif // JOBWIDGET_H_H_HEAD__FILE__
