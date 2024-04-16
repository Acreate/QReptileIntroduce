#ifndef HTMLDOC_H_H_HEAD__FILE__
#define HTMLDOC_H_H_HEAD__FILE__
#pragma once
#include <functional>
#include <memory>
#include <string>

#include "../export/XmlTools_export.h"

class XMLTOOLS_EXPORT HtmlDoc {
public:
	class XMLTOOLS_EXPORT RefWStr {
		const wchar_t *ptr;
		size_t len;
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
		/// 校验函数返回会与当前下标进行一个相加运算 <br/>
		/// </summary>
		/// <param name="c_w_str_ptr">源字符串</param>
		/// <param name="callFunction">校验函数，返回非0表示终结</param>
		/// <returns>引用的对象</returns>
		static RefWStr generateRefWStr( const wchar_t *c_w_str_ptr, const std::function< int64_t( const wchar_t &, size_t ) > &callFunction );
		/// <summary>
		/// 根据校验函数返回实现判断返回 <br/>
		/// 校验函数返回会与当前下标进行一个相加运算 <br/>
		/// </summary>
		/// <param name="src">源字符串</param>
		/// <param name="callFunction">校验函数，返回非0表示终结</param>
		/// <returns>引用的对象</returns>
		static RefWStr generateRefWStr( const RefWStr src, const std::function< int64_t( const wchar_t & ) > &callFunction );
	};
private: // 核心成员
	wchar_t *html;
	size_t htmlSize;
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
public:
	virtual ~HtmlDoc( );
public:
	static std::shared_ptr< HtmlDoc > parse( wchar_t *c_str, const size_t c_str_len );
};

#endif // HTMLDOC_H_H_HEAD__FILE__
