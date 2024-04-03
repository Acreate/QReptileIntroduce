#include "../extend/Thread/DateTimeThread.h"
#include "../extend/layout/HLayoutBox.h"
#include "../extend/layout/VLayoutBox.h"
#include "../extend/menu/Menu.h"
#include "../extend/netWork/Request.h"
#include "../extend/netWork/RequestConnect.h"
#include "../group/file/FileResult.h"
#include "../group/file/RWFileThread.h"
#include "../userHread/DebugInfo.h"
#include "MainWidget.h"
#include "NovelInfoWidget.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QRegExp>
#include <QResizeEvent>
#include <QScrollBar>
#include <qcoreapplication.h>

MainWidget::MainWidget( QWidget *parent, Qt::WindowFlags fg ) : QWidget( parent, fg ), currentFont( "Arial", 10 ), currentFontMetrics( currentFont ), drawColor( 255, 0, 0 ), compoentStrNlen( 0 ) {

	DEBUG_RUN( qDebug() << tr(u8"MainWidget::MainWidget 线程 id : ")<< QThread::currentThread( )->currentThreadId( ) );

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

	// todo : 每次文件读写操作
	fileThreadResult = rwFileThread->getFileResult( );
	//connect( fileThreadResult, &FileResult::finish, this, &MainWidget::changeTextComponentContents, Qt::QueuedConnection );
	connect( fileThreadResult, &FileResult::error, this, &MainWidget::error, Qt::QueuedConnection );

	/// 配置 路径
	QString progressIniPath = qApp->applicationDirPath( ).append( QDir::separator( ) ).append( tr( u8"ini" ) ).append( QDir::separator( ) ).append( tr( u8"progress" ) ).append( QDir::separator( ) );
	QString progressIniFileName = progressIniPath;
	progressIniFileName.append( "ini" ).append( QDir::separator( ) ).append( qApp->applicationName( ) ).append( ".ini" );
	QString directory = progressIniPath.append( "translations" );
	QLocale locale = QLocale::system( );
	QString pmFilename = locale.name( );
	pmFilename = qApp->applicationName( ).append( "_" ).append( pmFilename );

	translator = new QTranslator( );
	if( translator->load( pmFilename, directory ) )
		qApp->installTranslator( translator );
	DEBUG_RUN_CODE_FIRST( else, qDebug( ) << tr( u8"load error" ) );

	progressSetting = new QSettings( progressIniFileName, QSettings::IniFormat ); // 使用路径方式存储
	setWindowTitle( tr( u8"read novels" ) );
	// 窗口捕获鼠标
	setMouseTracking( true );

	// ui 组件
	auto oldLayout = this->layout( );
	if( oldLayout )
		delete oldLayout;

	textLine = new QLineEdit( this );

	converTransparentForMouseEventsBtn = new QPushButton( this );

	topLayout = new HLayoutBox( );
	topLayout->setSpacing( 0 );
	topLayout->addWidget( textLine );
	topLayout->addWidget( converTransparentForMouseEventsBtn );
	topLayout->setContentsMargins( 0, 0, 0, 0 );

	mainLayout = new VLayoutBox( );
	mainLayout->setSpacing( 0 );
	this->setLayout( mainLayout );
	mainLayout->setContentsMargins( 0, 0, 0, 0 );

	mainLayout->addLayout( topLayout );

	novelComponent = new NovelInfoWidget( this );
	mainLayout->addWidget( novelComponent );
	textLine->setFont( currentFont );
	textLine->setReadOnly( true );

	converTransparentForMouseEventsBtn->setAttribute( Qt::WA_TransparentForMouseEvents, true );
	textLine->setAttribute( Qt::WA_TransparentForMouseEvents, true );

	connect( converTransparentForMouseEventsBtn, &QPushButton::clicked, this, &MainWidget::changeTransparent, Qt::QueuedConnection );

	////////////// 菜单

	toolsMenu = new Menu( this );

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

	// 请求菜单
	auto requestMenu = new Menu;
	requestMenu->setTitle( tr( u8"request novel sub menu" ) );

	auto requestSettingFilePath = new Action;
	requestSettingFilePath->setText( tr( u8"设置请求网络的配置文件" ) );
	connect( requestSettingFilePath, &Action::triggered, [=]( ) {
		DEBUG_RUN( qDebug() << tr(u8"requestSettingFilePath, &Action::trigger slots") );
	} );
	requestMenu->addAction( requestSettingFilePath );

	toolsMenu->addMenu( requestMenu );
	//// 线程开始
	dateTimeThread->start( );

	bool flageTransparentForMouseEvents = progressSetting->value( transparentForMouseEvents, true ).toBool( );
	converTransparentForMouseEventsBtn->setText( QString( tr( u8"current state: [%1 transparent]" ) ).arg( flageTransparentForMouseEvents ? u8"" : tr( u8"not" ) ) );
	novelComponent->setAttribute( Qt::WA_TransparentForMouseEvents, flageTransparentForMouseEvents );
}
MainWidget::~MainWidget( ) {
	dateTimeThread->requestInterruption( );
	rwFileThread->requestInterruption( );
	progressSetting->sync( );

	delete progressSetting;
	delete translator;

	while( dateTimeThread->isRunning( ) && !dateTimeThread->isFinished( ) )
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
	DEBUG_RUN(
		static bool isOutDbug = true;
		if( isOutDbug ) {
		qDebug( ) << tr(u8"MainWidget::updateDateTimeStrFunction( const QString &currentDateTimeStr ) currentThreadId: ") << QThread::currentThread( )->currentThreadId( );
		isOutDbug = false;
		}
	);
	QString string = converTransparentForMouseEventsBtn->text( );
	qint64 newStrLen = currentDateTimeStr.length( ) + string.length( );
	if( compoentStrNlen < newStrLen ) {
		compoentStrNlen = newStrLen;
		updateWidgetWidth( { currentDateTimeStr, string } );
	}
	textLine->setText( currentDateTimeStr );
}
void MainWidget::changeTransparent( bool flage ) {
	DEBUG_RUN(
		static bool isOutDbug = true;
		if( isOutDbug ) {
		qDebug( ) <<tr( u8"MainWidget::changeTransparent( bool flage ) : ") << QThread::currentThread( )->currentThreadId( );
		isOutDbug = false;
		}
	);
	bool attribute = !novelComponent->testAttribute( Qt::WA_TransparentForMouseEvents );
	progressSetting->setValue( transparentForMouseEvents, attribute );
	progressSetting->sync( );
	novelComponent->setAttribute( Qt::WA_TransparentForMouseEvents, attribute );
	converTransparentForMouseEventsBtn->setText( QString( tr( u8"当前状态: [%1穿透]" ) ).arg( attribute ? u8"" : tr( u8"未" ) ) );
}

void MainWidget::error( int errorType, QFileDevice::FileError fileErrorCode, QFileDevice::FileError dirError ) {
	if( errorType | 1 )
		DEBUG_RUN( qDebug() << fileErrorCode ) ;
	if( errorType | 2 )
		DEBUG_RUN( qDebug() << dirError ) ;
}

void MainWidget::updateWidgetWidth( const QList< QString > &list ) {
	int width = 0;
	for( auto &str : list )
		width += currentFontMetrics.horizontalAdvance( str );
	width = width + 100;
	int thisWidthMinWidth = minimumWidth( );
	if( thisWidthMinWidth < width ) {
		setMinimumWidth( width );
		resize( width, height( ) );
		DEBUG_RUN( qDebug( ) << tr(u8"当前 MainWidget 窗口最小的宽度 : ") << width );
	}
}
