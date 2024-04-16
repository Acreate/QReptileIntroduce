#ifndef HTMLDOC_H_H_HEAD__FILE__
#define HTMLDOC_H_H_HEAD__FILE__
#pragma once
#include <functional>
#include <memory>
#include <string>

#include "../../export/XmlTools_export.h"
#include "../../htmls/HtmlNode/HtmlNode.h"

class HtmlDoc;
class XMLTOOLS_EXPORT HtmlDoc {
public:

private: // 核心成员
	wchar_t *html;
	size_t htmlSize;
private:
	/// <summary>
	/// 名称指针与结束点
	/// </summary>
	using SubNode = std::vector< HtmlNode >;

	/// <summary>
	/// 其他的常规节点
	/// </summary>
	SubNode *normalNode;
private: // 无法使用个构造函数
	HtmlDoc( );

	/// <summary>
	/// 开始解析 <br/>
	/// 成功返回 0，否则失败
	/// </summary>
	/// <returns>执行返回代码标识</returns>
	int32_t init( );
	HtmlDoc *newObj( );
public:
	virtual ~HtmlDoc( );
public:
	static std::shared_ptr< HtmlDoc > parse( wchar_t *c_str, const size_t c_str_len );
};

#endif // HTMLDOC_H_H_HEAD__FILE__
