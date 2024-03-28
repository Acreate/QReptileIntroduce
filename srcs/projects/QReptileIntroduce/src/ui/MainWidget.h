#ifndef MAINWIDGET_H_H_HEAD__FILE__
#define MAINWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
#include <QFontMetrics>
#include <QFont>
#include <QVBoxLayout>
#include <qtextedit.h>
#include <QLineEdit>
#include <QPushButton>
#include "../menu/Menu.h"
#include <QSettings>

class MainWidget : public QWidget {
	Q_OBJECT;
public:
	MainWidget( QWidget *parent = nullptr, Qt::WindowFlags fg = Qt::WindowFlags( ) );
	~MainWidget( ) override;
public:
	virtual QFont setFont( QFont &font );
protected:
	void mouseMoveEvent( QMouseEvent *event ) override;
	void mouseReleaseEvent( QMouseEvent *event ) override;
private: // 参考变量
	QPoint winCenterPoint; // 记录窗口中间
	QString qstrPoint = u8"坐标:( %1 , %2 )"; // 格式化字符串
private: // 计算变量
	QPoint mousePoint; // 当前鼠标位置
	QFont currentFont; // 当前字体
	QFontMetrics currentFontMetrics; // 当前字体计算器
	QString drawStr; // 当前绘制字体
	QPoint drawPoint; // 当前绘制坐标
	QColor drawColor; // 当前绘制颜色
private: // 组件
	QTextEdit *textComponent;
	QLineEdit *textLine;
	QPushButton *converTransparentForMouseEventsBtn;
private: // 布局
	QVBoxLayout *mainLayout;
	QHBoxLayout *topLayout;
private: // 菜单
	Menu *toolsMenu;
private: // 程序配置
	QSettings *progressSetting;
	const QString transparentForMouseEvents = "somponentStyle/TransparentForMouseEvents";
	const QString downIniTypes = "downIni/Types";
};

#endif // MAINWIDGET_H_H_HEAD__FILE__
