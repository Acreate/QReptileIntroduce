#include "FileSelectPathWidget.h"

#include <QFileDialog>
#include <qguiapplication.h>
#include <QLabel>
#include "../../extend/layout/HLayoutBox.h"
#include "../../extend/ui/EditLine.h"
#include "../../extend/ui/Button.h"
FileSelectPathWidget::FileSelectPathWidget( QWidget *parent ) : QWidget( parent ) {
	initComponent( );
	initUI( );
	initConnect( );
	initOver( );
}
FileSelectPathWidget::~FileSelectPathWidget( ) {
}
void FileSelectPathWidget::initComponent( ) {
	/*
	 HLayoutBox *hLayoutBox; // 主要布局
	QLabel *hint; // 提示
	EditLine *filePathEditLine; // 路径显示框（可编辑）
	Button *selectFileBtn; // 文件路径选择矿弹出按钮
	 */
	hint = new QLabel( tr( u8"路径 : " ), this );
	filePathEditLine = new EditLine( this );
	selectFileBtn = new Button( tr( u8"选择文件" ), this );
	hLayoutBox = new HLayoutBox( this );
}
void FileSelectPathWidget::initUI( ) {
	hLayoutBox->addWidget( hint );
	hLayoutBox->addWidget( filePathEditLine );
	hLayoutBox->addWidget( selectFileBtn );
}
void FileSelectPathWidget::initConnect( ) {
	connect( this, &FileSelectPathWidget::showFileSelectDialog, [&]( const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options ) {
		emit selectcSignal( );
		QString fileName = QFileDialog::getOpenFileName( this, caption, dir, filter, selectedFilter, options );
		if( fileName.isEmpty( ) ) {
			emit error( -1, tr( u8"文件选择错误" ) );
			return;
		}
		if( updatePath( fileName ) ) {
			emit selectFileOver( relativeFilePath );
		} else {
			emit error( -2, tr( u8"文件已存在" ) );
			return;
		}
	} );
	connect( this, &FileSelectPathWidget::setPath, [&]( const QString &filePath ) {
		if( updatePath( filePath ) ) {
			emit setFilePathFinish( relativeFilePath );
		} else {
			emit error( -2, tr( u8"文件已存在" ) );
			return;
		}
	} );
	connect( this, &FileSelectPathWidget::selectFileOver, [&]( const QString &filePath ) {
		if( updatePath( filePath ) ) {
			emit setFilePathFinish( relativeFilePath );
		} else {
			emit error( -2, tr( u8"文件已存在" ) );
			return;
		}
	} );
	connect( filePathEditLine, &QLineEdit::textEdited, this, &FileSelectPathWidget::editored );
	connect( filePathEditLine, &QLineEdit::editingFinished, [=]( ) {
		if( updatePath( filePathEditLine->text( ) ) ) {
			emit setFilePathFinish( relativeFilePath );
		} else {
			emit error( -2, tr( u8"文件已存在" ) );
			return;
		}
	} );
}
void FileSelectPathWidget::initOver( ) {
}
QString FileSelectPathWidget::getSelectFilePath( ) {
	return this->filePathEditLine->placeholderText( );
}
bool FileSelectPathWidget::updatePath( const QString &newPath ) {
	auto relativeFilePath = QDir( qApp->applicationDirPath( ) ).relativeFilePath( newPath );
	if( relativeFilePath != this->relativeFilePath ) {
		this->relativeFilePath = relativeFilePath;
		filePathEditLine->setText( relativeFilePath );
		return true;
	}
	return false;
}
;
