#include "DisplayWidget.h"
#include <QPalette>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QPainter>
#include <QMap>
#include <QStack>
#include <QPushButton>
#include <stack>

#include "../layout/HLayoutBox.h"
#include "../layout/VLayoutBox.h"
#include "../menu/MenuBar.h"
#include "../menu/Menu.h"
#include "../menu/Action.h"
#include "interface/IRequestNetInterfaceExtend.h"
#include "path/Dir.h"

#define q_connect_solts( type , signalPtr, signal, slotPtr, slot ) \
	do{\
		auto signal_QObject = QOverload< type>::of( signal ); \
		auto slot_QObject = QOverload< type>::of( slot ); \
		connect ( signalPtr,signal_QObject,slotPtr,slot_QObject); \
	}while(false)
#define q_connect_solts_theSamePtr(type , sycPtr, signal, slot) q_connect_solts(type , sycPtr, signal, sycPtr, slot)
#define q_connect_solts_thisPtr(type, signal, slot) q_connect_solts_theSamePtr(type , this, signal,  slot)

DisplayWidget::DisplayWidget( QWidget *parent, Qt::WindowFlags flags ) : QWidget( parent, flags ) {
	initComponent( );
	initComponentLayout( );
	initProperty( );
	initConnect( );
}
DisplayWidget::~DisplayWidget( ) {
	delete backImage;
	auto menus = menuMap.values( );
	for( auto menu : menus )
		if( menu->parent( ) == nullptr )
			menu->deleteLater( );
}
void DisplayWidget::initComponent( ) {
	backImage = new QImage( size( ), QImage::Format_BGR888 );
	mainVLayout = new VLayoutBox( this );
	topMenuBar = new MenuBar( this );
	topMenu = new Menu( this );
	plugTopMneu = new Menu( this );
	widgetTopMneu = new Menu( this );
	startGet = new Action( topMenu );
}
void DisplayWidget::initProperty( ) {
	startGet->setText( tr( u8"打开文件" ) );
	topMenu->setTitle( tr( u8"开始" ) );
	plugTopMneu->setTitle( tr( u8"插件菜单" ) );
	widgetTopMneu->setTitle( tr( u8"窗口菜单" ) );
	backImage->fill( Qt::gray );
	currentDisplayType = NORMALE;
	topHeight = topMenuBar->height( );
	topMenuBar->setMinimumWidth( 100 );
}
void DisplayWidget::initComponentLayout( ) {
	topMenuBar->addMenu( topMenu );
	topMenuBar->addMenu( plugTopMneu );
	topMenuBar->addMenu( widgetTopMneu );
	topMenu->addAction( startGet );
	mainVLayout->setMenuBar( topMenuBar );
}
void DisplayWidget::initConnect( ) {
	connect( topMenu, &Menu::click, this, &DisplayWidget::slot_click_action );

	// 链接自身的槽
	connect( this, &DisplayWidget::setType, this, &DisplayWidget::native_slot_setType );

	// 重载的槽
	q_connect_solts_thisPtr( QObject *, &DisplayWidget::display, &DisplayWidget::native_slot_display );
	q_connect_solts_thisPtr( const QString &, &DisplayWidget::display, &DisplayWidget::native_slot_display );
	q_connect_solts_thisPtr( const QArrayData &, &DisplayWidget::display, &DisplayWidget::native_slot_display );
	q_connect_solts_thisPtr( const QByteArray &, &DisplayWidget::display, &DisplayWidget::native_slot_display );
}
void DisplayWidget::slot_click_action( const Action *action ) {
	emit menuActionClick( *action->getActionXPath( ) );
}

void DisplayWidget::native_slot_display( const QString &data ) {
	QPainter painter;
	painter.begin( backImage );
	QPen pen;
	pen.setColor( Qt::red );
	painter.setPen( pen );
	painter.drawText( QPoint( 100, 100 ), data );
	painter.end( );
	update( );
}
void DisplayWidget::native_slot_display( const QArrayData &data ) {
}
void DisplayWidget::native_slot_display( const QByteArray &data ) {
}
Menu * DisplayWidget::getMenu( QObject *object ) {
	if( menuMap.contains( object ) )
		return menuMap[ object ];
	auto objMenu = new Menu( this );
	connect( objMenu, &Menu::click, this, &DisplayWidget::slot_click_action );
	objMenu->setTitle( object->objectName( ) );
	widgetTopMneu->addMenu( objMenu );
	menuMap.insert( object, objMenu );
	return objMenu;
}

Menu * DisplayWidget::getPlugMenu( IRequestNetInterfaceExtend *object ) {
	if( menuPlugMap.contains( object ) )
		return menuPlugMap[ object ];
	auto objMenu = new Menu( this );
	connect( objMenu, &Menu::click, this, &DisplayWidget::slot_click_action );
	std::string url;
	object->getUrl( &url );
	objMenu->setTitle( QString::fromStdString( url ) );
	plugTopMneu->addMenu( objMenu );
	menuPlugMap.insert( object, objMenu );
	return objMenu;
}
void DisplayWidget::paintEvent( QPaintEvent *event ) {
	// 绘制之前调用
	emit displayBefore( );
	// todo : 绘制信息
	QPainter painter;
	painter.begin( this );
	painter.drawImage( 0, topHeight, *backImage );
	painter.end( );
	QWidget::paintEvent( event );
	// 绘制之后调用
	emit displayLater( );
}
void DisplayWidget::mousePressEvent( QMouseEvent *event ) {
	QWidget::mousePressEvent( event );
}
void DisplayWidget::mouseReleaseEvent( QMouseEvent *event ) {
	QWidget::mouseReleaseEvent( event );
}
void DisplayWidget::mouseDoubleClickEvent( QMouseEvent *event ) {
	QWidget::mouseDoubleClickEvent( event );
}
void DisplayWidget::mouseMoveEvent( QMouseEvent *event ) {
	QWidget::mouseMoveEvent( event );
}
void DisplayWidget::resizeEvent( QResizeEvent *event ) {
	*backImage = backImage->scaled( event->size( ) );
}
void DisplayWidget::native_slot_display( QObject *data ) {
	backImage->fill( Qt::gray );
	update( );
}
void DisplayWidget::native_slot_setType( Display_Type type ) {
}
