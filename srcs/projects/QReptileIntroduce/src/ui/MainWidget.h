#ifndef MAINWIDGET_H_H_HEAD__FILE__
#define MAINWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
#include <QFontMetrics>
#include <QFont>
#include <qtextedit.h>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QNetworkReply>

#include "../menu/Menu.h"
#include <QSettings>
#include <QTranslator>
#include <QFileDevice>
#include <QTimer>

#include "../layout/HLyaoutBox.h"
class QFile;
class Request;
class FileResult;
class RWFileThread;
class DateTimeThread;
class RequestConnect;
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
	QString qstrPoint = u8"坐标:( %1 , %2 )"; // 格式化字符串
	FileResult *fileThreadResult = nullptr; // 文件读取绑定指针
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
	QTextEdit *textComponent; // 显示文本
	QLineEdit *textLine; // 显示时间
	QPushButton *converTransparentForMouseEventsBtn; // 切换与鼠标点击事件
private: // 布局
	QVBoxLayout *mainLayout;
	HLyaoutBox *topLayout;
private: // 菜单
	Menu *toolsMenu;
private: // 程序配置-读写
	QSettings *progressSetting;
	QTranslator *translator;
	RWFileThread *rwFileThread;
private: // 网络-请求
	Request *requestNetWrok = nullptr; // web 网络请求对象
	RequestConnect *requestConnect = nullptr; // web 网络响应槽函数对象
private: // 程序配置-名称
	const QString transparentForMouseEvents = tr( u8"somponentStyle/TransparentForMouseEvents" );
	const QString downIniTypes = tr( u8"downIni/Types" );
	const QString selectReadFileWorkPath = tr( u8"work/Read/Path" );
	const QString selectWriteFileWorkPath = tr( u8"work/Write/Path" );
	const QString selectWebBuffWorkPath = tr( u8"work/WebBuff/Path" );
private: // 程序配置-变量
	QStringList downNovelTypes;
private: // 定时调用
	DateTimeThread *dateTimeThread;
public slots: // 窗口子控件信号响应
	void updateDateTimeStrFunction( const QString &currentDateTimeStr );
	void changeTransparent( bool flage );
	void changeTextComponentContents( );
	void error( int errorType, QFileDevice::FileError fileErrorCode, QFileDevice::FileError dirError );
private:
	/// <summary>
	/// 更新窗口宽度
	/// </summary>
	void updateWidgetWidth( const QList< QString > &list );
public:
	// 事件测试
	QVector< QObject * > getCompoents( ) {
		return {
				mainLayout,
				topLayout,
				textComponent,
				textLine,
				converTransparentForMouseEventsBtn,
				toolsMenu,
			};
	}
};

#endif // MAINWIDGET_H_H_HEAD__FILE__
