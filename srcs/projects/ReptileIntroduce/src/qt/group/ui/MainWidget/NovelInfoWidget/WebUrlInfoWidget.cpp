#include "WebUrlInfoWidget.h"

#include <QApplication>
#include <QComboBox>
#include <QSettings>
#include <qscreen.h>
#include <string>
#include "../NovelInfoWidget.h"

#include "../../../../../qt/extend/exception/Exception.h"

#include "../../../../extend/ui/Button.h"
#include "../../../../extend/ui/EditLine.h"
#include "../../../../extend/layout/HLayoutBox.h"

#include <DebugInfo.h>
#include "./WebUrlInfoWidget/CountEditWidget.h"

#include "interface/IRequestNetInterfaceExtend.h"
#include "../../../../extend/netWork/Request.h"
#include "../../../../extend/netWork/RequestConnect.h"

const QString WebUrlInfoWidget::settingHostKey = tr( u8"host" );
const QString WebUrlInfoWidget::settingUrlKey = tr( u8"url" );
const QString WebUrlInfoWidget::rootKey = tr( u8"root" );
const QString WebUrlInfoWidget::schemeKey = tr( u8"httpType" );

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
void *WebUrlInfoWidget::getData( ) {
	return requestNetInterface->getData( );
}
size_t WebUrlInfoWidget::getUrl( std::string *outStr ) {
	return requestNetInterface->getUrl( outStr );
}
void WebUrlInfoWidget::setUrl( const StdString &url ) {
	requestNetInterface->setUrl( url );
}
IRequestNetInterface::un_ordered_map *WebUrlInfoWidget::getTypeUrls( const StdString &htmlText ) {
	return requestNetInterface->getTypeUrls( htmlText );
}
IRequestNetInterface::NovelPtrList WebUrlInfoWidget::getTypePageNovels( const StdString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) {
	return requestNetInterface->getTypePageNovels( htmlText, saveNovelInfos, appendDataPtr );
}
IRequestNetInterface::INovelInfoSharedPtr WebUrlInfoWidget::getUrlNovelInfo( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) {
	return requestNetInterface->getUrlNovelInfo( htmlText, saveNovelInfos, networkReplayNovel );
}
IRequestNetInterface::StdString WebUrlInfoWidget::getNext( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) {
	return requestNetInterface->getNext( htmlText, saveNovelInfos, lastNovelInfos );
}
bool WebUrlInfoWidget::setInterfaceParent( void *parent ) {
	auto widget = static_cast< QWidget * >( parent );
	setParent( widget );
	return true;
}

void WebUrlInfoWidget::novelTypeEnd( const NovelPtrList &saveNovelInfos ) {
	requestNetInterface->novelTypeEnd( saveNovelInfos );
}
void WebUrlInfoWidget::endHost( const NovelPtrList &saveNovelInfos ) {
	requestNetInterface->endHost( saveNovelInfos );
}
void WebUrlInfoWidget::deleteMember( ) {
}
WebUrlInfoWidget::WebUrlInfoWidget( QSettings *webPageSetting, NovelInfoWidget *parent, IRequestNetInterfaceExtend *requestNetInterface,
	Qt::WindowFlags f ) : QWidget( parent, f ) {
	auto novelInfoWidget = overNovelInfoWidgetPtr( parent );
	initInstance( webPageSetting, novelInfoWidget, requestNetInterface );
}
WebUrlInfoWidget *WebUrlInfoWidget::generateWebUrlInfoWidget( QSettings *webPageSetting, NovelInfoWidget *parent, IRequestNetInterfaceExtend *requestNetInterface, Qt::WindowFlags f ) {
	std::string curlLink;
	requestNetInterface->getUrl( &curlLink );
	QUrl url( curlLink.c_str( ) );
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
	connect( saveBtn, &QPushButton::clicked, [=]( ) {
		emit clickSaveBtn( );
		DEBUG_RUN( qDebug() << "emit clickSaveBtn( );" );
	} );
	connect( startBtn, &QPushButton::clicked, [=]( ) {
		emit startBtnClick( );
		DEBUG_RUN( qDebug() << "emit startBtnClick( );" );
	} );
	connect( loadDll, &QPushButton::clicked, [=]( ) {
		emit clickLoadBtn( );
		DEBUG_RUN( qDebug() << "emit clickLoadBtn( );" );
	} );
	connect( optionBoxWidget, &QComboBox::currentIndexChanged, [=]( int index ) {

		auto scheme = optionBoxWidget->itemText( index );
		QString host = urlInput->text( );
		std::string curlLink = QString( "%1://%2" ).arg( scheme ).arg( host ).toLocal8Bit( ).toStdString( );
		requestNetInterface->setUrl( curlLink );
		webPageSetting->beginGroup( host );
		webPageSetting->setValue( schemeKey, scheme );
		webPageSetting->endGroup( );

		emit currentIndexChanged( index );
		DEBUG_RUN( qDebug() << "emit currentIndexChanged( );" );
	} );
	connect( this, &WebUrlInfoWidget::startBtnClick, this, &WebUrlInfoWidget::webNetRequest );
}
void WebUrlInfoWidget::insterCompoentToLayout( ) {
	hasNovelInfoLayout->addWidget( loadDll );
	hasNovelInfoLayout->addWidget( optionBoxWidget );
	hasNovelInfoLayout->addWidget( urlInput );
	hasNovelInfoLayout->addWidget( allCount );
	hasNovelInfoLayout->addWidget( typeCount );
	hasNovelInfoLayout->addWidget( saveBtn );
	hasNovelInfoLayout->addWidget( startBtn );
}
void WebUrlInfoWidget::initInstance( QSettings *webPageSetting, NovelInfoWidget *novelInfoWidget, IRequestNetInterfaceExtend *requestNetInterface ) {
	setWindowTitle( __func__ );
	this->webPageSetting = webPageSetting;
	this->requestNetInterface = requestNetInterface;
	QWidget::setParent( novelInfoWidget );

	initComponentInstance( );
	insterCompoentToLayout( );
	initComponentText( );
	initComponentConnect( );
	initComponentPropertys( );
	initCompoentOver( );
}

