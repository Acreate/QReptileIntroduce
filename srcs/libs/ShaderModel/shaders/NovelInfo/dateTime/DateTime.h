#ifndef DATETIME_H_H_HEAD__FILE__
#define DATETIME_H_H_HEAD__FILE__
#pragma once

#include <chrono>
#include <qdatetime.h>
#include <QString>

#include "../nameSpace/interfacePlugsType.h"
class NOVELINFO_EXPORT DateTime {
public:
	/// <summary>
	/// 从时间戳当中获取-秒
	/// </summary>
	/// <param name="milliseconds">时间戳</param>
	/// <returns>秒</returns>
	static int16_t getTimeToSecond( const std::chrono::milliseconds &milliseconds );
	/// <summary>
	/// 从时间戳当中获取-分钟
	/// </summary>
	/// <param name="milliseconds">时间戳</param>
	/// <returns>分钟</returns>
	static int16_t getTimeToMinute( const std::chrono::milliseconds &milliseconds );
	/// <summary>
	/// 从时间戳当中获取-小时
	/// </summary>
	/// <param name="milliseconds">时间戳</param>
	/// <returns>小时</returns>
	static int16_t getTimeToHour( const std::chrono::milliseconds &milliseconds );
	/// <summary>
	/// 从时间戳当中获取-日
	/// </summary>
	/// <param name="milliseconds">时间戳</param>
	/// <returns>日</returns>
	static int16_t getTimeToDay( const std::chrono::milliseconds &milliseconds );
	/// <summary>
	/// 比较一个时间，并且返回时间差
	/// </summary>
	/// <param name="left_date_time">被减数</param>
	/// <param name="right_date_time">减数</param>
	/// <returns>时间差-日</returns>
	static std::chrono::milliseconds compareDateTime( const QDateTime &left_date_time, const QDateTime &right_date_time );
	/// <summary>
	/// 比较一个时间，并且返回时间差
	/// </summary>
	/// <param name="left_date_time_qstring">被减数字符串格式的时间</param>
	/// <param name="left_date_time_qstring_format">被减数时间转换格式</param>
	/// <param name="right_date_time_qstring">减数字符串格式的时间</param>
	/// <param name="right_date_time_qstring_format">减数时间转换格式</param>
	/// <returns>时间差-日</returns>
	static std::chrono::milliseconds compareDateTime( const QString &left_date_time_qstring, const QString &left_date_time_qstring_format, const QString &right_date_time_qstring, const QString &right_date_time_qstring_format );


};


#endif // DATETIME_H_H_HEAD__FILE__
