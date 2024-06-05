#ifndef FUNCTION_H_H_HEAD__FILE__
#define FUNCTION_H_H_HEAD__FILE__
#pragma once
#include <QString>

#include "argParser/ArgParser.h"
/// <summary>
/// 获取编译信息
/// </summary>
/// <returns>编译信息</returns>
QString getBuilderInfo( );
/// <summary>
/// std::string 转 std::wstring
/// </summary>
/// <param name="str"></param>
/// <returns></returns>
std::wstring conver( const std::string &str );
/// <summary>
/// 从文件读取关键字
/// </summary>
/// <param name="paths">路径</param>
/// <returns>关键字列表</returns>
std::vector< QString > readIngoreNameFiles( std::vector< cylStd::ArgParser::String > &paths );
/// <summary>
/// 调整关键字，存在子字符串则不需要源字符串(a -> aa / 其中只保留 a，不保留 aa)
/// </summary>
/// <param name="str_vector">匹配的字符串</param>
/// <returns>完成匹配的字符串</returns>
std::vector< QString > vectorStrAdjustSubStr( std::vector< QString > &str_vector );
/// <summary>
/// 去掉重复字符串
/// </summary>
/// <param name="str_vector">去除字符串</param>
/// <returns>唯一的元素</returns>
std::vector< QString > vectorStrduplicate( std::vector< QString > &str_vector );
/// <summary>
/// 字符串列表排序-名称
/// </summary>
/// <param name="str_vector">排序列表</param>
/// <returns>排序结果</returns>
std::vector< QString > vectorStrsort( std::vector< QString > &str_vector );
/// <summary>
/// 数组转换到以长度为 key 的映射列表
/// </summary>
/// <param name="str_vector">转换数组</param>
/// <returns>长度 map</returns>
std::unordered_map< size_t, std::shared_ptr< std::vector< QString > > > vectorStrToLenKeyMap( std::vector< QString > &str_vector );
/// <summary>
/// 数组转换到以长度为 key 的映射列表
/// </summary>
/// <param name="str_vector">转换数组</param>
/// <returns>长度 map</returns>
std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > > vectorStrToLenKeyMap( std::vector< std::wstring > &str_vector );
/// <summary>
/// 字符串转换到宽字符
/// </summary>
/// <param name="str_vector">转换的字符串</param>
/// <returns>宽字符列表</returns>
std::vector< std::wstring > converToWString( std::vector< QString > &str_vector );
std::unordered_map< size_t, std::shared_ptr< std::vector< std::wstring > > > vectorStrToLenKeyMap( std::vector< std::wstring > &str_vector );

/// <summary>
/// 检测元素是否存在列表当中
/// </summary>
/// <typeparam name="t_value_type">列表元素类型</typeparam>
/// <param name="vector">检测列表</param>
/// <param name="value">检测值</param>
/// <returns>存在返回 true</returns>
template< class t_value_type >
inline bool findVector( const std::vector< t_value_type > &vector, const t_value_type &value ) {
	auto begin = vector.begin( );
	auto end = vector.end( );
	for( ; begin != end; ++begin )
		if( value == *begin )
			return true;
	return false;
}

#endif // FUNCTION_H_H_HEAD__FILE__
