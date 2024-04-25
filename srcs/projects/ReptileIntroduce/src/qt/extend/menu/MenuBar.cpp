#include "MenuBar.h"
#include "Menu.h"
#include "Action.h"

MenuBar::MenuBar( QWidget *parent ): QMenuBar( parent ), parentMenu( nullptr ) {
}
MenuBar::~MenuBar( ) {
}
void MenuBar::addAction( Action *action ) {

	action->parentMenu = this;
	QMenuBar::addAction( action );
}
void MenuBar::addMenu( Menu *menu ) {

	menu->parentMenu = this;
	QMenuBar::addMenu( menu );
}
