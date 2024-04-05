#include "WebUrlInfoWidget.h"

#include <QApplication>
#include <QComboBox>
#include <QException>
#include <QLabel>
#include <QScrollArea>
#include <QSettings>
#include <qscreen.h>

#include "NovelInfoWidget.h"

#include "../../../qt/extend/exception/Exception.h"

#include "../../extend/ui/Button.h"
#include "../../extend/ui/EditLine.h"
#include "../../extend/ui/EditLine.h"
#include "../../extend/layout/HLayoutBox.h"

#include "../../userHread/DebugInfo.h"
#include "../../userHread/QtMorc.h"
#include "CountEditWidget.h"

QMap< NovelInfoWidget *, unsigned long long > WebUrlInfoWidget::pathCount = QMap< NovelInfoWidget *, unsigned long long >( );

NovelInfoWidget *WebUrlInfoWidget::overNovelInfoWidgetPtr( QObject *converPtr ) {
	auto novelInfoWidget = qobject_cast< NovelInfoWidget * >( converPtr );
	if( novelInfoWidget )
		return novelInfoWidget;
	Exception msg;
	setConverError( &msg );
	throw msg;
}
NovelInfoWidget *WebUrlInfoWidget::overNovelInfoWidgetPtrTry( QObject *converPtr, Exception *tryResult ) {
	auto novelInfoWidget = qobject_cast< NovelInfoWidget * >( converPtr );
	if( novelInfoWidget )
		return novelInfoWidget;
	if( tryResult )
		setConverError( tryResult );
	return nullptr;
}
void WebUrlInfoWidget::setConverError( Exception *tryResult ) {
	tryResult->setMsg( tr(
		u8R"(===========
文件名: 
	%1
调用名称: 
	WebUrlInfoWidget::WebUrlInfoWidget
行号: 
	%2 row
错误!请为 parent 参数设置一个有效的对象指针
	parent 总是 NovelInfoWidget* 类型
===========
)" ).arg( __FILE__ ).arg( __LINE__ ) );
}
WebUrlInfoWidget::WebUrlInfoWidget( QSettings *webPageSetting,
	NovelInfoWidget *parent,
	const QString &key,
	Qt::WindowFlags f ) : QWidget( parent, f ) {

	auto novelInfoWidget = overNovelInfoWidgetPtr( parent );
	initInstance( webPageSetting, novelInfoWidget );
	auto variant = webPageSetting->value( key );
	if( variant.isNull( ) )
		throw Exception( tr( u8"文件:\n%1\n行号:\n%2\n信息:\nQSettings * 对象指针不存在 [%3] 的值\n" ).arg( __FILE__ ).arg( __LINE__ ).arg( key ) );
	auto url = variant.toString( );
	urlInput->setText( url );
	toggle( Show_Mode::Info );
}

void WebUrlInfoWidget::initComponentConnect( ) {
	connect( insertlNovelInfoBtn, &QPushButton::clicked, [=]( ) {
		emit insterNovelInfo( );
	} );
	connect( startBtn, &QPushButton::clicked, [=]( ) {
		emit start( );
	} );
}
void WebUrlInfoWidget::insterCompoentToLists( ) {
	insterComponent = new QList< QWidget * >;
	insterComponent->append( insertlNovelInfoBtn );

	infoComponent = new QList< QWidget * >;
	infoComponent->append( loadDll );
	infoComponent->append( optionBoxWidget );
	infoComponent->append( urlInput );
	infoComponent->append( allCount );
	infoComponent->append( typeCount );
	infoComponent->append( startBtn );
}
void WebUrlInfoWidget::initInstance( QSettings *webPageSetting, NovelInfoWidget *novelInfoWidget ) {
	setWindowTitle( __func__ );
	pathCount[ novelInfoWidget ] += 1;
	this->webPageSetting = webPageSetting;
	initComponentInstance( );
	initComponentPropertys( );
	initComponentText( );
	initComponentConnect( );
	insterCompoentToLists( );
}
WebUrlInfoWidget::WebUrlInfoWidget( QSettings *webPageSetting, NovelInfoWidget *parent, Qt::WindowFlags f )
: QWidget( parent, f ) {
	auto novelInfoWidget = overNovelInfoWidgetPtr( parent );
	initInstance( webPageSetting, novelInfoWidget );

	toggle( Show_Mode::Inster );
}
WebUrlInfoWidget::~WebUrlInfoWidget( ) {
	DEBUG_RUN(
		qDebug() << tr(u8"WebUrlInfoWidget::~WebUrlInfoWidget : ") << windowTitle();
	);
	delete infoComponent;
	delete insterComponent;
}
QString WebUrlInfoWidget::getUrl( ) const {
	return urlInput->text( );
}
QString WebUrlInfoWidget::getHttpType( ) const {
	return optionBoxWidget->currentText( );
}
void WebUrlInfoWidget::setUrl( const QString &url ) {
	urlInput->setText( url );
}
void WebUrlInfoWidget::toggle( Show_Mode show_mode ) {
	switch( show_mode ) {
		case Show_Mode::Info : {
			for( auto widget : *insterComponent ) {
				hasNovelInfoLayout->removeWidget( widget );
				widget->setHidden( true );
			}
			for( auto widget : *infoComponent ) {
				hasNovelInfoLayout->addWidget( widget );
				widget->setHidden( false );
			}

			currentMode = Show_Mode::Info;
		}
		break;
		case Show_Mode::Inster : {
			for( auto widget : *infoComponent ) {
				hasNovelInfoLayout->removeWidget( widget );
				widget->setHidden( true );
			}
			for( auto widget : *insterComponent ) {
				hasNovelInfoLayout->addWidget( widget );
				widget->setHidden( false );
			}
			currentMode = Show_Mode::Inster;
		}
		break;
	}
	int maxWidth = 0;
	int maxHeight = 0;
	int count = hasNovelInfoLayout->count( );
	for( int index = 0 ; index < count ; ++index ) {
		QLayoutItem *layoutItem = hasNovelInfoLayout->itemAt( index );
		QSize sizeHint = layoutItem->sizeHint( );
		QSize minimumSize = layoutItem->minimumSize( );

		int width = minimumSize.width( );
		int width1 = sizeHint.width( );
		maxWidth += width > width1 ? width : width1;
		if( maxHeight < sizeHint.height( ) )
			maxHeight = sizeHint.height( );
		if( maxHeight < minimumSize.height( ) )
			maxHeight = minimumSize.height( );
	}
	QMargins contentsMargins = hasNovelInfoLayout->contentsMargins( );
	this->setMinimumSize( maxWidth + contentsMargins.left( ) + contentsMargins.right( ), maxHeight + contentsMargins.top( ) + contentsMargins.bottom( ) );
}
void WebUrlInfoWidget::initComponentPropertys( ) {
	hasNovelInfoLayout->setContentsMargins( 0, 0, 0, 0 );
	hasNovelInfoLayout->setSpacing( 0 );
	urlInput->setReadOnly( true );
	auto screens = qApp->screens( );
	uint32_t minWith = 888888888888888;
	for( auto &screen : screens ) {
		auto screenSize = screen->size( );
		DEBUG_RUN( qDebug() << screenSize ) ;
		int width = screenSize.width( );
		if( width < minWith )
			minWith = width;
	}
	urlInput->setMinimumWidth( minWith / 24 );
}
void WebUrlInfoWidget::initComponentText( ) {
	insertlNovelInfoBtn->setText( tr( u8"插入小说网站信息" ) );
	loadDll->setText( u8"加载" );
	optionBoxWidget->addItem( "http" );
	optionBoxWidget->addItem( "https" );
	startBtn->setText( tr( u8"保存" ) );
}
void WebUrlInfoWidget::initComponentInstance( ) {
	insertlNovelInfoBtn = new Button( this );

	hasNovelInfoLayout = new HLayoutBox( this );

	loadDll = new Button( this );
	optionBoxWidget = new QComboBox( this );
	urlInput = new EditLine( this );

	allCount = new CountEditWidget( this );
	typeCount = new CountEditWidget( this );
	startBtn = new Button( this );
}
