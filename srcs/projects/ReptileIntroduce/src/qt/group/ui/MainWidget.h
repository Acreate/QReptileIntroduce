#ifndef MAINWIDGET_H_H_HEAD__FILE__
#define MAINWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
#include <QFontMetrics>
#include <QFont>
#include <QVector>
#include <qtextedit.h>
#include <QLineEdit>
#include <QPushButton>

#include <QSettings>
#include <QTranslator>
#include <QFileDevice>
#include <QTimer>

class Menu;
class HLayoutBox;
class WebUrlInfoWidget;
class VLayoutBox;
class QFile;
class Request;
class FileResult;
class RWFileThread;
class DateTimeThread;
class RequestConnect;
class NovelInfoWidget;
class MainWidget : public QWidget {
	Q_OBJECT;
public:
	MainWidget( QWidget *parent = nullptr, Qt::WindowFlags fg = Qt::WindowFlags( ) );
	~MainWidget( ) override;
public:
	virtual QFont setFont( QFont &font );
protected: // 事件
	void mouseMoveEvent( QMouseEvent *event ) override;
	void mouseReleaseEvent( QMouseEvent *event ) override;
	void mousePressEvent( QMouseEvent *event ) override;
	void resizeEvent( QResizeEvent *event ) override;
private: // 参考变量 - 只读
	static const QString qstrPoint; // 格式化字符串
private: // 计算变量
	QPoint mousePoint; // 当前鼠标位置
	QFont currentFont; // 当前字体
	QFontMetrics currentFontMetrics; // 当前字体计算器
	QString drawStr; // 当前绘制字体
	QPoint drawPoint; // 当前绘制坐标
	QColor drawColor; // 当前绘制颜色
	int titleHeight; // 标题高度
	qsizetype showCount = 0; // 菜单显示计数
	int compoentStrNlen; // 额外增加的横向空间
	QPoint winCenterPoint; // 记录窗口中间
private: // 组件
	NovelInfoWidget *novelComponent; // 小说组件
	QLineEdit *textLine; // 显示时间
	QPushButton *converTransparentForMouseEventsBtn; // 切换与鼠标点击事件
private: // 布局
	VLayoutBox *mainLayout;
	HLayoutBox *topLayout;
private: // 菜单
	Menu *toolsMenu;
private: // 程序配置-读写
	QSettings *progressSetting;
	QTranslator *translator;
	RWFileThread *rwFileThread;
	FileResult *fileThreadResult = nullptr; // 文件读取绑定指针
private: // 程序配置-名称
	static const QString transparentForMouseEvents;
	static const QString selectReadFileWorkPath;
	static const QString selectWriteFileWorkPath;
	static const QString selectWebSettingPath;// 配置文件关键字
private: // 文字提示
	static const QString transparentText;
	static const QString transparentTextNot;
private: // 程序配置-变量
	QStringList downNovelTypes;
private: // 定时调用
	DateTimeThread *dateTimeThread;
public slots: // 窗口子控件信号响应
	void updateDateTimeStrFunction( const QString &currentDateTimeStr );
	void changeTransparent( bool flage );
	void error( int errorType, QFileDevice::FileError fileErrorCode, QFileDevice::FileError dirError );
	void changeWebComponents( );
private:
	/// <summary>
	/// 更新窗口宽度
	/// </summary>
	void updateWidgetWidth( const QList< QString > &list );
Q_SIGNALS:
	/// <summary>
	/// 当关联的 web 配置文件同步到该实例对象时候会触发该行为
	/// </summary>
	void changeOverWebComponentSetting( );
};

#endif // MAINWIDGET_H_H_HEAD__FILE__
