#ifndef NOVELDBJOB_H_H_HEAD__FILE__
#define NOVELDBJOB_H_H_HEAD__FILE__
#pragma once
#include <QDateTime>

#include "nameSpace/interfacePlugsType.h"
#include "../auto_generate_files/export/Plug_export.h"
#include "stream/OStream.h"
class PLUG_EXPORT NovelDBJob {
public: // 新类型
	using NovelTypeNameType = QString;
	using NovelUrlHostType = QString;
	using NovelInfoVector = interfacePlugsType::Vector_INovelInfoSPtr;
	using NovelInfoVector_Shared = std::shared_ptr< NovelInfoVector >;
	using NovelTypePair = std::pair< NovelTypeNameType, NovelInfoVector_Shared >;
	using NovelTypePairVector = std::vector< NovelTypePair >;
	using NovelTypePairVector_Shared = std::shared_ptr< NovelTypePairVector >;
	using NovelHostMap = std::unordered_map< NovelUrlHostType, NovelTypePairVector_Shared >;
	using NovelHostMap_Shared = std::shared_ptr< NovelHostMap >;
private: // - 静态成员变量
	static QDateTime currentTime; // 请求时间
	static QString currentTimeForm; // 当前请求时间的格式
	static QStringList tabFieldNames; // 表字体信息
	static QString updateCmd; // 更新命令
	static QString insterCmd; // 插入命令

public:// - 静态方法
	/// <summary>
	/// 对小说列表进行排序，并且返回结果
	/// </summary>
	/// <param name="infos">排序列表</param>
	/// <returns>排序结果</returns>
	static NovelInfoVector sort( const NovelInfoVector &infos );
	/// <summary>
	/// 剔除相同
	/// </summary>
	/// <param name="infos">剔除列表</param>
	/// <returns>剔除完毕的列表</returns>
	static NovelInfoVector identical( const NovelInfoVector &infos );
	/// <summary>
	/// 分解列表<br/>
	///	网站域名为 key - 网页类型与小说列表组成的 pair 成为 value
	/// </summary>
	/// <param name="infos">分解列表</param>
	/// <returns>分解返回</returns>
	static NovelHostMap decompose( const NovelInfoVector &infos );
	/// <summary>
	/// 写入数据库
	/// </summary>
	/// <param name="thisOStream">可选信息输出流</param>
	/// <param name="outPath">目标根目录</param>
	/// <param name="url">写入的url</param>
	/// <param name="saveNovelInfos">写入的小说</param>
	/// <param name="run">线程工作当中时，会调用该函数</param>
	/// <returns>写入个数</returns>
	static size_t writeDB( OStream *thisOStream, const QString &outPath, const QUrl &url, const NovelInfoVector &saveNovelInfos, const std::function< bool( const std::chrono::system_clock::time_point::duration & ) > &run );
	/// <summary>
	/// 读取数据库
	/// </summary>
	/// <param name="thisOStream">可选信息输出流</param>
	/// <param name="outPath">目标根目录</param>
	/// <param name="run">线程工作当中时，会调用该函数</param>
	/// <returns>写入个数</returns>
	static NovelInfoVector_Shared readDB( OStream *thisOStream, const QString &outPath, const std::function< bool( const std::chrono::system_clock::time_point::duration & ) > &run );
	/// <summary>
	/// 写入小说列表
	/// </summary>
	/// <param name="writeFilePath">写入路径</param>
	/// <param name="infos">待写入列表</param>
	/// <returns>写入列表-完成排序</returns>
	static NovelInfoVector writeFile( const QString &writeFilePath, const NovelInfoVector &infos );

	/// <summary>
	/// 写入小说
	/// </summary>
	/// <param name="root_path">根路径</param>
	/// <param name="novel_host">网站域名-子目录</param>
	/// <param name="infos">写入小说</param>
	/// <returns>已经排序的小说</returns>
	static NovelInfoVector writeFile( const QString &root_path, const QString &novel_host, const NovelInfoVector &infos );
	/// <summary>
	/// 写入小说
	/// </summary>
	/// <param name="root_path">根路径</param>
	/// <param name="novel_host">网站域名-次目录</param>
	/// <param name="novel_type">小说类型-次目录下的文件名</param>
	/// <param name="infos">写入小说</param>
	/// <returns>已经排序的小说</returns>
	static NovelInfoVector writeFile( const QString &root_path, const QString &novel_host, const QString &novel_type, const NovelInfoVector &infos );
};


#endif // NOVELDBJOB_H_H_HEAD__FILE__
