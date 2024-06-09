#ifndef NOVELNODEXPATHINFO_H_H_HEAD__FILE__
#define NOVELNODEXPATHINFO_H_H_HEAD__FILE__
#pragma once
#include <htmls/htmlDoc/HtmlDoc.h>
#include "htmls/htmlDoc/HtmlDoc.h"
#include "htmls/htmlNode/HtmlNode.h"
#include "htmls/htmlTools/XPath/XPath.h"
#include <QString>
#include <QObject>
#include <interface/instance_function.h>
namespace instance_function {
	// todo : 小说相关而的 xpath
	struct NovelNodeXPathInfo {
		/// <summary>
		/// 从小说页面获取小说
		/// </summary>
		static cylHtmlTools::HtmlString novels_type_get_novels_node_xpath;
		/// <summary>
		/// 从首页获取类型
		/// </summary>
		static cylHtmlTools::HtmlString novels_root_get_type_xpath;
		/// <summary>
		/// 从小说页面当中获取下一页
		/// </summary>
		static cylHtmlTools::HtmlString novels_type_get_type_next_xpath;
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
		/// 从类型页面 url 小说信息列表遍历中获取小说详细说明项的 xpath
		/// </summary>
		cylHtmlTools::HtmlString formTypePageGetNovelInfoXpath;
		/// <summary>
		/// 调整更新时间调用
		/// </summary>
		std::function< QString( cylHtmlTools::HtmlString_Shared &html_string_shared ) > normal_update_time_function;
	};


}

#endif // NOVELNODEXPATHINFO_H_H_HEAD__FILE__
