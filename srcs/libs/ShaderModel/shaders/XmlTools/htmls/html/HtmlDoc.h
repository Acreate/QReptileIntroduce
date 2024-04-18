#ifndef HTMLDOC_H_H_HEAD__FILE__
#define HTMLDOC_H_H_HEAD__FILE__
#pragma once
#include <functional>
#include <memory>
#include <string>

#include "../../export/XmlTools_export.h"
#include "../../htmls/HtmlNode/HtmlNode.h"

class XMLTOOLS_EXPORT HtmlDoc {
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
		std::shared_ptr< std::wstring > getWString( ) {
			std::shared_ptr< std::wstring > result( new std::wstring( html->c_str( ) + left, len ) );
			return result;
		}
	};
	using HtmlNodeCharPairSharedPtr = std::shared_ptr< HtmlNodeCharPair >;
	using HtmlNodeCharPairSharedPtrVector = std::vector< HtmlNodeCharPairSharedPtr >;
	using HtmlNodeCharPairSharedPtrVectorSharedPtr = std::shared_ptr< HtmlNodeCharPairSharedPtrVector >;
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
	/// <param name="start_Index">遍历下标，始终指向 '/'，如果不是，那么它将会指向 '>'</param>
	/// <param name="end_Index">结束下标，始终指向节点结束字符 '>'</param>
	/// <returns>true 表示单下标</returns>
	static bool isStartNode( const wchar_t *c_str, size_t *start_Index, size_t *end_Index );
	/// <summary>
	/// 判断节点是否为结束节点
	/// </summary>
	/// <param name="c_str">查找字符串</param>
	/// <param name="start_Index">遍历下标，始终指向 '/'，如果不是，那么它将会指向其他字符</param>
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
	static HtmlDocSharedPtrVectorSharedPtr parse( const wchar_t *c_str, const size_t c_str_len, size_t *startIndex );
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
	std::shared_ptr< std::wstring > html;
	/// <summary>
	/// 在引用内容当中的起始偏移指针
	/// </summary>
	size_t startPtr;
	/// <summary>
	/// 节点占用长度
	/// </summary>
	size_t nlen;
public: // get 或 set
	size_t getStartPtr( ) const {
		return startPtr;
	}
	void setStartPtr( size_t startPtr ) {
		this->startPtr = startPtr;
	}
	size_t getNen( ) const {
		return nlen;
	}
	void setlen( const size_t nlen ) {
		this->nlen = nlen;
	}

	/// <summary>
	/// 获取字符串
	/// </summary>
	/// <param name="out_result_w_string">返回的字符串</param>
	/// <returns>返回的字符串长度</returns>
	size_t getString( std::wstring *out_result_w_string );
private:
	HtmlDoc *parent;
	std::list< HtmlDoc * > *children;
public:
	void setParent( HtmlDoc *parent ) {
		this->parent->children->remove( this );
		if( parent != nullptr )
			this->parent->children->emplace_back( this );
		this->parent = parent;

	}
private: // 无法使用个构造函数
	HtmlDoc( );
	HtmlDoc( const std::shared_ptr< std::wstring > &html, size_t startPtr, size_t nlen );
public:
	virtual ~HtmlDoc( );

};

#endif // HTMLDOC_H_H_HEAD__FILE__
