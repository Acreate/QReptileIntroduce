#include "MainWidget.h"
#include <QTranslator>
#include <QTimer>
#include <QPushButton>
#include <QPlainTextEdit>

#include <Thread/DateTimeThread.h>
#include <plug/LoadPlug.h>
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

#include <setting/Setting.h>
#include <interface/IRequestNetInterfaceExtend.h>
#include <path/Path.h>

#include "../../extend/layout/HLayoutBox.h"
#include "../../extend/layout/VLayoutBox.h"
#include "../../extend/ui/DisplayWidget.h"
#include "FileSelectPathWidget.h"
#include "../../extend/menu/Action.h"
#include "../../extend/menu/Menu.h"
#include "novelNetJob/NovelNetJob.h"

const QString MainWidget::settingGroupWork = tr( u8"工作" );
const QString MainWidget::settingGroupWeb = tr( u8"网络" );
const QString MainWidget::settingGroupWebKey = tr( u8"配置文件路径" );
const QString MainWidget::settingGroupWorkKeyReadFileWorkPath = tr( u8"读取路径" );
const QString MainWidget::settingGroupWorkKeyWriteFileWorkPath = tr( u8"写入路径" );
const QString MainWidget::settingGroupSelectDefaultPaths = tr( u8"选择窗口默认路径" );
const QString MainWidget::settingGroupSelectDefaultPathskeyPlugPathKey = tr( u8"插件路径" );
const QString MainWidget::settingGroupPlugIniPathKeyMerge = tr( u8"插件/配置路径" );
const QChar MainWidget::settingPathSep = tr( u8";" )[ 0 ];

