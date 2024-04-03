#include "Menu.h"
Menu::Menu( QWidget *parent ) : QMenu( parent ) {
}
Menu::~Menu( ) {

}
void Menu::addAction( Action *action ) {
	action->setParent( this );
	QMenu::addAction( action );
}
