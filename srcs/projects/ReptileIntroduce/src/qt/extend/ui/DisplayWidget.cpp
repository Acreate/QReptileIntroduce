#include "DisplayWidget.h"
#include <QPalette>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QPainter>
#include <QMap>
#include <QPixmap>
#include <QStack>
#include <QPushButton>
#include <stack>
#include <QFontDialog>
#include "../layout/HLayoutBox.h"
#include "../layout/VLayoutBox.h"
#include "../menu/MenuBar.h"
#include "../menu/Menu.h"
#include "../menu/Action.h"
#include "font/Font.h"
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
	delete objectImage;
	delete stringMsgImage;
	delete byteArrayMsgImage;
	delete arrayDataMsgImage;
	auto menus = menuMap.values( );
	for( auto menu : menus )
		if( menu->parent( ) == nullptr )
			menu->deleteLater( );
}
void DisplayWidget::initComponent( ) {
	objectImage = new QImage( size( ), QImage::Format_ARGB32 );
	stringMsgImage = new QImage( size( ), QImage::Format_ARGB32 );
	byteArrayMsgImage = new QImage( size( ), QImage::Format_ARGB32 );
	arrayDataMsgImage = new QImage( size( ), QImage::Format_ARGB32 );
	mainVLayout = new VLayoutBox( this );
	topMenuBar = new MenuBar( this );
	topMenu = new Menu( this );
	plugTopMneu = new Menu( this );
	widgetTopMneu = new Menu( this );
	startGet = new Action( topMenu );
}
void DisplayWidget::initProperty( ) {
	topMenu->setTitle( tr( u8"绘制窗口" ) );
	startGet->setText( tr( u8"设置字体" ) );
	plugTopMneu->setTitle( tr( u8"插件菜单" ) );
	widgetTopMneu->setTitle( tr( u8"窗口菜单" ) );
	objectImage->fill( QColor( 0, 0, 0, 0 ) );
	stringMsgImage->fill( QColor( 0, 0, 0, 0 ) );
	currentDisplayType = NORMALE;
	topHeight = topMenuBar->height( );
	topMenuBar->setMinimumWidth( 100 );
	int spacing = mainVLayout->spacing( );
	auto contentsMargins = mainVLayout->contentsMargins( );
	subV = contentsMargins.top( ) + contentsMargins.bottom( ) + topHeight + spacing * 2;
	subH = contentsMargins.left( ) + contentsMargins.right( ) + spacing * 2;
	strBuffMaxSize = 100;
	msgFont = this->font( );
	msgFont.setBold( true );
	msgFont.setPixelSize( 14 );
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
	connect( this, &DisplayWidget::changeDisplayFont, [=]( QFont &font ) {
		msgFont = font;
		this->setFont( font );
		updatDisplay< QString_Type >( );
		update( );
	} );
	// 重载的槽
	q_connect_solts_thisPtr( const QObject &, &DisplayWidget::display, &DisplayWidget::native_slot_display );
	q_connect_solts_thisPtr( const QString &, &DisplayWidget::display, &DisplayWidget::native_slot_display );
	q_connect_solts_thisPtr( const QArrayData &, &DisplayWidget::display, &DisplayWidget::native_slot_display );
	q_connect_solts_thisPtr( const QByteArray &, &DisplayWidget::display, &DisplayWidget::native_slot_display );
	connect( startGet, &QAction::triggered, [this]( ) {
		bool isOk = false;
		QFont font = QFontDialog::getFont( &isOk, this->msgFont, this, tr( u8"选择一个显示在窗口的字体" ) );
		if( isOk )
			emit changeDisplayFont( font );
	} );
}
void DisplayWidget::slot_click_action( const Action *action ) {
	emit menuActionClick( *action->getActionXPath( ) );
}

