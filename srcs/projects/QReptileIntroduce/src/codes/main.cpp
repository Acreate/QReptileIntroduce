#include <QApplication>
#include <QWidget>
#include <QOpenGLWidget>
#include "../ui/MainWidget.h"
int main( int argc, char *argv[ ] ) {
	QApplication a( argc, argv );
	MainWidget qWidget;
	qWidget.show(  );
	return a.exec( );
}
