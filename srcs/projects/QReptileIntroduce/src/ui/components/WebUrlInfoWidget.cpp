#include "WebUrlInfoWidget.h"

#include <QLabel>
#include <QComboBox>
#include <QException>
#include <QScrollArea>

#include "../../userHread/DebugInfo.h"
#include "../../layout/HLayoutBox.h"
#include "../../exception/Exception.h"
#include "Button.h"
#include "EditLine.h"

QMap< QObject *, unsigned long long > WebUrlInfoWidget::pathCount = QMap< QObject *, unsigned long long >( );

WebUrlInfoWidget::WebUrlInfoWidget( QSettings *webPageSetting, QWidget *parent ) {
	if( parent ) {
		setParent( parent );
		setWindowTitle( __func__ );
		pathCount[ parent ] += 1;
		this->webPageSetting = webPageSetting;
		insertlNovelInfoBtn = new Button( this );
		insertlNovelInfoBtn->setText( tr( u8"insert novel url info" ) );

		hasNovelInfoLayout = new HLayoutBox( this );
		hasNovelInfoLayout->setContentsMargins( 0, 0, 0, 0 );
		hasNovelInfoLayout->setSpacing( 0 );

		urlSortIndex = new QLabel( this );
		urlSortIndex->setText( u8"1" );
		optionBoxWidget = new QComboBox( this );
		optionBoxWidget->addItem( "http" );
		optionBoxWidget->addItem( "https" );
		optionBoxWidget->setEditable( false );
		urlInput = new EditLine( this );
		urlInput->setText( "www.baidu.com" );
		int minw = urlInput->fontMetrics( ).horizontalAdvance( "www.baidu.com" );
		auto childrenRect = urlInput->minimumSizeHint( ).width( );
		urlInput->setMinimumWidth( minw + childrenRect );

		addNovelAllInCountBtn = new Button( this );
		addNovelAllInCountBtn->setText( "all add 1" );
		subNovelAllInCountBtn = new Button( this );
		subNovelAllInCountBtn->setText( "sub add 1" );
		addNovelTypeInCountBtn = new Button( this );
		addNovelTypeInCountBtn->setText( "type add 1" );
		subNovelTypeInCountBtn = new Button( this );
		subNovelTypeInCountBtn->setText( "type sub 1" );
		saveBtn = new Button( this );
		saveBtn->setText( "save" );

		urlSortIndex->setHidden( true );
		urlInput->setHidden( true );
		optionBoxWidget->setHidden( true );
		addNovelAllInCountBtn->setHidden( true );
		subNovelAllInCountBtn->setHidden( true );
		addNovelTypeInCountBtn->setHidden( true );
		subNovelTypeInCountBtn->setHidden( true );
		saveBtn->setHidden( true );

		connect( insertlNovelInfoBtn, &QPushButton::clicked, [=]( ) {
			toggle( Show_Mode::Info );
		} );
		connect( saveBtn, &QPushButton::clicked, [=]( ) {
			toggle( Show_Mode::Inster );
		} );
		toggle( Show_Mode::Inster );
	}

	QException qException = QException( );
	throw Exception( tr( u8"error ! please set valid value( --parent )" ) );

}
WebUrlInfoWidget::~WebUrlInfoWidget( ) {
	DEBUG_RUN(
		qDebug() << tr(u8"WebUrlInfoWidget::~WebUrlInfoWidget : ") << windowTitle();
	);

}
QString WebUrlInfoWidget::getUrl( ) const {
	return urlInput->text( );
}
QString WebUrlInfoWidget::getHttpType( ) const {
	return optionBoxWidget->currentText( );
}
void WebUrlInfoWidget::toggle( Show_Mode show_mode ) {
	switch( show_mode ) {
		case Show_Mode::Info : {
			hasNovelInfoLayout->removeWidget( insertlNovelInfoBtn );
			insertlNovelInfoBtn->setHidden( true );
			hasNovelInfoLayout->addWidget( urlSortIndex );
			hasNovelInfoLayout->addWidget( optionBoxWidget );
			hasNovelInfoLayout->addWidget( urlInput );
			hasNovelInfoLayout->addWidget( addNovelAllInCountBtn );
			hasNovelInfoLayout->addWidget( subNovelAllInCountBtn );
			hasNovelInfoLayout->addWidget( addNovelTypeInCountBtn );
			hasNovelInfoLayout->addWidget( subNovelTypeInCountBtn );
			hasNovelInfoLayout->addWidget( saveBtn );

			subNovelTypeInCountBtn->setHidden( false );
			addNovelTypeInCountBtn->setHidden( false );
			subNovelAllInCountBtn->setHidden( false );
			addNovelAllInCountBtn->setHidden( false );
			optionBoxWidget->setHidden( false );
			urlInput->setHidden( false );
			urlSortIndex->setHidden( false );
			saveBtn->setHidden( false );
		}
		break;
		case Show_Mode::Inster : {
			do {
				QLayoutItem *layoutItem = hasNovelInfoLayout->itemAt( 0 );
				if( layoutItem ) {
					hasNovelInfoLayout->removeItem( layoutItem );
					layoutItem->widget( )->setHidden( true );
				} else
					break;
			} while( true );
			hasNovelInfoLayout->addWidget( insertlNovelInfoBtn );
			insertlNovelInfoBtn->setHidden( false );
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
	emit toggled( show_mode );
}
