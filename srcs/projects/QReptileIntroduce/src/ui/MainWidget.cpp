#include "MainWidget.h"
#include <QMouseEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QMenu>
#include <QRegExp>
#include <QFileDialog>
#include "../Thread/DateTimeThread.h"
#include <qcoreapplication.h>

#include "../Thread/RWFileThread.h"
#include "../thread/FileThreadResult.h"

MainWidget::MainWidget( QWidget *parent, Qt::WindowFlags fg ) : QWidget( parent, fg ), compoentStrNlen( 0 ), currentFont( "Arial", 10 ), currentFontMetrics( currentFont ), drawColor( 255, 0, 0 ) {

	//qDebug( ) << "MainWidget::MainWidget : " << QThread::currentThread( )->currentThreadId( );

	///// 窗口信息的初始化

	//connect( this, &QWidget::show, [=]( ) {
	//
	//} );

	/////// 线程
	dateTimeThread = new DateTimeThread;
	dateTimeThread->setParent( this );
	connect( dateTimeThread, &DateTimeThread::updateDateTimeStr, this, &MainWidget::updateDateTimeStrFunction, Qt::QueuedConnection );

	rwFileThread = new RWFileThread( );
	rwFileThread->setParent( this );

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

	topLayout = new HLyaoutBox( );
	topLayout->setSpacing( 0 );
	topLayout->addWidget( textLine );
	topLayout->addWidget( converTransparentForMouseEventsBtn );
	topLayout->setContentsMargins( 0, 0, 0, 0 );

	mainLayout = new QVBoxLayout( this );
	mainLayout->setSpacing( 0 );
	this->setLayout( mainLayout );
	mainLayout->setContentsMargins( 0, 0, 0, 0 );

	mainLayout->addLayout( topLayout );

	textComponent = new QTextEdit( this );
	mainLayout->addWidget( textComponent );
	textLine->setFont( currentFont );
	textLine->setReadOnly( true );

	bool flageTransparentForMouseEvents = progressSetting->value( transparentForMouseEvents, true ).toBool( );
	textComponent->setAttribute( Qt::WA_TransparentForMouseEvents, flageTransparentForMouseEvents );

	converTransparentForMouseEventsBtn->setAttribute( Qt::WA_TransparentForMouseEvents, true );
	textLine->setAttribute( Qt::WA_TransparentForMouseEvents, true );
	converTransparentForMouseEventsBtn->setText( QString( tr( u8"current state: [%1 transparent]" ) ).arg( flageTransparentForMouseEvents ? u8"" : tr( u8"not" ) ) );

	connect( converTransparentForMouseEventsBtn, &QPushButton::clicked, this, &MainWidget::changeTransparent, Qt::QueuedConnection );

	////////////// 菜单

	toolsMenu = new Menu( this );

	//connect( toolsMenu, &QMenu::aboutToShow, [=]( ) {
	//	showCount = 1;
	//} );
	connect( toolsMenu, &QMenu::aboutToHide, [=]( ) {
		auto point = QCursor::pos( );
		auto pos = this->pos( );
		point -= pos;
		point.setY( point.y( ) - titleHeight );
		QRect geometry = converTransparentForMouseEventsBtn->geometry( );
		if( geometry.contains( point ) )
			showCount = 1;
	} );
	connect( toolsMenu, &QMenu::triggered, [=]( ) {
		showCount = 0;
	} );

	Action *selectSettingFileDialogAcion = new Action( );
	selectSettingFileDialogAcion->setText( tr( u8"assign file setting" ) );
	toolsMenu->addAction( selectSettingFileDialogAcion );
	connect( selectSettingFileDialogAcion, &QAction::triggered, [this] {
		QString dirPath = progressSetting->value( downIniTypes, qApp->applicationDirPath( ) ).toString( );
		QString fileName = QFileDialog::getOpenFileName( nullptr, tr( u8"set file setting" ), dirPath, u8"txt(*.txt *.ini *.setting) ;; *(*)" );
		if( fileName.isEmpty( ) )
			return;
		QFile file( fileName );
		if( file.open( QIODeviceBase::ReadOnly ) ) {
			progressSetting->setValue( downIniTypes, fileName );
			progressSetting->sync( );
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

	Action *readFileAction = new Action( );
	readFileAction->setText( tr( u8"readFile" ) );
	toolsMenu->addAction( readFileAction );
	connect( readFileAction, &QAction::triggered, [this] {
		QString dirPath = progressSetting->value( selectReadFileWorkPath, qApp->applicationDirPath( ) ).toString( );
		QString fileName = QFileDialog::getOpenFileName( nullptr, tr( u8"select file read to text editor" ), dirPath, u8"*(*)" );
		if( fileName.isEmpty( ) )
			return;
		progressSetting->setValue( selectReadFileWorkPath, fileName );
		progressSetting->sync( );
		rwFileThread->setFilePath( fileName );
		fileThreadResult = rwFileThread->readFile( );
		connect( fileThreadResult, &FileThreadResult::finish, this, &MainWidget::changeTextComponentContents, Qt::QueuedConnection );
		rwFileThread->start( );
	} );

	Action *writeAction = new Action;
	writeAction->setText( tr( u8"writeFile" ) );
	toolsMenu->addAction( writeAction );
	connect( writeAction, &QAction::triggered, [this] {
		QString dirPath = progressSetting->value( selectWriteFileWorkPath, qApp->applicationDirPath( ) ).toString( );
		QString fileName = QFileDialog::getSaveFileName( nullptr, tr( u8"select file write to text editor" ), dirPath, u8"*(*)" );
		progressSetting->setValue( selectWriteFileWorkPath, fileName );
		progressSetting->sync( );
		rwFileThread->setFilePath( fileName );
		fileThreadResult = rwFileThread->writeFile( fileThreadResult->getData( ) );
		if( !fileThreadResult )
			return;
		textComponent->clear( );
		connect( fileThreadResult, &FileThreadResult::finish, this, &MainWidget::changeTextComponentContents, Qt::QueuedConnection );
		rwFileThread->start( );
	} );
	//// 线程开始
	dateTimeThread->start( );

}
MainWidget::~MainWidget( ) {
	progressSetting->sync( );
	dateTimeThread->requestInterruption( );
	rwFileThread->requestInterruption( );

	delete progressSetting;
	delete translator;

	while( !dateTimeThread->isFinished( ) )
		QThread::usleep( 20 );

	while( !rwFileThread->isFinished( ) )
		QThread::usleep( 20 );
}
QFont MainWidget::setFont( QFont &font ) {
	auto oldFont = currentFont;
	currentFont = font;
	currentFontMetrics = QFontMetrics( currentFont );
	textLine->setFont( currentFont );
	return oldFont;
}

void MainWidget::mouseMoveEvent( QMouseEvent *event ) {
}
void MainWidget::mouseReleaseEvent( QMouseEvent *event ) {
	Qt::MouseButton mouseButton = event->button( );
	bool contains = converTransparentForMouseEventsBtn->geometry( ).contains( event->pos( ) );
	if( contains )
		switch( mouseButton ) {
			case Qt::RightButton : {
				QPointF position = event->globalPosition( );
				toolsMenu->exec( QPoint( position.x( ), position.y( ) ) );
			}
			break;
		}
}
void MainWidget::mousePressEvent( QMouseEvent *event ) {
	Qt::MouseButton mouseButton = event->button( );
	bool contains = converTransparentForMouseEventsBtn->geometry( ).contains( event->pos( ) );
	if( contains )
		switch( mouseButton ) {
			case Qt::LeftButton :
				if( showCount == 0 )
					emit converTransparentForMouseEventsBtn->clicked( true );
				else
					showCount = 0;
				break;
		}
}
void MainWidget::resizeEvent( QResizeEvent *event ) {
	titleHeight = frameSize( ).height( ) - event->size( ).height( );
}
void MainWidget::updateDateTimeStrFunction( const QString &currentDateTimeStr ) {
	//static bool isOutDbug = true;
	//if( isOutDbug ) {
	//	qDebug( ) << "MainWidget::updateDateTimeStrFunction( const QString &currentDateTimeStr ) : " << QThread::currentThread( )->currentThreadId( );
	//	isOutDbug = false;
	//}
	QString string = converTransparentForMouseEventsBtn->text( );
	qint64 newStrLen = currentDateTimeStr.length( ) + string.length( );
	if( compoentStrNlen < newStrLen ) {
		compoentStrNlen = newStrLen;
		updateWidgetWidth( { currentDateTimeStr, string } );
	}
	textLine->setText( currentDateTimeStr );
}
void MainWidget::changeTransparent( bool flage ) {
	//static bool isOutDbug = true;
	//if( isOutDbug ) {
	//	qDebug( ) << "MainWidget::changeTransparent( bool flage ) : " << QThread::currentThread( )->currentThreadId( );
	//	isOutDbug = false;
	//}
	bool attribute = !textComponent->testAttribute( Qt::WA_TransparentForMouseEvents );
	progressSetting->setValue( transparentForMouseEvents, attribute );
	progressSetting->sync( );
	textComponent->setAttribute( Qt::WA_TransparentForMouseEvents, attribute );
	converTransparentForMouseEventsBtn->setText( QString( tr( u8"current state: [%1 transparent]" ) ).arg( attribute ? u8"" : tr( u8"not" ) ) );
}
void MainWidget::changeTextComponentContents( ) {
	textComponent->setText( fileThreadResult->getData( ) );
}

void MainWidget::updateWidgetWidth( const QList< QString > &list ) {
	int width = 0;
	for( auto &str : list )
		width += currentFontMetrics.horizontalAdvance( str );
	width = width + 70;
	int thisWidthMinWidth = minimumWidth( );
	if( thisWidthMinWidth < width ) {
		setMinimumWidth( width );
		//qDebug( ) << "最小宽度 : " << width;
	}

}
