#ifndef HTMLDOC_H_H_HEAD__FILE__
#define HTMLDOC_H_H_HEAD__FILE__
#pragma once
#include <functional>
#include <memory>
#include <string>

#include "../../export/XmlTools_export.h"

class HtmlNode;

class XMLTOOLS_EXPORT HtmlDoc {
public: // 友元
	friend class HtmlNode;
public: // 定义
	using HtmlDocSharedPtr = std::shared_ptr< HtmlDoc >;
	using HtmlDocSharedPtrVector = std::vector< HtmlDocSharedPtr >;
	using HtmlDocSharedPtrVectorSharedPtr = std::shared_ptr< HtmlDocSharedPtrVector >;
	struct XMLTOOLS_EXPORT HtmlNodeCharPair {
		std::wstring *html;
		size_t left;
		size_t len;
		HtmlNodeCharPair( std::wstring *html = nullptr, size_t left = 0, size_t right = 0 )
		: html( html ),
		left( left ),
		len( right ) {
		}
		std::shared_ptr< std::wstring > getWSNode( ) {
			const wchar_t * c_w_str_star_ptr = html->c_str( ) + left;
			std::shared_ptr< std::wstring > result( new std::wstring( c_w_str_star_ptr, len ) );
			return result;
		}
		std::shared_ptr< std::wstring > getNodeWSName( );

	};
	using HtmlNodeCharPairSharedPtr = std::shared_ptr< HtmlNodeCharPair >;
	using HtmlNodeCharPairSharedPtrVector = std::vector< HtmlNodeCharPairSharedPtr >;
	using HtmlNodeCharPairSharedPtrVectorSharedPtr = std::shared_ptr< HtmlNodeCharPairSharedPtrVector >;
private:
	HtmlNodeCharPairSharedPtrVectorSharedPtr html;
private: // 静态属性
	static constexpr wchar_t singleQuotation = L'\''; // 单引号
	static constexpr wchar_t exclamation = L'!'; // 感叹号。用于识别 DOCTYPE 节点或注释节点
	static constexpr wchar_t doubleQuotation = L'\"'; // 双引号
	static constexpr wchar_t nodeStartChar = L'<'; // 节点开始
	static constexpr wchar_t nodeEndChar = L'>'; // 节点结束
	static constexpr wchar_t forwardSlash = L'/'; // 斜杠路径符。节点类型判定(单元素节点/双元素节点)
	static constexpr wchar_t backSlash = L'\\'; // 反斜杠路径符
private:
	/// <summary>
	/// 查找下一个节点结束符的位置
	/// </summary>
	/// <param name="c_str">查找字符串</param>
	/// <param name="c_str_len">字符串长度</param>
	/// <param name="start_Index">遍历下标</param>
	/// <returns>true 表示存在</returns>
	static bool findNextNodeEndChar( const wchar_t *c_str, const size_t c_str_len, size_t *start_Index );
	/// <summary>
	/// 查找下一个节点开始符的位置
	/// </summary>
	/// <param name="c_str">查找字符串</param>
	/// <param name="c_str_len">字符串长度</param>
	/// <param name="start_Index">遍历下标</param>
	/// <returns>true 表示存在</returns>
	static bool findNextNodeStartChar( const wchar_t *c_str, const size_t c_str_len, size_t *start_Index );
	/// <summary>
	/// 查找下一个节点的斜杠位置
	/// </summary>
	/// <param name="c_str">查找字符串</param>
	/// <param name="c_str_len">字符串长度</param>
	/// <param name="start_Index">遍历下标</param>
	/// <returns>true 表示存在</returns>
	static bool findNextNodeForwardSlash( const wchar_t *c_str, const size_t c_str_len, size_t *start_Index );
	/// <summary>
	/// 判断节点是否为单节点
	/// </summary>
	/// <param name="c_str">查找字符串</param>
	/// <param name="start_Index">遍历下标，始终指向节点开始字符 '<'，如果不是，那么它将会向后查询</param>
	/// <param name="end_Index">结束下标，始终指向节点结束字符 '>'</param>
	/// <returns>true 表示单下标</returns>
	static bool isSingelNode( const wchar_t *c_str, size_t *start_Index, size_t *end_Index );
	/// <summary>
	/// 判断节点是否为开始节点
	/// </summary>
	/// <param name="c_str">查找字符串</param>
	/// <param name="start_Index">遍历下标，始终指向 '<'</param>
	/// <param name="end_Index">结束下标，始终指向节点结束字符 '>'</param>
	/// <returns>true 表示单下标</returns>
	static bool isStartNode( const wchar_t *c_str, size_t *start_Index, size_t *end_Index );
	/// <summary>
	/// 判断节点是否为结束节点
	/// </summary>
	/// <param name="c_str">查找字符串</param>
	/// <param name="start_Index">遍历下标，始终指向 '<'</param>
	/// <param name="end_Index">结束下标，始终指向节点结束字符 '>'</param>
	/// <returns>true 表示单下标</returns>
	static bool isEndNode( const wchar_t *c_str, size_t *start_Index, size_t *end_Index );
	/// <summary>
	/// 判断节点是否为注释节点(包含 !DOCTYPE 节点)
	/// </summary>
	/// <param name="c_str">查找字符串</param>
	/// <param name="start_Index">遍历下标，始终指向节点开始字符 '<'，如果不是，那么它将会向后查询</param>
	/// <param name="end_Index">结束下标，始终指向节点结束字符 '>'</param>
	/// <returns>true 表示注释节点</returns>
	static bool isAnnotation( const wchar_t *c_str, size_t *start_Index, size_t *end_Index );
public: // 静态对象生成器
	/// <summary>
	/// 根据字符串内容生成节点列表
	/// </summary>
	/// <param name="c_str">指向字符串的指针</param>
	/// <param name="c_str_len">字符串长度</param>
	/// <param name="startIndex">开始下标，最终下标</param>
	/// <returns>解析后的列表</returns>
	static HtmlDoc parse( const wchar_t *c_str, const size_t c_str_len, size_t *startIndex );
	/// <summary>
	/// 生成 < 与 > 的配对
	/// </summary>
	/// <param name="std_w_str">检查的字符串</param>
	/// <param name="c_str_len">字符串长度</param>
	/// <returns>配对列表</returns>
	static HtmlNodeCharPairSharedPtrVectorSharedPtr parseHtmlNodeCharPair( std::wstring *std_w_str, const size_t c_str_len );
private: // 核心成员
	/// <summary>
	/// 引用的 html 内容
	/// </summary>
	std::shared_ptr< std::wstring > html_W_C_Str;
private:
	std::vector< HtmlNode * > *children;
private: // 无法使用个构造函数
	HtmlDoc( );
public:
	virtual ~HtmlDoc( );

};

#endif // HTMLDOC_H_H_HEAD__FILE__
