#ifndef MAINWIDGET_H_H_HEAD__FILE__
#define MAINWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>

class NovelNetJob;
class IRequestNetInterfaceExtend;
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
	VLayoutBox *mainVLayoutBox;
private: // 显示组件
	FileSelectPathWidget *selectPathWidget; // 路径选择组件
	DisplayWidget *display; // 信息显示组件
	Action *LoadWebInfoBtn; // 开始请求按键
public:
	MainWidget( QWidget *parent = nullptr, Qt::WindowFlags fg = Qt::WindowFlags( ) );
	~MainWidget( ) override;
protected: // 事件
	void mouseMoveEvent( QMouseEvent *event ) override;
	void mouseReleaseEvent( QMouseEvent *event ) override;
	void mousePressEvent( QMouseEvent *event ) override;
	void resizeEvent( QResizeEvent *event ) override;
private: // 控件菜单
	Menu *fromDisplayWidgetMenu;
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
private: // 程序配置-名称
	static const QString settingGroupWork; // 工作路径组名称
	static const QString settingGroupWorkKeyReadFileWorkPath; // 基于 settingGroupWork 组的选择的读入路径
	static const QString settingGroupWorkKeyWriteFileWorkPath; // 基于 settingGroupWork 组的选择的写入路径
	static const QString settingGroupWeb;// 配置文件组名称
	static const QString settingGroupWebKey;//  基于 settingGroupWeb 组的配置文件关键字
	static const QString settingGroupSelectDefaultPaths; // 默认的窗口选择路径组
	static const QString settingGroupSelectDefaultPathskeyPlugPathKey; // 基于 settingGroupSelectDefaultPaths 插件路径
	static const QString settingGroupPlugIniPathKeyMerge; // 插件的配置文件路径
	static const QString settingGroupPlugIniPathSettingPath; // 插件的配置文件路径
	static const QChar settingPathSep; // 路径分隔符
private: // 程序配置-变量
	/// <summary>
	/// 配置与路径的映射<br/>
	/// </summary>
	QMap< QString, Setting * > webSettingMap;
private: // 定时调用
	DateTimeThread *dateTimeThread;
private: // ui 组件
	FileSelectPathWidget *pathHint; // 提示语
public:
	/// <summary>
	/// 更新配置文件路径，并且应用配置<br/>
	/// 重复路径返回 false，并且跳过过程
	/// </summary>
	/// <param name="filePath">新的路径</param>
	/// <returns>成功返回 true</returns>
	bool updateSettingFileInfo( const QString &filePath );
private:
	QMap< QString, QSharedPointer< NovelNetJob > > plugs;
public slots: // 窗口子控件信号响应
	/// <summary>
	/// 时间响应信号
	/// </summary>
	void updateDateTimeStrFunction( const QString &currentDateTimeStr );

	/// <summary>
	/// 显示插件选择窗口
	/// </summary>
	void showSelectPlugPathDialog( );
	/// <summary>
	/// 加载插件
	/// </summary>
	void loadingPlug( );
	/// <summary>
	/// 开始爬虫
	/// </summary>
	void LoadWebInfo( );
Q_SIGNALS:
	/// <summary>
	/// 当关联的 web 配置文件同步到该实例对象时候会触发该行为
	/// </summary>
	void changeOverWebComponentSetting( );
};

#endif // MAINWIDGET_H_H_HEAD__FILE__
