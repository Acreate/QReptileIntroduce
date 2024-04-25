#include "Action.h"
#include "Menu.h"
#include "MenuBar.h"
Action::Action( QObject *parent ) : QAction( parent ), parentMenu( nullptr ) {
	connect( this, &QAction::triggered, [=]( ) {
		emit click( this );
	} );
}
Action::~Action( ) {

}
MenuBar * Action::converToMenuBar( QObject *converObj ) {
	return qobject_cast< MenuBar * >( converObj );
}
Menu * Action::converToMenu( QObject *converObj ) {
	return qobject_cast< Menu * >( converObj );
}
