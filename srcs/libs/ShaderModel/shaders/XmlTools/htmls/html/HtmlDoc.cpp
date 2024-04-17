#include "HtmlDoc.h"

#include <QDebug>
#include <clocale>
#include <qdir.h>
#include "../../wstr/WStrTools.h"
#include "../../refWStr/RefWStr.h"
#include "../../macro/cmake_to_c_cpp_header_env.h"
#include <path/Path.h>

void print_console_msg( const RefWStr &msg ) {
	std::wstring outMsg;
	msg.converStdWstring( &outMsg );
	qDebug( ) << "void print_console_msg( const RefWStr &" << &msg << " )""\n""======""\n" << QString::fromStdWString( outMsg ).toLocal8Bit( ).toStdString( ).c_str( ) << "\n" "=======";
}
QString conver_console_QString_msg( const RefWStr &msg ) {
	std::wstring outMsg;
	msg.converStdWstring( &outMsg );
	return QString::fromStdWString( outMsg );
}

HtmlDoc::HtmlDoc( ): html( nullptr ), htmlSize( 0 ) {
	normalNode = new SubNode;
}
int32_t HtmlDoc::init( ) {
	// 写入文件
	QString writeFilePath( u8"%1%2%3%2%4" );
	QString cachePtah = writeFilePath.arg( Project_Run_bin ).arg( QDir::separator( ) ).arg( u8"write_test_cache" );
	writeFilePath = cachePtah.arg( u8"www.121ds.cc.txt" );
	bool isExis = Path::creatFilePath( writeFilePath );
	QFile file( writeFilePath );
	bool open = file.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate );
	// 缓冲字符串引用
	RefWStr buffStartRefWStr, buffEndRefWStr;
	// 缓冲字符串
	std::wstring buffStdWString;
	// 缓冲输出
	QString buffQString;
	// 节点数量
	size_t nodeCount = 0;
	// 用于查找偏移
	size_t resultWCharPotion;
	// 当前字符
	wchar_t currentChar = '\0';
	// < 开始
	size_t startIndex,
		// /> 结束
		endIndex;
	constexpr wchar_t singleQuotation = L'\'';
	constexpr wchar_t doubleQuotation = L'\"';
	constexpr wchar_t nodeStartChar = L'<';
	constexpr wchar_t nodeEndChar = L'>';
	constexpr wchar_t forwardSlash = L'/';
	constexpr wchar_t backSlash = L'\\';
	for( size_t index = 0 ; index < this->htmlSize ; ++index ) {

		if( !WStrTools::findNextWCharPotion( this->html, index, nodeStartChar, this->htmlSize, &resultWCharPotion ) ) // 异常
			return -1; // 找不到 < 的存在
		startIndex = resultWCharPotion + index;
		auto newIndex = startIndex + 1; // 放弃 < 的位置，并且检测下一个位置
		// 跳过空格
		for( ; newIndex < this->htmlSize ; ++newIndex ) {
			currentChar = this->html[ newIndex ];
			if( !WStrTools::isJumpSpace( currentChar ) )
				break;
		}
		if( currentChar == L'!' ) {
			if( !WStrTools::findNextWCharPotion( this->html, newIndex, nodeEndChar, this->htmlSize, &resultWCharPotion ) ) // 异常
				return -2; // <! 错误
			index = newIndex + resultWCharPotion;
			continue;
		}
		do { // todo : 获取一个当前正常的节点
			currentChar = this->html[ index ];
			if( currentChar == singleQuotation ) { // 单引号字符串
				if( !WStrTools::findNextWCharPotion( this->html, newIndex, singleQuotation, this->htmlSize, &resultWCharPotion ) ) // 异常
					return -3; // 没有匹配的 '
				index = newIndex + resultWCharPotion;
			} else if( currentChar == doubleQuotation ) {
				if( !WStrTools::findNextWCharPotion( this->html, newIndex, doubleQuotation, this->htmlSize, &resultWCharPotion ) ) // 异常
					return -4; // 没有匹配的"
				index = newIndex + resultWCharPotion;
			} else if( currentChar == nodeStartChar ) { // 找到开始字符
				startIndex = index + 1;
				// 跳过空格
				for( ; startIndex < this->htmlSize ; ++startIndex ) {
					currentChar = this->html[ startIndex ];
					if( !WStrTools::isJumpSpace( currentChar ) )
						break;
				}
				buffStartRefWStr.setPtr( this->html + startIndex );
				endIndex = startIndex;
				// 查找空格、 /、或者 >
				for( ; endIndex < this->htmlSize ; ++endIndex ) {
					currentChar = this->html[ endIndex ];
					if( WStrTools::isJumpSpace( currentChar ) || currentChar == nodeEndChar || currentChar == forwardSlash )
						break;
				}
				size_t len = endIndex - startIndex;
				buffStartRefWStr.setLen( len );
				buffStartRefWStr.converStdWstring( &buffStdWString );
				buffQString = QString::fromStdWString( buffStdWString );
				qDebug( ) << "开始单节点名称 : " << buffQString;
				// 找到 >
				for( ; endIndex < this->htmlSize ; ++endIndex ) {
					currentChar = this->html[ endIndex ];
					if( currentChar == nodeEndChar ) {
						// 判断节点类型
						for( auto subIndex = endIndex ; subIndex > startIndex ; --subIndex ) {
							currentChar = this->html[ subIndex ];
							if( currentChar == forwardSlash ) {
								// 识别为 /
								// 单节点
								/* 获取整个节点内容 */
								auto subNodeEndCharIndex = subIndex + 1;
								for( ; subNodeEndCharIndex < htmlSize ; ++subNodeEndCharIndex ) {
									currentChar = this->html[ subNodeEndCharIndex ];
									if( currentChar == nodeEndChar )
										break;
								}
								auto ref = RefWStr( html + index, subNodeEndCharIndex - index + 1 );
								ref.converStdWstring( &buffStdWString );
								buffQString = QString::fromStdWString( buffStdWString );
								qDebug( ) << "=============" "\n" << "获得节点:""\n" << buffQString.toLocal8Bit( ).toStdString( ).c_str( ) << "\n" "=============" "\n";
							} else if( !WStrTools::isJumpSpace( currentChar ) ) {
								// 识别为文字
								// 双节点
								auto subNodeEndCharIndex = subIndex + 1;
								for( ; subNodeEndCharIndex < htmlSize ; ++subNodeEndCharIndex ) { // 找到双节点的结束
									currentChar = this->html[ subNodeEndCharIndex ];

									if( currentChar == singleQuotation ) {
										// 跳过单引号
										for( ; subNodeEndCharIndex < htmlSize ; ++subNodeEndCharIndex ) {
											currentChar = this->html[ subNodeEndCharIndex ];
											if( currentChar == singleQuotation )
												break;
										}
									} else if( currentChar == doubleQuotation ) {
										// 跳过双引号
										for( ; subNodeEndCharIndex < htmlSize ; ++subNodeEndCharIndex ) {
											currentChar = this->html[ subNodeEndCharIndex ];
											if( currentChar == doubleQuotation )
												break;
										}
									} else if( currentChar == nodeStartChar ) {// 找到了节点结束的 <
										++subNodeEndCharIndex;
										// 跳过空格
										for( ; subNodeEndCharIndex < htmlSize ; ++subNodeEndCharIndex ) {
											currentChar = this->html[ subNodeEndCharIndex ];
											if( !WStrTools::isJumpSpace( currentChar ) )
												break;
										}
										if( currentChar != forwardSlash ) {
											subIndex = subNodeEndCharIndex;
											continue;
										}
										subNodeEndCharIndex += 1;
										startIndex = subNodeEndCharIndex;
										// 去掉空格
										for( ; subNodeEndCharIndex < htmlSize ; ++subNodeEndCharIndex ) {
											currentChar = this->html[ subNodeEndCharIndex ];
											if( WStrTools::isJumpSpace( currentChar ) || currentChar == nodeEndChar )
												break;
										}

										auto nodeName = RefWStr( this->html + startIndex, subNodeEndCharIndex - startIndex );
										if( startIndex > 57500 ) {
											print_console_msg( nodeName );
										}
										if( nodeName.equeWStr( buffStartRefWStr.getPtr( ), buffStartRefWStr.getLen( ) ) ) {
											QString consoleQStringMsg = conver_console_QString_msg( buffStartRefWStr );
											std::string stdString = consoleQStringMsg.toLocal8Bit( ).toStdString( );
											qDebug( ) << "===> @找到 : " << stdString.c_str( );
											break;
										}
										QString consoleQStringMsg = conver_console_QString_msg( nodeName );
										std::string stdString = consoleQStringMsg.toLocal8Bit( ).toStdString( );
										qDebug( ) << "---> !跳过 : " << stdString.c_str( );
										continue;
									}
								}
								endIndex = subNodeEndCharIndex;
							}
						}
						break;
					}
				}
				index = endIndex;
			}
			index++;
		} while( index < this->htmlSize ); // 递交给上层 for 继续

	}

	file.close( );
	return 0;
}
HtmlDoc *HtmlDoc::newObj( ) {
	// todo : 使用 new 的方式创建的一个独立的内存。不再发生引用
	return nullptr;
}
HtmlDoc::~HtmlDoc( ) {
	if( this->html )
		delete[] this->html;
	delete normalNode;
}
std::shared_ptr< HtmlDoc > HtmlDoc::parse( wchar_t *c_str, const size_t c_str_len ) {
	auto result = new HtmlDoc( );
	result->html = c_str;
	result->htmlSize = c_str_len;
	if( !result->init( ) ) {
		result->html = new wchar_t [ c_str_len ];
		for( size_t index = 0 ; index < c_str_len ; ++index )
			result->html[ index ] = c_str[ index ];
		return std::shared_ptr< HtmlDoc >( result );
	}
	result->html = nullptr;
	return nullptr;
}
