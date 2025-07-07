#ifndef NOVELTIME_H_H_HEAD__FILE__
#define NOVELTIME_H_H_HEAD__FILE__
#pragma once

#include <QObject>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "../auto_generate_files/export/Plug_export.h"
class PLUG_EXPORT NovelTime : public QObject {
private:
	using Ratio = std::ratio< 1, 10000000 >;
	using Duration = std::chrono::duration< long long, Ratio >;
	using Clock = std::chrono::system_clock;
	using TimePoint = std::chrono::time_point< Clock >;
	TimePoint current_time;
public:
	NovelTime( ) : NovelTime( Clock::now( ) ) {
	}
	explicit NovelTime( const TimePoint &current_time )
		: current_time( current_time ) { }

	template< class Conver_Duration_Type >
	Conver_Duration_Type toDuration( ) const {
		return std::chrono::duration_cast< Conver_Duration_Type >( current_time.time_since_epoch( ) );
	}
	/// @brief 格式化输出-utc 时间 \n
	/// %Y - xxxx 年 \n
	/// %m - xx 月 \n
	/// %d - xx 日 \n
	/// %H - xx 小时 \n
	/// %M - xx 分钟 \n
	/// %S - xx 秒	 \n
	/// @see std::put_time
	/// @param formatting_string 格式化字符串
	/// @return 格式化后的字符串
	std::string toFormattingUtcString( const std::string &formatting_string ) {
		std::time_t tt = Clock::to_time_t( current_time );
		std::tm tm = *std::gmtime( &tt ); //GMT (UTC)
		std::stringstream ss;
		ss << std::put_time( &tm, formatting_string.c_str( ) );
		return ss.str( );
	}
	/// @brief 格式化输出-本地时间 \n
	/// %Y - xxxx 年 \n
	/// %m - xx 月 \n
	/// %d - xx 日 \n
	/// %H - xx 小时 \n
	/// %M - xx 分钟 \n
	/// %S - xx 秒	 \n
	/// @see std::put_time
	/// @param formatting_string 格式化字符串
	/// @return 格式化后的字符串
	std::string toFormattingLocalString( const std::string &formatting_string ) {
		std::time_t tt = Clock::to_time_t( current_time );
		std::tm tm = *std::localtime( &tt ); //Locale time-zone, usually UTC by default.
		std::stringstream ss;
		ss << std::put_time( &tm, formatting_string.c_str( ) );
		return ss.str( );
	}

	/// @brief 获取时间点
	/// @return 时间点
	TimePoint getCurrentTime( ) const { return current_time; }
	/// @brief 设置新的时间点
	/// @param current_time 新的时间点
	void setCurrentTime( const TimePoint &current_time ) { this->current_time = current_time; }
	/// @brief 更新当前时间
	void updateTimeDate( ) {
		current_time = Clock::now( );
	}
	NovelTime operator-( const NovelTime &right ) {
		Duration duration = this->current_time - right.current_time;
		auto ms = std::chrono::duration_cast< std::chrono::milliseconds >( duration );
		return NovelTime( TimePoint( ms ) );
	}
};


#endif // NOVELTIME_H_H_HEAD__FILE__