void MainWidget::initMumberPtrMemory( ) {
	dateTimeThread = new DateTimeThread;
	progressSetting = new Setting( this );
	translator = new QTranslator( this );
	selectPathWidget = new FileSelectPathWidget( this );
	display = new DisplayWidget( this );
	LoadWebInfoBtn = new Action;
}
void MainWidget::initComponentPropertys( ) {
	dateTimeThread->setParent( this );


	setWindowTitle( tr( u8"小说阅读" ) );
	// 窗口捕获鼠标
	setMouseTracking( true );
	setMinimumSize( 800, 500 );
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
		qDebug( ) << "路径 (" << absPath << ") 不存在;";
	} else {
		qDebug( ) << "路径 (" << absPath << ") 有一个配置文件;";
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
	connect( display, &DisplayWidget::menuActionClick, [=]( const QString &xpath ) {
		qDebug( ) << xpath;
	} );
}
void MainWidget::initComponentOver( ) {
	//// 线程开始
	dateTimeThread->start( );
	emit selectPathWidget->setPath( QDir( qApp->applicationDirPath( ) ).relativeFilePath( progressSetting->getFilePath( ) ) );
	fromDisplayWidgetMenu = display->getMenu( this );
	fromDisplayWidgetMenu->setTitle( windowTitle( ) );
	Action *loadPlug = new Action;
	loadPlug->setText( tr( u8"插件路径..." ) );
	connect( loadPlug, &QAction::triggered, this, &MainWidget::showSelectPlugPathDialog );
	fromDisplayWidgetMenu->addAction( loadPlug );
	loadPlug = new Action;
	loadPlug->setText( tr( u8"加载插件" ) );
	fromDisplayWidgetMenu->addAction( loadPlug );
	connect( loadPlug, &QAction::triggered, this, &MainWidget::loadingPlug );
	LoadWebInfoBtn->setText( tr( u8"开始任务" ) );
	fromDisplayWidgetMenu->addAction( LoadWebInfoBtn );
	connect( LoadWebInfoBtn, &QAction::triggered, this, &MainWidget::LoadWebInfo );
	loadingPlug( );
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
		qsizetype webSettingMapSize = webSettingMap.size( );
		auto value = progressSetting->getValue( settingGroupPlugIniPathKeyMerge ).toString( );
		if( value.isEmpty( ) )
			return false;
		QStringList stringList = value.split( settingPathSep );
		QFileInfo fileInfo;
		for( auto &path : stringList ) {
			fileInfo.setFile( path );
			if( fileInfo.exists( ) ) {
				QString absolutePath = fileInfo.absolutePath( );
				if( webSettingMap.contains( absolutePath ) ) // 存在则跳过
					continue;
				webSettingMap.insert( absolutePath, new Setting( absolutePath, this ) );
			}
		}
		if( webSettingMapSize > webSettingMap.size( ) )
			return true;
	}
	return false;
}
void MainWidget::updateDateTimeStrFunction( const QString &currentDateTimeStr ) {

}
void MainWidget::showSelectPlugPathDialog( ) {
	auto variant = progressSetting->getValue( settingGroupSelectDefaultPaths );
	auto plugSelectPath = variant.toString( );
	if( plugSelectPath.isEmpty( ) )
		plugSelectPath = qApp->applicationDirPath( );
	do {
		auto openFileName = QFileDialog::getOpenFileName( this, tr( u8"选择一个插件路径" ), plugSelectPath );
		if( openFileName.isEmpty( ) ) {
			if( QMessageBox::question( this, tr( u8"路径异常" ), tr( u8"需要重新选择路径吗？" ) ) == QMessageBox::Ok )
				continue;
			return;
		}
		plugSelectPath = openFileName;
		break;
	} while( true );
	QFileInfo info( plugSelectPath );
	QString fileDirPath = info.absoluteDir( ).path( );
	QDir qDir( qApp->applicationDirPath( ) );
	QString relativeFilePath = qDir.relativeFilePath( fileDirPath );
	progressSetting->setValue( settingGroupSelectDefaultPaths, relativeFilePath );
	plugSelectPath = progressSetting->getValue( settingGroupPlugIniPathKeyMerge ).toString( );
	if( plugSelectPath.isEmpty( ) )
		progressSetting->setValue( settingGroupPlugIniPathKeyMerge, relativeFilePath );
	else {
		// 检测是否存在相同
		QStringList paths;
		auto stringList = plugSelectPath.split( settingPathSep );
		size_t index = 0;
		for( auto &path : stringList ) {
			QString filePath = qDir.relativeFilePath( path );
			paths << filePath;
			if( filePath == relativeFilePath )
				continue;
			++index;
		}
		if( index == paths.size( ) )
			paths << relativeFilePath;
		paths.sort( );
		progressSetting->setValue( settingGroupPlugIniPathKeyMerge, paths.join( settingPathSep ) );
		progressSetting->sync( );
	}
	loadingPlug( );
}
void MainWidget::loadingPlug( ) {
	auto variant = progressSetting->getValue( settingGroupPlugIniPathKeyMerge ).toString( );
	if( variant.isEmpty( ) ) {
		emit display->display( QString( tr( u8"没有发现配置文件的(%1)值内容" ) ).arg( settingGroupPlugIniPathKeyMerge ) );
		return;
	}
	auto stringList = variant.split( settingPathSep );
	if( stringList.size( ) == 0 ) {
		emit display->display( QString( tr( u8"配置文件的(%1)值内容异常，切分过后不存在路径" ) ).arg( settingGroupPlugIniPathKeyMerge ) );
		return;
	}
	for( auto &plugSelectPath : stringList ) {
		auto pathInfo = Path::getPathInfo( plugSelectPath );
		for( auto &file : pathInfo.second ) {
			QString filePtah = file.getCurrentFilePtah( );
			if( plugs.contains( filePtah ) )
				continue;
			LoadPlug load( filePtah );
			auto requestNetInterfaceExtends = load.loadPlugs( );
			if( requestNetInterfaceExtends.size( ) == 0 )
				continue;
			auto iterator = requestNetInterfaceExtends.begin( );
			auto end = requestNetInterfaceExtends.end( );
			std::string outUrl;
			for( ; iterator != end; ++iterator ) {
				IRequestNetInterfaceExtend *requestNetInterfaceExtend = iterator.value( );
				IRequestNetInterfaceExtend *interfaceExtend = requestNetInterfaceExtend;
				if( interfaceExtend->getUrl( &outUrl ) )
					emit display->display( QString( tr( u8"获取url : %1" ) ).arg( QString::fromStdString( outUrl ) ) );
				outUrl.clear( );
				interfaceExtend->setInterfaceParent( this );
				QObject *object = iterator.key( );
				plugs.insert( filePtah, QSharedPointer< NovelNetJob >{ new NovelNetJob( this, object, requestNetInterfaceExtend ) } );
			}
		}

	}
	if( plugs.size( ) == 0 )
		LoadWebInfoBtn->setEnabled( false );
	else
		LoadWebInfoBtn->setEnabled( true );
}
void MainWidget::LoadWebInfo( ) {
	auto iterator = plugs.begin( );
	auto end = plugs.end( );
	std::string outUrl;
	for( ; iterator != end; ++iterator ) {
		qsizetype indexOf = iterator.key( ).indexOf( "www.121ds.cc" );
		if( indexOf != -1 ) {
			QSharedPointer< NovelNetJob > netJob = iterator.value( );
			netJob->start( nullptr );
			netJob->getUrl( &outUrl );
			*display << outUrl << __FILE__ << __LINE__;
		}
	}
}
