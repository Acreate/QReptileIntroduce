#include "Menu.h"
#include "Action.h"
Menu::Menu( QWidget *parent ) : QMenu( parent ), parentMenu( nullptr ) {
}
Menu::~Menu( ) {

}
void Menu::addAction( Action *action ) {
	action->parentMenu = this;
	QMenu::addAction( action );
}
void Menu::addMenu( Menu *menu ) {
	menu->parentMenu = this;
	QMenu::addMenu( menu );
}
