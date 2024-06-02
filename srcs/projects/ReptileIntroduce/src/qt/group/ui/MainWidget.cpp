#include "MainWidget.h"
#include <QTranslator>
#include <QTimer>
#include <QPushButton>
#include <QPlainTextEdit>

#include <plug/LoadPlug.h>
#include <DebugInfo.h>
#include <QThread>
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
#include <path/Path.h>

#include "../../extend/layout/HLayoutBox.h"
#include "../../extend/layout/VLayoutBox.h"
#include "../../extend/ui/DisplayWidget.h"
#include "FileSelectPathWidget.h"
#include "../../extend/menu/Action.h"
#include "../../extend/menu/Menu.h"
#include "novelNetJob/NovelNetJob.h"
#include <nameSpace/interfacePlugsType.h>
#include <interface/IRequestNetInterface.h>
using interfacePlugsType::IRequestNetInterface;
const QString MainWidget::settingGroupWork = tr( u8"工作" );
const QString MainWidget::settingGroupWeb = tr( u8"网络" );
const QString MainWidget::settingGroupWebKey = tr( u8"配置文件路径" );
const QString MainWidget::settingGroupWorkKeyReadFileWorkPath = tr( u8"读取路径" );
const QString MainWidget::settingGroupWorkKeyWriteFileWorkPath = tr( u8"写入路径" );
const QString MainWidget::settingGroupSelectDefaultPaths = tr( u8"选择窗口默认路径" );
const QString MainWidget::settingGroupSelectDefaultPathskeyPlugPathKey = tr( u8"插件路径" );
const QString MainWidget::settingGroupPlugIniPathKeyMerge = tr( u8"插件/配置路径" );
const QString MainWidget::settingGroupMenuFont = tr( u8"菜单字体" );
const QString MainWidget::settingGroupDrawplayFont = tr( u8"绘制字体" );
const QString MainWidget::settingGroupFontFamily = tr( u8"名称" );
const QString MainWidget::settingGroupFontPointSize = tr( u8"点大小" );
const QString MainWidget::settingGroupFontWeightSize = tr( u8"加重" );
const QString MainWidget::settingGroupFontItalic = tr( u8"斜体" );
const QString MainWidget::settingGroupFontBold = tr( u8"加粗" );
const QString MainWidget::settingGroupFontStrikeOut = tr( u8"划线" ); // 划线
const QString MainWidget::settingGroupFontUnderline = tr( u8"下划线" ); // 下划线
const QString MainWidget::settingGroupFontStyle = tr( u8"字体风格" ); // 字体风格
const QChar MainWidget::settingPathSep = tr( u8";" )[ 0 ];

void MainWidget::initMumberPtrMemory( ) {
	progressSetting = new Setting( this );
	translator = new QTranslator( this );
	selectPathWidget = new FileSelectPathWidget( this );
	display = new DisplayWidget( this );
	LoadWebInfoBtn = new Action;
}
void MainWidget::initComponentPropertys( ) {
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
	connect( selectPathWidget
		, &FileSelectPathWidget::editorOver
		, [&]( const QString &newFilePath ) {
			updateSettingFileInfo( newFilePath );
		} );
	connect( selectPathWidget
		, &FileSelectPathWidget::setFilePathFinish
		, this
		, [&]( const QString &filePath ) {
			updateSettingFileInfo( filePath );
		} );
	connect( display
		, &DisplayWidget::menuActionClick
		, [=]( const QString &xpath ) {
			qDebug( ) << xpath;
		} );
	connect( display
		, &DisplayWidget::changeDisplayFont
		, [=]( QFont &font ) {
			progressSetting->setValue( settingGroupDrawplayFont
				, {
					{ settingGroupFontFamily
						, font.family( ) }
					, { settingGroupFontPointSize
						, font.pointSizeF( ) }
					, { settingGroupFontWeightSize
						, font.weight( ) }
					, { settingGroupFontItalic
						, font.italic( ) }
					, { settingGroupFontBold
						, font.bold( ) }
					, { settingGroupFontStrikeOut
						, font.strikeOut( ) }
					, { settingGroupFontUnderline
						, font.underline( ) }
					, { settingGroupFontStyle
						, font.style( ) }
				}
			);
			progressSetting->sync( );
		} );
	connect( display
		, &DisplayWidget::changeMenuFont
		, [=]( QFont &font ) {
			progressSetting->setValue( settingGroupMenuFont
				, {
					{ settingGroupFontFamily
						, font.family( ) }
					, { settingGroupFontPointSize
						, font.pointSizeF( ) }
					, { settingGroupFontWeightSize
						, font.weight( ) }
					, { settingGroupFontItalic
						, font.italic( ) }
					, { settingGroupFontBold
						, font.bold( ) }
					, { settingGroupFontStrikeOut
						, font.strikeOut( ) }
					, { settingGroupFontUnderline
						, font.underline( ) }
					, { settingGroupFontStyle
						, font.style( ) }
				}
			);
			progressSetting->sync( );
		} );
}

