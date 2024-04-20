#ifndef HTMLNODE_H_H_HEAD__FILE__
#define HTMLNODE_H_H_HEAD__FILE__
#pragma once
#include <string>

#include "../../nameSpace/XmlTools.h"

#include "../enum/HtmlNodeType/Html_Node_Type.h"

namespace XmlTools {
	class XMLTOOLS_EXPORT HtmlNode {
	public:
		friend class HtmlDoc;
	private: // 私有成员变量
		/// <summary>
		/// 字符串指针
		/// </summary>
		std::shared_ptr< std::wstring > c_w_z_str = nullptr;
		/// <summary>
		/// 字符串偏移量
		/// </summary>
		size_t ptr_offset = 0;
		/// <summary>
		/// 字符串长度
		/// </summary>
		size_t ptr_c_str_len = 0;
		/// <summary>
		/// 节点类型
		/// </summary>
		Html_Node_Type nodeType = Html_Node_Type::None;
	public:
		HtmlNode( );
		virtual ~HtmlNode( );
		std::shared_ptr< std::wstring > getWSNode( ) const;
		std::shared_ptr< std::wstring > getNodeWSName( ) const;
	private:
		/// <summary>
		/// 生成 < 与 > 的配对
		/// </summary>
		/// <param name="std_w_str">检查的字符串</param>
		/// <param name="start_index">遍历的开始下标</param>
		/// <param name="max_index">遍历的结束下标</param>
		/// <param name="index_count">遍历的个数</param>
		/// <returns>配对列表</returns>
		static Vector_HtmlNodeSPtr_Shared parseHtmlNodeCharPair( std::shared_ptr< std::wstring > std_w_str, size_t start_index, const size_t max_index, size_t &index_count );
	};

}

#endif // HTMLNODE_H_H_HEAD__FILE__
