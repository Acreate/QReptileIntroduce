#include "function.h"
QString getBuilderInfo( ) {

	QString compilerString = "<unknown>";
#if defined(Q_CC_CLANG)
    QString isAppleString;
#if defined(__apple_build_version__)
    isAppleString = QLatin1String(" (Apple)");
#endif
    compilerString = QLatin1String("Clang ") + QString::number(__clang_major__) + QLatin1Char('.') + QString::number(__clang_minor__) + isAppleString;
#elif defined(Q_CC_GNU)
    compilerString = QLatin1String("GCC ") + QLatin1String(__VERSION__);
#elif defined(Q_CC_MSVC)
	if( _MSC_VER > 1999 ) {
		compilerString = QLatin1String( "MSVC <unknown>" );
	} else if( _MSC_VER >= 1930 ) {
		compilerString = QLatin1String( "MSVC 2022" );
	} else if( _MSC_VER >= 1920 ) {
		compilerString = QLatin1String( "MSVC 2019" );
	} else if( _MSC_VER >= 1910 ) {
		compilerString = QLatin1String( "MSVC 2017" );
	} else if( _MSC_VER >= 1900 ) {
		compilerString = QLatin1String( "MSVC 2015" );
	} else if( _MSC_VER >= 1800 ) {
		compilerString = QLatin1String( "MSVC 2013" );
	} else if( _MSC_VER >= 1700 ) {
		compilerString = QLatin1String( "MSVC 2012" );
	} else if( _MSC_VER >= 1600 ) {
		compilerString = QLatin1String( "MSVC 2010" );
	} else {
		compilerString = QLatin1String( "MSVC <old>" );
	}
#endif
	return compilerString;
}