// todo : 绘制文字消息
void DisplayWidget::native_slot_display( const QString &data ) {
	auto backImageRect = stringMsgImage->rect( );
	int width = backImageRect.width( );
	if( width == 0 )
		return;
	if( !data.isEmpty( ) )
		this->msgList << data;
	stringMsgImage->fill( QColor( 0, 0, 0, 0 ) );
	QFontMetrics fontMetrics( msgFont );
	int height = fontMetrics.height( ); // 每次换行都这么高
	int maxLine = backImageRect.height( ) / height; // 最多可以容纳的行数
	auto showMsg = this->msgList.join( "" ); // 统计行数
	QStringList showMsgList = showMsg.split( '\n' ), displayList;
	for( auto str : showMsgList ) {
		int horizontalAdvance = fontMetrics.horizontalAdvance( str ); // 占用横向
		do {
			if( horizontalAdvance > width ) { // 大于图片的横向
				size_t fullLineIndex = Font::getFullLineIndex( str, fontMetrics, width ).first;
				auto mid = str.mid( 0, fullLineIndex );
				displayList << mid;
				qint64 length = str.length( );
				if( length < fullLineIndex )
					break;
				str = str.mid( fullLineIndex );
				horizontalAdvance = fontMetrics.horizontalAdvance( str ); // 占用横向
				if( horizontalAdvance < width ) {
					displayList << str;
					break;
				}
				continue;
			}
			displayList << str;
			break;
		} while( true );
	}
	displayList = displayList.mid( displayList.size( ) - maxLine );

	// 绘制
	QPainter painter;
	painter.begin( stringMsgImage );
	painter.setFont( msgFont );
	painter.setPen( QColor( 255, 0, 0, 255 ) );
	qsizetype size = displayList.size( );
	for( qsizetype index = 0; index < size; ++index ) {
		QString chars = displayList[ index ];
		painter.drawText( QPoint( 0, ( index + 1 ) * height ), chars );
	}
	painter.end( );
	size = this->msgList.size( );
	if( size > strBuffMaxSize )
		this->msgList = this->msgList.mid( size - strBuffMaxSize );
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
QFont DisplayWidget::getDisplayFont( ) const {
	return msgFont;
}

template< >
void DisplayWidget::updatDisplay< DisplayWidget::QString_Type >( ) {
	this->updataStatus |= QString_Type;
}
template< >
void DisplayWidget::updatDisplay< DisplayWidget::QObject_Type >( ) {
	this->updataStatus |= QObject_Type;
}
template< >
void DisplayWidget::updatDisplay< DisplayWidget::QArrayData_Type >( ) {
	this->updataStatus |= QArrayData_Type;
}
template< >
void DisplayWidget::updatDisplay< DisplayWidget::QByteArray_Type >( ) {
	this->updataStatus |= QByteArray_Type;
}
void DisplayWidget::paintEvent( QPaintEvent *event ) {

	if( updataStatus & QString_Type )
		native_slot_display( QString( ) );
	if( updataStatus & QObject_Type )
		native_slot_display( QObject( ) );
	if( updataStatus & QArrayData_Type )
		native_slot_display( QArrayData( ) );
	if( updataStatus & QByteArray_Type )
		native_slot_display( QByteArray( ) );
	updataStatus = 0;
	// 绘制之前调用
	emit displayBefore( );
	// todo : 绘制信息
	QPainter painter;
	painter.begin( this );
	size_t sep = subH / 2;
	painter.drawImage( 0, topHeight + sep, *objectImage );
	painter.drawImage( 0, topHeight + sep, *stringMsgImage );
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
	QSize size = event->size( );
	size.setHeight( size.height( ) - subV );
	size.setWidth( size.width( ) - subH );
	*objectImage = objectImage->scaled( size );
	*stringMsgImage = stringMsgImage->scaled( size );
	*byteArrayMsgImage = stringMsgImage->scaled( size );
	*arrayDataMsgImage = stringMsgImage->scaled( size );
	updatDisplay< >( );
}
void DisplayWidget::native_slot_display( const QObject &data ) {
	objectImage->fill( QColor( 0, 0, 0, 0 ) );
	update( );
}
void DisplayWidget::native_slot_setType( Display_Type type ) {
}


IStream & DisplayWidget::operator>>( QChar &msg ) {
	if( msgList.size( ) > 0 ) {
		QString list = msgList.last( );
		msg = list[ 0 ];
	} else
		msg = QChar( 0 );
	return *this;
}
IStream & DisplayWidget::operator>>( QArrayData &msg ) {
	return *this;
}
IStream & DisplayWidget::operator>>( QByteArray &msg ) {
	return *this;
}
IStream & DisplayWidget::operator>>( QString &msg ) {
	if( msgList.size( ) > 0 ) {
		for( auto &qstr : msgList )
			msg.append( qstr );
	} else
		msg.clear( );
	return *this;
}
IStream & DisplayWidget::operator>>( std::string &msg ) {
	if( msgList.size( ) > 0 ) {
		QString buff;
		for( auto &qstr : msgList )
			buff.append( qstr );
		msg = buff.toLocal8Bit( ).toStdString( );
	} else
		msg.clear( );
	return *this;
}
IStream & DisplayWidget::operator>>( std::wstring &msg ) {
	if( msgList.size( ) > 0 ) {
		QString buff;
		for( auto &qstr : msgList )
			buff.append( qstr );
		msg = buff.toStdWString( );
	} else
		msg.clear( );
	return *this;
}

IStream & DisplayWidget::operator>>( int8_t &msg ) {
	qint64 qsizetype = msgList.size( );
	if( qsizetype > 0 ) {
		bool isOk = false;
		for( --qsizetype; qsizetype >= 0; --qsizetype ) {
			QString string = msgList[ qsizetype ];
			int num = string.toInt( &isOk );
			if( isOk ) {
				msg = num;
				return *this;
			}
		}
	}
	msg = 0;
	return *this;
}
IStream & DisplayWidget::operator>>( int16_t &msg ) {
	qint64 qsizetype = msgList.size( );
	if( qsizetype > 0 ) {
		bool isOk = false;
		for( --qsizetype; qsizetype >= 0; --qsizetype ) {
			QString string = msgList[ qsizetype ];
			int num = string.toInt( &isOk );
			if( isOk ) {
				msg = num;
				return *this;
			}
		}
	}
	msg = 0;
	return *this;
}
IStream & DisplayWidget::operator>>( int32_t &msg ) {
	qint64 qsizetype = msgList.size( );
	if( qsizetype > 0 ) {
		bool isOk = false;
		for( --qsizetype; qsizetype >= 0; --qsizetype ) {
			QString string = msgList[ qsizetype ];
			int num = string.toLong( &isOk );
			if( isOk ) {
				msg = num;
				return *this;
			}
		}
	}
	msg = 0;
	return *this;
}
IStream & DisplayWidget::operator>>( int64_t &msg ) {
	qint64 qsizetype = msgList.size( );
	if( qsizetype > 0 ) {
		bool isOk = false;
		for( --qsizetype; qsizetype >= 0; --qsizetype ) {
			QString string = msgList[ qsizetype ];
			int num = string.toLongLong( &isOk );
			if( isOk ) {
				msg = num;
				return *this;
			}
		}
	}
	msg = 0;
	return *this;
}
IStream & DisplayWidget::operator>>( uint8_t &msg ) {
	qint64 qsizetype = msgList.size( );
	if( qsizetype > 0 ) {
		bool isOk = false;
		for( --qsizetype; qsizetype >= 0; --qsizetype ) {
			QString string = msgList[ qsizetype ];
			int num = string.toUInt( &isOk );
			if( isOk ) {
				msg = num;
				return *this;
			}
		}
	}
	msg = 0;
	return *this;
}
IStream & DisplayWidget::operator>>( uint16_t &msg ) {
	qint64 qsizetype = msgList.size( );
	if( qsizetype > 0 ) {
		bool isOk = false;
		for( --qsizetype; qsizetype >= 0; --qsizetype ) {
			QString string = msgList[ qsizetype ];
			int num = string.toUInt( &isOk );
			if( isOk ) {
				msg = num;
				return *this;
			}
		}
	}
	msg = 0;
	return *this;
}
IStream & DisplayWidget::operator>>( uint32_t &msg ) {
	qint64 qsizetype = msgList.size( );
	if( qsizetype > 0 ) {
		bool isOk = false;
		for( --qsizetype; qsizetype >= 0; --qsizetype ) {
			QString string = msgList[ qsizetype ];
			int num = string.toULong( &isOk );
			if( isOk ) {
				msg = num;
				return *this;
			}
		}
	}
	msg = 0;
	return *this;
}
IStream & DisplayWidget::operator>>( uint64_t &msg ) {
	qint64 qsizetype = msgList.size( );
	if( qsizetype > 0 ) {
		bool isOk = false;
		for( --qsizetype; qsizetype >= 0; --qsizetype ) {
			QString string = msgList[ qsizetype ];
			int num = string.toULongLong( &isOk );
			if( isOk ) {
				msg = num;
				return *this;
			}
		}
	}
	msg = 0;
	return *this;
}
IStream & DisplayWidget::operator>>( float_t &msg ) {
	qint64 qsizetype = msgList.size( );
	if( qsizetype > 0 ) {
		bool isOk = false;
		for( --qsizetype; qsizetype >= 0; --qsizetype ) {
			QString string = msgList[ qsizetype ];
			int num = string.toFloat( &isOk );
			if( isOk ) {
				msg = num;
				return *this;
			}
		}
	}
	msg = 0;
	return *this;
}
IStream & DisplayWidget::operator>>( double_t &msg ) {
	qint64 qsizetype = msgList.size( );
	if( qsizetype > 0 ) {
		bool isOk = false;
		for( --qsizetype; qsizetype >= 0; --qsizetype ) {
			QString string = msgList[ qsizetype ];
			int num = string.toDouble( &isOk );
			if( isOk ) {
				msg = num;
				return *this;
			}
		}
	}
	msg = 0;
	return *this;
}
OStream & DisplayWidget::operator<<( const QChar &msg ) {
	msgList << msg;
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const char16_t &msg ) {
	msgList << QChar( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const char32_t &msg ) {
	msgList << QChar( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const wchar_t &msg ) {
	msgList << QString::fromStdWString( std::wstring( ) + msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const char &msg ) {
	msgList << QChar( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const QArrayData &msg ) {
	updatDisplay< QArrayData_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const QByteArray &msg ) {
	updatDisplay< QByteArray_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const QString &msg ) {
	msgList << msg;
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const char *msg ) {
	msgList << msg;
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const std::string &msg ) {
	msgList << QString::fromStdString( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const std::wstring &msg ) {
	msgList << QString::fromStdWString( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const int8_t &msg ) {
	msgList << QString::number( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const int16_t &msg ) {
	msgList << QString::number( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const int32_t &msg ) {
	msgList << QString::number( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const int64_t &msg ) {
	msgList << QString::number( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const uint8_t &msg ) {
	msgList << QString::number( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const uint16_t &msg ) {
	msgList << QString::number( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const uint32_t &msg ) {
	msgList << QString::number( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const uint64_t &msg ) {
	msgList << QString::number( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const float_t &msg ) {
	msgList << QString::number( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const double_t &msg ) {
	msgList << QString::number( msg );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const void *msg ) {
	msgList << QString::number( reinterpret_cast< uint64_t >( msg ), 16 );
	updatDisplay< QString_Type >( );
	return *this;
}
OStream & DisplayWidget::operator<<( const HtmlTools::XPathTools &msg ) {
	*this << msg.getWString( );
	return *this;
}

void DisplayWidget::flush( ) {
	update( );
}
