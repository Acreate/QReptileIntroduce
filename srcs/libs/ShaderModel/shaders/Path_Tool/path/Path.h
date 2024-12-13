#ifndef PATH_H_H_HEAD__FILE__
#define PATH_H_H_HEAD__FILE__
#pragma once
#include <qcontainerfwd.h>

#include "./Dir.h"

#include "../auto_generate_files/export/Path_Tool_export.h"
#include "../auto_generate_files/macro/cmake_to_c_cpp_header_env.h"

class PATH_TOOL_EXPORT Path {
public:
	using FileList = std::vector< File >;
	using DirList = std::vector< Dir >;
	static std::pair< DirList, FileList > getPathInfo( const QString &path );
	static std::pair< DirList, FileList > getDirInfo( const QString &path );
	static std::pair< DirList, FileList > getFileInfo( const QString &path );

	/// <summary>
	/// 切分一个路径
	/// </summary>
	/// <param name="path">被切分的路径</param>
	/// <returns>切分完毕的路径</returns>
	static QStringList splitPath( const QString &path );
	/// <summary>
	/// 创建文件
	/// </summary>
	/// <param name="path">文件的路径</param>
	/// <returns>失败返回 false</returns>
	static bool creatFilePath( const QString &path );
	/// <summary>
	/// 创建目录
	/// </summary>
	/// <param name="path">目录路径</param>
	/// <returns>失败返回 false</returns>
	static bool creatDirPath( const QString &path );
	/// <summary>
	/// 删除路径
	/// </summary>
	/// <param name="path">删除的路径</param>
	/// <returns>失败返回 false</returns>
	static bool removePath( const QString &path );
	/// <summary>
	/// 删除文件
	/// </summary>
	/// <param name="path">文件路径</param>
	/// <returns>失败返回 false</returns>
	static bool removeFilePath( const QString &path );
	/// <summary>
	/// 删除目录
	/// </summary>
	/// <param name="path">目录路径</param>
	/// <returns>失败返回 false</returns>
	static bool removeDirPath( const QString &path );
	/// <summary>
	/// 更新 Dir 路径信息
	/// </summary>
	/// <param name="dir">更新对象</param>
	/// <returns>更新个数</returns>
	static size_t updateInfo( Dir &dir );
	/// <summary>
	/// 拷贝路径数据
	/// </summary>
	/// <param name="src">拷贝源</param>
	/// <param name="des">数据输出目标</param>
	/// <returns>拷贝个数</returns>
	static size_t copyDirInfoToOtherDir( Dir &src, Dir &des );
};

#endif // PATH_H_H_HEAD__FILE__
