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

include_directories( ${www_121ds_cc_include_dir} )
include_directories( ${NoveInfo_include_dir} )
include_directories( ${UserHread_include_dir} )
include_directories( ${Tools_include_dir} )
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

call_qt_deploy( ${PROJECT_NAME}
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Network
	Qt${QT_VERSION_MAJOR}::OpenGLWidgets
	Qt${QT_VERSION_MAJOR}::Core5Compat
	NoveInfo
	Tools
)

copy_dir_path( ${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/res/ini/progress/" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/progress/" )
copy_dir_path( ${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/res/ini/web/ini" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/user_plugs/" )
