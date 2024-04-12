#include "NovelInfoWidget.h"

#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMouseEvent>
#include <QNetworkReply>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QShowEvent>
#include <qdir.h>
#include <qguiapplication.h>

#include "../../../../qt/extend/exception/Exception.h"
#include "../../../../qt/extend/netWork/Request.h"
#include "../../../../qt/extend/netWork/RequestConnect.h"
#include "../../../../qt/group/file/FileResult.h"
#include "../../../../qt/group/file/RWFileThread.h"
#include <DebugInfo.h>
#include <QGenericPlugin>

#include "../../../extend/layout/HLayoutBox.h"
#include "../../../extend/layout/VLayoutBox.h"
#include "../../../extend/ui/Button.h"
#include "../../../extend/ui/EditLine.h"

#include "NovelInfoWidget/WebUrlInfoWidget.h"
#include <QPluginLoader>

#include "path/Path.h"
#include <QMetaEnum>

const QString NovelInfoWidget::settingWebBuffWorkPathKey = tr( u8"work/WebBuff/Path" );
const QString NovelInfoWidget::settingHostKey = tr( u8"host" );
const QString NovelInfoWidget::loadClassName = tr( u8"RequestNet" );
const QString NovelInfoWidget::loadClassMethodName = tr( u8"getRequestNetInterfaceExtendPatr" );
const QByteArray NovelInfoWidget::loadClassNameByteArry = loadClassName.toLocal8Bit( );
std::unordered_map< QString, IRequestNetInterfaceExtend * > NovelInfoWidget::loadPlugs;

IRequestNetInterfaceExtend *NovelInfoWidget::metaGetResult( QObject *outObj, const char *methodName ) {
	const QMetaObject *metaObject = outObj->metaObject( );
	if( metaObject->className( ) == loadClassName ) {
		IRequestNetInterfaceExtend *res = nullptr;
		bool invokeMethod = metaObject->invokeMethod( outObj, methodName, Qt::DirectConnection, Q_RETURN_ARG( IRequestNetInterfaceExtend *, res ) );
		if( invokeMethod )
			return res;
	}
	return nullptr;
}
IRequestNetInterfaceExtend *NovelInfoWidget::getIRequestNetInterface( const QString &plugFilePath, const QString &name, const QString &spec ) {
	QPluginLoader loader( plugFilePath );
	if( loader.load( ) ) {
		QObject *instance = loader.instance( );
		QGenericPlugin *genericPlugin = qobject_cast< QGenericPlugin * >( instance );
		if( genericPlugin ) {
			QObject *object = genericPlugin->create( name, spec );
			IRequestNetInterfaceExtend *requestNetInterface = metaGetResult( object, loadClassMethodName.toLocal8Bit( ) );
			if( requestNetInterface ) {
				requestNetInterface->setInterfaceParent( nullptr );

				std::string curlLink;
				requestNetInterface->getUrl( &curlLink );
				QUrl url( curlLink.c_str( ) );
				QString host = url.host( );
				loadPlugs.insert_or_assign( host, requestNetInterface );
				return requestNetInterface;
			}
		}
	}
	return nullptr;
}
NovelInfoWidget::NovelInfoWidget( QWidget *parent, Qt::WindowFlags flag ) : QWidget( parent, flag ) {
	/////// 链接信号槽
	initComponentMeoryObj( );
	initComponentPropers( );
	initComponentLayout( );
	initComponentConnect( );
}

