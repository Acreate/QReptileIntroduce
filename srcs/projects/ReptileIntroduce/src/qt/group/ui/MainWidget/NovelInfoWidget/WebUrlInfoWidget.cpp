﻿#include "WebUrlInfoWidget.h"

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
#include "../../../setting/Setting.h"

#include <DebugInfo.h>
#include "./WebUrlInfoWidget/CountEditWidget.h"

#include "interface/IRequestNetInterfaceExtend.h"
#include "../../../../extend/netWork/Request.h"
#include "../../../../extend/netWork/RequestConnect.h"
#include "nameSpace/HtmlTools.h"
#include <htmls/htmlDoc/HtmlDoc.h>

const QString WebUrlInfoWidget::settingHostKey = tr( u8"host" );
const QString WebUrlInfoWidget::settingUrlKey = tr( u8"url" );
const QString WebUrlInfoWidget::rootKey = tr( u8"root" );
const QString WebUrlInfoWidget::schemeKey = tr( u8"scheme" );

QMap< WebUrlInfoWidget *, QString > WebUrlInfoWidget::webHost;

NovelInfoWidget * WebUrlInfoWidget::overNovelInfoWidgetPtr( QObject *converPtr ) {
	auto novelInfoWidget = qobject_cast< NovelInfoWidget * >( converPtr );
	if( novelInfoWidget )
		return novelInfoWidget;
	Exception msg;
	setConverError( &msg );
	throw msg;
}
NovelInfoWidget * WebUrlInfoWidget::overNovelInfoWidgetPtrTry( QObject *converPtr, Exception *tryResult ) {
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

size_t WebUrlInfoWidget::getUrl( std::string *outStr ) {
	return requestNetInterface->getUrl( outStr );
}
void WebUrlInfoWidget::setUrl( const StdString &url ) {
	requestNetInterface->setUrl( url );
}
IRequestNetInterface::un_ordered_map * WebUrlInfoWidget::formHtmlGetTypeTheUrls( const StdString &htmlText ) {
	return requestNetInterface->formHtmlGetTypeTheUrls( htmlText );
}
IRequestNetInterface::NovelPtrList WebUrlInfoWidget::formHtmlGetTypePageNovels( const StdString &htmlText, const NovelPtrList &saveNovelInfos, void *appendDataPtr ) {
	return requestNetInterface->formHtmlGetTypePageNovels( htmlText, saveNovelInfos, appendDataPtr );
}
IRequestNetInterface::INovelInfoSharedPtr WebUrlInfoWidget::formHtmlGetUrlNovelInfo( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const INovelInfoSharedPtr &networkReplayNovel ) {
	return requestNetInterface->formHtmlGetUrlNovelInfo( htmlText, saveNovelInfos, networkReplayNovel );
}
IRequestNetInterface::StdString WebUrlInfoWidget::formHtmlGetNext( const StdString &htmlText, const NovelPtrList &saveNovelInfos, const NovelPtrList &lastNovelInfos ) {
	return requestNetInterface->formHtmlGetNext( htmlText, saveNovelInfos, lastNovelInfos );
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
void WebUrlInfoWidget::getData( void *resultAnyPtr ) {
	requestNetInterface->getData( resultAnyPtr );
}
void WebUrlInfoWidget::setHost( const StdString &host ) {
	requestNetInterface->setHost( host );
}
size_t WebUrlInfoWidget::getHost( StdString *outHost ) {
	return requestNetInterface->getHost( outHost );
}
void WebUrlInfoWidget::setScheme( const StdString &scheme ) {
	requestNetInterface->setScheme( scheme );
}
size_t WebUrlInfoWidget::getScheme( StdString *outScheme ) {
	return requestNetInterface->getScheme( outScheme );;
}
WebUrlInfoWidget::WebUrlInfoWidget( Setting *webPageSetting, NovelInfoWidget *parent, IRequestNetInterfaceExtend *requestNetInterface,
	Qt::WindowFlags f ) : QWidget( parent, f ) {
	auto novelInfoWidget = overNovelInfoWidgetPtr( parent );
	initInstance( webPageSetting, novelInfoWidget, requestNetInterface );
}
WebUrlInfoWidget * WebUrlInfoWidget::generateWebUrlInfoWidget( Setting *webPageSetting, NovelInfoWidget *parent, IRequestNetInterfaceExtend *requestNetInterface, Qt::WindowFlags f ) {
	std::string curlLink;
	requestNetInterface->getUrl( &curlLink );
	QUrl url( curlLink.c_str( ) );
	QString host = url.host( );
	if( webHost.count( ) != 0 ) {
		auto iterator = webHost.begin( );
		auto end = webHost.end( );
		for( ; iterator != end; ++iterator ) {
			auto hostName = iterator.value( );
			if( hostName == host ) {
				auto webUrlInfoWidget = iterator.key( );
				webUrlInfoWidget->setWindowFlags( f );
				return webUrlInfoWidget;
			}
		}
	}
	auto result = new WebUrlInfoWidget( webPageSetting, parent, requestNetInterface, f );
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
	connect( optionBoxWidget, &QComboBox::currentIndexChanged, this, &WebUrlInfoWidget::slot_changeScheme );
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
void WebUrlInfoWidget::initInstance( Setting *webPageSetting, NovelInfoWidget *novelInfoWidget, IRequestNetInterfaceExtend *requestNetInterface ) {
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
	webHost.remove( this );
}

QString WebUrlInfoWidget::getScheme( ) const {
	return optionBoxWidget->currentText( );
}
void WebUrlInfoWidget::setScheme( const Scheme_Type schemeType ) {
	int maxCount = optionBoxWidget->maxCount( );
	int currentIndex = optionBoxWidget->currentIndex( );
	for( int index = 0; index < maxCount; ++index )
		if( optionBoxWidget->itemData( index ) == schemeType ) {
			if( currentIndex != index ) {
				optionBoxWidget->setCurrentIndex( index );
				auto itemText = optionBoxWidget->itemText( index );
				std::string curlLink;
				requestNetInterface->getUrl( &curlLink );
				QUrl url( curlLink.c_str( ) );
				webPageSetting->setValue( url.host( ), schemeKey, itemText );
				url.setScheme( itemText );
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
	for( int index = 0; index < optionCount; ++index ) {
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
	for( int index = 0; index < count; ++index ) {
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
	Request *request = new Request( this );
	RequestConnect *requestConnect = new RequestConnect( request );
	request->netGetWork( url, requestConnect );
	connect( requestConnect, &RequestConnect::networkAccessManagerFinished, [=]( QNetworkReply *reply ) {
		webNetRequestOver( reply, requestConnect );
		// 释放上个请求
		request->deleteLater( );
	} );
	DEBUG_RUN( qDebug() << "WebUrlInfoWidget::webNetRequest( " << url.host( ) << ") " );
}
void WebUrlInfoWidget::slot_changeScheme( int index ) {
	auto scheme = optionBoxWidget->itemText( index );
	QString host = urlInput->text( );
	std::string curlLink = QString( "%1://%2" ).arg( scheme ).arg( host ).toLocal8Bit( ).toStdString( );
	requestNetInterface->setUrl( curlLink );
	webPageSetting->setValue( host, schemeKey, scheme );

	emit currentIndexChanged( index );
	DEBUG_RUN( qDebug() << "emit currentIndexChanged( );" );
}
void WebUrlInfoWidget::webNetRequestOver( QNetworkReply *reply, RequestConnect *sender ) {
	QByteArray byteArray = reply->readAll( );
	QString html( byteArray );

	auto typeTheUrls = formHtmlGetTypeTheUrls( html.toStdString( ) );
	qDebug( ) << "getr :\n" << byteArray.toStdString( );

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
	QString fileName = webPageSetting->getFilePath( );
	qDebug( ) << fileName;
	auto allKeys = webPageSetting->getAllKey( );
	for( auto key : allKeys )
		qDebug( ) << "\t" << key;
	qDebug( ) << "childGroups";
	allKeys = webPageSetting->getAllKey( host );
	qDebug( ) << "host : " << host;
	for( auto key : allKeys )
		qDebug( ) << "\t" << key;
	QString scheme = webPageSetting->getValue( host, schemeKey, [&]( const QVariant &getValue ) {
		if( getValue.isNull( ) )
			return url.scheme( );
		return getValue.toString( );
	} ).toString( );
	int maxVisibleItems = optionBoxWidget->count( );
	for( int index = 0; index < maxVisibleItems; ++index ) {
		if( optionBoxWidget->itemText( index ) == scheme ) {
			optionBoxWidget->setCurrentIndex( index );
			break;
		}
	}
}
void WebUrlInfoWidget::initComponentInstance( ) {

	hasNovelInfoLayout = new HLayoutBox( this );

	loadDll = new Button( this );
	optionBoxWidget = new QComboBox( this );
	urlInput = new EditLine( this );

	allCount = new CountEditWidget( this );
	typeCount = new CountEditWidget( this );
	startBtn = new Button( this );
	saveBtn = new Button( this );
}
