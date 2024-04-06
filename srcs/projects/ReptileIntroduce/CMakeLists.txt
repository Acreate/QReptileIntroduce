get_current_dir_name( prject_name ${CMAKE_CURRENT_SOURCE_DIR} )
message( "============ ${prject_name}" )
message( "name =" ${prject_name} )
project( ${prject_name} VERSION 0.1 LANGUAGES CXX )
message( "============ ${CURRENT_FOLDER}" )


SET( PROJECT_BINARY_DIR "${CMAKE_HOME_DIRECTORY}/builder/${CMAKE_BUILD_TYPE}_${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}_${CMAKE_CXX_COMPILER_ID}/" CACHE STRING "" FORCE )
SET( CMAKE_CURRENT_BINARY_DIR "${PROJECT_BINARY_DIR}/" CACHE STRING "" FORCE )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/" CACHE STRING "" FORCE )
SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/" CACHE STRING "" FORCE )
SET( CMAKE_INSTALL_BINDIR "${PROJECT_BINARY_DIR}" CACHE PATH "" FORCE )
SET( CMAKE_INSTALL_LIBDIR "${PROJECT_BINARY_DIR}" CACHE PATH "" FORCE )
SET( CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}" CACHE PATH "" FORCE )
SET( EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}" CACHE STRING "" FORCE )


# 如果发生错误，请重新配置 qt 6 环境，该文件位于根目录的 /cmake/QT6/Qt6EventConfig.cmake
find_package( Qt6Event REQUIRED PATHS "${CMAKE_HOME_DIRECTORY}/srcs/cmake/QT6/" )

FIND_PACKAGE( Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
	Core
	Widgets
	Gui
	LinguistTools
	Network
	openglwidgets
	Core5Compat
	LinguistTools
)

qt_standard_project_setup()

file( GLOB_RECURSE SRC_LIST
	"*.h"
	"*.c"
	"*.cpp"
	"*.cxx"
	"*.hpp"
)
include_directories( ${www_121ds_cc_include_dir} )
file( GLOB_RECURSE SRC_UI
	"*.ui"
)

# # 合并源码
list( APPEND ${SRC_LIST} ${SRC_UI} ${QM_FILES} )

if( ${QT_VERSION_MAJOR} GREATER_EQUAL 6 )
	qt_add_executable( ${PROJECT_NAME}
		MANUAL_FINALIZATION
		${SRC_LIST}
	)
else()
	if( ANDROID )
		add_library( ${PROJECT_NAME} SHARED
			${SRC_LIST}
		)
	else()
		add_executable( ${PROJECT_NAME}
			${SRC_LIST}
		)
	endif()
endif()

set( translation_file_list

	# # 美语
	"${CMAKE_CURRENT_LIST_DIR}/translations/${PROJECT_NAME}_en.ts"

	# # 汉语
	"${CMAKE_CURRENT_LIST_DIR}/translations/${PROJECT_NAME}_zh_CN.ts"
)

set_source_files_properties(${translation_file_list}
     PROPERTIES OUTPUT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/progress/translations")
qt_add_translations(
	${PROJECT_NAME}
	TS_FILES
	${translation_file_list}
	SOURCES ${SRC_LIST}
	LUPDATE_OPTIONS -no-obsolete
	QM_FILES_OUTPUT_VARIABLE qm_files
)

# install( FILES ${qm_files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ini/progress/translations/" )

call_qt_deploy( ${PROJECT_NAME}
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Network
	Qt${QT_VERSION_MAJOR}::OpenGLWidgets
	Qt${QT_VERSION_MAJOR}::Core5Compat
)

copy_dir_path( ${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/res" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/" )
