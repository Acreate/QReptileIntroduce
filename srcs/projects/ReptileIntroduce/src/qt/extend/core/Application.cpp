#include "Application.h"
#include "../../group/ui/MainWidget.h"
#include <QEvent>
#include <QMouseEvent>
Application::Application( int &argc, char **argv, int i ): QApplication( argc, argv, i ) {
}
Application::~Application( ) {
}
bool Application::notify( QObject *object, QEvent *event ) {
	//const char *className = object->metaObject( )->className( );
	//if( catchEventAtObjectPtrList.contains( object ) ) {
	//	if( className == Menu::staticMetaObject.className( ) )
	//		if( event->type( ) == QEvent::Hide )
	//			qDebug( ) << className << " : " << event->type( );

	//}
	//if( catchEventAtClassNameList.contains( className ) )
	//	if( className == MainWidget::staticMetaObject.className( ) )
	//		if( event->type( ) == QEvent::MouseButtonPress )
	//			qDebug( ) << className << " : " << event->type( );
	return QApplication::notify( object, event );
}
