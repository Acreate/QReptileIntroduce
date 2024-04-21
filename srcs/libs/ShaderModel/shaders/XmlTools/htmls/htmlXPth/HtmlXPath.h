#ifndef XPATH_H_H_HEAD__FILE__
#define XPATH_H_H_HEAD__FILE__
#pragma once

#include <string>

#include "../../nameSpace/XmlTools.h"

namespace XmlTools {
	class XMLTOOLS_EXPORT HtmlXPath {
	public: // 友元
		friend class HtmlDoc;
		friend class HtmlNode;
	private:
		/// <summary>
		/// 从 HtmlDoc 类型转到 HtmlXPath
		/// </summary>
		/// <param name="html_doc">需要被转换的对象指针</param>
		/// <returns>完成转换的 HtmlXPath 指针。失败返回 nullptr</returns>
		static HtmlXPath_Shared converXPtah( const HtmlDoc_Shared html_doc );
	private: // 引用
		HtmlDoc_Shared htmlDoc; // 解析引用的节点对象
	private: // 成员
		HtmlXPath_Shared root; // 根节点
	private: // 属性
		HtmlNode_Shared refNode; // 当前节点信息
		std::shared_ptr< std::wstring > nodeName; // 路径
	private:
		HtmlXPath( );
		HtmlXPath( HtmlDoc_Shared clone_objPtr, HtmlXPath_Shared parent, HtmlNode_Shared html_node_shared );
		HtmlXPath( HtmlDoc_Shared clone_objPtr );
	public:
		virtual ~HtmlXPath( );
	public:
		bool isRootNode( ) const {
			if( root.get( ) == this )
				return true;
			return false;
		}
	public:
		//HtmlXPath_Shared findNodeFromName( const std::wstring &name );
		std::shared_ptr< std::wstring > getNode( );
	};

}


#endif // XPATH_H_H_HEAD__FILE__
