#include <QFileDialog>
#include <QGenericPlugin>
#include <QPluginLoader>

#include "../qt/group/ui/MainWidget.h"
#include "../qt/extend/core/Appliction.h"

#include "interface/IRequestNetInterface.h"

int main( int argc, char *argv[ ] ) {
	Appliction appliction( argc, argv );
	do {
		QString fileName = QFileDialog::getOpenFileName( nullptr, u8"选择一个有效的插件", qApp->applicationDirPath( ), u8"(*.dll)" );
		if( fileName.isEmpty( ) )
			continue;
		QPluginLoader loader( fileName );
		if( !loader.load( ) )
			continue;
		QObject *object = loader.instance( );
		if( object ) {
			auto genericPlugin = qobject_cast< QGenericPlugin * >( object );
			QObject *object1 = genericPlugin->create( "", "" );
			if( !object1 )
				continue;
			IRequestNetInterface *requestNetInterface = qobject_cast< IRequestNetInterface * >( object1 );
			auto ptr = requestNetInterface->getData( );
			auto msg = ( QString * )ptr.get( );
			qDebug( ) << "插件获取消息指针为 :(" << msg << ")\n内容为 :" << *msg;
		}
		break;
	} while( true );
	MainWidget qWidget;
	qWidget.show( );
	return appliction.exec( );
}
