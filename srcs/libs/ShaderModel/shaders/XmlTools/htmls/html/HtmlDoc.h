﻿#ifndef HTMLDOC_H_H_HEAD__FILE__
#define HTMLDOC_H_H_HEAD__FILE__
#pragma once
#include <functional>
#include <memory>
#include <string>

#include "../../nameSpace/XmlTools.h"

#include "../enum/HtmlNodeType/Html_Node_Type.h"

namespace XmlTools {
	class HtmlNode;

	class XMLTOOLS_EXPORT HtmlDoc {
	public: // 友元
		friend class HtmlNode;
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
		/// <param name="std_c_w_string">查找字符串</param>
		/// <param name="max_index">字符串的推测长度</param>
		/// <param name="start_index">遍历下标</param>
		/// <returns>true 表示存在</returns>
		static bool findNextNodeEndChar( const std::shared_ptr< std::wstring > std_c_w_string, size_t &max_index, size_t &start_index );

		/// <summary>
		/// 查找下一个节点开始符的位置
		/// </summary>
		/// <param name="std_c_w_string">查找字符串</param>
		/// <param name="max_index">字符串的推测长度</param>
		/// <param name="start_index">遍历下标</param>
		/// <returns>true 表示存在</returns>
		static bool findNextNodeStartChar( const std::shared_ptr< std::wstring > std_c_w_string, size_t &max_index, size_t &start_index );

		/// <summary>
		/// 查找下一个节点的斜杠位置
		/// </summary>
		/// <param name="std_c_w_string">查找字符串</param>
		/// <param name="max_index">字符串的推测长度</param>
		/// <param name="start_index">遍历下标</param>
		/// <returns>true 表示存在</returns>
		static bool findNextNodeForwardSlash( const std::shared_ptr< std::wstring > std_c_w_string, size_t &max_index, size_t &start_index );

		/// <summary>
		/// 判断节点是否为单节点
		/// </summary>
		/// <param name="std_c_w_string">查找字符串</param>
		/// <param name="start_index">遍历下标，始终指向节点开始字符 '<'，如果不是，那么它将会向后查询</param>
		/// <param name="end_index">结束下标，始终指向节点结束字符 '>'</param>
		/// <returns>true 表示单下标</returns>
		static bool isSingelNode( const std::shared_ptr< std::wstring > std_c_w_string, size_t &start_index, size_t &end_index );

		/// <summary>
		/// 判断节点是否为开始节点
		/// </summary>
		/// <param name="std_c_w_string">查找字符串</param>
		/// <param name="start_index">遍历下标，始终指向 '<'</param>
		/// <param name="end_index">结束下标，始终指向节点结束字符 '>'</param>
		/// <returns>true 表示单下标</returns>
		static bool isStartNode( const std::shared_ptr< std::wstring > std_c_w_string, size_t &start_index, size_t &end_index );

		/// <summary>
		/// 判断节点是否为结束节点
		/// </summary>
		/// <param name="std_c_w_string">查找字符串</param>
		/// <param name="start_index">遍历下标，始终指向 '<'</param>
		/// <param name="end_index">结束下标，始终指向节点结束字符 '>'</param>
		/// <returns>true 表示单下标</returns>
		static bool isEndNode( const std::shared_ptr< std::wstring > std_c_w_string, size_t &start_index, size_t &end_index );

		/// <summary>
		/// 判断节点是否为注释节点(包含 !DOCTYPE 节点)
		/// </summary>
		/// <param name="std_c_w_string">查找字符串</param>
		/// <param name="start_index">遍历下标，始终指向节点开始字符 '<'，如果不是，那么它将会向后查询</param>
		/// <param name="end_index">结束下标，始终指向节点结束字符 '>'</param>
		/// <returns>true 表示注释节点</returns>
		static bool isAnnotation( const std::shared_ptr< std::wstring > std_c_w_string, size_t &start_index, size_t &end_index );
	public: // 静态对象生成器
		/// <summary>
		/// 根据字符串内容生成节点列表
		/// </summary>
		/// <param name="std_c_w_string">指向字符串的指针</param>
		/// <param name="end_index">结束下标</param>
		/// <param name="start_index">开始下标，最终下标</param>
		/// <returns>解析后的列表</returns>
		static HtmlDoc parse( const std::shared_ptr< std::wstring > std_c_w_string, size_t &end_index, size_t &start_index );
	private: // 核心成员
		/// <summary>
		/// 引用的 html 内容
		/// </summary>
		std::shared_ptr< std::wstring > html_W_C_Str;

		/// <summary>
		/// 文件的顶级节点
		/// </summary>
		Vector_HtmlNodeSPtr_Shared htmlDocNode;
	public:
		/// <summary>
		/// 获取指定名称的节点
		/// </summary>
		/// <param name="nodeName">获取的名称</param>
		/// <returns>节点，失败返回 nullptr</returns>
		HtmlNode_Shared getNodeFromName( const std::wstring &nodeName ) const;

		/// <summary>
		/// 获取指定的节点<br/>
		/// 未发生 return true 时，该函数会继续滴啊用 callFun 遍历成员
		/// </summary>
		/// <param name="callFun">校验函数</param>
		/// <returns>节点，失败返回 nullptr</returns>
		HtmlNode_Shared getNodeFromName( const std::function< bool( const std::wstring &nodeName, Html_Node_Type htmlNodeType ) > &callFun ) const;
	private: // 无法使用个构造函数
		HtmlDoc( );
	public:
		virtual ~HtmlDoc( );
	};
}

#endif // HTMLDOC_H_H_HEAD__FILE__
