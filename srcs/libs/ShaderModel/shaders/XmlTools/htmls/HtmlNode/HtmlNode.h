#ifndef HTMLNODE_H_H_HEAD__FILE__
#define HTMLNODE_H_H_HEAD__FILE__
#pragma once
#include "../../export/XmlTools_export.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct HtmlNode {
	/// <summary>
	/// html 的内容指针
	/// </summary>
	const wchar_t *htmlContent;
	/// <summary>
	/// 名称指针
	/// </summary>
	const wchar_t *namePtr;
	/// <summary>
	/// 父节点
	/// </summary>
	struct HtmlNode *parent;
	/// <summary>
	/// 子节点指针，以 nullptr 结束的列表
	/// </summary>
	struct HtmlNode *childs;
	/// <summary>
	/// 名称长度
	/// </summary>
	size_t nameLen;

} HtmlNode;
typedef HtmlNode *HtmlNodePtr;
typedef HtmlNode **HtmlNodeList;
/// <summary>
/// 根据节点内容进行解析<br/>
/// 返回的列表以 nullptr 为结束符号的列表<br/>
/// 失败返回 nullptr
/// </summary>
/// <param name="htmlContent">解析的内容</param>
/// <returns>解析的实现</returns>
XMLTOOLS_EXPORT const HtmlNodeList htmlNodeParse( const wchar_t *htmlContent );
/// <summary>
/// 获取列表成员个数
/// </summary>
/// <param name="list">检查的列表</param>
/// <returns>返回个数</returns>
XMLTOOLS_EXPORT size_t htmlNodeListSize( const HtmlNodeList list );
/// <summary>
/// 向列表当中追加一个元素
/// </summary>
/// <param name="list">追加的所在列表</param>
/// <param name="htmlNode">追加的元素</param>
/// <returns>元素位置</returns>
XMLTOOLS_EXPORT size_t htmlNodeListAppend( HtmlNodeList *list, const HtmlNodePtr htmlNode );
/// <summary>
/// 释放一个节点元素列表
/// </summary>
/// <param name="list">释放的列表</param>
/// <returns>成功返回 true</returns>
XMLTOOLS_EXPORT bool htmlNodeListFree( HtmlNodeList *list );
/// <summary>
/// 获取父节点内容<br/>
/// 如果不存在，则返回 nullptr
/// </summary>
/// <param name="currentHtmlNodePtr">检测的节点</param>
/// <returns>父节点</returns>
XMLTOOLS_EXPORT HtmlNodePtr htmlNodeGetRootNodePtr( const HtmlNodePtr currentHtmlNodePtr );

#ifdef __cplusplus
}
#endif

#endif // HTMLNODE_H_H_HEAD__FILE__
