#include "FileSelectPathWidget.h"

#include <QFileDialog>
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
		emit selectFileOver( fileName );
	} );
	connect( this, &FileSelectPathWidget::setPath, [&]( const QString &filePath ) {
		filePathEditLine->setText( filePath );
		emit setFilePathFinish( filePathEditLine->placeholderText( ) );
	} );
	connect( filePathEditLine, &QLineEdit::textEdited, this, &FileSelectPathWidget::editored );
	connect( filePathEditLine, &QLineEdit::editingFinished, [=]( ) {
		emit setFilePathFinish( filePathEditLine->placeholderText( ) );
	} );
}
void FileSelectPathWidget::initOver( ) {
}
QString FileSelectPathWidget::getSelectFilePath( ) {
	return this->filePathEditLine->placeholderText( );
}
