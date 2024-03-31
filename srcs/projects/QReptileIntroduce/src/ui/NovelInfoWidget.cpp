#include "NovelInfoWidget.h"

#include <QFileDialog>
#include <QShowEvent>
#include <QMessageBox>
#include <QNetworkReply>
#include <QMouseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <qdir.h>
#include <qguiapplication.h>
#include "../netWork/Request.h"
#include "../netWork/RequestConnect.h"
#include "../userHread/DebugInfo.h"
#include "../file/RWFileThread.h"
#include "../file/FileResult.h"

#include "../layout/HLyaoutBox.h"
#include "../layout/VLayoutBox.h"

#include "components/Button.h"
#include "components/EditLine.h"

NovelInfoWidget::NovelInfoWidget( QWidget *parent, Qt::WindowFlags flag ) : QWidget( parent, flag ) {
	rwFileThread = new RWFileThread( this );
	this->fileThreadResult = rwFileThread->getFileResult( );
	////// 网络请求
	requestNetWrok = new Request( this );
	requestConnect = new RequestConnect( this );
	connect( requestConnect, &RequestConnect::networkReplyFinished, this, &NovelInfoWidget::networkReplyFinished );

	resize( 500, 500 );
	// 子组件
	mainLayout = new VLayoutBox;
	settingInfoLayoutBox = new HLyaoutBox;
	msgLayoutBox = new HLyaoutBox;
	runInfoLayoutBox = new HLyaoutBox;
	this->setLayout( mainLayout );
	mainLayout->addLayout( settingInfoLayoutBox, 1 );
	mainLayout->addLayout( msgLayoutBox, 2 );
	mainLayout->addLayout( runInfoLayoutBox );

	settingPathTitle = new QLabel;
	settingPathTitle->setText( tr( u8"setting file path:" ) );
	settingInfoLayoutBox->addWidget( settingPathTitle );

	inputSettingPathLine = new EditLine;
	inputSettingPathLine->setPlaceholderText( tr( u8"please input setting file path" ) );
	editorStatus = 0;
	connect( inputSettingPathLine, &QLineEdit::editingFinished, this, &NovelInfoWidget::settingPathCompoentWriteOver );
	connect( inputSettingPathLine, &QLineEdit::textEdited, [=]( ) {
		editorStatus = 1;
	} );
	settingInfoLayoutBox->addWidget( inputSettingPathLine, 1 );
	inputSettingPathLine->setParent( this );

	btn = new Button( this );
	settingInfoLayoutBox->addWidget( btn );
	connect( this, &NovelInfoWidget::errorSettingPath, [=]( ) {
		btn->setText( tr( u8"error" ) );
		btn->setStyleSheet( tr( u8R"(
	Button{
		color : red;
	})" ) );
	} );
	connect( this, &NovelInfoWidget::overSettingPath, [=]( ) {
		if( checkStatus == 1 ) {
			btn->setText( tr( u8"lock path" ) );
			checkStatus = 2;
		}
		btn->setStyleSheet( tr( u8R"(
	Button{
		color : black;
	})" ) );
	} );
	connect( btn, &QPushButton::clicked, [=]( ) {
		if( checkStatus == -1 ) {
			do {
				auto fileName = QFileDialog::getOpenFileName( this, tr( u8"select singleton setting file" ), qApp->applicationFilePath( ), "setting file(*.ini *.txt *.set *.setting);;all type(*)" );
				if( fileName.isEmpty( ) ) {
					QMessageBox::StandardButton standardButton = QMessageBox::question( this, tr( u8"please select option" ), tr( u8"file path is error! now repetition select file ?" ) );
					if( standardButton == QMessageBox::Yes )
						continue;
					return;
				}
				inputSettingPathLine->setText( fileName );
				settingPathCompoentWriteOver( );
				break;
			} while( true );
		} else if( checkStatus == 2 ) {
			inputSettingPathLine->setReadOnly( true );
			btn->setStyleSheet( tr( u8R"(
				Button{
					color : blue;
				}
				)" ) );
			checkStatus = 3;
		} else if( checkStatus > 2 ) {
			inputSettingPathLine->setReadOnly( false );
			btn->setStyleSheet( tr( u8R"(
				Button{
					color : black;
				}
				)" ) );
			checkStatus = 2;
		}
	} );
}
NovelInfoWidget::~NovelInfoWidget( ) {

	DEBUG_RUN(
		qDebug() << tr(u8"NovelInfoWidget::~NovelInfoWidget");
	);

	if( netSetFileSettings ) {
		netSetFileSettings->sync( );
		delete netSetFileSettings;
	}
	if( rwFileThread ) {
		rwFileThread->await( );
		delete rwFileThread;
	}
}
void NovelInfoWidget::setNetWorkSettingFilePath( const QString &filePath ) {
	if( checkStatus > 2 ) // 已经被锁定
		return;
	inputSettingPathLine->setText( filePath );
	settingPathCompoentWriteOver( );
}
void NovelInfoWidget::setRWFileThread( RWFileThread *rwFileThread ) {
	if( this->rwFileThread )
		this->rwFileThread->deleteLater( );
	this->rwFileThread = rwFileThread;
	this->fileThreadResult = rwFileThread->getFileResult( );
}
void NovelInfoWidget::setRequestNetWrok( Request *const requestNetWrok ) {
	if( this->requestNetWrok )
		this->requestNetWrok->deleteLater( );
	this->requestNetWrok = requestNetWrok;
}
void NovelInfoWidget::setRequestConnect( RequestConnect *const requestConnect ) {
	if( this->requestConnect )
		this->requestConnect->deleteLater( );
	this->requestConnect = requestConnect;
	connect( requestConnect, &RequestConnect::networkReplyFinished, this, &NovelInfoWidget::networkReplyFinished );
}

void NovelInfoWidget::showEvent( QShowEvent *event ) {
	DEBUG_RUN( qDebug( ) << tr( u8"====== NovelInfoWidget::showEvent ===========" ) );
	QWidget::showEvent( event );
	inputSettingPathLine->setText( settingFileAbsoluteFilePath );
	settingPathCompoentWriteOver( );
}
bool NovelInfoWidget::nativeEvent( const QByteArray &eventType, void *message, qintptr *result ) {
	return QWidget::nativeEvent( eventType, message, result );
}
void NovelInfoWidget::mousePressEvent( QMouseEvent *event ) {
	if( editorStatus && !inputSettingPathLine->geometry( ).contains( event->pos( ) ) )
		settingPathCompoentWriteOver( );
}
void NovelInfoWidget::networkReplyFinished( ) {
	QNetworkReply *networkReply = requestConnect->getNetworkReply( );
	auto byteArray = networkReply->readAll( );
	if( byteArray.isEmpty( ) )
		return;
	QString existingDirectory;
	if( !netSetFileSettings ) {
		do {
			QString filePath = QFileDialog::getOpenFileName( this, tr( u8"selection setting file" ), qApp->applicationDirPath( ), tr( u8"setting file type(*.ini *.setting *.set *.txt);; all type(*)" ) );
			if( !filePath.isEmpty( ) ) {
				netSetFileSettings = new QSettings( filePath, QSettings::Format::IniFormat, this );
				break;
			}
			if( QMessageBox::No == QMessageBox::question( this, tr( u8"please select option" ), tr( u8"path dir can not wirte file. now again select dir path ?" ) ) )
				return;
		} while( true );
	}
	auto variant = netSetFileSettings->value( selectWebBuffWorkPath );
	if( variant.isNull( ) ) {
		QFileInfo info;
		bool isUseCanNotWrite = false;
		do {
			existingDirectory = QFileDialog::getExistingDirectory( this, tr( u8"select singleton dir save web request buff" ), qApp->applicationDirPath( ) );
			if( existingDirectory.isEmpty( ) )
				return;
			info.setFile( existingDirectory );
			isUseCanNotWrite = !info.isWritable( );
			if( isUseCanNotWrite ) {
				QMessageBox::StandardButton standardButton = QMessageBox::question( this, tr( u8"please select option" ), tr( u8"path dir can not wirte file. now again select dir path ?" ) );
				if( standardButton | QMessageBox::No )
					return;
			}
		} while( isUseCanNotWrite ) ;
		existingDirectory.append( QDir::separator( ) );
		netSetFileSettings->setValue( selectWebBuffWorkPath, existingDirectory );
		netSetFileSettings->sync( );
	} else
		existingDirectory = variant.toString( ) + QDir::separator( );
	QUrl url = networkReply->url( );
	DEBUG_RUN(
		qDebug() << "default - url.host( FullyDecoded ) : " << url.host( );
		qDebug() << "default - PrettyDecoded : " << url.toString( );
	);
	auto saveTemPath = existingDirectory + url.host( ) + QDir::separator( );
	rwFileThread->setFilePath( saveTemPath + "index.html" );
	rwFileThread->writeFile( byteArray );
	rwFileThread->start( );
}
void NovelInfoWidget::settingPathCompoentWriteOver( ) {
	QString inputPath = inputSettingPathLine->text( );
	editorStatus = 0;
	QFileInfo fileInfo( inputPath );
	if( !fileInfo.exists( ) ) {
		checkStatus = -1;
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
	checkStatus = 1;
	emit overSettingPath( settingFileAbsoluteFilePath, newPath );
	settingFileAbsoluteFilePath = newPath;
}
