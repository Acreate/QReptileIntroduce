#include "DisplayWidget.h"
#include <QPalette>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QPainter>
#include <QMap>
#include <QPushButton>

#include "../layout/HLayoutBox.h"
#include "../layout/VLayoutBox.h"
#include "../menu/MenuBar.h"
#include "../menu/Menu.h"
#include "../menu/Action.h"

DisplayWidget::DisplayWidget( QWidget *parent, Qt::WindowFlags flags ) : QWidget( parent, flags ) {
	initComponent( );
	initComponentLayout( );
	initProperty( );
	initConnect( );
}
DisplayWidget::~DisplayWidget( ) {
	menuHLayout->deleteLater( );
	delete backImage;
	auto menus = menuMap.values( );
	for( auto menu : menus )
		if( menu->parent( ) == nullptr )
			menu->deleteLater( );
}
void DisplayWidget::initComponent( ) {
	backImage = new QImage( size( ), QImage::Format_BGR888 );
	mainVLayout = new VLayoutBox( this );
	menuHLayout = new HLayoutBox;
	topMenuBar = new MenuBar( this );
	topMenu = new Menu( this );
	startGet = new Action( topMenu );
}
void DisplayWidget::initProperty( ) {
	startGet->setText( tr( u8"打开文件" ) );
	topMenu->setTitle( tr( u8"开始" ) );
	backImage->fill( Qt::gray );
	currentDisplayType = NORMALE;
	topHeight = topMenuBar->height( );
}
void DisplayWidget::initComponentLayout( ) {
	topMenuBar->addMenu( topMenu );
	topMenu->addAction( startGet );
	menuHLayout->setMenuBar( topMenuBar );
	mainVLayout->addLayout( menuHLayout );
}
void DisplayWidget::initConnect( ) {
	connect( this, &DisplayWidget::setType, this, &DisplayWidget::native_slots_setType );
	connect( this, &DisplayWidget::display, this, &DisplayWidget::native_slots_display );
	connect( topMenu, &QMenu::triggered, [=]( QAction *action ) {
		emit menuActionClick( action, topMenuBar );
	} );
}
Menu * DisplayWidget::getMenu( QObject *object ) {
	if( menuMap.contains( object ) )
		return menuMap[ object ];
	auto objMenu = new Menu( this );
	objMenu->setTitle( object->objectName( ) );
	menuMap.insert( object, objMenu );
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
void DisplayWidget::native_slots_display( QObject *data ) {
	backImage->fill( Qt::gray );
	update( );
}
void DisplayWidget::native_slots_setType( Display_Type type ) {
}
