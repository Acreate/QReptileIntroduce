#ifndef HTMLNODE_H_H_HEAD__FILE__
#define HTMLNODE_H_H_HEAD__FILE__
#pragma once
#include "../../export/XmlTools_export.h"
class XMLTOOLS_EXPORT HtmlNode {
public:
	friend class HtmlDoc;
private: // 核心成员
	/// <summary>
	/// 指向开始的地址
	/// </summary>
	wchar_t *cWStrPtr;
	/// <summary>
	/// 节点结束地址
	/// </summary>
	wchar_t *cWStrEndPtr;
	/// <summary>
	/// 字符串长度 <br/>
	/// 表示节点名称需要从 cWStrPtr 获取的长度
	/// </summary>
	size_t htmlSize;
public:
	HtmlNode( );
	HtmlNode( wchar_t *cWStrPtr, wchar_t *cWStrEndPtr, size_t htmlSize );
	virtual ~HtmlNode( );
public:
	wchar_t *getCWStrPtr( ) const {
		return cWStrPtr;
	}
	void setCWStrPtr( wchar_t *const cWStrPtr ) {
		this->cWStrPtr = cWStrPtr;
	}
	wchar_t *getCWStrEndPtr( ) const {
		return cWStrEndPtr;
	}
	void setCWStrEndPtr( wchar_t *const cWStrEndPtr ) {
		this->cWStrEndPtr = cWStrEndPtr;
	}
	size_t getHtmlSize( ) const {
		return htmlSize;
	}
	void setHtmlSize( const size_t htmlSize ) {
		this->htmlSize = htmlSize;
	}
};
#endif // HTMLNODE_H_H_HEAD__FILE__
