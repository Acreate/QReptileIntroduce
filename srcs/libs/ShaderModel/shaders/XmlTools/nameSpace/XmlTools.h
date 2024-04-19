#ifndef XMLTOOLS_H_H_HEAD__FILE__
#define XMLTOOLS_H_H_HEAD__FILE__
#pragma once
#include <memory>
#include <vector>

#include "../export/XmlTools_export.h"
namespace XmlTools {
	class XMLTOOLS_EXPORT RefWStr;
	class XMLTOOLS_EXPORT WStrTools;
	class XMLTOOLS_EXPORT HtmlDoc;
	class XMLTOOLS_EXPORT WStrTools;
	class XMLTOOLS_EXPORT HtmlNode;
	enum class XMLTOOLS_EXPORT Html_Node_Type;
	using HtmlNode_Shared = std::shared_ptr< HtmlNode >;
	using Vector_HtmlNodeSPtr = std::vector< HtmlNode_Shared >;
	using Vector_HtmlNodeSPtr_Shared = std::shared_ptr< Vector_HtmlNodeSPtr >;
}

#endif // XMLTOOLS_H_H_HEAD__FILE__
