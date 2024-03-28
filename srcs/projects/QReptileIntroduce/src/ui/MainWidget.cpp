#include "MainWidget.h"
#include <QMouseEvent>
#include <QScrollBar>
#include <QMenu>
#include <QRegExp>
#include <QFileDialog>
#include <qcoreapplication.h>

MainWidget::MainWidget( QWidget *parent, Qt::WindowFlags fg ) : QWidget( parent, fg ), currentFont( "Arial", 10 ), currentFontMetrics( currentFont ), drawColor( 255, 0, 0 ) {

	/// 配置 路径
	QString progressIniPath = qApp->applicationDirPath( ).append( QDir::separator( ) ).append( tr( u8"ini" ) ).append( QDir::separator( ) ).append( tr( u8"progress" ) ).append( QDir::separator( ) );
	QString progressIniFileName = progressIniPath;
	progressIniFileName.append( "ini" ).append( QDir::separator( ) ).append( qApp->applicationName( ) ).append( ".ini" );
	QString directory = progressIniPath.append( "translations" );
	QLocale locale = QLocale::system( );
	QString pmFilename = locale.name( );
	pmFilename = qApp->applicationName( ).append( "_" ).append( pmFilename );

	translator = new QTranslator( );
	if( translator->load( pmFilename, directory ) ) {
		qApp->installTranslator( translator );
	} else {
		qDebug( ) << tr( u8"load error" );
	}

	progressSetting = new QSettings( progressIniFileName, QSettings::IniFormat ); // 使用路径方式存储

	setWindowTitle( tr( u8"read novels" ) );

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
	converTransparentForMouseEventsBtn->setText( QString( tr( u8"current state: [%1 transparent]" ) ).arg( flageTransparentForMouseEvents ? u8"" : tr( u8"not" ) ) );
	connect( converTransparentForMouseEventsBtn, &QPushButton::clicked, [=]( bool flage ) {
		bool attribute = !textComponent->testAttribute( Qt::WA_TransparentForMouseEvents );
		progressSetting->setValue( transparentForMouseEvents, attribute );
		textComponent->setAttribute( Qt::WA_TransparentForMouseEvents, attribute );
		converTransparentForMouseEventsBtn->setText( QString( tr( u8"current state: [%1 transparent]" ) ).arg( attribute ? u8"" : tr( u8"not" ) ) );
	} );

	////////////// 菜单

	toolsMenu = new Menu( this );
	Action *action = new Action( );
	action->setText( tr( u8"assign file setting" ) );
	toolsMenu->addAction( action );
	connect( action, &QAction::triggered, [=] {
		QString dirPath = progressSetting->value( downIniTypes, qApp->applicationDirPath( ) ).toString( );
		QString fileName = QFileDialog::getOpenFileName( nullptr, tr( u8"set file setting" ), dirPath, u8"txt(*.txt *.ini *.setting) ;; *(*)" );
		if( fileName.isEmpty( ) )
			return;
		QFile file( fileName );
		if( file.open( QIODeviceBase::ReadOnly ) ) {
			progressSetting->setValue( downIniTypes, fileName );
			QRegExp qRegExp( "\\s" );
			QByteArray readAll = file.readAll( );
			QString contents( readAll );
			QStringList list = contents.split( "\n" );
			downNovelTypes.clear( );
			for( QString subStr : list ) {
				subStr = qRegExp.removeIn( subStr );
				if( !downNovelTypes.contains( subStr ) )
					downNovelTypes.append( subStr );
			}
			textComponent->setText( downNovelTypes.join( "\n" ) );
			file.close( );
		}
	} );
}
MainWidget::~MainWidget( ) {
	progressSetting->sync( );
	delete progressSetting;
	delete translator;
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
