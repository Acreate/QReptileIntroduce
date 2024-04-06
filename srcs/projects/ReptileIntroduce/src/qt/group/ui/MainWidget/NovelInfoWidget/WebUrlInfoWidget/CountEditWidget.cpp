#include "CountEditWidget.h"

#include <QRegExp>
#include <qregularexpression.h>
#include <qvalidator.h>

#include "../../../../../extend/layout/HLayoutBox.h"
#include "../../../../../extend/layout/VLayoutBox.h"
#include "../../../../../extend/ui/Button.h"
#include "../../../../../extend/ui/EditLine.h"
#include <DebugInfo.h>

const QString CountEditWidget::defaultCount = tr( u8"20000" );
const QString CountEditWidget::addBtnText = tr( u8"加" );
const QString CountEditWidget::subBtnText = tr( u8"减" );

void CountEditWidget::initComponents( ) {
	mainLayout = new HLayoutBox( this );
	btnWidget = new QWidget( this );
	btnLayout = new VLayoutBox( btnWidget );
	allCount = new EditLine( this );
	addBtn = new Button( this );
	subBtn = new Button( this );

	exp = new QRegularExpression( "[0-9]*" );
}
void CountEditWidget::initComponentPropertys( ) {
	allCount->setFixedWidth( 50 );
	mainLayout->setSpacing( 0 );
	mainLayout->setContentsMargins( 0, 0, 0, 0 );
	btnLayout->setSpacing( 0 );
	btnLayout->setContentsMargins( 0, 0, 0, 0 );
}
void CountEditWidget::initComponentLayouts( ) {
	mainLayout->addWidget( allCount );
	mainLayout->addWidget( btnWidget );

	btnLayout->addWidget( addBtn );
	btnLayout->addWidget( subBtn );
}
void CountEditWidget::initComponentTexts( ) {
	bool isVoncer = false;
	defaultCount.toLongLong( &isVoncer );
	if( isVoncer )
		allCount->setText( defaultCount );
	else
		allCount->setText( "20000" );
	addBtn->setText( addBtnText );
	subBtn->setText( subBtnText );

	QRegularExpressionValidator *expression = new QRegularExpressionValidator( *exp, allCount );
	allCount->setValidator( expression );
}
void CountEditWidget::initInstance( ) {
	initComponents( );
	initComponentPropertys( );
	initComponentLayouts( );
	initComponentTexts( );
	initComponentConnect( );
}
void CountEditWidget::initComponentConnect( ) {
	connect( addBtn, &QPushButton::clicked, [=]( ) {
		DEBUG_RUN( qDebug() << "addBtn, &QPushButton::clicked" );
		enClickAdd( );
		emit clickAddBtn( );
	} );
	connect( subBtn, &QPushButton::clicked, [=]( ) {
		DEBUG_RUN( qDebug() << "subBtn, &QPushButton::clicked" );
		enClickSub( );
		emit clickSubBtn( );
	} );
	connect( allCount, &QLineEdit::editingFinished, [=]( ) {
		DEBUG_RUN( qDebug() << "allCount, &QLineEdit::editingFinished : "<< allCount->text( ) );
		emit changeValueFinish( allCount->text( ).toLongLong( ) );
	} );
	connect( allCount, &QLineEdit::textChanged, [=]( ) {
		DEBUG_RUN( qDebug()<< "allCount, &QLineEdit::textChanged : " << allCount->text( ) );
		emit changeValueFinish( allCount->text( ).toLongLong( ) );
	} );
}
CountEditWidget::CountEditWidget( QWidget *parent, Qt::WindowFlags flags ): QWidget( parent, flags ) {
	initInstance( );
}
CountEditWidget::CountEditWidget( ) {
	initInstance( );
}
long long CountEditWidget::getValue( ) {
	return allCount->text( ).toLongLong( );
}
void CountEditWidget::enClickSub( ) {
	qlonglong value = allCount->text( ).toLongLong( ) - 1;
	setValue( value );
}
void CountEditWidget::enClickAdd( ) {
	qlonglong value = allCount->text( ).toLongLong( ) + 1;
	setValue( value );
}
bool CountEditWidget::setValue( long long value ) {
	allCount->setText( QString::number( value ) );
	emit changeValueFinish( value );
	return true;
}
