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
#include "../../../extend/layout/HLayoutBox.h"
#include "../../../extend/layout/VLayoutBox.h"
#include "../../../extend/ui/Button.h"
#include "../../../extend/ui/EditLine.h"

#include "NovelInfoWidget/WebUrlInfoWidget.h"
#include <QPluginLoader>

#include "path/Path.h"

const QString NovelInfoWidget::settingWebBuffWorkPathKey = tr( u8"work/WebBuff/Path" );
const QString NovelInfoWidget::settingHostKey = tr( u8"host" );
const QString NovelInfoWidget::loadClassName = tr( u8"RequestNet" );
const QByteArray NovelInfoWidget::loadClassNameByteArry = loadClassName.toLocal8Bit( );
std::unordered_map< QString, IRequestNetInterfaceExtend * > NovelInfoWidget::loadPlugs;
IRequestNetInterfaceExtend *NovelInfoWidget::getIRequestNetInterface( const QString &plugFilePath, const QString &name, const QString &spec ) {
	QPluginLoader loader( plugFilePath );
	if( loader.load( ) ) {
		QObject *instance = loader.instance( );
		QGenericPlugin *genericPlugin = qobject_cast< QGenericPlugin * >( instance );
		if( genericPlugin ) {
			QObject *object = genericPlugin->create( name, spec );
			auto className = object->metaObject( )->className( );
			if( strcmp( className, loadClassNameByteArry ) == 0 ) {
				IRequestNetInterfaceExtend *result = reinterpret_cast< IRequestNetInterfaceExtend * >( object );
				if( result ) {
					result->setParent( nullptr );
					QString host = result->getUrl( ).host( );
					loadPlugs.insert_or_assign( host, result );
					return result;
				}
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
	rwFileThread = new RWFileThread( this );
	requestNetWrok = new Request( this );
	requestConnect = new RequestConnect( this );

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

	this->fileThreadResult = rwFileThread->getFileResult( );
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
	connect( requestConnect, &RequestConnect::networkReplyFinished, this, &NovelInfoWidget::networkReplyFinished );
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
	connect( webUrlWidget, &WebUrlInfoWidget::startBtnClick, this, &NovelInfoWidget::componentRequestStart );
}

NovelInfoWidget::~NovelInfoWidget( ) {

	DEBUG_RUN(
		qDebug() << tr(u8"NovelInfoWidget::~NovelInfoWidget");
	);

	if( netSetFileSettings ) {
		netSetFileSettings->sync( );
		rwFileThread->deleteLater( );
	}
	if( rwFileThread ) {
		rwFileThread->await( );
		rwFileThread->deleteLater( );
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
void NovelInfoWidget::networkReplyFinished( ) {
	QNetworkReply *networkReply = requestConnect->getNetworkReply( );
	auto byteArray = networkReply->readAll( );
	if( byteArray.isEmpty( ) )
		return;
	QString existingDirectory;
	if( !netSetFileSettings ) {
		do {
			QString filePath = QFileDialog::getOpenFileName( this, tr( u8"选择配置文件" ), qApp->applicationDirPath( ), tr( u8"配置文件类型(*.ini *.setting *.set);;全部类型(*)" ) );
			if( !filePath.isEmpty( ) ) {
				netSetFileSettings = new QSettings( filePath, QSettings::Format::IniFormat, this );
				break;
			}
			if( QMessageBox::No == QMessageBox::question( this, tr( u8"请选择" ), tr( u8"路径无法写入文件。现在重新选择路径吗？" ) ) )
				return;
		} while( true );
	}
	auto variant = netSetFileSettings->value( settingWebBuffWorkPathKey );
	if( variant.isNull( ) ) {
		QFileInfo info;
		bool isUseCanNotWrite = false;
		do {
			existingDirectory = QFileDialog::getExistingDirectory( this, tr( u8"选择一个 web 缓冲目录" ), qApp->applicationDirPath( ) );
			if( existingDirectory.isEmpty( ) )
				return;
			info.setFile( existingDirectory );
			isUseCanNotWrite = !info.isWritable( );
			if( isUseCanNotWrite ) {
				QMessageBox::StandardButton standardButton = QMessageBox::question( this, tr( u8"请选择" ), tr( u8"路径无法写入。现在重新选择路径吗？" ) );
				if( standardButton | QMessageBox::No )
					return;
			}
		} while( isUseCanNotWrite ) ;
		existingDirectory.append( QDir::separator( ) );
		netSetFileSettings->setValue( settingWebBuffWorkPathKey, existingDirectory );
		netSetFileSettings->sync( );
	} else
		existingDirectory = variant.toString( ) + QDir::separator( );
	QUrl url = networkReply->url( );
	DEBUG_RUN(
		qDebug() << "默认 - url.host( FullyDecoded ) : " << url.host( );
		qDebug() << "默认 - PrettyDecoded : " << url.toString( );
	);
	auto saveTemPath = existingDirectory + url.host( ) + QDir::separator( );
	rwFileThread->setFilePath( saveTemPath + "index.html" );
	rwFileThread->writeFile( byteArray );
	rwFileThread->start( );
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
			netSetFileSettings->setPath( QSettings::IniFormat, QSettings::UserScope, newPath );
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
	auto key = "root";
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
				QUrl url = ire->getUrl( );
				QString host = url.host( );
				netSetFileSettings->beginGroup( settingHostKey );
				currentFilePtah = current.relativeFilePath( currentFilePtah );
				netSetFileSettings->setValue( host, currentFilePtah );
				netSetFileSettings->endGroup( );
				netSetFileSettings->beginGroup( host );
				netSetFileSettings->setValue( key, url.url( ) );
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
void NovelInfoWidget::componentRequestStart( ) {
	QObject *src = sender( );
	if( src ) {
		auto webUrlInfoWidget = qobject_cast< WebUrlInfoWidget * >( src );
		if( webUrlInfoWidget ) {
			auto url = webUrlInfoWidget->getUrl( );
			DEBUG_RUN( qDebug() << "qobject_cast<WebUrlInfoWidget*>(src) = " << webUrlInfoWidget->getUrl( ) );
			requestNetWrok->netGetWork( url, requestConnect );
		}
	}
}
void NovelInfoWidget::slotsSetNetWorkSettingFilePath( const QString &filePath ) {
	QFileInfo fileInfo( filePath );
	auto absoluteFilePath = fileInfo.absoluteFilePath( );
	if( fileInfo.exists( ) ) {
		if( netSetFileSettings ) {
			netSetFileSettings->setPath( QSettings::IniFormat, QSettings::UserScope, absoluteFilePath );
			netSetFileSettings->sync( );
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
