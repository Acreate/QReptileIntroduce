#ifndef MAINWIDGET_H_H_HEAD__FILE__
#define MAINWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>

class DisplayWidget;
class FileSelectPathWidget;
class QLabel;
class Setting;
class JobWidget;
class Menu;
class Action;
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
class QTranslator;
class MainWidget : public QWidget {
	Q_OBJECT;;
private:
	Setting *progressSetting; // 配置文件
	QTranslator *translator; // 语言本地化
private: // 布局组件
	VLayoutBox * mainVLayoutBox;
private: // 显示组件
	FileSelectPathWidget* selectPathWidget; // 路径选择组件
	DisplayWidget* display; // 信息显示组件
public:
	MainWidget( QWidget *parent = nullptr, Qt::WindowFlags fg = Qt::WindowFlags( ) );
	~MainWidget( ) override;
protected: // 事件
	void mouseMoveEvent( QMouseEvent *event ) override;
	void mouseReleaseEvent( QMouseEvent *event ) override;
	void mousePressEvent( QMouseEvent *event ) override;
	void resizeEvent( QResizeEvent *event ) override;
protected:
	/// <summary>
	/// 初始化组件的指针对象
	/// </summary>
	void initMumberPtrMemory( );
	/// <summary>
	/// 初始化组件的属性
	/// </summary>
	void initComponentPropertys( );
	/// <summary>
	/// 初始化组件的布局
	/// </summary>
	void initComponentLayout( );
	/// <summary>
	/// 初始化组件的信号链接
	/// </summary>
	void initComponentConnect( );
	/// <summary>
	/// 实现组件的完成结尾
	/// </summary>
	void initComponentOver( );
private: // 参考变量 - 只读
	static const QString qstrPoint; // 格式化字符串

private: // 程序配置-名称
	static const QString selectReadFileWorkPath;
	static const QString selectWriteFileWorkPath;
	static const QString selectWebSettingPath;// 配置文件关键字
private: // 程序配置-变量
	QStringList downNovelTypes;
private: // 定时调用
	DateTimeThread *dateTimeThread;
private: // ui 组件
	FileSelectPathWidget *pathHint; // 提示语
	DisplayWidget *displayWidget; // 信息显示框
public slots: // 窗口子控件信号响应
	/// <summary>
	/// 时间响应信号
	/// </summary>
	void updateDateTimeStrFunction( const QString &currentDateTimeStr );
Q_SIGNALS:
	/// <summary>
	/// 当关联的 web 配置文件同步到该实例对象时候会触发该行为
	/// </summary>
	void changeOverWebComponentSetting( );
};

#endif // MAINWIDGET_H_H_HEAD__FILE__
