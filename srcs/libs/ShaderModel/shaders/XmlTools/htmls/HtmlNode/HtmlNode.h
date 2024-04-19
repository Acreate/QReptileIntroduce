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
		const std::wstring *cppstd_w_str_ptr = nullptr;
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
		std::shared_ptr< std::wstring > getWSNode( );
		std::shared_ptr< std::wstring > getNodeWSName( );
	};

}

#endif // HTMLNODE_H_H_HEAD__FILE__
