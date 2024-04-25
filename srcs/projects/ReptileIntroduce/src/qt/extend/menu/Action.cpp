#include "Action.h"

#include <stack>

#include "Menu.h"
#include "MenuBar.h"
#include "path/Dir.h"
Action::Action( QObject *parent ) : QAction( parent ), parentMenu( nullptr ) {
}
Action::~Action( ) {

}
QSharedPointer< QString > Action::getActionXPath( ) const {
	QObject *parentMenu = getParentMenu( );
	if( parentMenu ) {
		std::stack< QString > xpathStack;
		xpathStack.push( text( ) );
		do {
			Menu *menu = Action::converToMenu( parentMenu );
			if( menu ) {
				xpathStack.push( menu->title( ) );
				parentMenu = menu->getParentMenu( );
				continue;
			}
			MenuBar *menuBar = Action::converToMenuBar( parentMenu );
			if( menuBar ) {
				xpathStack.push( menuBar->windowTitle( ) );
				parentMenu = menuBar->getParentMenu( );
				continue;
			}
			break;
		} while( parentMenu );
		QStringList xpath;
		while( xpathStack.size( ) ) {
			xpath << xpathStack.top( );
			xpathStack.pop( );
		}

		return QSharedPointer< QString >( new QString( xpath.join( QDir::separator( ) ) ) );
	}
	return QSharedPointer< QString >( new QString( text( ) ) );
}
MenuBar * Action::converToMenuBar( QObject *converObj ) {
	return qobject_cast< MenuBar * >( converObj );
}
Menu * Action::converToMenu( QObject *converObj ) {
	return qobject_cast< Menu * >( converObj );
}
