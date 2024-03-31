#include "EditLine.h"
#include "../../userHread/DebugInfo.h"
EditLine::~EditLine( ) {
	DEBUG_RUN(
		qDebug() << tr(u8"EditLine::~EditLine");
	);
}