void NovelInfoWidget::initComponentMeoryObj( ) {

	mainLayout = new VLayoutBox;
	settingInfoLayoutBox = new HLayoutBox;
	msgLayoutBox = new HLayoutBox;
	runInfoLayoutBox = new HLayoutBox;
	settingPathTitle = new QLabel;
	inputSettingPathLine = new EditLine;
	btn = new Button( this );
	listView = new QScrollArea( this );

	listViewWidget = new QWidget( listView );
	listViewWidgetVBox = new VLayoutBox( listViewWidget );
}
void NovelInfoWidget::initComponentPropers( ) {

	settingPathTitle->setText( tr( u8"配置文件路径:" ) );
	inputSettingPathLine->setPlaceholderText( tr( u8"请输入一个配置文件" ) );
	editorStatus = 0;
	inputSettingPathLine->setParent( this );
	btn->setText( tr( u8"添加插件到配置文件" ) );

}
void NovelInfoWidget::initComponentLayout( ) {
	this->setLayout( mainLayout );
	mainLayout->addLayout( settingInfoLayoutBox, 1 );
	mainLayout->addLayout( msgLayoutBox, 9 );
	mainLayout->addLayout( runInfoLayoutBox, 1 );
	settingInfoLayoutBox->addWidget( settingPathTitle );
	settingInfoLayoutBox->addWidget( inputSettingPathLine, 1 );
	settingInfoLayoutBox->addWidget( btn );
	msgLayoutBox->addWidget( listView );
	listView->setWidget( listViewWidget );
}

void NovelInfoWidget::initComponentConnect( ) {
	connect( this, &NovelInfoWidget::setNetWorkSettingFilePath, this, &NovelInfoWidget::slotsSetNetWorkSettingFilePath );
	connect( this, &NovelInfoWidget::overSettingPath, this, &NovelInfoWidget::slotsOverSettingPath );
	connect( this, &NovelInfoWidget::errorSettingPath, this, &NovelInfoWidget::slotsErrorSettingPath );
	connect( btn, &QPushButton::clicked, this, &NovelInfoWidget::loadPathPlugs );
	connect( inputSettingPathLine, &QLineEdit::editingFinished, this, &NovelInfoWidget::inputSettingPathLinePathCompoentEditFinish );
}
void NovelInfoWidget::initWidgetSize( ) {
	QMargins contentsMargins = listViewWidgetVBox->contentsMargins( );
	QScrollBar *horizontalScrollBar = listView->horizontalScrollBar( );
	auto horizontalScrollBarHint = horizontalScrollBar->size( );

	QScrollBar *verticalScrollBar = listView->verticalScrollBar( );
	auto verticalScrollBarHint = verticalScrollBar->size( );

	listView->setMinimumSize( horizontalScrollBarHint.width( ) + contentsMargins.left( ) + contentsMargins.right( ), verticalScrollBarHint.height( ) + contentsMargins.top( ) + contentsMargins.bottom( ) );

}
void NovelInfoWidget::initWebUrlInfoWidgetCompoent( WebUrlInfoWidget *webUrlWidget ) {
	connect( this, &NovelInfoWidget::clickRequestStart, webUrlWidget, &WebUrlInfoWidget::startBtnClick );
	connect( webUrlWidget, &WebUrlInfoWidget::currentIndexChanged, this, &NovelInfoWidget::componentCurrentIndexChanged );
}

NovelInfoWidget::~NovelInfoWidget( ) {

	DEBUG_RUN(
		qDebug() << tr(u8"NovelInfoWidget::~NovelInfoWidget");
	);

	if( netSetFileSettings ) {
		netSetFileSettings->sync( );
	}
}

IRequestNetInterfaceExtend *NovelInfoWidget::loadPlug( const QString &plugFilePath ) {
	// todo : 加载插件
	return getIRequestNetInterface( plugFilePath, plugFilePath, metaObject( )->className( ) );
}

