#ifndef HTMLDOC_H_H_HEAD__FILE__
#define HTMLDOC_H_H_HEAD__FILE__
#pragma once
#include <functional>
#include <memory>
#include <string>

#include "../export/XmlTools_export.h"

class HtmlDoc;
class XMLTOOLS_EXPORT HtmlDoc {
public:
	class XMLTOOLS_EXPORT RefWStr {
		const wchar_t *ptr;
		size_t len;
	public:
		using CheckFunction = std::function< int64_t( const wchar_t &, size_t &, size_t & ) >;
	public:
		RefWStr( const wchar_t *ptr = nullptr, const size_t len = 0 );
		virtual ~RefWStr( );
	public:   /// get / set
		const wchar_t *getPtr( ) const {
			return ptr;
		}
		void setPtr( const wchar_t *const ptr ) {
			this->ptr = ptr;
		}
		size_t getLen( ) const {
			return len;
		}
		void setLen( const size_t len ) {
			this->len = len;
		}
		/// <summary>
		/// 转换到 std::wstring 类型 <br/>
		/// </summary>
		/// <param name="outStr">输出的字符串</param>
		/// <returns>字符串个数</returns>
		size_t converStdWstring( std::wstring *outStr );
	public: // 属性 / 行为
		/// <summary>
		/// 该对象是否正确被引用 <br/>
		/// 返回 true 并不表示被正确使用，但它确实已经被使用
		/// </summary>
		/// <returns>true 表示已经被引用</returns>
		bool isRef( ) const {
			return ptr && len;
		}
		/// <summary>
		/// 根据长度匹配字符串
		/// </summary>
		/// <param name="c_w_str">字符串起始地址</param>
		/// <param name="c_w_str_len">匹配长度</param>
		/// <returns>相匹配返回 true</returns>
		bool equeWStr( const wchar_t *c_w_str, const size_t c_w_str_len );
		/// <summary>
		/// 使用本对象的长度匹配字符串
		/// </summary>
		/// <param name="c_w_str">匹配的字符串</param>
		/// <returns>相匹配返回 true</returns>
		bool equeWStr( const wchar_t *c_w_str );

		/// <summary>
		/// 比较字符串
		/// </summary>
		/// <param name="c_w_str">比较的字符串</param>
		/// <param name="c_w_str_len">比较长度</param>
		/// <returns>第一个不相等的字符串相差值</returns>
		wchar_t comp( const wchar_t *c_w_str, const size_t c_w_str_len );
	public:
		/// <summary>
		/// 根据指定的结束字符从已有字符串中获取引用 <br/>
		/// 返回的 RefWStr 对象需要使用 RefWStr::isRef() 验证
		/// </summary>
		/// <param name="c_w_str_ptr">源字符串</param>
		/// <param name="endChar">结束字符</param>
		/// <returns>返回一个对象</returns>
		static RefWStr generateRefWStr( const wchar_t *c_w_str_ptr, const wchar_t endChar );
		/// <summary>
		/// 根据指定的结束字符从已有字符串中获取引用 <br/>
		/// 返回的 RefWStr 对象需要使用 RefWStr::isRef() 验证
		/// </summary>
		/// <param name="c_w_str_ptr">源字符串</param>
		/// <param name="c_w_str_end">结束字符串</param>
		/// <returns>返回一个对象</returns>
		static RefWStr generateRefWStr( const wchar_t *c_w_str_ptr, const wchar_t *c_w_str_end );
		/// <summary>
		/// 根据指定的结束字符从已有字符串中获取引用 <br/>
		/// 返回的 RefWStr 对象需要使用 RefWStr::isRef() 验证
		/// </summary>
		/// <param name="c_w_str_ptr">源字符串</param>
		/// <param name="c_w_str_end">结束字符串</param>
		/// <param name="c_w_str_end_len">结束字符串的长度</param>
		/// <returns>返回一个对象</returns>
		static RefWStr generateRefWStr( const wchar_t *c_w_str_ptr, const wchar_t *c_w_str_end, const size_t c_w_str_end_len );
		/// <summary>
		/// 根据指定的结束字符从已有字符串中获取引用 <br/>
		/// 返回的 RefWStr 对象需要使用 RefWStr::isRef() 验证
		/// </summary>
		/// <param name="c_w_str_ptr">源字符串</param>
		/// <param name="c_w_str_ptr_len">源字符串的长度</param>
		/// <param name="c_w_str_end">结束字符串</param>
		/// <param name="c_w_str_end_len">结束字符串的长度</param>
		/// <returns>返回一个对象</returns>
		static RefWStr generateRefWStr( const wchar_t *c_w_str_ptr, const size_t c_w_str_ptr_len, const wchar_t *c_w_str_end, const size_t c_w_str_end_len );
		/// <summary>
		/// 根据指定的结束字符从已有字符串中获取引用 <br/>
		/// 返回的 RefWStr 对象需要使用 RefWStr::isRef() 验证
		/// </summary>
		/// <param name="src">源字符串</param>
		/// <param name="des">结束字符串</param>
		/// <returns>返回一个对象</returns>
		static RefWStr generateRefWStr( const RefWStr src, const RefWStr des );
		/// <summary>
		/// 根据校验函数返回实现判断返回 <br/>
		/// 校验函数返回非0表示找到 <br/>
		///		大于 0 会与 第三参数相加 <br/>
		///		小于 0 会与 第三参数相减
		/// </summary>
		/// <param name="c_w_str_ptr">源字符串</param>
		/// <param name="callFunction">校验函数，返回非0表示终结</param>
		/// <returns>引用的对象</returns>
		static RefWStr generateRefWStr( const wchar_t *c_w_str_ptr, const CheckFunction &callFunction );
		/// <summary>
		/// 根据校验函数返回实现判断返回 <br/>
		/// 校验函数返回非0表示找到 <br/>
		///		大于 0 会与 第三参数相加 <br/>
		///		小于 0 会与 第三参数相减
		/// </summary>
		/// <param name="src">源字符串</param>
		/// <param name="callFunction">校验函数，返回非0表示终结</param>
		/// <returns>引用的对象</returns>
		static RefWStr generateRefWStr( const RefWStr src, const CheckFunction &callFunction );
	};
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
	/// 是否为一个跳过的空字符 <br/>
	/// 其中包含控制字符等
	/// </summary>
	/// <param name="currentChar">检测的字符</param>
	/// <returns>true 表示空字符</returns>
	bool isJumpSpace( wchar_t currentChar );
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
