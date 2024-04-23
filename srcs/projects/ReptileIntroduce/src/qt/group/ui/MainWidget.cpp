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
#include "MainWidget/JobWidget.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QRegExp>
#include <QResizeEvent>
#include <QScrollBar>
#include <qcoreapplication.h>

#include "../setting/Setting.h"

const QString MainWidget::qstrPoint = tr( u8"坐标:( %1 , %2 )" );

const QString MainWidget::transparentForMouseEvents = tr( u8"somponentStyle/TransparentForMouseEvents" );
const QString MainWidget::selectReadFileWorkPath = tr( u8"work/ReadDirPath" );
const QString MainWidget::selectWriteFileWorkPath = tr( u8"work/WriteDirPath" );
const QString MainWidget::selectWebSettingPath = tr( u8"web/SettingFilePath" );

const QString MainWidget::transparentText = tr( u8"当前状态: [%1穿透]" );
const QString MainWidget::transparentTextNot = tr( u8"未" );
void MainWidget::initMumberPtrMemory( ) {
	dateTimeThread = new DateTimeThread;
	rwFileThread = new RWFileThread( );
	translator = new QTranslator( );
	textLine = new QLineEdit( this );
	converTransparentForMouseEventsBtn = new QPushButton( this );
	topLayout = new HLayoutBox( );
	mainLayout = new VLayoutBox( );

	novelComponent = new NovelInfoWidget( this );
	jobWidget = new JobWidget( this );
	toolsMenu = new Menu( this );
	loadSettingFile = new Action;
}
void MainWidget::initComponentPropertys( ) {
	dateTimeThread->setParent( this );
	rwFileThread->setParent( this );
	fileThreadResult = rwFileThread->getFileResult( );

	// progressIniFileName, QSettings::IniFormat
	setWindowTitle( tr( u8"小说阅读" ) );
	// 窗口捕获鼠标
	setMouseTracking( true );

	/// 配置 路径
	QString progressIniPath = qApp->applicationDirPath( ).append( QDir::separator( ) ).append( tr( u8"progress" ) ).append( QDir::separator( ) );
	QString progressIniFileName = progressIniPath;
	progressIniFileName.append( "ini" ).append( QDir::separator( ) ).append( qApp->applicationName( ) ).append( ".ini" );
	QString directory = progressIniPath;
	directory.append( "translations" );
	QLocale locale = QLocale::system( );
	QString pmFilename = QString( u8"QReptileIntroduce_" ).append( locale.name( ) );

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
	progressSetting = new Setting( progressIniFileName, this );
	textLine->setReadOnly( true );
	converTransparentForMouseEventsBtn->setAttribute( Qt::WA_TransparentForMouseEvents, true );
	textLine->setAttribute( Qt::WA_TransparentForMouseEvents, true );

	loadSettingFile->setText( tr( u8"加载配置文件" ) );
}
void MainWidget::initComponentLayout( ) {
	// ui 组件
	auto oldLayout = this->layout( );
	if( oldLayout )
		delete oldLayout;

	topLayout->setSpacing( 0 );
	topLayout->addWidget( textLine );
	topLayout->addWidget( converTransparentForMouseEventsBtn );
	topLayout->setContentsMargins( 0, 0, 0, 0 );

	mainLayout->setSpacing( 0 );
	this->setLayout( mainLayout );
	mainLayout->setContentsMargins( 0, 0, 0, 0 );
	textLine->setFont( currentFont );

	mainLayout->addLayout( topLayout );

	mainLayout->addWidget( novelComponent );
	mainLayout->addWidget( jobWidget );
	toolsMenu->addAction( loadSettingFile );
}
void MainWidget::initComponentConnect( ) {
	connect( dateTimeThread, &DateTimeThread::updateDateTimeStr, this, &MainWidget::updateDateTimeStrFunction, Qt::QueuedConnection );
	connect( fileThreadResult, &FileResult::error, this, &MainWidget::error, Qt::QueuedConnection );

	connect( converTransparentForMouseEventsBtn, &QPushButton::clicked, this, &MainWidget::changeTransparent, Qt::QueuedConnection );

	connect( jobWidget, &JobWidget::click, [=]( ) {
		emit novelComponent->clickRequestStart( );
		emit jobWidget->setProgressValue( 100 );
	} );
	////////////// 菜单
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

	connect( loadSettingFile, &Action::triggered, [=]( ) {
		DEBUG_RUN( qDebug() << tr(u8"requestSettingFilePath, &Action::trigger slots") );
		changeWebComponents( );
	} );
}
void MainWidget::initComponentOver( ) {
	//// 线程开始
	dateTimeThread->start( );
	bool flageTransparentForMouseEvents = progressSetting->getValue( transparentForMouseEvents, []( const QVariant &getValue ) {
		if( getValue.isNull( ) )
			return true;
		return getValue.toBool( );
	} ).toBool( );

	converTransparentForMouseEventsBtn->setText( QString( transparentText ).arg( flageTransparentForMouseEvents ? u8"" : transparentTextNot ) );
	novelComponent->setAttribute( Qt::WA_TransparentForMouseEvents, flageTransparentForMouseEvents );

	/// 配置文件设置
	auto variant = progressSetting->getValue( selectWebSettingPath );
	if( !variant.isNull( ) ) {
		QDir current( qApp->applicationDirPath( ) );
		QFileInfo info( variant.toString( ) );
		if( info.exists( ) ) {
			QString absoluteFilePath = info.absoluteFilePath( );
			auto relativeFilePath = current.relativeFilePath( absoluteFilePath );
			progressSetting->setValue( selectWebSettingPath, relativeFilePath );
			novelComponent->setNetWorkSettingFilePath( relativeFilePath );
		} else {
			do {
				QString fileName = QFileDialog::getOpenFileName( this, tr( u8"选择一个配置文件" ), qApp->applicationDirPath( ), tr( u8"配置文件(*.set *.setting *.ini);;文本文件(*.txt);;全部文件(*)" ) );
				if( fileName.isEmpty( ) && QMessageBox::question( this, tr( u8"重新选择一个有效文件" ), tr( u8"是否重新选择一个有效的文件？" ) ) == QMessageBox::Yes )
					continue;
				auto relativeFilePath = current.relativeFilePath( fileName );
				progressSetting->setValue( selectWebSettingPath, relativeFilePath );
				progressSetting->sync( );
				novelComponent->setNetWorkSettingFilePath( relativeFilePath );
				break;
			} while( true );
		}
	}
	variant = progressSetting->getValue( selectReadFileWorkPath );
	QDir dir( qApp->applicationDirPath( ) );
	if( variant.isNull( ) || variant.toString( ).isEmpty( ) ) {
		auto defaultPath = dir.relativeFilePath( qApp->applicationDirPath( ) + QDir::separator( ) + "read_dir_path" );
		progressSetting->setValue( selectReadFileWorkPath, defaultPath );
	} else {
		auto defaultPath = dir.relativeFilePath( variant.toString( ) );
		progressSetting->setValue( selectReadFileWorkPath, defaultPath );
	}
	variant = progressSetting->getValue( selectWriteFileWorkPath );
	if( variant.isNull( ) || variant.toString( ).isEmpty( ) ) {
		auto defaultPath = dir.relativeFilePath( qApp->applicationDirPath( ) + QDir::separator( ) + "write_dir_path" );
		progressSetting->setValue( selectWriteFileWorkPath, defaultPath );
	} else {
		auto defaultPath = dir.relativeFilePath( variant.toString( ) );
		progressSetting->setValue( selectWriteFileWorkPath, defaultPath );
	}

	QDateTime dateTime;
	QDate date;
	date.setDate( 2004, 10, 10 );
	QTime time;
	time.setHMS( 10, 10, 10, 999 );
	MainWidget::updateDateTimeStrFunction( QString( "%1 - %2" ).arg( date.toString( u8"yyyy 年 MM 月 dd 日" ) ).arg( time.toString( u8"hh 时 mm 分 ss 秒 zzz" ) ) );
}
MainWidget::MainWidget( QWidget *parent, Qt::WindowFlags fg ) : QWidget( parent, fg ), currentFont( "Arial", 10 ), currentFontMetrics( currentFont ), drawColor( 255, 0, 0 ), compoentStrNlen( 0 ) {

	DEBUG_RUN( qDebug() << tr(u8"MainWidget::MainWidget 线程 id : ")<< QThread::currentThread( )->currentThreadId( ) );

	initMumberPtrMemory( );
	initComponentPropertys( );
	initComponentLayout( );

	initComponentConnect( );

	initComponentOver( );
}
MainWidget::~MainWidget( ) {
	dateTimeThread->requestInterruption( );
	rwFileThread->requestInterruption( );
	progressSetting->sync( );

	delete progressSetting;
	delete translator;

	while( dateTimeThread->isRunning( ) && !dateTimeThread->isFinished( ) )
		QThread::usleep( 20 );

	while( dateTimeThread->isRunning( ) && !rwFileThread->isFinished( ) )
		rwFileThread->await( );
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
	auto variant = progressSetting->getValue( selectWebSettingPath );
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
			progressSetting->sync( );
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
