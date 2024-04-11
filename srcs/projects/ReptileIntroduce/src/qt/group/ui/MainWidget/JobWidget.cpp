#include "JobWidget.h"

#include <qtextedit.h>

#include "../../../extend/ui/Button.h"
#include "../../../extend/layout/HLayoutBox.h"
#include "../../../extend/layout/VLayoutBox.h"


JobWidget::JobWidget( QWidget *parent ): QWidget( parent ) {
	initCompoentObjMemory( );
	initComponentProperty( );
	initComponentLayout( );
	initComponentConnect( );
	initComponentOver( );
}
JobWidget::~JobWidget( ) {
}
void JobWidget::initCompoentObjMemory( ) {
	mainLayout = new VLayoutBox( this );
	subLayout = new HLayoutBox( );
	button = new Button( this );
	progressBar = new QProgressBar( this );
	edit = new QTextEdit( this );
}
void JobWidget::initComponentProperty( ) {
	QString text = tr( u8"开始" );
	button->setText( text );
	progressBar->setMaximum( 100 );
	progressBar->setMinimum( 0 );
	progressBar->setValue( 0 );

	edit->setReadOnly( true );
	edit->setMaximumHeight( 50 );
	mainLayout->setContentsMargins( 0, 5, 0, 5 );
	subLayout->setContentsMargins( 0, 0, 0, 0 );
	subLayout->setSpacing( 0 );
	mainLayout->setSpacing( 0 );
}
void JobWidget::initComponentLayout( ) {
	mainLayout->addLayout( subLayout );
	mainLayout->addWidget( edit );
	subLayout->addWidget( progressBar, 9 );
	subLayout->addWidget( button, 1 );

}
void JobWidget::initComponentConnect( ) {
	connect( this, &JobWidget::setProgressValue, progressBar, &QProgressBar::setValue );
	connect( button, &QPushButton::clicked, this, &JobWidget::click );
}
void JobWidget::initComponentOver( ) {
	auto fontMetrics = edit->fontMetrics( );
	int height = fontMetrics.height( );
	height = height * 4;
	setFixedHeight( height );
}
