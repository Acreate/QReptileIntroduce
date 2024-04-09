#include "JobWidget.h"

#include <qtextedit.h>

#include "../../../extend/ui/Button.h"
#include "../../../extend/layout/HLayoutBox.h"
#include "../../../extend/layout/VLayoutBox.h"
JobWidget::JobWidget( QWidget *parent ): QWidget( parent ) {

	mainLayout = new VLayoutBox( this );

	subLayout = new HLayoutBox( );
	mainLayout->addLayout( subLayout );

	button = new Button( this );
	QString text = tr( u8"开始" );
	button->setText( text );
	progressBar = new QProgressBar( this );
	progressBar->setMaximum( 100 );
	progressBar->setMinimum( 0 );
	progressBar->setValue( 0 );
	subLayout->addWidget( progressBar, 9 );
	subLayout->addWidget( button, 1 );

	QTextEdit *edit = new QTextEdit( this );
	edit->setReadOnly( true );
	mainLayout->addWidget( edit );
	edit->setMaximumHeight( 50 );
	auto fontMetrics = edit->fontMetrics( );
	int height = fontMetrics.height( );
	mainLayout->setContentsMargins( 0, 5, 0, 5 );
	subLayout->setContentsMargins( 0, 0, 0, 0 );
	subLayout->setSpacing( 0 );
	mainLayout->setSpacing( 0 );
	height = height * 4;
	setFixedHeight( height );
	connect( this, &JobWidget::setProgressValue, progressBar, &QProgressBar::setValue );
	connect( button, &QPushButton::clicked, this, &JobWidget::click );
}
JobWidget::~JobWidget( ) {
}
