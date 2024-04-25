#include "File.h"
File::File( const QString &currentFilePtah ): currentFilePtah( QFileInfo( currentFilePtah ).absolutePath( ) ) {
}
File::~File( ) {
}
