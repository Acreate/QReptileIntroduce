﻿#ifndef DISPLAYWIDGET_H_H_HEAD__FILE__
#define DISPLAYWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
#include <stream/IStream.h>
#include <stream/OStream.h>
#include <nameSpace/interfacePlugsType.h>


class QMutex;
namespace cylHtmlTools {
	class XPath;
}

class QMenuBar;
class MenuBar;
class Action;
class Menu;
class DisplayContentWidget;
class Button;
class HLayoutBox;
class VLayoutBox;
class DisplayWidget : public QWidget, public IStream, public OStream {
	Q_OBJECT;
public:
	enum Display_Type {
		NORMALE
		, TEXT
		, DB_SQLITE
		, FILE_BIN
	};
	enum Display_Repaint_Type {
		None_Type = 0x0
		, QString_Type = 0x1
		, QObject_Type = 0x2
		, QArrayData_Type = 0x4
		, QByteArray_Type = 0x8
		, Any_Type = 0xffffffff
	};
private: // 用户界面布局
	VLayoutBox *mainVLayout; // 顶级纵向主要布局
private:
	MenuBar *topMenuBar; // 菜单显示栏
	Menu *topMenu; // 菜单
	Menu *fontMenu; // 字体菜单
	Menu *plugTopMneu; // 插件的顶级菜单
	Menu *widgetTopMneu; // 窗口的顶级菜单
	Action *setDrawPlayFont; // 绘制字体菜单
	Action *setMenuFont; // 菜单字体菜单
	Action *clearDisplay; // 清除显示内容
private: // 绘制相关变量
	/// <summary>
	/// 缓存最大持有
	/// </summary>
	size_t strBuffMaxSize;
	QFont msgFont; // 消息字体
	int64_t lineGainSpace; // 字体每行增益空间(可正负)，每行字体的高度加上该增益得到实际绘制高度间隔
	/// <summary>
	/// 更新标识
	/// </summary>
	uint8_t updataStatus;
private: // 类成员变量
	QImage *objectImage; // 要绘制的内容
	QImage *stringMsgImage; // 要绘制的文字
	QImage *arrayDataMsgImage; // 数据数组
	QImage *byteArrayMsgImage; // 二进制数据数组
	Display_Type currentDisplayType; // 当前绘制类型
private: // 线程
	std::shared_ptr< QMutex > mutex;
public:
	DisplayWidget( QWidget *parent, Qt::WindowFlags flags = Qt::WindowFlags( ) );
	~DisplayWidget( ) override;
private:
	void initComponent( ); // 初始化对象
	void initProperty( ); // 初始化属性
	void initComponentLayout( ); // 初始化组件布局
	void initConnect( ); // 初始化信号
	void initOver( ); // 初始化完毕
public:
	Display_Type getDisplay( ) const {
		return currentDisplayType;
	}
private slots:
	void slot_click_action( const Action *action );
private:
	QMap< QObject *, Menu * > menuMap; // 保存主要菜单
	QMap< QString, Menu * > menuPlugMap; // 保存插件菜单
	QMap< QObject *, QSharedPointer< QString > > actionXpath; // 保存菜单的路径
public:
	void clear( );
	/// <summary>
	/// 获取绑定对象的菜单<br/>
	/// 当绑定对象不存在时，将会重新创建一个菜单
	/// </summary>
	/// <param name="object">绑定对象</param>
	/// <returns>对象的菜单</returns>
	Menu * getMenu( QObject *object );
	/// <summary>
	/// 获取插件绑定对象的菜单<br/>
	/// 当绑定对象不存在时，将会重新创建一个菜单
	/// </summary>
	/// <param name="url">绑定 url 地址</param>
	/// <returns>插件对象的菜单</returns>
	Menu * getPlugMenu( QString url );
	/// <summary>
	/// 获取字体
	/// </summary>
	/// <returns>消息字体</returns>
	QFont getDisplayFont( ) const;

