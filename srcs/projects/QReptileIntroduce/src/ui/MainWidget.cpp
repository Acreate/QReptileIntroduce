#include "MainWidget.h"
#include <QMouseEvent>
#include <QScrollBar>
#include <QMenu>
#include <QRegExp>
#include <QFileDialog>
#include <qcoreapplication.h>

MainWidget::MainWidget( QWidget *parent, Qt::WindowFlags fg ) : QWidget( parent, fg ), currentFont( "Arial", 10 ), currentFontMetrics( currentFont ), drawColor( 255, 0, 0 ) {

	setWindowTitle( u8"小说阅读" );

	/// 配置
	QString progressIniFileName = qApp->applicationDirPath( ).append( QDir::separator( ) ).append( u8"ini" ).append( QDir::separator( ) ).append( u8"progress" ).append( QDir::separator( ) ).append( qApp->applicationName( ) ).append( ".ini" );
	progressSetting = new QSettings( progressIniFileName, QSettings::IniFormat ); // 使用路径方式存储

	setMouseTracking( true );

	auto oldLayout = this->layout( );
	if( oldLayout )
		delete oldLayout;

	textLine = new QLineEdit( this );

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

	bool flageTransparentForMouseEvents = progressSetting->value( transparentForMouseEvents, true ).toBool( );
	textComponent->setAttribute( Qt::WA_TransparentForMouseEvents, flageTransparentForMouseEvents );
	
	converTransparentForMouseEventsBtn->setAttribute( Qt::WA_TransparentForMouseEvents, true );
	textLine->setAttribute( Qt::WA_TransparentForMouseEvents, true );
	converTransparentForMouseEventsBtn->setText( QString( u8"当前状态 [%1穿透]" ).arg( flageTransparentForMouseEvents ? u8"" : u8"未" ) );
	connect( converTransparentForMouseEventsBtn, &QPushButton::clicked, [=]( bool flage ) {
		bool attribute = !textComponent->testAttribute( Qt::WA_TransparentForMouseEvents );
		progressSetting->setValue( transparentForMouseEvents, attribute );
		textComponent->setAttribute( Qt::WA_TransparentForMouseEvents, attribute );
		converTransparentForMouseEventsBtn->setText( QString( u8"当前状态 [%1穿透]" ).arg( attribute ? u8"" : u8"未" ) );
	} );

	////////////// 菜单

	toolsMenu = new Menu( this );
	Action *action = new Action( );
	action->setText( "指定类型配置" );
	toolsMenu->addAction( action );
	connect( action, &QAction::triggered, [=] {
		QString dirPath = progressSetting->value( downIniTypes, qApp->applicationDirPath( ) ).toString( );
		QString fileName = QFileDialog::getOpenFileName( nullptr, u8"设置配置文件", dirPath, u8"txt(*.txt *.ini *.setting) ;; *(*)" );
		if( fileName.isEmpty( ) )
			return;
		QFile file( fileName );
		if( file.open( QIODeviceBase::ReadOnly ) ) {
			progressSetting->setValue( downIniTypes, fileName );
			QRegExp qRegExp( "\\s" );
			QByteArray readAll = file.readAll( );
			QString contents( readAll );
			QStringList list = contents.split( "\n" ),
				overList;
			for( QString subStr : list ) {
				subStr = qRegExp.removeIn( subStr );
				if( !overList.contains( subStr ) )
					overList.append( subStr );
			}
			textComponent->setText( overList.join( "\n" ) );
			file.close( );
		}
	} );
}
MainWidget::~MainWidget( ) {
	progressSetting->sync( );
	delete progressSetting;
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
	Qt::MouseButton mouseButton = event->button( );
	switch( mouseButton ) {
		case Qt::LeftButton :
			if( converTransparentForMouseEventsBtn->geometry( ).contains( event->pos( ) ) ) {
				emit converTransparentForMouseEventsBtn->clicked( true );
			}
			break;
		case Qt::RightButton :
			if( converTransparentForMouseEventsBtn->geometry( ).contains( event->pos( ) ) ) {
				QPointF position = event->globalPosition( );
				toolsMenu->move( position.x( ), position.y( ) );
				toolsMenu->show( );
			}
			break;
		case Qt::MiddleButton :
			break;
	}
}
