#ifndef DATETIME_H_H_HEAD__FILE__
#define DATETIME_H_H_HEAD__FILE__
#pragma once

#include <chrono>
#include <iomanip>
#include <qdatetime.h>

#include "../nameSpace/interfacePlugsType.h"

#include <sstream>

class NOVELINFO_EXPORT DateTime {
	using Ratio = std::ratio< 1, 10000000 >;
	using Duration = std::chrono::duration< long long, Ratio >;
	using Clock = std::chrono::system_clock;
	using TimePoint = std::chrono::time_point< Clock >;
	static TimePoint runStartInitTime;
public:
	/// @brief 获取开始运行的初始化时间
	/// @return 初始化时间
	static TimePoint getRunStartInitTime( ) {
		return runStartInitTime;
	}
	/// @brief 时间数据格式化输出字符串 - utc 格式
	/// %Y - xxxx 年 \n
	/// %m - xx 月 \n
	/// %d - xx 日 \n
	/// %H - xx 小时 \n
	/// %M - xx 分钟 \n
	/// %S - xx 秒	 \n
	/// @see std::put_time
	/// @param form_time_string 格式化字符串
	/// @param to_string_time 转换对象
	/// @return 转换后的字符串
	static std::string toStringOnUTC( const std::string &form_time_string, const TimePoint &to_string_time ) {
		std::time_t tt = Clock::to_time_t( to_string_time );
		std::tm tm = *std::gmtime( &tt ); //GMT (UTC)
		std::stringstream ss;
		ss << std::put_time( &tm, form_time_string.c_str( ) );
		return ss.str( );
	}

	/// @brief 时间数据格式化输出字符串 - 本地格式
	/// %Y - xxxx 年 \n
	/// %m - xx 月 \n
	/// %d - xx 日 \n
	/// %H - xx 小时 \n
	/// %M - xx 分钟 \n
	/// %S - xx 秒	 \n
	/// @see std::put_time
	/// @param form_time_string 格式化字符串
	/// @param to_string_time 转换对象
	/// @return 转换后的字符串
	static std::string toStringOnLocal( const std::string &form_time_string, const TimePoint &to_string_time ) {
		std::time_t tt = Clock::to_time_t( to_string_time );
		std::tm tm = *std::localtime( &tt ); //Locale time-zone, usually UTC by default.
		std::stringstream ss;
		ss << std::put_time( &tm, form_time_string.c_str( ) );
		return ss.str( );
	}
	/// @brief 获取毫秒
	/// @param conver_time_point 时间点
	/// @return 毫秒
	static std::chrono::milliseconds getTimeToMilliseconds( const TimePoint &conver_time_point ) {
		return std::chrono::duration_cast< std::chrono::milliseconds >( conver_time_point.time_since_epoch( ) );
	}

