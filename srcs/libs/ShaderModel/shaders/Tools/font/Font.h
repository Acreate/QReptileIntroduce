#ifndef FONT_H_H_HEAD__FILE__
#define FONT_H_H_HEAD__FILE__
#pragma once
#include <qfontmetrics.h>
#include "../auto_generate_files/export/Tools_export.h"
class TOOLS_EXPORT Font {
public:
	/// <summary>
	/// 获取填充整行的建议长度
	/// </summary>
	/// <param name="str">参考字符串</param>
	/// <param name="font_metrics">字体材质</param>
	/// <param name="width">参考宽度</param>
	/// <returns>最小与最大的建议值</returns>
	static std::pair< size_t, size_t > getFullLineIndex( const QString &str, const QFontMetrics &font_metrics, const int width );

};


#endif // FONT_H_H_HEAD__FILE__
