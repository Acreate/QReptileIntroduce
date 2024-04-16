#ifndef WSTRTOOLS_H_H_HEAD__FILE__
#define WSTRTOOLS_H_H_HEAD__FILE__
#pragma once

class WStrTools {
public:
	/// <summary>
	/// 是否为一个跳过的空字符 <br/>
	/// 其中包含控制字符等
	/// </summary>
	/// <param name="currentChar">检测的字符</param>
	/// <returns>true 表示空字符</returns>
	static bool isJumpSpace( wchar_t currentChar );
	/// <summary>
	/// 获取下个反斜杠的相对位置
	/// </summary>
	/// <param name="w_c_ptr">指向字符串检测的开始位置</param>
	/// <param name="startIndex">字符串开始检查的下标</param>
	/// <param name="find_w_c_char">查找的字符</param>
	/// <param name="maxIndex">最大的下标</param>
	/// <returns>相对于 startIndex 的大小</returns>
	static size_t findNextWCharPotion( const wchar_t *w_c_ptr, size_t startIndex, const wchar_t find_w_c_char, size_t maxIndex );
};

#endif // WSTRTOOLS_H_H_HEAD__FILE__
