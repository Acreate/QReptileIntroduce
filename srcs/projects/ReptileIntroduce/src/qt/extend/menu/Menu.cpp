#include "Menu.h"
#include "Action.h"
Menu::Menu( QWidget *parent ) : QMenu( parent ), parentMenu( nullptr ) {
}
Menu::~Menu( ) {

}
void Menu::addAction( Action *action ) {
	action->parentMenu = this;
	QMenu::addAction( action );
	auto overload = qOverload< bool >( &QAction::triggered );
	connect( action, overload, [=]( bool flage ) {
		emit click( action );
	} );
	connect( this, &Menu::click, [=]( Action *sendAction ) {
		emit action->click( sendAction );
	} );
}
void Menu::addMenu( Menu *menu ) {
	menu->parentMenu = this;
	QMenu::addMenu( menu );
}
