#include "./function.h"

using time_point = std::chrono::system_clock::time_point;
#define Time_Fommatting( Formmation_String, Time_Object ) \
	std::cout << "e.g ( "<< Formmation_String <<" ) : "<<  Time_Object.toFormattingLocalString( Formmation_String ) << std::endl
int main( int argc, char *argv[ ] ) {

	std::locale locale( "zh_CN.UTF8" );
	std::locale::global( locale );
	std::cout.imbue( locale );
	std::wcout.imbue( locale );

	QCoreApplication application( argc, argv ); // 初始化程序
	applicationPid = qApp->applicationPid( );

	std::shared_ptr< cylStd::ArgParser > argParser = cylStd::ArgParser::parser( argc, argv ); // 命令行参数解析对象
	showStartRunTime( argParser );
	auto pairs = argParser->getPairs( );
	size_t pairsSize = pairs.size( );
	if( pairsSize == 1 ) {
		ErrorCout_MACRO( QString(u8"(进程 id :%1) 没有发生任何有效的行为，请检查选项是否正确").arg(applicationPid ) );
		showHelp( );
		return 0;
	}
	// 显示名称
	showAppName( argParser );
	// 显示版本信息
	showVersion( argParser );
	// 显示帮助信息
	showHelp( argParser );
	// 运行插件
	runRequestDownloadPlugs( argParser );
	// 检查关键字文件
	checkKeyFile( argParser );
	// 数据库操作
	dbReadWriteChanger( argParser );

	showEndRunTime( argParser );
	showRunTime( argParser );
	return 0;
}