WebUrlInfoWidget::~WebUrlInfoWidget( ) {
	DEBUG_RUN(
		qDebug() << tr(u8"WebUrlInfoWidget::~WebUrlInfoWidget : ") << windowTitle();
	);
}

QString WebUrlInfoWidget::getScheme( ) const {
	return optionBoxWidget->currentText( );
}
void WebUrlInfoWidget::setScheme( const Scheme_Type schemeType ) {
	int maxCount = optionBoxWidget->maxCount( );
	int currentIndex = optionBoxWidget->currentIndex( );
	for( int index = 0 ; index < maxCount ; ++index )
		if( optionBoxWidget->itemData( index ) == schemeType ) {
			if( currentIndex != index ) {
				optionBoxWidget->setCurrentIndex( index );
				auto itemText = optionBoxWidget->itemText( index );
				std::string curlLink;
				requestNetInterface->getUrl( &curlLink );
				QUrl url( curlLink.c_str( ) );
				webPageSetting->beginGroup( url.host( ) );
				webPageSetting->setValue( schemeKey, itemText );
				webPageSetting->endGroup( );
				url.setScheme( itemText );
				//requestNetInterface.ur
			}
			break;
		}
}

void WebUrlInfoWidget::resizeEvent( QResizeEvent *event ) {
	QWidget::resizeEvent( event );
	DEBUG_RUN(
		qDebug() << tr(u8"WebUrlInfoWidget::resizeEvent : ")
	);
	QSize widgetSize = size( );
	emit widgetReseize( widgetSize.width( ), widgetSize.height( ) );
}
void WebUrlInfoWidget::computerSize( ) {
	QFontMetrics fontMetrics = urlInput->fontMetrics( );
	int horizontalAdvance = fontMetrics.horizontalAdvance( urlInput->text( ) );
	urlInput->setMinimumWidth( horizontalAdvance );
	int optionCount = optionBoxWidget->count( );
	fontMetrics = optionBoxWidget->fontMetrics( );
	horizontalAdvance = 0;
	for( int index = 0 ; index < optionCount ; ++index ) {
		QString itemText = optionBoxWidget->itemText( index );
		int advance = fontMetrics.horizontalAdvance( itemText );
		if( advance > horizontalAdvance )
			horizontalAdvance = advance;
		DEBUG_RUN( qDebug() << tr(u8"检查文字 :") << itemText << ", 长度 : " << horizontalAdvance );
	}
	optionBoxWidget->setMinimumWidth( horizontalAdvance );

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
}
void WebUrlInfoWidget::webNetRequest( ) {
	std::string curlLink;
	getUrl( &curlLink );
	QUrl url( curlLink.c_str( ) );
	DEBUG_RUN( qDebug() << "WebUrlInfoWidget::webNetRequest( " << url.host( ) << ") " );
	requestNetWrok->netGetWork( url, requestConnect );
}
void WebUrlInfoWidget::initCompoentOver( ) {
	computerSize( );

}
void WebUrlInfoWidget::initComponentPropertys( ) {
	hasNovelInfoLayout->setContentsMargins( 0, 0, 0, 0 );
	hasNovelInfoLayout->setSpacing( 0 );
	urlInput->setReadOnly( true );
}
void WebUrlInfoWidget::initComponentText( ) {
	loadDll->setText( tr( u8"加载" ) );
	optionBoxWidget->addItem( tr( u8"http" ), Scheme_Type::HttP );
	optionBoxWidget->addItem( tr( u8"https" ), Scheme_Type::HttpS );
	saveBtn->setText( tr( u8"保存" ) );
	startBtn->setText( tr( u8"开始" ) );

	std::string curlLink;
	requestNetInterface->getUrl( &curlLink );
	QUrl url( curlLink.c_str( ) );
	QString host = url.host( );
	urlInput->setText( host );
	QString fileName = webPageSetting->fileName( );
	qDebug( ) << fileName;
	auto allKeys = webPageSetting->allKeys( );
	for( auto key : allKeys )
		qDebug( ) << "\t" << key;
	webPageSetting->beginGroup( host );
	QString scheme = webPageSetting->value( schemeKey, url.scheme( ) ).toString( );
	webPageSetting->endGroup( );
	int maxVisibleItems = optionBoxWidget->count( );
	for( int index = 0 ; index < maxVisibleItems ; ++index ) {
		if( optionBoxWidget->itemText( index ) == scheme ) {
			optionBoxWidget->setCurrentIndex( index );
			break;
		}
	}
}
void WebUrlInfoWidget::initComponentInstance( ) {

	requestNetWrok = new Request( this );
	requestConnect = new RequestConnect( this );
	hasNovelInfoLayout = new HLayoutBox( this );

	loadDll = new Button( this );
	optionBoxWidget = new QComboBox( this );
	urlInput = new EditLine( this );

	allCount = new CountEditWidget( this );
	typeCount = new CountEditWidget( this );
	startBtn = new Button( this );
	saveBtn = new Button( this );
}
