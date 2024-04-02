#ifndef NOVELINFO_H_H_HEAD__FILE__
#define NOVELINFO_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
#include <qfileinfo.h>
#include <qlabel.h>

#include "../netWork/Request.h"
#include "../netWork/RequestConnect.h"

#include "components/Button.h"
#include "components/EditLine.h"
#include "components/WebUrlInfoWidget.h"

class HLayoutBox;
class VLayoutBox;
class RWFileThread;
class FileResult;
class QSettings;
class Request;
class RequestConnect;
class QScrollArea;
class WebUrlInfoWidget;
class NovelInfoWidget : public QWidget {
	Q_OBJECT;
public: // 友元列表
	//friend WebUrlInfoWidget;
public:
	NovelInfoWidget( QWidget *parent = nullptr, Qt::WindowFlags flag = Qt::WindowFlags( ) );
	~NovelInfoWidget( ) override;
private: // 配置文件关键字
	const QString selectWebBuffWorkPath = tr( u8"work/WebBuff/Path" );
public: // 配置文件
	QSettings *netSetFileSettings = nullptr;
	RWFileThread *rwFileThread = nullptr;
	FileResult *fileThreadResult = nullptr; // 文件读取绑定指针
	QString settingFileAbsoluteFilePath; // setting 的绝对路径
public:
	bool setSettingInstance( WebUrlInfoWidget *webUrlInfoWidget );
	QVariant getSettingValue( const QAnyStringView &key ) const {
		return netSetFileSettings->value( key );
	}
	QVariant getSettingValue( const QAnyStringView &key, const QVariant &defaultValue ) const {
		return netSetFileSettings->value( key, defaultValue );
	}
	void setSettingValue( const QAnyStringView &key, const QVariant &value ) {
		netSetFileSettings->setValue( key, value );
	}
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
	void setNetWorkSettingFilePath( const QString &filePath );
	void setRWFileThread( RWFileThread *rwFileThread );
	RWFileThread *getRwFileThread( ) const {
		return rwFileThread;
	}
	QString getAbsoluteFilePath( ) const {
		return settingFileAbsoluteFilePath;
	}
	void setAbsoluteFilePath( const QString &absoluteFilePath ) {
		setNetWorkSettingFilePath( absoluteFilePath );
	}
	Request *getRequestNetWrok( ) const {
		return requestNetWrok;
	}
	void setRequestNetWrok( Request *const requestNetWrok );
	RequestConnect *getRequestConnect( ) const {
		return requestConnect;
	}
	void setRequestConnect( RequestConnect *const requestConnect );
protected: // 重载事件
	void showEvent( QShowEvent *event ) override;
	bool nativeEvent( const QByteArray &eventType, void *message, qintptr *result ) override;
	void mousePressEvent( QMouseEvent *event ) override;
private slots: // slots
	void networkReplyFinished( );
	void settingPathCompoentWriteOver( );
Q_SIGNALS :
	void errorSettingPath( const QString &oldPath, const QString &errorPath );
	void overSettingPath( const QString &oldPath, const QString &errorPath );
};

#endif // NOVELINFO_H_H_HEAD__FILE__
