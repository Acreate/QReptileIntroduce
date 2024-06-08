#ifndef LOADPLUG_H_H_HEAD__FILE__
#define LOADPLUG_H_H_HEAD__FILE__
#pragma once
#include <QFileInfo>
#include <QGenericPlugin>

#include "path/File.h"
#include <nameSpace/interfacePlugsType.h>

#include "../auto_generate_files/export/Plug_export.h"


class PLUG_EXPORT LoadPlug : public QObject {
public: // 静态函数调用
	/// <summary>
	/// 根据路径生成插件对象
	/// </summary>
	/// <param name="plugFilePath">查找路径</param>
	/// <param name="result_error">返回的错误信息</param>
	/// <param name="name">传递到插件生成器的第一个参数</param>
	/// <param name="spec">传递到插件生成器的第二个参数</param>
	/// <param name="loadClassName">元数据类型名称</param>
	/// <param name="methodName">元数据函数名称</param>
	/// <returns>生成对应与指针的匹配映射</returns>
	static std::pair< QObject *, interfacePlugsType::IRequestNetInterface * > getIRequestNetInterface( const QString &plugFilePath, QString &result_error, const QString &name = QObject::tr( u8"LoadPlug" ), const QString &spec = QObject::tr( u8"getIRequestNetInterface" ), const QString &loadClassName = QObject::tr( u8"RequestNet" ), const QString &methodName = QObject::tr( u8"getRequestNetInterfacePatr" ) );
	/// <summary>
	/// 调用插件的生成函数
	/// </summary>
	/// <param name="outObj">元数据对象</param>
	/// <param name="loadClassName">元数据对象名称</param>
	/// <param name="methodName">元数据调用的函数名称</param>
	/// <returns>返回对象指针</returns>
	static interfacePlugsType::IRequestNetInterface * metaGetResult( QObject *outObj, const QString &loadClassName = QObject::tr( u8"RequestNet" ), const QString &methodName = QObject::tr( u8"getRequestNetInterfacePatr" ) );
private:
	Q_OBJECT;
private:// 运算参考成员
	QString loadClassName = QObject::tr( u8"RequestNet" );  // 加载的类名
	QString methodName = QObject::tr( u8"getRequestNetInterfacePatr" ); // 元数据调用的函数名称
	QString name; // 传递到构造器的第一参数
	QString spec; // 传递到构造器的第二参数
	QString plugPath; // 目录或者文件
	QMap< QString, std::pair< QObject *, interfacePlugsType::IRequestNetInterface * > > lib; // 已经库存的插件
public:
	/// <summary>
	/// 指定搜索路径<br/>
	/// 路径为目录时，递归查找所有文件。<br/>
	/// 路径为文件时，加载该文件
	/// </summary>
	/// <param name="path"></param>
	/// <param name="parent"></param>
	LoadPlug( const QString &path, QObject *parent = nullptr );
	~LoadPlug( ) override;
public: // 属性列表
	QString getLoadClassName( ) const { return loadClassName; }
	QString getMethodName( ) const { return methodName; }
	QString getName( ) const { return name; }
	QString getSpec( ) const { return spec; }
	QString getPlugPath( ) const { return plugPath; }
	void setLoadClassName( const QString &load_class_name ) { loadClassName = load_class_name; }
	void setMethodName( const QString &method_name ) { methodName = method_name; }
	void setName( const QString &name ) { this->name = name; }
	void setSpec( const QString &spec ) { this->spec = spec; }
	void setPlugPath( const QString &plug_path ) { plugPath = plug_path; }
public: // 查找
	/// <summary>
	/// 找库中查找对应的插件<br/>
	/// 如果存在，则存储在 result 上
	/// </summary>
	/// <param name="path">路径</param>
	/// <param name="result">存储容器</param>
	/// <returns>存在返回 true</returns>
	bool findLib( const QString &path, std::pair< QObject *, interfacePlugsType::IRequestNetInterface * > *result );
public: // 公开调用
	/// <summary>
	/// 获取所有插件<br/>
	/// 返回原始对象与转换的匹配对象（二者地址相同）
	/// </summary>
	/// <returns>插件列表映射</returns>
	QMap< QObject *, interfacePlugsType::IRequestNetInterface * > loadPlugs( );
};


#endif // LOADPLUG_H_H_HEAD__FILE__
