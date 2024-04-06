#include "../../extend/Thread/DateTimeThread.h"
#include "../../extend/layout/HLayoutBox.h"
#include "../../extend/layout/VLayoutBox.h"
#include "../../extend/menu/Menu.h"
#include "../../extend/netWork/Request.h"
#include "../../extend/netWork/RequestConnect.h"
#include "../../group/file/FileResult.h"
#include "../../group/file/RWFileThread.h"
#include <DebugInfo.h>
#include "MainWidget.h"
#include "MainWidget/NovelInfoWidget.h"
#include "MainWidget/NovelInfoWidget/WebUrlInfoWidget.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QRegExp>
#include <QResizeEvent>
#include <QScrollBar>
#include <qcoreapplication.h>

const QString MainWidget::qstrPoint = tr( u8"坐标:( %1 , %2 )" );

const QString MainWidget::transparentForMouseEvents = tr( u8"somponentStyle/TransparentForMouseEvents" );
const QString MainWidget::selectReadFileWorkPath = tr( u8"work/ReadDirPath" );
const QString MainWidget::selectWriteFileWorkPath = tr( u8"work/WriteDirPath" );
const QString MainWidget::selectWebSettingPath = tr( u8"web/SettingFilePath" );

const QString MainWidget::transparentText = tr( u8"当前状态: [%1穿透]" );
const QString MainWidget::transparentTextNot = tr( u8"未" );
MainWidget::MainWidget( QWidget *parent, Qt::WindowFlags fg ) : QWidget( parent, fg ), currentFont( "Arial", 10 ), currentFontMetrics( currentFont ), drawColor( 255, 0, 0 ), compoentStrNlen( 0 ) {

	DEBUG_RUN( qDebug() << tr(u8"MainWidget::MainWidget 线程 id : ")<< QThread::currentThread( )->currentThreadId( ) );

	///// 本实例对象实现槽函数

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
	QString progressIniPath = qApp->applicationDirPath( ).append( QDir::separator( ) ).append( tr( u8"progress" ) ).append( QDir::separator( ) );
	QString progressIniFileName = progressIniPath;
	progressIniFileName.append( "ini" ).append( QDir::separator( ) ).append( qApp->applicationName( ) ).append( ".ini" );
	QString directory = progressIniPath;
	directory.append( "translations" );
	QLocale locale = QLocale::system( );
	QString pmFilename = QString( u8"QReptileIntroduce_" ).append( locale.name( ) );

	translator = new QTranslator( );
	if( translator->load( pmFilename, directory ) )
		qApp->installTranslator( translator );
	DEBUG_RUN_CODE_ELSE_END( qDebug( ) << tr( u8"翻译(*.pm)文件加载错误" ) );

	DEBUG_RUN(
		QFileInfo info(progressIniFileName);
		auto absPath = info.absoluteFilePath( );
		if(!info.exists( )) {
		qDebug() << "path (" << absPath << ") is not exists;";
		}else {
		qDebug() << "path (" << absPath<< ") has setting file;";
		}
	);

	progressSetting = new QSettings( progressIniFileName, QSettings::IniFormat ); // 使用路径方式存储
	setWindowTitle( tr( u8"小说阅读" ) );
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

	auto loadSettingFile = new Action;
	loadSettingFile->setText( tr( u8"加载配置文件" ) );
	connect( loadSettingFile, &Action::triggered, [=]( ) {
		DEBUG_RUN( qDebug() << tr(u8"requestSettingFilePath, &Action::trigger slots") );
		changeWebComponents( );
	} );
	toolsMenu->addAction( loadSettingFile );

	//// 线程开始
	dateTimeThread->start( );

	bool flageTransparentForMouseEvents = progressSetting->value( transparentForMouseEvents, true ).toBool( );
	converTransparentForMouseEventsBtn->setText( QString( transparentText ).arg( flageTransparentForMouseEvents ? u8"" : transparentTextNot ) );
	novelComponent->setAttribute( Qt::WA_TransparentForMouseEvents, flageTransparentForMouseEvents );

	/// 配置文件设置
	auto variant = progressSetting->value( selectWebSettingPath );
	if( !variant.isNull( ) ) {
		QFileInfo info( variant.toString( ) );
		if( info.exists( ) )
			novelComponent->setNetWorkSettingFilePath( info.absoluteFilePath( ) );
		else {
			do {
				QString fileName = QFileDialog::getOpenFileName( this, tr( u8"选择一个配置文件" ), qApp->applicationDirPath( ), tr( u8"配置文件(*.set *.setting *.ini);;文本文件(*.txt);;全部文件(*)" ) );
				if( fileName.isEmpty( ) && QMessageBox::question( this, tr( u8"重新选择一个有效文件" ), tr( u8"是否重新选择一个有效的文件？" ) ) == QMessageBox::Yes )
					continue;
				progressSetting->setValue( selectWebSettingPath, fileName );
				progressSetting->sync( );
				novelComponent->setNetWorkSettingFilePath( fileName );
				break;
			} while( true );
		}
	}
	variant = progressSetting->value( selectReadFileWorkPath );
	if( variant.isNull( ) || variant.toString( ).isEmpty( ) )
		progressSetting->setValue( selectReadFileWorkPath, qApp->applicationDirPath( ) );
	variant = progressSetting->value( selectWriteFileWorkPath );
	if( variant.isNull( ) || variant.toString( ).isEmpty( ) )
		progressSetting->setValue( selectWriteFileWorkPath, qApp->applicationDirPath( ) );
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
	QWidget::mouseMoveEvent( event );
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
	QWidget::mouseReleaseEvent( event );
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

	QWidget::mousePressEvent( event );
}
void MainWidget::resizeEvent( QResizeEvent *event ) {
	titleHeight = frameSize( ).height( ) - event->size( ).height( );

	// raise(  );
	QWidget::resizeEvent( event );
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
	converTransparentForMouseEventsBtn->setText( QString( transparentText ).arg( attribute ? u8"" : transparentTextNot ) );
}

void MainWidget::error( int errorType, QFileDevice::FileError fileErrorCode, QFileDevice::FileError dirError ) {
	if( errorType | 1 )
		DEBUG_RUN( qDebug() << fileErrorCode ) ;
	if( errorType | 2 )
		DEBUG_RUN( qDebug() << dirError ) ;
}
void MainWidget::changeWebComponents( ) {
	auto variant = progressSetting->value( selectWebSettingPath );
	QString filePath;
	if( !variant.isNull( ) )
		filePath = variant.toString( );
	QFileInfo info( filePath );
	if( info.exists( ) )
		filePath = info.absoluteDir( ).absolutePath( );
	else
		filePath = qApp->applicationDirPath( );
	do {
		filePath = QFileDialog::getOpenFileName( this, tr( u8"指定一个 web 配置文件" ), filePath, tr( u8"配置类型(*.set *.ini *.setting );;全部类型(*)" ) );
		info.setFile( filePath );
		if( info.exists( ) ) {
			QString absoluteFilePath = info.absoluteFilePath( );
			progressSetting->setValue( selectWebSettingPath, absoluteFilePath );
			novelComponent->setNetWorkSettingFilePath( absoluteFilePath );

			// 触发信号
			emit changeOverWebComponentSetting( );
			break;
		}
		if( QMessageBox::question( this, tr( u8"请选择" ), tr( u8"文件错误，是否重新选择？" ) ) == QMessageBox::No )
			break;
	} while( true );
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
