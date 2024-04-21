#ifndef XMLTOOLS_H_H_HEAD__FILE__
#define XMLTOOLS_H_H_HEAD__FILE__
#pragma once

#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

#include "../export/XmlTools_export.h" // 导出导入声明
namespace XmlTools {
	static constexpr wchar_t singleQuotation = L'\''; // 单引号
	static constexpr wchar_t exclamation = L'!'; // 感叹号。用于识别 DOCTYPE 节点或注释节点
	static constexpr wchar_t doubleQuotation = L'\"'; // 双引号
	static constexpr wchar_t nodeStartChar = L'<'; // 节点开始
	static constexpr wchar_t nodeEndChar = L'>'; // 节点结束
	static constexpr wchar_t forwardSlash = L'/'; // 斜杠路径符。节点类型判定(单元素节点/双元素节点)
	static constexpr wchar_t backSlash = L'\\'; // 反斜杠路径符
	class XMLTOOLS_EXPORT RefWStr;
	// html 文档
	class XMLTOOLS_EXPORT HtmlDoc;
	// 宽字符工具
	class XMLTOOLS_EXPORT WStrTools;
	// 节点
	class XMLTOOLS_EXPORT HtmlNode;
	// 基于 html 实现的 xptah
	class XMLTOOLS_EXPORT HtmlXPath;
	// 类型
	enum class XMLTOOLS_EXPORT Html_Node_Type;
	// std::shared_ptr< HtmlDoc > 封装
	using HtmlDoc_Shared = std::shared_ptr< HtmlDoc >;
	// std::shared_ptr< HtmlNode > 封装
	using HtmlNode_Shared = std::shared_ptr< HtmlNode >;
	//  std::vector< std::shared_ptr< HtmlNode_Shared > > 封装
	using Vector_HtmlNodeSPtr = std::vector< HtmlNode_Shared >;
	// std::shared_ptr< std::vector< std::shared_ptr< Vector_HtmlNodeSPtr > > > 封装
	using Vector_HtmlNodeSPtr_Shared = std::shared_ptr< Vector_HtmlNodeSPtr >;
	// std::shared_ptr< HtmlXPath > 封装指针
	using HtmlXPath_Shared = std::shared_ptr< HtmlXPath >;
	//  std::vector< std::shared_ptr< HtmlXPath > > 封装
	using Vector_HtmlXPathSPtr = std::vector< HtmlXPath_Shared >;
	// std::shared_ptr< std::vector< std::shared_ptr< HtmlXPath > > > 封装
	using Vector_HtmlXPathSPtr_Shared = std::shared_ptr< Vector_HtmlXPathSPtr >;
	/// <summary>
	/// 字符串配对的列表映射
	/// </summary>
	using WStringPairUnorderMap = std::unordered_map< std::wstring, std::wstring >;
	/// <summary>
	/// 字符串配对的列表映射 的共享指针
	/// </summary>
	using WStringPairUnorderMap_Shared = std::shared_ptr< WStringPairUnorderMap >;
}

#endif // XMLTOOLS_H_H_HEAD__FILE__
