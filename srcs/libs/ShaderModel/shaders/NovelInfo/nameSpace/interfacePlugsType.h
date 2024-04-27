#ifndef INTERFACETYPE_H_H_HEAD__FILE__
#define INTERFACETYPE_H_H_HEAD__FILE__
#pragma once

#include "../export/NovelInfo_export.h"

namespace interfacePlugsType {
	class NOVELINFO_EXPORT INovelInfo;
	class NOVELINFO_EXPORT IRequestNetInterface;
	class NOVELINFO_EXPORT IRequestNetInterfaceExtend;

	using INovelInfoSharedPtr = std::shared_ptr< INovelInfo >;
	using NovelPtrList = std::vector< INovelInfoSharedPtr >;
	using HtmlDocString = std::wstring;
	using StdStringPtr = HtmlDocString *;
	using un_ordered_map = std::unordered_map< HtmlDocString, HtmlDocString >;
};


#endif // INTERFACETYPE_H_H_HEAD__FILE__
