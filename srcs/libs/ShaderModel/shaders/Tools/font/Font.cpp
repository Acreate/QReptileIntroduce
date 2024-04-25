#include "Font.h"
std::pair< size_t, size_t > Font::getFullLineIndex( const QString &str, const QFontMetrics &font_metrics, const int width ) {
	qsizetype maxLen = str.length( );
	qsizetype minBin = maxLen / 2;
	if( minBin < 2 )
		return { 2, maxLen > 2 ? maxLen : 2 };
	qsizetype maxBin;
	QString subStr = str.mid( 0, minBin );
	int horizontalAdvance = font_metrics.horizontalAdvance( subStr );
	if( width < horizontalAdvance ) {
		maxBin = minBin * 2;
		while( maxBin < maxLen ) {
			subStr = str.mid( 0, maxBin );
			horizontalAdvance = font_metrics.horizontalAdvance( subStr );
			if( width > horizontalAdvance )
				break;
			maxBin /= 2;
		}
		if( maxBin != minBin * 2 ) {
			minBin = maxBin;
			maxBin = minBin * 2;
		}
		while( maxBin < maxLen ) { // 宽度小于比较
			subStr = str.mid( 0, maxBin );
			horizontalAdvance = font_metrics.horizontalAdvance( subStr );
			if( width < horizontalAdvance )
				break;
			maxBin *= 2;
		}
		for( ; minBin < maxBin; ++minBin ) {
			subStr = str.mid( 0, minBin );
			horizontalAdvance = font_metrics.horizontalAdvance( subStr );
			if( width < horizontalAdvance )
				break;
		}
		maxBin = minBin;
		--minBin;

	} else {
		qint64 sep = minBin / 2;
		maxBin = sep + minBin;
		while( maxBin < maxLen ) {
			subStr = str.mid( 0, maxBin );
			horizontalAdvance = font_metrics.horizontalAdvance( subStr );
			if( width < horizontalAdvance )
				break;
			maxBin += sep;
		}
		if( maxBin != sep + minBin )
			minBin = maxBin - sep;

		while( minBin < maxLen ) { // 宽度小于比较
			subStr = str.mid( 0, minBin );
			horizontalAdvance = font_metrics.horizontalAdvance( subStr );
			if( width > horizontalAdvance )
				break;
			minBin -= sep;
		}
		for( ; minBin < maxBin; ++minBin ) {
			subStr = str.mid( 0, minBin );
			horizontalAdvance = font_metrics.horizontalAdvance( subStr );
			if( width < horizontalAdvance )
				break;
		}
		maxBin = minBin;
		--minBin;

	}

	return { minBin, maxBin };
}