	void updatDisplay(Display_Repaint_Type type = Any_Type ) {
		this->updataStatus |= type;
	}
	
protected:
	void paintEvent( QPaintEvent *event ) override;
	void mousePressEvent( QMouseEvent *event ) override;
	void mouseReleaseEvent( QMouseEvent *event ) override;
	void mouseDoubleClickEvent( QMouseEvent *event ) override;
	void mouseMoveEvent( QMouseEvent *event ) override;
	void resizeEvent( QResizeEvent *event ) override;
Q_SIGNALS:
	/// <summary>
	/// 设置显示类型
	/// </summary>
	/// <param name="type">类型</param>
	void setType( Display_Type type );
	/// <summary>
	/// 显示数据
	/// </summary>
	/// <param name="data">数据</param>
	void display( const QObject &data );
	/// <summary>
	/// 显示字符串
	/// </summary>
	/// <param name="data">字符串数据</param>
	void display( const QString &data );
	/// <summary>
	/// 显示数据
	/// </summary>
	/// <param name="data">数据</param>
	void display( const QArrayData &data );
	/// <summary>
	/// 显示数据
	/// </summary>
	/// <param name="data">数据</param>
	void display( const QByteArray &data );
	/// <summary>
	/// 数据显示完毕
	/// </summary>
	void displayLater( );
	/// <summary>
	/// 数据显示之前
	/// </summary>
	void displayBefore( );
	/// <summary>
	/// 菜单或者选项被点击时发生
	/// </summary>
	/// <param name="path">菜单按键的路径</param>
	void menuActionClick( const QString &path );
	/// <summary>
	/// 设置绘制字体
	/// </summary>
	/// <param name="font">新的字体</param>
	void changeDisplayFont( QFont &font );
	/// <summary>
	/// 设置绘制字体间隔
	/// </summary>
	/// <param name="space">空间</param>
	void changeDisplayFontLineHeight( int64_t space );
	/// <summary>
	/// 设置菜单字体
	/// </summary>
	/// <param name="font">新的字体</param>
	void changeMenuFont( QFont &font );
private: // 信号槽所需要变量
	QStringList msgList;
protected slots: // 响应自身信号的槽
	void native_slot_setType( Display_Type type );
	/// <summary>
	/// 显示数据
	/// </summary>
	/// <param name="data">数据</param>
	void native_slot_display( const QObject &data );
	/// <summary>
	/// 显示字符串
	/// </summary>
	/// <param name="data">字符串数据</param>
	void native_slot_display( const QString &data );
	/// <summary>
	/// 显示数据
	/// </summary>
	/// <param name="data">数据</param>
	void native_slot_display( const QArrayData &data );
	/// <summary>
	/// 显示数据
	/// </summary>
	/// <param name="data">数据</param>
	void native_slot_display( const QByteArray &data );
public: // 流接口
	IStream & operator>>( QChar &msg ) override;
	IStream & operator>>( QArrayData &msg ) override;
	IStream & operator>>( QByteArray &msg ) override;
	IStream & operator>>( QString &msg ) override;
	IStream & operator>>( std::string &msg ) override;
	IStream & operator>>( std::wstring &msg ) override;
	IStream & operator>>( int8_t &msg ) override;
	IStream & operator>>( int16_t &msg ) override;
	IStream & operator>>( int32_t &msg ) override;
	IStream & operator>>( int64_t &msg ) override;
	IStream & operator>>( uint8_t &msg ) override;
	IStream & operator>>( uint16_t &msg ) override;
	IStream & operator>>( uint32_t &msg ) override;
	IStream & operator>>( uint64_t &msg ) override;
	IStream & operator>>( float_t &msg ) override;
	IStream & operator>>( double_t &msg ) override;

	OStream & operator<<( const QChar &msg ) override;
	OStream & operator<<( const char16_t &msg ) override;
	OStream & operator<<( const char32_t &msg ) override;
	OStream & operator<<( const wchar_t &msg ) override;
	OStream & operator<<( const char &msg ) override;
	OStream & operator<<( const QArrayData &msg ) override;
	OStream & operator<<( const QByteArray &msg ) override;
	OStream & operator<<( const QString &msg ) override;
	OStream & operator<<( const char *msg ) override;
	OStream & operator<<( const std::string &msg ) override;
	OStream & operator<<( const std::wstring &msg ) override;
	OStream & operator<<( const int8_t &msg ) override;
	OStream & operator<<( const int16_t &msg ) override;
	OStream & operator<<( const int32_t &msg ) override;
	OStream & operator<<( const int64_t &msg ) override;
	OStream & operator<<( const uint8_t &msg ) override;
	OStream & operator<<( const uint16_t &msg ) override;
	OStream & operator<<( const uint32_t &msg ) override;
	OStream & operator<<( const uint64_t &msg ) override;
	OStream & operator<<( const float_t &msg ) override;
	OStream & operator<<( const double_t &msg ) override;
	OStream & operator<<( const void *msg ) override;
	OStream & operator<<( const cylHtmlTools::XPath &msg );

	void flush( ) override;
};




#endif // DISPLAYWIDGET_H_H_HEAD__FILE__
