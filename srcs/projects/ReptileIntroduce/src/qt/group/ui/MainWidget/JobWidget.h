#ifndef JOBWIDGET_H_H_HEAD__FILE__
#define JOBWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
#include <QProgressBar>
class VLayoutBox;
class Button;
class HLayoutBox;
class JobWidget : public QWidget {
	Q_OBJECT;
public:
	JobWidget( QWidget *parent );
	~JobWidget( ) override;
public: // 布局
	VLayoutBox *mainLayout;
	HLayoutBox *subLayout;
	Button *button;
	QProgressBar *progressBar;
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
