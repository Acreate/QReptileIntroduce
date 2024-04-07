get_current_dir_name( prject_name ${CMAKE_CURRENT_SOURCE_DIR} )
message( "============ ${prject_name}" )
message( "name =" ${prject_name} )
project( ${prject_name} VERSION 0.1 LANGUAGES CXX )
message( "============ ${CURRENT_FOLDER}" )


SET( PROJECT_BINARY_DIR "${CMAKE_HOME_DIRECTORY}/builder/${CMAKE_BUILD_TYPE}_${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}_${CMAKE_CXX_COMPILER_ID}/user_plugs/${prject_name}/" CACHE STRING "" FORCE )
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

file( GLOB_RECURSE SRC_DATA
	"*.json"
)

string( REPLACE "." "_" project_name_include_dir ${PROJECT_NAME} )
include_directories( ../../shaders/NoveInfo )
include_directories( ../../../shaders/UserHread )
set( ${project_name_include_dir}_include_dir ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/ PARENT_SCOPE )

# # 合并源码
list( APPEND ${SRC_LIST} ${SRC_UI} ${QM_FILES} ${SRC_DATA} )
qt_add_plugin( ${PROJECT_NAME} SHARED
	${SRC_LIST}
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	Qt${QT_VERSION_MAJOR}::Gui
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Network
	Qt${QT_VERSION_MAJOR}::Core5Compat
	NoveInfo
)

# www_121ds_cc_EXPORTS
# target_compile_definitions( ${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_EXPORTS )

# copy_dir_path( ${PROJECT_NAME} "${CMAKE_CURRENT_LIST_DIR}/res" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/" )
STRING( SUBSTRING "${PROJECT_NAME}" 0 1 exportFileStartName )
STRING( SUBSTRING "${PROJECT_NAME}" 1 -1 exportFileName )
STRING( TOUPPER ${exportFileStartName} exportFileStartName )
SET( exportFileName ${CMAKE_CURRENT_SOURCE_DIR}/export/${exportFileStartName}${exportFileName}_export.h )
GENERATE_EXPORT_HEADER( ${PROJECT_NAME}
	EXPORT_FILE_NAME ${exportFileName}
)
TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
