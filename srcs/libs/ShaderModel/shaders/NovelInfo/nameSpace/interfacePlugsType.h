﻿#ifndef INTERFACETYPE_H_H_HEAD__FILE__
#define INTERFACETYPE_H_H_HEAD__FILE__
#pragma once

#include "../export/NovelInfo_export.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
namespace interfacePlugsType {
	class NOVELINFO_EXPORT INovelInfo;
	class NOVELINFO_EXPORT IRequestNetInterface;

	using INovelInfoPtr = INovelInfo *;
	using INovelInfo_Shared = std::shared_ptr< INovelInfo >;
	using Vector_NovelSPtr = std::vector< INovelInfo_Shared >;
	using Vector_NovelSPtr_Shared = std::shared_ptr< Vector_NovelSPtr >;
	using HtmlDocString = std::wstring;
	using StdStringPtr = HtmlDocString *;
	using Map_HtmlStrK_HtmlStrV = std::unordered_map< HtmlDocString, HtmlDocString >;
};


#endif // INTERFACETYPE_H_H_HEAD__FILE__
