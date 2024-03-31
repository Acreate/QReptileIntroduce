#include "MainWidget.h"
#include <QMouseEvent>
#include <QResizeEvent>
#include <QScrollBar>
#include <QMenu>
#include <QRegExp>
#include <QFileDialog>
#include "../Thread/DateTimeThread.h"
#include <qcoreapplication.h>

#include "../file/RWFileThread.h"
#include "../file/FileResult.h"
#include "../netWork/Request.h"
#include "../netWork/RequestConnect.h"
#include "../userHread/DebugInfo.h"
#include <QInputDialog>
#include <QMessageBox>

#include "NovelInfoWidget.h"

#include "../layout/VLayoutBox.h"

MainWidget::MainWidget( QWidget *parent, Qt::WindowFlags fg ) : QWidget( parent, fg ), compoentStrNlen( 0 ), currentFont( "Arial", 10 ), currentFontMetrics( currentFont ), drawColor( 255, 0, 0 ) {

	DEBUG_RUN( qDebug() << tr(u8"MainWidget::MainWidget currentThreadId : ")<< QThread::currentThread( )->currentThreadId( ) );

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

	topLayout = new HLyaoutBox( );
	topLayout->setSpacing( 0 );
	topLayout->addWidget( textLine );
	topLayout->addWidget( converTransparentForMouseEventsBtn );
	topLayout->setContentsMargins( 0, 0, 0, 0 );

	mainLayout = new VLayoutBox( );
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
		progressSetting->setValue( downIniTypes, fileName );
		progressSetting->sync( );
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
		auto result = rwFileThread->readFile( );
		if( !result )
			return;
		textComponent->clear( );
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
		auto result = rwFileThread->writeFile( textComponent->toPlainText( ).toUtf8( ) );
		if( !result )
			return;
		textComponent->clear( );
		rwFileThread->start( );
	} );

	// 请求菜单
	Menu *requestMenu = new Menu;
	requestMenu->setTitle( tr( u8"request novel sub menu" ) );

	Action *requestSettingFilePath = new Action;
	requestSettingFilePath->setText( tr( u8"set request novel setting file path" ) );
	connect( requestSettingFilePath, &Action::triggered, [=]( ) {
		DEBUG_RUN( qDebug() << tr(u8"requestSettingFilePath, &Action::trigger slots") );
		QString settingFilePath = progressSetting->value( selectWebSettingPath ).toString( );
		QString caption = tr( u8"select singleton dir save web request buff" );
		QFileInfo fileInfo;
		do {
			settingFilePath = QFileDialog::getOpenFileName( this, caption, qApp->applicationDirPath( ), tr( u8"setting file(*.txt *.ini *.setting *.set) ;; all type file(*)" ) );
			if( settingFilePath.isEmpty( ) ) {
				auto selectOption = QMessageBox::question( this, tr( u8"please select option" ), tr( u8"web request setting file error, alagin select setting file?" ) );
				if( selectOption == QMessageBox::No )
					return;
			} else {
				fileInfo.setFile( settingFilePath );
				if( fileInfo.isFile( ) ) {
					progressSetting->setValue( selectWebSettingPath, fileInfo.absoluteFilePath( ) );
					progressSetting->sync( );
					subWidgetNovelInfoWidget->setNetWorkSettingFilePath( fileInfo.absoluteFilePath( ) );
					break;
				}
			}
		} while( true );
	} );
	requestMenu->addAction( requestSettingFilePath );

	Action *requestWebWidgetShow = new Action;
	requestWebWidgetShow->setText( tr( u8"start request novel web page" ) );
	connect( requestWebWidgetShow, &Action::triggered, this, &MainWidget::webWidgetOnShow );
	requestMenu->addAction( requestWebWidgetShow );

	toolsMenu->addMenu( requestMenu );
	//// 线程开始
	dateTimeThread->start( );

	//// 子窗口
	subWidgetNovelInfoWidget = new NovelInfoWidget( );
}
MainWidget::~MainWidget( ) {
	dateTimeThread->requestInterruption( );
	rwFileThread->requestInterruption( );
	progressSetting->sync( );
	if( subWidgetNovelInfoWidget )
		delete subWidgetNovelInfoWidget;
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
void MainWidget::closeEvent( QCloseEvent *event ) {
	subWidgetNovelInfoWidget->hide( );
	delete subWidgetNovelInfoWidget;
	subWidgetNovelInfoWidget = nullptr;
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
	bool attribute = !textComponent->testAttribute( Qt::WA_TransparentForMouseEvents );
	progressSetting->setValue( transparentForMouseEvents, attribute );
	progressSetting->sync( );
	textComponent->setAttribute( Qt::WA_TransparentForMouseEvents, attribute );
	converTransparentForMouseEventsBtn->setText( QString( tr( u8"current state: [%1 transparent]" ) ).arg( attribute ? u8"" : tr( u8"not" ) ) );
}
void MainWidget::changeTextComponentContents( ) {
	textComponent->setText( fileThreadResult->getData( ) );
}
void MainWidget::error( int errorType, QFileDevice::FileError fileErrorCode, QFileDevice::FileError dirError ) {
	if( errorType | 1 )
		DEBUG_RUN( qDebug() << fileErrorCode ) ;
	if( errorType | 2 )
		DEBUG_RUN( qDebug() << dirError ) ;
}
void MainWidget::webWidgetOnShow( ) {
	DEBUG_RUN( qDebug() << tr(u8"call in MainWidget::webWidgetOnShow") );
	QVariant value = progressSetting->value( selectWebSettingPath );
	bool isNull = value.isNull( );
	if( isNull ) {
		QFileInfo fileInfo( value.toString( ) );
		if( !fileInfo.exists( ) ) {
			auto defaultPath = progressSetting->value( selectReadFileWorkPath, qApp->applicationFilePath( ) ).toString( );
			do {
				auto fileName = QFileDialog::getOpenFileName( this, tr( u8"select singleton setting file" ), defaultPath );
				if( fileName.isEmpty( ) ) {
					QMessageBox::StandardButton standardButton = QMessageBox::question( this, tr( u8"please select option" ), tr( u8"file path is error! now repetition select file ?" ) );
					if( standardButton == QMessageBox::Yes )
						continue;
					return;
				}
				progressSetting->setValue( selectWebSettingPath, fileName );
				progressSetting->sync( );
				subWidgetNovelInfoWidget->setAbsoluteFilePath( fileName );
				break;
			} while( true );
		} else
			subWidgetNovelInfoWidget->setAbsoluteFilePath( fileInfo.absoluteFilePath( ) );
	} else
		subWidgetNovelInfoWidget->setAbsoluteFilePath( value.toString( ) );
	subWidgetNovelInfoWidget->show( );
	QRect rect = geometry( );
	int x = rect.width( ) / 2 + rect.x( ) - subWidgetNovelInfoWidget->width( ) / 2;
	int y = rect.height( ) / 2 + rect.y( ) - subWidgetNovelInfoWidget->height( ) / 2;
	subWidgetNovelInfoWidget->move( x, y );
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
		DEBUG_RUN( qDebug( ) << tr(u8"this MainWidget window min width is : ") << width );
	}
}
