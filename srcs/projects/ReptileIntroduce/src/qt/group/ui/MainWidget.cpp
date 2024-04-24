#include "MainWidget.h"
#include <QTranslator>
#include <QTimer>

#include "../../extend/Thread/DateTimeThread.h"
#include "../../extend/layout/HLayoutBox.h"
#include "../../extend/layout/VLayoutBox.h"
#include "../../extend/ui/DisplayWidget.h"
#include "FileSelectPathWidget.h"
#include <DebugInfo.h>
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

const QString MainWidget::selectReadFileWorkPath = tr( u8"work/ReadDirPath" );
const QString MainWidget::selectWriteFileWorkPath = tr( u8"work/WriteDirPath" );
const QString MainWidget::selectWebSettingPath = tr( u8"web/SettingFilePath" );

void MainWidget::initMumberPtrMemory( ) {
	dateTimeThread = new DateTimeThread;
	progressSetting = new Setting( this );
	translator = new QTranslator( this );
	selectPathWidget = new FileSelectPathWidget( this );
	display = new DisplayWidget( this );
}
void MainWidget::initComponentPropertys( ) {
	dateTimeThread->setParent( this );

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

	QFileInfo info( progressIniFileName );
	auto absPath = info.absoluteFilePath( );
	if( !info.exists( ) ) {
		qDebug( ) << "path (" << absPath << ") is not exists;";
	} else {
		qDebug( ) << "path (" << absPath << ") has setting file;";
	}
	updateSettingFileInfo( progressIniFileName );
	emit selectPathWidget->setPath( progressIniFileName );
}
void MainWidget::initComponentLayout( ) {
	// ui 组件
	auto oldLayout = this->layout( );
	if( oldLayout )
		delete oldLayout;

	mainVLayoutBox = new VLayoutBox( this );
	mainVLayoutBox->addWidget( selectPathWidget, 1 );
	mainVLayoutBox->addWidget( display, 19 );
}
void MainWidget::initComponentConnect( ) {
	connect( dateTimeThread, &DateTimeThread::updateDateTimeStr, this, &MainWidget::updateDateTimeStrFunction, Qt::QueuedConnection );
	connect( selectPathWidget, &FileSelectPathWidget::editorOver, [&]( const QString &newFilePath ) {
		updateSettingFileInfo( newFilePath );
	} );
	connect( selectPathWidget, &FileSelectPathWidget::setFilePathFinish, this, [&]( const QString &filePath ) {
		updateSettingFileInfo( filePath );
	} );
}
void MainWidget::initComponentOver( ) {
	//// 线程开始
	dateTimeThread->start( );
	emit selectPathWidget->setPath( QDir( qApp->applicationDirPath( ) ).relativeFilePath( progressSetting->getFilePath( ) ) );
}
MainWidget::MainWidget( QWidget *parent, Qt::WindowFlags fg ) : QWidget( parent, fg ) {

	DEBUG_RUN( qDebug() << tr(u8"MainWidget::MainWidget 线程 id : ")<< QThread::currentThread( )->currentThreadId( ) );

	initMumberPtrMemory( );
	initComponentPropertys( );
	initComponentLayout( );

	initComponentConnect( );

	initComponentOver( );
}
MainWidget::~MainWidget( ) {
	dateTimeThread->requestInterruption( );
	progressSetting->sync( );

	delete progressSetting;
	delete translator;

	while( dateTimeThread->isRunning( ) && !dateTimeThread->isFinished( ) )
		QThread::usleep( 20 );

	QThread::usleep( 20 );
}

void MainWidget::mouseMoveEvent( QMouseEvent *event ) {
	QWidget::mouseMoveEvent( event );
}
void MainWidget::mouseReleaseEvent( QMouseEvent *event ) {
	Qt::MouseButton mouseButton = event->button( );
	QWidget::mouseReleaseEvent( event );
}
void MainWidget::mousePressEvent( QMouseEvent *event ) {
	Qt::MouseButton mouseButton = event->button( );

	QWidget::mousePressEvent( event );
}
void MainWidget::resizeEvent( QResizeEvent *event ) {

	QWidget::resizeEvent( event );
}

bool MainWidget::updateSettingFileInfo( const QString &filePath ) {
	if( progressSetting->setFilePath( filePath ) ) {
		
	}
	return false;
}
void MainWidget::updateDateTimeStrFunction( const QString &currentDateTimeStr ) {

}
