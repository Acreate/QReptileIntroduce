get_current_dir_name( prject_name ${CMAKE_CURRENT_SOURCE_DIR} )
message( "============ ${prject_name}" )
message( "name =" ${prject_name} )
project( ${prject_name} VERSION 0.1 LANGUAGES CXX )
message( "============ ${CURRENT_FOLDER}" )

m_set_env( "${Project_Run_bin}" "${Project_Install_bin}" )

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

# 设置变量
set( Cmake_Project_Name "${PROJECT_NAME}" )
set( Cmake_Project_Binary_Dir "${CMAKE_BINARY_DIR}" )
set( Cmake_Project_Binary_Dir "${PROJECT_BINARY_DIR}" )
set( Cmake_Project_Binary_File_Name "$<TARGET_FILE:tgt>" )

set( ${project_name_include_dir}_include_dir ${CMAKE_CURRENT_LIST_DIR} PARENT_SCOPE )

include_directories( ${NovelInfo_include_dir} )
include_directories( ${Tools_include_dir} )
include_directories( ${UserHread_include_dir} )
include_directories( ${HtmlTools_include_dir} )
include_directories( ${IOStream_include_dir} )
include_directories( ${Path_Tool_include_dir} )
include_directories( ${QtExtendDB_include_dir} )

set( cmake_definitions_env_out "${CMAKE_CURRENT_LIST_DIR}/${cmake_auto_sub_dir_name}/macro/cmake_to_c_cpp_header_env.h" )
configure_file( ${cmake_definitions_env_in} ${cmake_definitions_env_out} )

# # 合并源码
list( APPEND ${SRC_LIST} ${SRC_UI} ${QM_FILES} ${SRC_DATA} ${cmake_definitions_env_out} )

# # qt_add_library( ${PROJECT_NAME} SHARED
# # 	${SRC_LIST}
# # )
add_library( ${PROJECT_NAME} SHARED
	${SRC_LIST}
)

call_qt_deploy( ${PROJECT_NAME}
	PRIVATE
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Gui
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Network
	Qt${QT_VERSION_MAJOR}::Core5Compat
	Qt${QT_VERSION_MAJOR}::Sql
	NovelInfo
	Tools
	HtmlTools
	Path_Tool
	IOStream
	QtExtendDB
)

string( REPLACE "." "_" project_name_include_dir ${PROJECT_NAME} )
set( ${project_name_include_dir}_include_dir ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/ PARENT_SCOPE )

# # target_link_libraries( ${PROJECT_NAME}
# # 	PRIVATE
# # 	Qt${QT_VERSION_MAJOR}::Widgets
# # 	Qt${QT_VERSION_MAJOR}::Gui
# # 	Qt${QT_VERSION_MAJOR}::Core
# # 	Qt${QT_VERSION_MAJOR}::Network
# # 	Qt${QT_VERSION_MAJOR}::Core5Compat
# # 	Qt${QT_VERSION_MAJOR}::Sql
# # 	NovelInfo
# # 	Tools
# # 	HtmlTools
# # 	Path_Tool
# # 	IOStream
# # 	QtExtendDB
# # )
configure_file( ${cmake_definitions_env_in} ${cmake_definitions_env_out} )

STRING( SUBSTRING "${PROJECT_NAME}" 0 1 exportFileStartName )
STRING( SUBSTRING "${PROJECT_NAME}" 1 -1 exportFileName )
STRING( TOUPPER ${exportFileStartName} exportFileStartName )
SET( exportFileName ${CMAKE_CURRENT_SOURCE_DIR}/${cmake_auto_sub_dir_name}/export/${exportFileStartName}${exportFileName}_export.h )
GENERATE_EXPORT_HEADER( ${PROJECT_NAME}
	EXPORT_FILE_NAME ${exportFileName}
)
TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
unset( SRC_LIST )
unset( SRC_UI )
unset( SRC_DATA )
unset( cmake_definitions_out )