	/// <summary>
	/// 从时间戳当中获取-秒
	/// </summary>
	/// <param name="milliseconds">时间戳</param>
	/// <returns>秒</returns>
	static int16_t getMillisecondsTimeToSecond( const std::chrono::milliseconds &milliseconds ) {
		auto count = milliseconds.count( );
		int64_t result = count / 1000 % 60;
		return result;
	}
	/// <summary>
	/// 从时间戳当中获取-分钟
	/// </summary>
	/// <param name="milliseconds">时间戳</param>
	/// <returns>分钟</returns>
	static int16_t getMillisecondsTimeToMinute( const std::chrono::milliseconds &milliseconds ) {
		auto count = milliseconds.count( );
		int64_t result = count / 1000 / 60 % 60;
		return result;
	}
	/// <summary>
	/// 从时间戳当中获取-小时
	/// </summary>
	/// <param name="milliseconds">时间戳</param>
	/// <returns>小时</returns>
	static int16_t getMillisecondsTimeToHour( const std::chrono::milliseconds &milliseconds ) {
		auto count = milliseconds.count( );
		int64_t result = count / 1000 / 60 / 60 % 24;
		return result;
	}
	/// <summary>
	/// 从时间戳当中获取-日
	/// </summary>
	/// <param name="milliseconds">时间戳</param>
	/// <returns>日</returns>
	static int16_t getMillisecondsTimeToDay( const std::chrono::milliseconds &milliseconds ) {
		auto count = milliseconds.count( );
		int64_t result = count / 1000 / 60 / 60 / 24;
		return result;
	}
	/// <summary>
	/// 比较一个时间，并且返回时间差
	/// </summary>
	/// <param name="left_date_time">被减数</param>
	/// <param name="right_date_time">减数</param>
	/// <returns>时间差-日</returns>
	static std::chrono::milliseconds compareDateTime( const QDateTime &left_date_time, const QDateTime &right_date_time ) {

		std::chrono::milliseconds stdMilliseconds = left_date_time - right_date_time;
		return stdMilliseconds;
	}
	/// <summary>
	/// 比较一个时间，并且返回时间差
	/// </summary>
	/// <param name="left_date_time_qstring">被减数字符串格式的时间</param>
	/// <param name="left_date_time_qstring_format">被减数时间转换格式</param>
	/// <param name="right_date_time_qstring">减数字符串格式的时间</param>
	/// <param name="right_date_time_qstring_format">减数时间转换格式</param>
	/// <returns>时间差-日</returns>
	static std::chrono::milliseconds compareDateTime( const QString &left_date_time_qstring, const QString &left_date_time_qstring_format, const QString &right_date_time_qstring, const QString &right_date_time_qstring_format ) {
		QDateTime left_date_time = QDateTime::fromString( left_date_time_qstring, left_date_time_qstring_format );
		QDateTime right_date_time = QDateTime::fromString( right_date_time_qstring, right_date_time_qstring_format );
		return DateTime::compareDateTime( left_date_time, right_date_time );
	}
	/// <summary>
	/// 获取当前距离格林时间的秒数
	/// </summary>
	/// <returns>当前距离格林时间的秒数</returns>
	static size_t getCurrentSeconds( ) {
		auto timePoint = std::chrono::system_clock::now( );
		auto timeSinceEpoch = timePoint.time_since_epoch( );
		auto durationCast = std::chrono::duration_cast< std::chrono::seconds >( timeSinceEpoch );
		return durationCast.count( );
	}
	/// <summary>
	/// 获取当前距离格林时间的毫秒
	/// </summary>
	/// <returns>当前距离格林时间的毫秒</returns>
	static size_t getCurrentMilliseconds( ) {
		auto timePoint = std::chrono::system_clock::now( );
		auto timeSinceEpoch = timePoint.time_since_epoch( );
		auto durationCast = std::chrono::duration_cast< std::chrono::milliseconds >( timeSinceEpoch );
		return durationCast.count( );
	}
	/// <summary>
	/// 获取当前距离格林时间的微秒
	/// </summary>
	/// <returns>当前距离格林时间的微秒</returns>
	static size_t getCurrentMicroseconds( ) {
		auto timePoint = std::chrono::system_clock::now( );
		auto timeSinceEpoch = timePoint.time_since_epoch( );
		auto durationCast = std::chrono::duration_cast< std::chrono::microseconds >( timeSinceEpoch );
		return durationCast.count( );
	}
	/// <summary>
	/// 获取当前距离格林时间的纳秒
	/// </summary>
	/// <returns>当前距离格林时间的纳秒</returns>
	static size_t getCurrentNanoseconds( ) {
		auto timePoint = std::chrono::system_clock::now( );
		auto timeSinceEpoch = timePoint.time_since_epoch( );
		auto durationCast = std::chrono::duration_cast< std::chrono::nanoseconds >( timeSinceEpoch );
		return durationCast.count( );
	}
};


#endif // DATETIME_H_H_HEAD__FILE__
