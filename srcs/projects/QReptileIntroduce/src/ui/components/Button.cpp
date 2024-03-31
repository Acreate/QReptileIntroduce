#include "Button.h"
#include "../../userHread/DebugInfo.h"
#include <QPaintEvent>
#include <QPainter>
Button::Button( QWidget *parent ): QPushButton( parent ){
}
Button::~Button( ) {
	DEBUG_RUN(
		qDebug() << tr(u8"Button::~Button");
	);
}

