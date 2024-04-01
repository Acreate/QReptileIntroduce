#include "Button.h"
#include "../../userHread/DebugInfo.h"
#include <QPaintEvent>
#include <QPainter>
Button::Button( const QString &text, QWidget *parent  ): QPushButton( text, parent ) {
}
Button::Button( const QIcon &icon, const QString &text, QWidget *parent ): QPushButton( icon, text, parent ) {
}
Button::Button( QWidget *parent ): QPushButton( parent ){
}
Button::~Button( ) {
	DEBUG_RUN(
		qDebug() << tr(u8"Button::~Button : ") << text();
	);
}