template< typename TKey, typename TValue >
void setFont( typename QMap< TKey, TValue >::Iterator &iterator, typename QMap< TKey, TValue >::Iterator &end, QFont &font ) {
	for( ; iterator != end; ++iterator ) {
		auto key = iterator.key( );
		if( key.isEmpty( ) )
			continue;
		if( MainWidget::settingGroupFontFamily == key ) {
			QString family = iterator.value( ).toString( );
			if( !family.isEmpty( ) )
				font.setFamily( family );
		} else if( MainWidget::settingGroupFontPointSize == key ) {
			bool converOk = false;
			auto size = iterator.value( ).toDouble( &converOk );
			if( converOk )
				font.setPointSizeF( size );
		} else if( MainWidget::settingGroupFontWeightSize == key ) {
			bool converOk = false;
			QFont::Weight weight = static_cast< QFont::Weight >( iterator.value( ).toInt( &converOk ) );
			if( converOk )
				font.setWeight( weight );
		} else if( MainWidget::settingGroupFontItalic == key ) {
			auto variant = iterator.value( );
			bool isItalic = variant.toBool( );
			font.setItalic( isItalic );
		} else if( MainWidget::settingGroupFontBold == key ) {
			auto variant = iterator.value( );
			bool isBold = variant.toBool( );
			font.setBold( isBold );
		} else if( MainWidget::settingGroupFontStrikeOut == key ) {
			auto variant = iterator.value( );
			bool isBold = variant.toBool( );
			font.setStrikeOut( isBold );
		} else if( MainWidget::settingGroupFontUnderline == key ) {
			auto variant = iterator.value( );
			bool isBold = variant.toBool( );
			font.setUnderline( isBold );
		} else if( MainWidget::settingGroupFontStyle == key ) {
			bool converOk = false;
			QFont::Style style = static_cast< QFont::Style >( iterator.value( ).toInt( &converOk ) );
			if( converOk )
				font.setStyle( style );
		}
	}

}

void MainWidget::initComponentOver( ) {
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

	auto map = progressSetting->getAllInfo( settingGroupDrawplayFont );
	QFont font;
	if( map.size( ) > 0 ) {
		auto iterator = map.begin( );
		auto end = map.end( );
		::setFont< QString, QVariant >( iterator, end, font );
	} else {
		font = this->font( );
		progressSetting->setValue( settingGroupDrawplayFont
			, {
				{ settingGroupFontFamily
					, font.family( ) }
				, { settingGroupFontPointSize
					, font.pointSizeF( ) }
				, { settingGroupFontWeightSize
					, font.weight( ) }
				, { settingGroupFontItalic
					, font.italic( ) }
				, { settingGroupFontBold
					, font.bold( ) }
				, { settingGroupFontStrikeOut
					, font.strikeOut( ) }
				, { settingGroupFontUnderline
					, font.underline( ) }
				, { settingGroupFontStyle
					, font.style( ) }
			}
		);
		progressSetting->sync( );
	}
	emit display->changeDisplayFont( font );

	map = progressSetting->getAllInfo( settingGroupMenuFont );
	if( map.size( ) > 0 ) {
		auto iterator = map.begin( );
		auto end = map.end( );
		::setFont< QString, QVariant >( iterator, end, font );
	} else {
		font = this->font( );
		progressSetting->setValue( settingGroupDrawplayFont
			, {
				{ settingGroupFontFamily
					, font.family( ) }
				, { settingGroupFontPointSize
					, font.pointSizeF( ) }
				, { settingGroupFontWeightSize
					, font.weight( ) }
				, { settingGroupFontItalic
					, font.italic( ) }
				, { settingGroupFontBold
					, font.bold( ) }
				, { settingGroupFontStrikeOut
					, font.strikeOut( ) }
				, { settingGroupFontUnderline
					, font.underline( ) }
				, { settingGroupFontStyle
					, font.style( ) }
			}
		);
		progressSetting->sync( );
	}
	emit display->changeDisplayFont( font );
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
	progressSetting->sync( );

	delete progressSetting;
	delete translator;

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
			interfacePlugsType::HtmlDocString outUrl;
			for( ; iterator != end; ++iterator ) {
				IRequestNetInterface *requestNetInterfaceExtend = iterator.value( );
				IRequestNetInterface *interfaceExtend = requestNetInterfaceExtend;
				if( interfaceExtend->getRootUrl( &outUrl ) )
					*display << QString( tr( u8"获取url : %1" ) ).arg( QString::fromStdWString( outUrl ) ) << '\n';
				interfaceExtend->setInterfaceParent( this );
				QObject *object = iterator.key( );
				NovelNetJob *novelNetJob = new NovelNetJob( display, object, requestNetInterfaceExtend );
				QSharedPointer< NovelNetJob > netJob{ novelNetJob };
				plugs.insert( filePtah, netJob );
				display->flush( );
				auto menu = display->getPlugMenu( requestNetInterfaceExtend );
				menu->setTitle( QUrl( QString::fromStdWString( outUrl ) ).host( ).append( " 选项" ) );
				Action *requestBtn = new Action;
				requestBtn->setText( "开始获取任务" );
				connect( requestBtn
					, &QAction::triggered
					, [=]( ) {
						*display << QString( u8"%1:(%2 %3)" ).arg( netJob->getUrl( ) ).arg( __FILE__ ).arg( __LINE__ ) << '\n';
						display->flush( );
						netJob->start( );
					} );
				menu->addAction( requestBtn );
				// todo :: 加入按钮列表
				plugsActions.append( requestBtn );
				outUrl.clear( );
			}
		}

	}
	if( plugs.size( ) == 0 )
		LoadWebInfoBtn->setEnabled( false );
	else
		LoadWebInfoBtn->setEnabled( true );
	display->flush( );
}
void MainWidget::LoadWebInfo( ) {
	auto iterator = plugsActions.begin( );
	auto end = plugsActions.end( );
	std::string outUrl;
	for( ; iterator != end; ++iterator )
		emit ( *iterator )->trigger( );
}