void NovelInfoWidget::computeListViewWidgetSize( ) {
	auto widget = listView->widget( );
	auto vBox = widget->layout( );
	QMargins contentsMargins = vBox->contentsMargins( );
	int maxWidth = 0;
	int maxHeight = 0;
	int count = vBox->count( );
	for( int index = 0 ; index < count ; ++index ) {
		QLayoutItem *item = vBox->itemAt( index );
		auto currentWidget = item->widget( );
		QSize size = currentWidget->minimumSizeHint( );
		QSize minimumSize = currentWidget->minimumSize( );
		int height = size.height( );
		int height1 = minimumSize.height( );
		maxHeight += height > height1 ? height : height1;
		if( maxWidth < size.width( ) )
			maxWidth = size.width( );
		if( maxWidth < minimumSize.width( ) )
			maxWidth = minimumSize.width( );
	}
	int spacing = vBox->spacing( );
	QSize reSize( maxWidth + contentsMargins.left( ) + contentsMargins.right( ), maxHeight + spacing * count + contentsMargins.top( ) + contentsMargins.bottom( ) );
	widget->setMinimumSize( reSize );
}
void NovelInfoWidget::showEvent( QShowEvent *event ) {
	DEBUG_RUN( qDebug( ) << tr( u8"====== NovelInfoWidget::showEvent ===========" ) );
	computeListViewWidgetSize( );
	auto widgetSize = size( );
	QWidget::showEvent( event );
	emit resetWidgetSize( widgetSize.width( ), widgetSize.height( ) );
}
bool NovelInfoWidget::nativeEvent( const QByteArray &eventType, void *message, qintptr *result ) {
	return QWidget::nativeEvent( eventType, message, result );
}
void NovelInfoWidget::mousePressEvent( QMouseEvent *event ) {
	if( editorStatus && !inputSettingPathLine->geometry( ).contains( event->pos( ) ) )
		inputSettingPathLinePathCompoentEditFinish( );
}
void NovelInfoWidget::inputSettingPathLinePathCompoentEditFinish( ) {
	QString inputPath = inputSettingPathLine->text( );
	QFileInfo fileInfo( inputPath );
	if( !fileInfo.exists( ) ) {
		emit errorSettingPath( settingFileAbsoluteFilePath, fileInfo.absoluteFilePath( ) );
		return;
	}
	QString newPath = fileInfo.absoluteFilePath( );
	if( netSetFileSettings ) {
		if( settingFileAbsoluteFilePath != newPath ) {
			netSetFileSettings->sync( );
			netSetFileSettings->deleteLater( );
			netSetFileSettings = new QSettings( newPath, QSettings::IniFormat, this );
		}
	} else
		netSetFileSettings = new QSettings( newPath, QSettings::Format::IniFormat, this );

	// todo 开始加载组件
	DEBUG_RUN(
		netSetFileSettings->beginGroup( settingHostKey );
		QStringList childKeys = netSetFileSettings->childKeys( );
		for( auto &key : childKeys ) {
		QVariant value = netSetFileSettings->value( key );
		qDebug() << "当前 key 为 : "<< key<< ", 获取到的值为 : " << value.toString( );
		}
		netSetFileSettings->endGroup( );
	);
	emit overSettingPath( settingFileAbsoluteFilePath, newPath );
	settingFileAbsoluteFilePath = newPath;
}
void NovelInfoWidget::loadPathPlugs( ) {
	QFileInfo settingFileInfo( netSetFileSettings->fileName( ) );
	QString caption = tr( u8"选择一个 qt 插件路径" );
	QString title = tr( u8"插件错误" );
	QString questionMsg = tr( u8"文件打开错误!现在重新选择吗?" );
	auto rootKey = WebUrlInfoWidget::getRootKey( );
	auto schemeKey = WebUrlInfoWidget::getSchemeKey( );
	do {
		auto fileName = QFileDialog::getExistingDirectory( this, caption, settingFileInfo.absoluteDir( ).absolutePath( ) );
		if( fileName.isEmpty( ) ) {
			QMessageBox::StandardButton standardButton = QMessageBox::question( this, title, questionMsg );
			if( standardButton == QMessageBox::Yes )
				continue;
			break;
		}
		auto dirInfo = Path::getDirInfo( fileName );
		for( auto &dir : dirInfo.first ) {
			qDebug( ) << u8"发现文件夹 : " << dir.getCurrentPath( );
		}
		size_t loadPlugCount = 0;
		QDir current( qApp->applicationDirPath( ) );
		for( auto &file : dirInfo.second ) {
			QString currentFilePtah = file.getCurrentFilePtah( );
			qDebug( ) << u8"发现文件 : " << currentFilePtah;
			auto ire = loadPlug( currentFilePtah );
			if( ire ) {
				++loadPlugCount;

				std::string curlLink;
				ire->getUrl( &curlLink );
				QUrl url( curlLink.c_str( ) );
				QString host = url.host( );
				netSetFileSettings->beginGroup( settingHostKey );
				currentFilePtah = current.relativeFilePath( currentFilePtah );
				netSetFileSettings->setValue( host, currentFilePtah );
				netSetFileSettings->endGroup( );
				netSetFileSettings->beginGroup( host );
				netSetFileSettings->setValue( rootKey, url.url( ) );
				auto scheme = netSetFileSettings->value( schemeKey, url.scheme( ) );
				netSetFileSettings->setValue( schemeKey, scheme );
				netSetFileSettings->endGroup( );
				auto webUrlInfoWidget = WebUrlInfoWidget::generateWebUrlInfoWidget( netSetFileSettings, this, ire );

				if( webUrlInfoWidget ) {
					auto layout = listView->widget( )->layout( );
					int count = layout->count( );
					int index = 0;
					for( ; index < count ; ++index )
						if( layout->itemAt( index )->widget( ) == webUrlInfoWidget )
							break;
					if( index == count )
						layout->addWidget( webUrlInfoWidget );
				}
			}
		}
		if( loadPlugCount > 0 ) {
			netSetFileSettings->sync( );
			computeListViewWidgetSize( );
			break;
		}
		QMessageBox::StandardButton standardButton = QMessageBox::question( this, title, questionMsg );
		if( standardButton == QMessageBox::No )
			break;
	} while( true );
}

