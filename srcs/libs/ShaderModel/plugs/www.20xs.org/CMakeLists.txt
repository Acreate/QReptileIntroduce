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



# 设置变量
set( urlStart "https" )
set( iid \"${PROJECT_NAME}.json.IRequestNetInterfaceExtend\" )
set( get_url \"${urlStart}://${PROJECT_NAME}\" )

string(TOUPPER ${project_name_include_dir} project_head_start)

set( cmake_definitions_in ${CMAKE_HOME_DIRECTORY}/srcs/cmake/cmake_in/cmake_to_c_cpp_header_macro.h.in )
set( cmake_definitions_out ${CMAKE_CURRENT_LIST_DIR}/macro/cmake_to_c_cpp_header_macro.h )
configure_file( ${cmake_definitions_in} ${cmake_definitions_out} )
