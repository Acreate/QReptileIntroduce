get_current_dir_name( prject_name ${CMAKE_CURRENT_SOURCE_DIR} )
message( "============ ${prject_name}" )
message( "name =" ${prject_name} )
project( ${prject_name} VERSION 0.1 LANGUAGES CXX )
message( "============ ${CURRENT_FOLDER}" )


qt_standard_project_setup()

file( GLOB_RECURSE SRC_LIST
	"*.h"
	"*.c"
	"*.cpp"
	"*.cxx"
	"*.hpp"
)

file( GLOB_RECURSE SRC_UI
	"*.ui"
)

file( GLOB_RECURSE SRC_DATA
	"*.json"
)

string(REPLACE "." "_" project_name_include_dir ${PROJECT_NAME})

set( ${project_name_include_dir}_include_dir ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/.. PARENT_SCOPE)
# # 合并源码
list( APPEND ${SRC_LIST} ${SRC_UI} ${QM_FILES} ${SRC_DATA})
qt_add_library( ${PROJECT_NAME} SHARED
	${SRC_LIST}
)

string( REPLACE "." "_" project_name_include_dir ${PROJECT_NAME} )
set( ${project_name_include_dir}_include_dir ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/ PARENT_SCOPE )

target_link_libraries( ${PROJECT_NAME}
	PRIVATE
	Qt${QT_VERSION_MAJOR}::Gui
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Network
	Qt${QT_VERSION_MAJOR}::Core5Compat
)

STRING( SUBSTRING "${PROJECT_NAME}" 0 1 exportFileStartName )
STRING( SUBSTRING "${PROJECT_NAME}" 1 -1 exportFileName )
STRING( TOUPPER ${exportFileStartName} exportFileStartName )
SET( exportFileName ${CMAKE_CURRENT_SOURCE_DIR}/export/${exportFileStartName}${exportFileName}_export.h )
GENERATE_EXPORT_HEADER( ${PROJECT_NAME}
	EXPORT_FILE_NAME ${exportFileName}
)
TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
