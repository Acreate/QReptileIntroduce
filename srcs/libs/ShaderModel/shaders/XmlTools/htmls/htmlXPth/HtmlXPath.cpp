#include "HtmlXPath.h"

#include <qDebug>
#include <qdir.h>

#include "../htmlDoc/HtmlDoc.h"
#include "../htmlNode/HtmlNode.h"
#include <string>

#include "../../../../../../projects/ReptileIntroduce/src/qt/extend/thread/FileThread.h"
#include "../../macro/cmake_to_c_cpp_header_env.h"
#include "path/Path.h"
using namespace XmlTools;
HtmlXPath_Shared HtmlXPath::converXPtah( HtmlDoc_Shared html_doc ) {
	HtmlXPath_Shared root( new HtmlXPath( html_doc ) );

	//// 遍历容器
	//auto foreachVector = *( html_doc->htmlDocNode.get( ) );;
	//// 转换容器
	//Vector_HtmlXPathSPtr tmpVector;

	//size_t
	//	// 循环所需要的个数
	//	vectorSize = foreachVector.size( ),
	//	// 遍历下标
	//	currentCount = 0,
	//	// 二次遍历下标
	//	subCurrentCount = 0,
	//	// 节点位于字符串的左侧下标
	//	leftIndex = 0,
	//	// 节点位于字符串的右侧下标
	//	rightIndex = 0,
	//	// 临时保存次级的左下标
	//	subLeftIndex = 0,
	//	// 临时保存次级的右下标
	//	subRightIndex = 0,
	//	// 临时保存次级的左下标
	//	parentLeftIndex = 0,
	//	// 临时保存次级的右下标
	//	parentRightIndex = 0;
	//// 顶级循环使用 HtmlXPath_Shared
	//HtmlXPath_Shared htmlXPath,
	//	// 次级循环使用 HtmlXPath_Shared
	//	subHtmlXPath,
	//	// 保存父节点 HtmlXPath_Shared
	//	parentXPath;
	//// 顶级循环的开始节点使用 HtmlNode_Shared
	//HtmlNode_Shared htmlNode,
	//	// 顶级循环的结束节点使用 HtmlNode_Shared
	//	endHtmlNode,
	//	// 次级循环的开始节点使用 HtmlNode_Shared
	//	subHtmlNode,
	//	// 次级循环的结束节点使用 HtmlNode_Shared
	//	subEndHtmlNode;
	//// 生成所有匹配的对象
	//for( ; currentCount < vectorSize; ++currentCount ) {
	//	auto node = foreachVector.at( currentCount );
	//	htmlXPath = HtmlXPath_Shared( new HtmlXPath( html_doc, root, node ) );
	//	htmlXPath->thisSharedPtr = htmlXPath;
	//	tmpVector.emplace_back( htmlXPath );
	//	htmlXPath->setParent( root );
	//}

	//QStringList msgList;
	//QString
	//	// 顶级循环的左下标转换的字符串
	//	leftIndexQStr,
	//	// 顶级循环的右下标转换的字符串 
	//	rightIndexQStr,
	//	// 顶级循环的节点名称
	//	htmlNodeQStr,
	//	// 次级循环的左下标转换的字符串
	//	subLeftIndexQStr,
	//	// 次级循环的右下标转换的字符串 
	//	subRightIndexQStr,
	//	// 次级循环的节点名称
	//	subHtmlNodeQStr;
	//// 运算匹配
	//vectorSize = tmpVector.size( );
	//// 父子关系管理
	//for( currentCount = 0; currentCount < vectorSize; ++currentCount ) {
	//	htmlXPath = tmpVector[ currentCount ];
	//	htmlNode = htmlXPath->refNode;
	//	leftIndex = htmlNode->ptrOffset;
	//	htmlNodeQStr = QString::fromStdWString( *htmlNode->getWSNode( ).get( ) );
	//	qDebug( ) << u8"检测 :(" << htmlNodeQStr.toStdString( ).c_str( ) << ")<=\n";
	//	rightIndex = leftIndex + htmlNode->nodeSize( );

	//	leftIndexQStr = QString::number( leftIndex );
	//	rightIndexQStr = QString::number( rightIndex );
	//	qDebug( ) << u8"\t" << leftIndexQStr.toStdString( ).c_str( ) << "," << rightIndexQStr.toStdString( ).c_str( );
	//	for( subCurrentCount = 0; subCurrentCount < vectorSize; ++subCurrentCount ) {
	//		subHtmlXPath = tmpVector[ subCurrentCount ];
	//		subHtmlNode = subHtmlXPath->refNode;

	//		if( subHtmlNode->getType( ) == Html_Node_Type::DoubleNode && subHtmlNode.get( ) != htmlNode.get( ) ) {
	//			subLeftIndex = subHtmlNode->ptrOffset;
	//			subRightIndex = subLeftIndex + subHtmlNode->nodeSize( );

	//			subLeftIndexQStr = QString::number( subLeftIndex );
	//			subRightIndexQStr = QString::number( subRightIndex );
	//			subHtmlNodeQStr = QString::fromStdWString( *subHtmlNode->getWSNode( ).get( ) );
	//			qDebug( ) << u8"\t" u8"发现 :(" << subHtmlNodeQStr.toStdString( ).c_str( ) << ")<=";
	//			qDebug( ) << u8"\t" << subLeftIndexQStr.toStdString( ).c_str( ) << "," << subRightIndexQStr.toStdString( ).c_str( );
	//			if( leftIndex > subLeftIndex && rightIndex < subRightIndex ) {
	//				if( parentXPath ) {
	//					parentLeftIndex = parentXPath->refNode->ptrOffset;
	//					if( parentLeftIndex > subLeftIndex )
	//						continue;
	//					parentRightIndex = parentLeftIndex + parentXPath->refNode->nodeSize( );
	//					if( parentRightIndex < subRightIndex )
	//						continue;
	//				}
	//				parentXPath = subHtmlXPath;
	//			}

	//		} else {
	//			subHtmlNodeQStr = QString::fromStdWString( *subHtmlNode->getWSNode( ).get( ) );
	//			qDebug( ) << u8"\t" u8"跳过 :(" << subHtmlNodeQStr.toStdString( ).c_str( ) << ")<=";
	//		}
	//	}
	//	if( parentXPath.get( ) != nullptr ) {
	//		htmlXPath->setParent( parentXPath );
	//		subHtmlNode = parentXPath->refNode;
	//		subLeftIndex = subHtmlNode->ptrOffset;
	//		subRightIndex = subLeftIndex + subHtmlNode->nodeSize( );
	//		subLeftIndexQStr = QString::number( subLeftIndex );
	//		subRightIndexQStr = QString::number( subRightIndex );
	//		subHtmlNodeQStr = QString::fromStdWString( *subHtmlNode->getWSNode( ).get( ) );

	//		leftIndex = htmlNode->ptrOffset;
	//		rightIndex = leftIndex + htmlNode->nodeSize( );
	//		leftIndexQStr = QString::number( leftIndex );
	//		rightIndexQStr = QString::number( rightIndex );
	//		htmlNodeQStr = QString::fromStdWString( *htmlNode->getWSNode( ).get( ) );
	//		QString msg;
	//		msg.append( u8"!!找到 :" u8"\n" u8"\t""\t" )
	//			.append( htmlNodeQStr )
	//			.append( u8" 的父节点 " )
	//			.append( subHtmlNodeQStr )
	//			.append( u8"\n" u8"\t""\t""(" )
	//			.append( subLeftIndexQStr )
	//			.append( ", " )
	//			.append( subRightIndexQStr )
	//			.append( u8")->( " )
	//			.append( leftIndexQStr )
	//			.append( ", " )
	//			.append( rightIndexQStr )
	//			.append( ")" "\n" );
	//		msgList << msg;
	//		qDebug( ) << msg.toStdString( ).c_str( );
	//		parentXPath = nullptr;
	//	}
	//}
	//vectorSize = root->subChildren->size( );
	//QString writeFile( u8"%1%2%3%2%4" );
	//writeFile = writeFile.arg( Project_Run_bin ).arg( QDir::separator( ) ).arg( u8"write_cache" ).arg( u8"xpath.txt" );
	//QFile file( writeFile );
	//if( Path::creatFilePath( writeFile ) )
	//	if( file.open( QIODeviceBase::ExistingOnly | QIODeviceBase::Truncate | QIODeviceBase::Text | QIODeviceBase::WriteOnly ) ) {
	//		file.write( msgList.join( "" ).toLocal8Bit( ) );
	//		file.close( );
	//	}
	return root;
}
HtmlXPath::HtmlXPath( )
	: htmlDoc( new HtmlDoc )
	, root( this )
	/*, parent( nullptr )
	, brother( new Vector_HtmlXPathSPtr )
	, subChildren( new Vector_HtmlXPathSPtr )*/
	, refNode( nullptr )
	, nodeName( std::make_shared< std::wstring >( ) ) {

}
HtmlXPath::HtmlXPath( HtmlDoc_Shared clone_objPtr, HtmlXPath_Shared parent, HtmlNode_Shared html_node_shared )
	: htmlDoc( clone_objPtr )
	, root( parent->root )
	//, parent( parent )
	//, brother( new Vector_HtmlXPathSPtr )
	//, subChildren( new Vector_HtmlXPathSPtr )
	, refNode( html_node_shared )
	, nodeName( std::make_shared< std::wstring >( ) ) {
	*nodeName = *html_node_shared->getNodeWSName( ); // 设置名称
	//refNodeAttributes = html_node_shared->analysisAttribute( );
}
HtmlXPath::HtmlXPath( HtmlDoc_Shared clone_objPtr )
	: htmlDoc( clone_objPtr )
	, root( this )
	//, parent( nullptr )
	//, brother( new Vector_HtmlXPathSPtr )
	//, subChildren( new Vector_HtmlXPathSPtr )
	, refNode( nullptr )
	, nodeName( std::make_shared< std::wstring >( ) ) {
}
HtmlXPath::~HtmlXPath( ) {
}
//HtmlXPath_Shared HtmlXPath::findNodeFromName( const std::wstring &name ) {
//	size_t size = subChildren->size( );
//	if( size > 0 )
//		for( size_t index = 0; index < size; ++index )
//			if( *subChildren->at( index )->refNode->getNodeWSName( ) == name )
//				return subChildren->at( index );
//	return nullptr;
//}
std::shared_ptr< std::wstring > HtmlXPath::getNode( ) {
	return refNode->getWSNode( );
}
