get_current_dir_name( prject_name ${CMAKE_CURRENT_SOURCE_DIR} )
message( "============ ${prject_name}" )
message( "name =" ${prject_name} )
project( ${prject_name} VERSION 0.1 LANGUAGES CXX )
message( "============ ${CURRENT_FOLDER}" )

# 如果发生错误，请重新配置 qt 6 环境，该文件位于根目录的 /cmake/QT6/Qt6EventConfig.cmake
find_package( Qt6Event REQUIRED PATHS "${CMAKE_HOME_DIRECTORY}/srcs/cmake/QT6/" )

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Gui)
FIND_PACKAGE( Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
	Gui
	Core
	LinguistTools
	Network
	Core5Compat
)

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
add_library( ${PROJECT_NAME} SHARED
	${SRC_LIST}
)
set( translation_file_list
	# # 美语
	"${CMAKE_CURRENT_LIST_DIR}/translations/${PROJECT_NAME}_en.ts"
	# # 汉语
	"${CMAKE_CURRENT_LIST_DIR}/translations/${PROJECT_NAME}_zh_CN.ts"
)

qt_add_translations(
	${PROJECT_NAME}
	TS_FILES
	${translation_file_list}
	LUPDATE_OPTIONS -no-obsolete
	QM_FILES_OUTPUT_VARIABLE qm_files
)

install( FILES ${qm_files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ini/progress/translations/" )

call_qt_deploy( ${PROJECT_NAME}
	PRIVATE
	Qt${QT_VERSION_MAJOR}::Gui
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Network
	Qt${QT_VERSION_MAJOR}::Core5Compat
)

# www_121ds_cc_EXPORTS
target_compile_definitions(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_EXPORTS)
# copy_dir_path( ${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/res" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/" )


STRING( SUBSTRING "${PROJECT_NAME}" 0 1 exportFileStartName )
STRING( SUBSTRING "${PROJECT_NAME}" 1 -1 exportFileName )
STRING( TOUPPER ${exportFileStartName} exportFileStartName )
SET( exportFileName ${CMAKE_CURRENT_SOURCE_DIR}/export/${exportFileStartName}${exportFileName}_export.h )
GENERATE_EXPORT_HEADER( ${PROJECT_NAME}
	EXPORT_FILE_NAME ${exportFileName}
)
TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
