#ifndef FILESELECTPATHWIDGET_H_H_HEAD__FILE__
#define FILESELECTPATHWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QObject>
#include <QWidget>
class EditLine;
class QLabel;
class Button;
class HLayoutBox;
class FileSelectPathWidget : public QWidget {

	Q_OBJECT;
private:
	HLayoutBox *hLayoutBox; // 主要布局
	QLabel *hint; // 提示
	EditLine *filePathEditLine; // 路径显示框（可编辑）
	Button *selectFileBtn; // 文件路径选择矿弹出按钮
public:
	FileSelectPathWidget( QWidget *parent );
	~FileSelectPathWidget( ) override;
private:
	void initComponent( ); // 初始化声明
	void initUI( ); // 初始化 ui（应用布局）
	void initConnect( ); // 初始化链接（链接信号槽）
	void initOver( ); // 初始化完毕
public:
	QString getSelectFilePath();
Q_SIGNALS: // 产生信号
	/// <summary>
	/// 设置路径<br/>
	/// 路径将会显示在 filePathEditLine 当中
	/// </summary>
	/// <param name="filePath">路径</param>
	void setFilePath( const QString &filePath );
	/// <summary>
	/// 当 filePathEditLine 编辑完成之后产生该信号
	/// </summary>
	/// <param name="filePath">路径</param>
	void editored( const QString &filePath );
	/// <summary>
	/// 错误诞生时，发送该信号
	/// </summary>
	/// <param name="type">信号类型</param>
	void error( int type );
	/// <summary>
	/// 选择文件信号
	/// </summary>
	void selectcSignal( );
};


#endif // FILESELECTPATHWIDGET_H_H_HEAD__FILE__
