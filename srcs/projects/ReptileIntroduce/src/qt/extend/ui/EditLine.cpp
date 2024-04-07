#include "EditLine.h"
#include <DebugInfo.h>
#include <QPaintEvent>
EditLine::~EditLine( ) {
	DEBUG_RUN(
		qDebug() << tr(u8"EditLine::~EditLine : ") << text();
	);
}
void EditLine::paintEvent( QPaintEvent *paintEvent ) {
	QLineEdit::paintEvent( paintEvent );
}
