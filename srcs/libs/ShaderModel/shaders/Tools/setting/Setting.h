#ifndef SETTING_H_H_HEAD__FILE__
#define SETTING_H_H_HEAD__FILE__
#pragma once

#include <QSettings>
#include <qmutex.h>
#include "../auto_generate_files/export/Tools_export.h"


class TOOLS_EXPORT Setting : public QObject {
	Q_OBJECT;
private:
	QSettings *setting;
	QMutex *instanceMutex;
public: // 构造函数
	Setting( const QString &filePath, QObject *parent = nullptr );
	Setting( QObject *parent  = nullptr);
	~Setting( ) override;
public: // 配置文件的值操作
	/// <summary>
	/// 根据键获取指定的值
	/// </summary>
	/// <param name="key">键</param>
	/// <returns>获取到的值</returns>
	QVariant getValue( const QAnyStringView &key ) const;
	/// <summary>
	/// 根据键获取指定组名中的值
	/// </summary>
	/// <param name="groupName">组名</param>
	/// <param name="key">键</param>
	/// <returns>获取到的值</returns>
	QVariant getValue( const QAnyStringView &groupName, const QAnyStringView &key ) const;
	/// <summary>
	/// 根据键获取指定组名中的值
	/// 返回值必须经过函数验证
	/// 函数验证生成返回值
	/// </summary>
	/// <param name="groupName">组名</param>
	/// <param name="key">键</param>
	/// <param name="ifFunction">验证函数</param>
	/// <returns>经过验证后的值</returns>
	QVariant getValue( const QAnyStringView &groupName, const QAnyStringView &key, const std::function< QVariant( const QVariant & ) > &ifFunction ) const;
	/// <summary>
	/// 根据键获取指定的值
	/// 返回值必须经过函数验证
	/// 函数验证生成返回值
	/// </summary>
	/// <param name="key">键</param>
	/// <param name="ifFunction">验证函数</param>
	/// <returns>经过验证后的值</returns>
	QVariant getValue( const QAnyStringView &key, const std::function< QVariant( const QVariant & ) > &ifFunction ) const;
	/// <summary>
	/// 设置键值对
	/// </summary>
	/// <param name="key">键</param>
	/// <param name="value">值</param>
	bool setValue( const QAnyStringView &key, const QVariant &value );
	/// <summary>
	/// 设置指定组名下的键值对
	/// </summary>
	/// <param name="groupName">组名</param>
	/// <param name="key">键</param>
	/// <param name="value">值</param>
	bool setValue( const QAnyStringView &groupName, const QAnyStringView &key, const QVariant &value );
	/// <summary>
	/// 设置所有键值对
	/// </summary>
	/// <param name="valuePair">键值对列表</param>
	bool setValue( const QMap< QAnyStringView, QVariant > &valuePair );
	/// <summary>
	/// 设置指定组名下的所有键值对
	/// </summary>
	/// <param name="groupName">组名</param>
	/// <param name="valuePair">键值对列表</param>
	bool setValue( const QAnyStringView &groupName, const QMap< QAnyStringView, QVariant > &valuePair );

	/// <summary>
	/// 获取所有键
	/// </summary>
	/// <returns>所有键</returns>
	QStringList getAllKey( ) const;
	/// <summary>
	/// 获取指定组的所有键
	/// </summary>
	/// <param name="group">组名</param>
	/// <returns>组的所有键</returns>
	QStringList getAllKey( const QAnyStringView &group ) const;
	/// <summary>
	/// 获取所有值
	/// </summary>
	/// <returns>获取所有值</returns>
	QList< QVariant > getAllValue( ) const;
	/// <summary>
	/// 获取组名下的所有值
	/// </summary>
	/// <param name="group">组名</param>
	/// <returns>获取所有值</returns>
	QList< QVariant > getAllValue( const QAnyStringView &group ) const;
	/// <summary>
	/// 获取所有键值对
	/// </summary>
	/// <returns>获取所有键值对</returns>
	QMap< QString, QVariant > getAllInfo( ) const;
	/// <summary>
	/// 获取指定组下的所有键值对
	/// </summary>
	/// <param name="group">组名</param>
	/// <returns>组下的所有键值对</returns>
	QMap< QString, QVariant > getAllInfo( const QAnyStringView &group ) const;
	/// <summary>
	/// 同步
	/// </summary>
	bool sync( );
public:
	/// <summary>
	/// 设置配置文件路径<br/>
	/// 对待重复路径，它会跳过设置功能，并且返回 false
	/// </summary>
	/// <param name="filePath">新的文件路径</param>
	/// <returns>成功返回 true</returns>
	bool setFilePath( const QString &filePath );
	/// <summary>
	/// 获取文件路径
	/// </summary>
	/// <returns>当前的文件路径</returns>
	QString getFilePath( ) const;
private:
	void clearGroup( );
};

#endif // SETTING_H_H_HEAD__FILE__
