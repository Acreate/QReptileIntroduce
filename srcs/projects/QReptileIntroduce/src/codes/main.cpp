#include <QApplication>
#include <QWidget>
#include <QOpenGLWidget>
int main( int argc, char *argv[ ] ) {
	QApplication a( argc, argv );
	QOpenGLWidget qWidget;
	qWidget.show(  );
	return a.exec( );
}
