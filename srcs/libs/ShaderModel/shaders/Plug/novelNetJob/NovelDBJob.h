#ifndef NOVELDBJOB_H_H_HEAD__FILE__
#define NOVELDBJOB_H_H_HEAD__FILE__
#pragma once
#include <QDateTime>

#include "nameSpace/interfacePlugsType.h"
#include "../auto_generate_files/export/Plug_export.h"
#include "stream/OStream.h"
class PLUG_EXPORT NovelDBJob {
	static QDateTime currentTime; // 请求时间
	static QString currentTimeForm; // 当前请求时间的格式
	static QStringList tabFieldNames; // 表字体信息
	static QString updateCmd; // 更新命令
	static QString insterCmd; // 插入命令
public:
	/// <summary>
	/// 对小说列表进行排序，并且返回结果
	/// </summary>
	/// <param name="infos">排序列表</param>
	/// <returns>排序结果</returns>
	static interfacePlugsType::Vector_INovelInfoSPtr sort( const interfacePlugsType::Vector_INovelInfoSPtr &infos );
	/// <summary>
	/// 剔除相同
	/// </summary>
	/// <param name="infos">剔除列表</param>
	/// <returns>剔除完毕的列表</returns>
	static interfacePlugsType::Vector_INovelInfoSPtr identical( const interfacePlugsType::Vector_INovelInfoSPtr &infos );
	/// <summary>
	/// 写入数据库
	/// </summary>
	/// <param name="thisOStream">可选信息输出流</param>
	/// <param name="outPath">目标根目录</param>
	/// <param name="url">写入的url</param>
	/// <param name="saveNovelInfos">写入的小说</param>
	/// <param name="run">线程工作当中时，会调用该函数</param>
	/// <returns>写入个数</returns>
	static size_t writeDB( OStream *thisOStream, const QString &outPath, const QUrl &url, const interfacePlugsType::Vector_INovelInfoSPtr &saveNovelInfos, const std::function< bool( const std::chrono::system_clock::time_point::duration & ) > &run );
	/// <summary>
	/// 读取数据库
	/// </summary>
	/// <param name="thisOStream">可选信息输出流</param>
	/// <param name="outPath">目标根目录</param>
	/// <param name="run">线程工作当中时，会调用该函数</param>
	/// <returns>写入个数</returns>
	static interfacePlugsType::Vector_INovelInfoSPtr_Shared readDB( OStream *thisOStream, const QString &outPath, const std::function< bool( const std::chrono::system_clock::time_point::duration & ) > &run );
	/// <summary>
	/// 写入小说列表
	/// </summary>
	/// <param name="writeFilePath">写入路径</param>
	/// <param name="infos">待写入列表</param>
	/// <returns>写入列表-完成排序</returns>
	static interfacePlugsType::Vector_INovelInfoSPtr writeFile( const QString &writeFilePath, const interfacePlugsType::Vector_INovelInfoSPtr &infos );
};


#endif // NOVELDBJOB_H_H_HEAD__FILE__
