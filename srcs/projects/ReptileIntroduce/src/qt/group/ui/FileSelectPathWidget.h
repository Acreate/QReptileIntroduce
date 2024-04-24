#ifndef FILESELECTPATHWIDGET_H_H_HEAD__FILE__
#define FILESELECTPATHWIDGET_H_H_HEAD__FILE__
#pragma once
#include <QFileDialog>
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
	QString relativeFilePath; // 保存相对路径
public:
	FileSelectPathWidget( QWidget *parent );
	~FileSelectPathWidget( ) override;
private:
	void initComponent( ); // 初始化声明
	void initUI( ); // 初始化 ui（应用布局）
	void initConnect( ); // 初始化链接（链接信号槽）
	void initOver( ); // 初始化完毕
public:
	QString getSelectFilePath( );
	/// <summary>
	/// 更新路径。失败返回 false
	/// </summary>
	/// <param name="newPath">新的路径</param>
	/// <returns>成功返回 true</returns>
	inline bool updatePath( const QString &newPath );
Q_SIGNALS: // 产生信号-被动触发
	/// <summary>
	/// 设置路径
	/// </summary>
	/// <param name="filePath">路径</param>
	void setPath( const QString &filePath );
	/// <summary>
	/// 显示窗口信号
	/// </summary>
	/// <param name="caption">显示标题</param>
	/// <param name="dir">显示路径</param>
	/// <param name="filter">过滤器列表</param>
	/// <param name="selectedFilter">默认的过滤器</param>
	/// <param name="options">选项</param>
	void showFileSelectDialog( const QString &caption = QString( ), const QString &dir = QString( ), const QString &filter = QString( ), QString *selectedFilter = nullptr, QFileDialog::Options options = QFileDialog::Options( ) );
Q_SIGNALS:// 产生信号-主动触发
	/// <summary>
	/// 当 filePathEditLine 编辑时会产生该信号
	/// </summary>
	/// <param name="filePath">路径</param>
	void editored( const QString &filePath );
	/// <summary>
	/// 当 filePathEditLine 编辑完成时会产生该信号
	/// </summary>
	/// <param name="filePath">路径</param>
	void editorOver( const QString &filePath );
	/// <summary>
	/// 错误诞生时，发送该信号
	/// </summary>
	/// <param name="type">信号类型</param>
	/// <param name="msg">错误信息</param>
	void error( int type, const QString &msg );
	/// <summary>
	/// 选择文件信号
	/// </summary>
	void selectcSignal( );
	/// <summary>
	/// 路径设置完毕之后产生信号
	/// </summary>
	/// <param name="file">文件路径</param>
	void selectFileOver( const QString &file );
	/// <summary>
	/// 设置路径<br/>
	/// 路径将会显示在 filePathEditLine 当中
	/// </summary>
	/// <param name="filePath">路径</param>
	void setFilePathFinish( const QString &filePath );
};


#endif // FILESELECTPATHWIDGET_H_H_HEAD__FILE__
