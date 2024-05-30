#ifndef NOVELNODEXPATHINFO_H_H_HEAD__FILE__
#define NOVELNODEXPATHINFO_H_H_HEAD__FILE__
#pragma once

namespace instance_function {
	// todo : 小说相关而的 xpath
	struct NovelNodeXPathInfo {
		/// <summary>
		/// 从类型页面 url 小说信息列表遍历中获取小说名称的 xpath
		/// </summary>
		cylHtmlTools::HtmlString formTypePageGetNovelNameXpath;
		/// <summary>
		/// 从类型页面 url 小说信息列表遍历中获取小说更新时间的 xpath
		/// </summary>
		cylHtmlTools::HtmlString formTypePageGetNovelUpdateTimeXpath;
		/// <summary>
		/// 从类型页面 url 小说信息列表遍历中获取小说作者信息的 xpath
		/// </summary>
		cylHtmlTools::HtmlString formTypePageGetNovelAuthorXpath;
		/// <summary>
		/// 从类型页面 url 小说信息列表遍历中获取小说最后更新项的 xpath
		/// </summary>
		cylHtmlTools::HtmlString formTypePageGetNovelLastUpdateItemXpath;
		/// <summary>
		/// 调整更新时间调用
		/// </summary>
		std::function< QString( cylHtmlTools::HtmlString_Shared &html_string_shared ) > normal_update_time_function;
		/// <summary>
		/// 校验页面是否第一页
		/// </summary>
		std::function< bool( cylHtmlTools::HtmlNode *html_node ) > check_html_is_first_type_url;
	};
}


#endif // NOVELNODEXPATHINFO_H_H_HEAD__FILE__