void NovelInfoWidget::componentCurrentIndexChanged( int index ) {
	if( index == -1 )
		return;
	QObject *object = sender( );
	auto webUrlInfoWidget = qobject_cast< WebUrlInfoWidget * >( object );
	if( webUrlInfoWidget )
		emit changedScheme( webUrlInfoWidget->getScheme( ) );

}
void NovelInfoWidget::slotsSetNetWorkSettingFilePath( const QString &filePath ) {
	QFileInfo fileInfo( filePath );
	auto absoluteFilePath = fileInfo.absoluteFilePath( );
	if( fileInfo.exists( ) ) {
		if( netSetFileSettings ) {
			netSetFileSettings->sync( );
			netSetFileSettings->deleteLater( );
			netSetFileSettings = new QSettings( absoluteFilePath, QSettings::IniFormat, this );
		} else
			netSetFileSettings = new QSettings( absoluteFilePath, QSettings::IniFormat );

		auto temp = settingFileAbsoluteFilePath;
		settingFileAbsoluteFilePath = absoluteFilePath;
		inputSettingPathLine->setText( settingFileAbsoluteFilePath );
		emit overSettingPath( temp, settingFileAbsoluteFilePath );
		return;
	}
	emit errorSettingPath( getAbsoluteFilePath( ), absoluteFilePath );
}
void NovelInfoWidget::slotsOverSettingPath( const QString &oldPath, const QString &newPath ) {
	netSetFileSettings->beginGroup( settingHostKey );
	auto allKeys = netSetFileSettings->allKeys( );
	if( allKeys.length( ) != 0 ) {
		auto layout = listView->widget( )->layout( );
		for( auto &settingKey : allKeys ) {
			QVariant variant = netSetFileSettings->value( settingKey );
			auto value = variant.toString( );
			if( value.isEmpty( ) )
				continue;
			IRequestNetInterfaceExtend *requestNetInterface = loadPlug( value );
			if( requestNetInterface ) {
				auto webUrlWidget = WebUrlInfoWidget::generateWebUrlInfoWidget( netSetFileSettings, this, requestNetInterface );
				if( webUrlWidget ) {
					int count = layout->count( );
					int index = 0;
					for( ; index < count ; ++index )
						if( layout->itemAt( index )->widget( ) == webUrlWidget )
							break;
					if( index == count ) {
						layout->addWidget( webUrlWidget );
						initWebUrlInfoWidgetCompoent( webUrlWidget );

					}
				}
			}
		}
	}
	netSetFileSettings->endGroup( );
}
void NovelInfoWidget::slotsErrorSettingPath( const QString &currentPath, const QString &errorPath ) {

	DEBUG_RUN( qDebug() << u8"NovelInfoWidget::errorSettingPath" );
}
