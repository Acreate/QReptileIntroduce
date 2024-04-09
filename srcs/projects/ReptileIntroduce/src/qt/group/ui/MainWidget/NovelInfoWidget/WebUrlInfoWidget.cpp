#include "WebUrlInfoWidget.h"

#include <QApplication>
#include <QComboBox>
#include <QSettings>
#include <qscreen.h>

#include "../NovelInfoWidget.h"

#include "../../../../../qt/extend/exception/Exception.h"

#include "../../../../extend/ui/Button.h"
#include "../../../../extend/ui/EditLine.h"
#include "../../../../extend/layout/HLayoutBox.h"

#include <DebugInfo.h>
#include "./WebUrlInfoWidget/CountEditWidget.h"

#include "interface/IRequestNetInterface.h"

const QString WebUrlInfoWidget::settingHostKey = tr( u8"host" );
const QString WebUrlInfoWidget::settingUrlKey = tr( u8"url" );
QMap< NovelInfoWidget *, QVector< WebUrlInfoWidget * > > WebUrlInfoWidget::pathCount;
QMap< WebUrlInfoWidget *, QString > WebUrlInfoWidget::webHost;

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
WebUrlInfoWidget::WebUrlInfoWidget( QSettings *webPageSetting, NovelInfoWidget *parent, IRequestNetInterface *requestNetInterface,
	Qt::WindowFlags f ) : QWidget( parent, f ) {

	auto novelInfoWidget = overNovelInfoWidgetPtr( parent );
	initInstance( webPageSetting, novelInfoWidget, requestNetInterface );

	toggle( Show_Mode::Info );
}
WebUrlInfoWidget *WebUrlInfoWidget::generateWebUrlInfoWidget( QSettings *webPageSetting, NovelInfoWidget *parent, IRequestNetInterface *requestNetInterface, Qt::WindowFlags f ) {
	QUrl url = requestNetInterface->getUrl( );
	QString host = url.host( );
	if( webHost.count( ) != 0 ) {
		auto iterator = webHost.begin( );
		auto end = webHost.end( );
		for( ; iterator != end ; ++iterator ) {
			auto hostName = iterator.value( );
			if( hostName == host ) {
				auto webUrlInfoWidget = iterator.key( );
				webUrlInfoWidget->setWindowFlags( f );
				return webUrlInfoWidget;
			}
		}
	}
	auto result = new WebUrlInfoWidget( webPageSetting, parent, requestNetInterface, f );
	pathCount[ parent ].emplace_back( result );
	webHost[ result ] = host;
	return result;
}

void WebUrlInfoWidget::initComponentConnect( ) {
	connect( insertlNovelInfoBtn, &QPushButton::clicked, [=]( ) {
		emit insterNovelInfo( );
		DEBUG_RUN( qDebug() << "emit insterNovelInfo( );" );
	} );
	connect( saveBtn, &QPushButton::clicked, [=]( ) {
		emit saveBtnClick( );
		DEBUG_RUN( qDebug() << "emit saveBtnClick( );" );
	} );
	connect( startBtn, &QPushButton::clicked, [=]( ) {
		emit startBtnClick(  );
		DEBUG_RUN( qDebug() << "emit startBtnClick( );" );
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
	infoComponent->append( saveBtn );
	infoComponent->append( startBtn );
}
void WebUrlInfoWidget::initInstance( QSettings *webPageSetting, NovelInfoWidget *novelInfoWidget, IRequestNetInterface *requestNetInterface ) {
	setWindowTitle( __func__ );
	initComponentInstance( );
	initComponentPropertys( webPageSetting, novelInfoWidget, requestNetInterface );
	initComponentText( );
	initComponentConnect( );
	insterCompoentToLists( );
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
	int minw = maxWidth + contentsMargins.left( ) + contentsMargins.right( );
	int minh = maxHeight + contentsMargins.top( ) + contentsMargins.bottom( );
	this->setMinimumSize( minw, minh );
	//emit widgetReseize( minw, minh );
}
void WebUrlInfoWidget::resizeEvent( QResizeEvent *event ) {
	QWidget::resizeEvent( event );
	DEBUG_RUN(
		qDebug() << tr(u8"WebUrlInfoWidget::resizeEvent : ")
	);
	QSize widgetSize = size( );
	emit widgetReseize( widgetSize.width( ), widgetSize.height( ) );
}
void WebUrlInfoWidget::initComponentPropertys( QSettings *webPageSetting, NovelInfoWidget *novelInfoWidget, IRequestNetInterface *requestNetInterface ) {

	this->webPageSetting = webPageSetting;
	this->requestNetInterface = requestNetInterface;
	QWidget::setParent( novelInfoWidget );
	hasNovelInfoLayout->setContentsMargins( 0, 0, 0, 0 );
	hasNovelInfoLayout->setSpacing( 0 );
	urlInput->setReadOnly( true );
	auto screens = qApp->screens( );
	uint32_t minWith = 888888888;
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
	saveBtn->setText( tr( u8"保存" ) );
	startBtn->setText( tr( u8"开始" ) );

	QUrl url = requestNetInterface->getUrl( );
	urlInput->setText( url.host( ) );
	QString scheme = url.scheme( );
	int maxVisibleItems = optionBoxWidget->count( );
	for( int index = 0 ; index < maxVisibleItems ; ++index ) {
		if( optionBoxWidget->itemText( index ) == scheme ) {
			optionBoxWidget->setCurrentIndex( index );
			break;
		}
	}
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
	saveBtn = new Button( this );
}
