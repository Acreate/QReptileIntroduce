#ifndef NOVELINFOWIDGET_H_H_HEAD__FILE__
#define NOVELINFOWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
#include <qfileinfo.h>
#include <qlabel.h>

class IRequestNetInterface;
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
	static IRequestNetInterface *getIRequestNetInterface( const QString &plugFilePath, const QString &name, const QString &spec );
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
	static const QString selectWebBuffWorkPath;
public: // 配置文件
	QSettings *netSetFileSettings = nullptr;
	RWFileThread *rwFileThread = nullptr;
	FileResult *fileThreadResult = nullptr; // 文件读取绑定指针
	QString settingFileAbsoluteFilePath; // setting 的绝对路径
public:
	const QString &getWebBuffWorkPathKey( ) const {
		return selectWebBuffWorkPath;
	}
private: // 组件
	VLayoutBox *mainLayout;
	HLayoutBox *settingInfoLayoutBox;
	HLayoutBox *msgLayoutBox;
	HLayoutBox *runInfoLayoutBox;
protected:

private: // 顶端组件-settingInfoLayoutBox
	Button *btn;
	Button *startBtn;
	EditLine *inputSettingPathLine;
	QLabel *settingPathTitle;
private: // 居中组件-msgLayoutBox
	QScrollArea *listView;
private: // 底部组件-runInfoLayoutBox

private:
	int checkStatus = 0;
	int editorStatus = 0;
private: // 网络-请求
	Request *requestNetWrok = nullptr; // web 网络请求对象
	RequestConnect *requestConnect = nullptr; // web 网络响应槽函数对象
public: // get / set

	RWFileThread *getRwFileThread( ) const {
		return rwFileThread;
	}
	QString getAbsoluteFilePath( ) const {
		return settingFileAbsoluteFilePath;
	}
	Request *getRequestNetWrok( ) const {
		return requestNetWrok;
	}

	RequestConnect *getRequestConnect( ) const {
		return requestConnect;
	}

	IRequestNetInterface *loadPlug( const QString &plugFilePath );
	void computeListViewWidgetSize( );
protected: // 重载事件
	void showEvent( QShowEvent *event ) override;
	bool nativeEvent( const QByteArray &eventType, void *message, qintptr *result ) override;
	void mousePressEvent( QMouseEvent *event ) override;
private slots: // 槽函数
	void networkReplyFinished( );
	void settingPathCompoentWriteOver( );
	void loadPathPlugs();
Q_SIGNALS :

	/// <summary>
	/// 请求开始信号
	/// </summary>
	void clickRequestStart();
	
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
	/// 设置请求对象
	/// </summary>
	/// <param name="requestNetWrok">请求对象指针</param>
	void setRequestNetWrok( const Request *requestNetWrok );

	/// <summary>
	/// 设置读写对象
	/// </summary>
	/// <param name="rwFileThread">读写对象指针</param>
	void setRWFileThread( const RWFileThread *rwFileThread );

	/// <summary>
	/// 设置请求信号绑定对象
	/// </summary>
	/// <param name="requestConnect">信号绑定对象指针</param>
	void setRequestConnect( const RequestConnect *requestConnect );
};

#endif // NOVELINFOWIDGET_H_H_HEAD__FILE__
