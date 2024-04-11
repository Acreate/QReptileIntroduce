#ifndef NOVELINFOWIDGET_H_H_HEAD__FILE__
#define NOVELINFOWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
#include <qfileinfo.h>
#include <qlabel.h>

class IRequestNetInterface;
class IRequestNetInterfaceExtend;
class EditLine;
class Button;
class HLayoutBox;
class VLayoutBox;
class RWFileThread;
class FileResult;
class QSettings;
class Request;
class RequestConnect;
class QScrollArea;
class WebUrlInfoWidget;
/// <summary>
/// 无法继承的类<br/>
/// 可用于小说小说信息管理的类
/// </summary>
class NovelInfoWidget final : public QWidget {
	Q_OBJECT;
public: // 友元列表
	//friend WebUrlInfoWidget;
public: // 静态成员函数列表
	static IRequestNetInterfaceExtend *getIRequestNetInterface( const QString &plugFilePath, const QString &name, const QString &spec );
protected:
	static IRequestNetInterfaceExtend *metaGetResult( QObject *outObj, const char *methodName );
public:
	NovelInfoWidget( QWidget *parent = nullptr, Qt::WindowFlags flag = Qt::WindowFlags( ) );
	~NovelInfoWidget( ) override;
private:
	QString className;
public:
	const QString &getClassName( ) {
		if( className.isEmpty( ) )
			className = QString( staticMetaObject.className( ) );
		return className;
	}
private: // 配置文件关键字
	/// <summary>
	/// web 缓冲写入路径
	/// </summary>
	static const QString settingWebBuffWorkPathKey;
	/// <summary>
	/// web 子控件路径
	/// </summary>
	static const QString settingHostKey;
	/// <summary>
	/// 动态加载的类名
	/// </summary>
	static const QString loadClassName;
	/// <summary>
	/// 
	/// </summary>
	static const QString loadClassMethodName;
	static const QByteArray loadClassNameByteArry;
protected:
	static std::unordered_map< QString, IRequestNetInterfaceExtend * > loadPlugs;
public: // 配置文件
	QSettings *netSetFileSettings = nullptr;
	QString settingFileAbsoluteFilePath; // setting 的绝对路径
public:
	const QString &getWebBuffWorkPathKey( ) const {
		return settingWebBuffWorkPathKey;
	}
private: // 组件
	VLayoutBox *mainLayout;
	HLayoutBox *settingInfoLayoutBox;
	HLayoutBox *msgLayoutBox;
	HLayoutBox *runInfoLayoutBox;
protected:
	/// <summary>
	/// 实现组件对象创建
	/// </summary>
	void initComponentMeoryObj( );
	/// <summary>
	/// 实现组件对象属性
	/// </summary>
	void initComponentPropers( );
	/// <summary>
	/// 实现组件布局
	/// </summary>
	void initComponentLayout( );
	/// <summary>
	/// 实现组件信号槽内容
	/// </summary>
	void initComponentConnect( );
	/// <summary>
	/// 初始化 widget 大小
	/// </summary>
	void initWidgetSize( );
private: // 初始化子组件
	void initWebUrlInfoWidgetCompoent( WebUrlInfoWidget *webUrlWidget );
private: // 顶端组件-settingInfoLayoutBox
	Button *btn;
	Button *startBtn;
	EditLine *inputSettingPathLine;
	QLabel *settingPathTitle;
private: // 居中组件-msgLayoutBox
	QScrollArea *listView;
private: // listView 组件中的子组件
	QWidget *listViewWidget;
	VLayoutBox *listViewWidgetVBox;
private: // 底部组件-runInfoLayoutBox

private:
	int editorStatus = 0;
public: // get / set

	QString getAbsoluteFilePath( ) const {
		return settingFileAbsoluteFilePath;
	}

	IRequestNetInterfaceExtend *loadPlug( const QString &plugFilePath );
	void computeListViewWidgetSize( );
protected: // 重载事件
	void showEvent( QShowEvent *event ) override;
	bool nativeEvent( const QByteArray &eventType, void *message, qintptr *result ) override;
	void mousePressEvent( QMouseEvent *event ) override;
private slots: // 子控件槽函数

	/// <summary>
	/// 加载子控件时激活
	/// </summary>
	void loadPathPlugs( );
	/// <summary>
	/// 子控件的列表项被改变时激活
	/// </summary>
	/// <param name="index">当前选项，-1 表示没有</param>
	void componentCurrentIndexChanged( int index );
	/// <summary>
	/// 在 inputSettingPathLine 设置配置文件路径完成时被激活
	/// </summary>
	void inputSettingPathLinePathCompoentEditFinish( );
public slots: // 当前窗口槽函数
	/// <summary>
	/// 设置工作路径配置文件
	/// </summary>
	/// <param name="filePath"></param>
	void slotsSetNetWorkSettingFilePath( const QString &filePath );
	/// <summary>
	/// 
	/// </summary>
	/// <param name="oldPath"></param>
	/// <param name="newPath"></param>
	void slotsOverSettingPath( const QString &oldPath, const QString &newPath );
	void slotsErrorSettingPath( const QString &currentPath, const QString &errorPath );
Q_SIGNALS :
	/// <summary>
	/// 重置大小信号
	/// </summary>
	void resetWidgetSize( const int width, const int height );
	/// <summary>
	/// 请求开始信号
	/// </summary>
	void clickRequestStart( );

	/// <summary>
	/// 路径错误发生消息
	/// </summary>
	/// <param name="currentPath">当前路径</param>
	/// <param name="errorPath">错误路径</param>
	void errorSettingPath( const QString &currentPath, const QString &errorPath );
	/// <summary>
	/// 路径设置完毕事件
	/// </summary>
	/// <param name="oldPath">旧路径</param>
	/// <param name="newPath">新路径</param>
	void overSettingPath( const QString &oldPath, const QString &newPath );
	/// <summary>
	/// 设置 setting 路径的消息
	/// </summary>
	/// <param name="filePath">路径</param>
	void setNetWorkSettingFilePath( const QString &filePath );

	/// <summary>
	/// 设置读写对象
	/// </summary>
	/// <param name="rwFileThread">读写对象指针</param>
	void setRWFileThread( const RWFileThread *rwFileThread );

};

#endif // NOVELINFOWIDGET_H_H_HEAD__FILE__
