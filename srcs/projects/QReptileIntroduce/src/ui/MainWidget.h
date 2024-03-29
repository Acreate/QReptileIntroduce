#ifndef MAINWIDGET_H_H_HEAD__FILE__
#define MAINWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
#include <QFontMetrics>
#include <QFont>
#include <qtextedit.h>
#include <QLineEdit>
#include <QPushButton>
#include "../menu/Menu.h"
#include <QSettings>
#include <QTranslator>
#include <QTimer>

#include "../layout/HLyaoutBox.h"

class FileThreadResult;
class RWFileThread;
class DateTimeThread;
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
	void resizeEvent( QResizeEvent *event ) override;
private: // 参考变量
	QPoint winCenterPoint; // 记录窗口中间
	QString qstrPoint = u8"坐标:( %1 , %2 )"; // 格式化字符串
	int compoentStrNlen; // 额外增加的横向空间


	QSharedPointer< FileThreadResult > fileThreadResult = nullptr;
	
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
	HLyaoutBox *topLayout;
private: // 菜单
	Menu *toolsMenu;
private: // 程序配置-读写
	QSettings *progressSetting;
	QTranslator *translator;
private: // 程序配置-名称
	const QString transparentForMouseEvents = tr( u8"somponentStyle/TransparentForMouseEvents" );
	const QString downIniTypes = tr( u8"downIni/Types" );
	const QString selectWorkPath = tr( u8"work/Path" );
private: // 程序配置-变量
	QStringList downNovelTypes;
private: // 定时调用
	DateTimeThread *dateTimeThread;
	RWFileThread *fileThread;
public slots:
	void updateDateTimeStrFunction( const QString &currentDateTimeStr );
private:
	/// <summary>
	/// 更新窗口宽度
	/// </summary>
	void updateWidgetWidth( const QList< QString > &list );
};

#endif // MAINWIDGET_H_H_HEAD__FILE__
