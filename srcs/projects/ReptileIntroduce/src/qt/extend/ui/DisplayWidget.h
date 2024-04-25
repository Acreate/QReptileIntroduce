#ifndef DISPLAYWIDGET_H_H_HEAD__FILE__
#define DISPLAYWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QWidget>
class IRequestNetInterfaceExtend;
class QMenuBar;
class MenuBar;
class Action;
class Menu;
class DisplayContentWidget;
class Button;
class HLayoutBox;
class VLayoutBox;
class DisplayWidget : public QWidget {
	Q_OBJECT;
public:
	enum Display_Type {
		NORMALE,
		TEXT,
		DB_SQLITE,
		FILE_BIN
	};
private: // 用户界面布局
	VLayoutBox *mainVLayout; // 顶级纵向主要布局
private:
	MenuBar *topMenuBar; // 菜单显示栏
	Menu *topMenu; // 菜单
	Menu *plugTopMneu; // 插件的顶级菜单
	Menu *widgetTopMneu; // 窗口的顶级菜单
	Action *startGet; // 开始请求
private: // 类成员变量
	QImage *backImage; // 要绘制的内容
	QImage *stringMsgImage; // 要绘制的文字
	Display_Type currentDisplayType; // 当前绘制类型
private: // 运算参考
	/// <summary>
	/// 顶部大小<br/>
	/// 绘制时，会根据该值来确定开始的 y 轴。
	/// </summary>
	size_t topHeight;
	size_t subV, subH;
public:
	DisplayWidget( QWidget *parent, Qt::WindowFlags flags = Qt::WindowFlags( ) );
	~DisplayWidget( ) override;
private:
	void initComponent( ); // 初始化对象
	void initProperty( ); // 初始化属性
	void initComponentLayout( ); // 初始化组件布局
	void initConnect( ); // 初始化信号
public:
	Display_Type getDisplay( ) const {
		return currentDisplayType;
	}
private slots:
	void slot_click_action( const Action *action );
private:
	QMap< QObject *, Menu * > menuMap; // 保存主要菜单
	QMap< IRequestNetInterfaceExtend *, Menu * > menuPlugMap; // 保存插件菜单
	QMap< QObject *, QSharedPointer< QString > > actionXpath; // 保存菜单的路径
public:
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
	/// <param name="object">绑定插件对象</param>
	/// <returns>插件对象的菜单</returns>
	Menu * getPlugMenu( IRequestNetInterfaceExtend *object );
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
	void display( QObject *data );
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
private: // 信号槽所需要变量
	QStringList msgList;
protected slots: // 响应自身信号的槽
	void native_slot_setType( Display_Type type );
	/// <summary>
	/// 显示数据
	/// </summary>
	/// <param name="data">数据</param>
	void native_slot_display( QObject *data );
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
};


#endif // DISPLAYWIDGET_H_H_HEAD__FILE__
