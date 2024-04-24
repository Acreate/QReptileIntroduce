#include "FileSelectPathWidget.h"

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
	hint = new QLabel( u8"路径 : ", this );
	filePathEditLine = new EditLine( this );
	selectFileBtn = new Button( u8"选择文件", this );
	hLayoutBox = new HLayoutBox( this );
	hLayoutBox->addWidget( hint );
	hLayoutBox->addWidget( filePathEditLine );
	hLayoutBox->addWidget( selectFileBtn );
}
void FileSelectPathWidget::initUI( ) {
}
void FileSelectPathWidget::initConnect( ) {
}
void FileSelectPathWidget::initOver( ) {
}
QString FileSelectPathWidget::getSelectFilePath( ) {
	return this->filePathEditLine->placeholderText( );
}
