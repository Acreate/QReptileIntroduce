#include "HtmlDoc.h"

#include <QDebug>
#include <clocale>
#include <qdir.h>
#include "../../wstr/WStrTools.h"
#include "../../refWStr/RefWStr.h"
#include "../../macro/cmake_to_c_cpp_header_env.h"
#include <path/Path.h>

HtmlDoc::HtmlDoc( ): html( nullptr ), htmlSize( 0 ) {
	normalNode = new SubNode;
}
int32_t HtmlDoc::init( ) {
	// 写入文件
	QString writeFilePath( u8"%1%2%3%2%4" );
	writeFilePath = writeFilePath.arg( Project_Run_bin ).arg( QDir::separator( ) ).arg( u8"write_test_cache" ).arg( u8"www.121ds.cc.txt" );
	bool isExis = Path::creatFilePath( writeFilePath );
	QFile file( writeFilePath );
	bool open = file.open( QIODeviceBase::Text | QIODeviceBase::WriteOnly | QIODeviceBase::Truncate );
	// 缓冲字符串引用
	RefWStr buffRefWStr;
	// 缓冲字符串
	std::wstring buffStdWString;
	// 缓冲输出
	QString buffQString;
	size_t nodeCount = 0;

	for( size_t index = 0 ; index < this->htmlSize ; ++index ) {
		wchar_t currentChar = this->html[ index ];
		if( WStrTools::isJumpSpace( currentChar ) ) {
			//qDebug( ) << u8"遭遇空格 [" << index << "]";
		} else if( currentChar == L'<' ) {
			currentChar = this->html[ index + 1 ];
			if( currentChar == L'!' ) {// 不可解析内容
				size_t nextWCharPotion = WStrTools::findNextWCharPotion( this->html, index + 1, L'!', this->htmlSize );
				index += nextWCharPotion;
				continue;
			}
			auto spaceJumpIndex = index + 1;
			// 过滤空格
			for( ; spaceJumpIndex < this->htmlSize ; ++spaceJumpIndex )
				if( !WStrTools::isJumpSpace( this->html[ spaceJumpIndex ] ) )
					break;
			auto nodeStartPtr = this->html + index;
			auto mixLen = this->htmlSize - index;

			for( auto orgIndex = 0 ; orgIndex < mixLen ; ++orgIndex ) { // todo : 找到 / 之后的 >
				currentChar = nodeStartPtr[ orgIndex ];
				if( currentChar == L'\'' ) {// todo : 遭遇单引号字符串
					orgIndex = orgIndex + 1;
					orgIndex = WStrTools::findNextWCharPotion( nodeStartPtr, orgIndex, L'\'', mixLen ) + orgIndex;
				} else if( currentChar == L'\"' ) { // todo : 遭遇算引号字符串
					orgIndex = orgIndex + 1;
					orgIndex = WStrTools::findNextWCharPotion( nodeStartPtr, orgIndex, L'\"', mixLen ) + orgIndex;
				} else if( currentChar == L'/' ) { // todo : 找到了 /
					// todo : 查找 / 前的名称
					auto orgSubIndex = 0;
					for( ; orgSubIndex < orgIndex ; ++orgSubIndex ) {
						if( !WStrTools::isJumpSpace( nodeStartPtr[ orgSubIndex ] ) )
							break;
					}
					RefWStr refWStr( nodeStartPtr, orgSubIndex - 1 );
					buffRefWStr.setPtr( nodeStartPtr );
					buffRefWStr.setLen( orgSubIndex - 1 );
					refWStr.converStdWstring( &buffStdWString );
					qDebug( ) << "找到名称 : " << QString::fromStdWString( buffStdWString );
					orgSubIndex = orgIndex + 1;
					for( ; orgSubIndex < mixLen ; ++orgSubIndex ) {
						currentChar = nodeStartPtr[ orgSubIndex ];
						if( currentChar == L'>' ) { // todo : 找到了 >
							index = index + orgSubIndex;
							
							buffRefWStr.setPtr( nodeStartPtr );
							buffRefWStr.setLen( orgSubIndex - 1 );
							refWStr.converStdWstring( &buffStdWString );
							buffQString.clear( );
							buffQString.append( u8"\t(\t" );
							buffQString.append( QString::number( ++nodeCount ) );
							buffQString.append( u8"\t)\n" );
							buffQString.append( u8"------------>\n" );
							buffQString.append( QString::fromStdWString( buffStdWString ) );
							buffQString.append( u8"\n<==========" );

							QByteArray local8Bit = buffQString.toLocal8Bit( );
							qDebug( ) << local8Bit;
							buffQString.append( u8"\n" );
							local8Bit = buffQString.toLocal8Bit( );
							if( open )
								file.write( local8Bit );
							break;
						}
					}
					break;
				}
			}
		}
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
