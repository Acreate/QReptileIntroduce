#include "MainWidget.h"
#include <QMouseEvent>
#include <QScrollBar>

MainWidget::MainWidget( QWidget *parent, Qt::WindowFlags fg ) : QWidget( parent, fg ), currentFont( "Arial", 10 ), currentFontMetrics( currentFont ), drawColor( 255, 0, 0 ) {
	setWindowTitle( u8"小说阅读" );
	setMouseTracking( true );
	auto oldLayout = this->layout( );
	if( oldLayout )
		delete oldLayout;

	textLine = new QLineEdit( this );
	textLine->setAttribute( Qt::WA_TransparentForMouseEvents, true );

	converTransparentForMouseEventsBtn = new QPushButton( this );

	topLayout = new QHBoxLayout( );

	topLayout->addWidget( textLine );
	topLayout->addWidget( converTransparentForMouseEventsBtn );

	mainLayout = new QVBoxLayout( this );
	this->setLayout( mainLayout );
	mainLayout->setContentsMargins( 0, 0, 0, 0 );

	mainLayout->addLayout( topLayout );
	topLayout->setParent( mainLayout );
	textComponent = new QTextEdit( this );
	mainLayout->addWidget( textComponent );
	textLine->setFont( currentFont );
	textLine->setReadOnly( true );
	textComponent->setAttribute( Qt::WA_TransparentForMouseEvents, true );
	converTransparentForMouseEventsBtn->setAttribute( Qt::WA_TransparentForMouseEvents, true );
	converTransparentForMouseEventsBtn->setText( QString( u8"当前状态 [穿透]" ) );
	connect( converTransparentForMouseEventsBtn, &QPushButton::clicked, [=]( bool flage ) {
		bool attribute = !textComponent->testAttribute( Qt::WA_TransparentForMouseEvents );
		textComponent->setAttribute( Qt::WA_TransparentForMouseEvents, attribute );
		converTransparentForMouseEventsBtn->setText( QString( u8"当前状态 [%1穿透]" ).arg( attribute ? u8"" : u8"未" ) );
	} );
}
MainWidget::~MainWidget( ) {
}
QFont MainWidget::setFont( QFont &font ) {
	auto oldFont = currentFont;
	currentFont = font;
	currentFontMetrics = QFontMetrics( currentFont );
	textLine->setFont( currentFont );
	return oldFont;
}

void MainWidget::mouseMoveEvent( QMouseEvent *event ) {
	mousePoint = event->pos( );
	drawStr = qstrPoint.arg( mousePoint.x( ) ).arg( mousePoint.y( ) );
	textLine->setText( drawStr );
}
void MainWidget::mouseReleaseEvent( QMouseEvent *event ) {
	if( converTransparentForMouseEventsBtn->geometry( ).contains( event->pos( ) ) ) {
		emit converTransparentForMouseEventsBtn->clicked( true );
	}
}
