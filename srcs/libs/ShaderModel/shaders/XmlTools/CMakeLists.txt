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


include_directories( ${Tools_include_dir} )

# 设置变量
set( Cmake_Project_Name "${PROJECT_NAME}" )
set( Cmake_Project_Binary_Dir "${CMAKE_BINARY_DIR}" )
set( Cmake_Project_Binary_Dir "${PROJECT_BINARY_DIR}" )
set( Cmake_Project_Binary_File_Name "$<TARGET_FILE:tgt>" )
### 		CMAKE_EXECUTABLE_SUFFIX	.exe	string
set( cmake_definitions_out "${CMAKE_CURRENT_LIST_DIR}/macro/cmake_to_c_cpp_header_env.h" )
set( cmake_definitions_org "${cmake_definitions_tmp}/cmake_in/cmake_to_c_cpp_header_env.h.in" )
configure_file( "${cmake_definitions_org}" "${cmake_definitions_out}" )


set( ${project_name_include_dir}_include_dir ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/.. PARENT_SCOPE)
# # 合并源码
list( APPEND ${SRC_LIST} ${SRC_UI} ${QM_FILES} ${SRC_DATA} ${cmake_definitions_out})
qt_add_library( ${PROJECT_NAME} SHARED
	${SRC_LIST}
)

string( REPLACE "." "_" project_name_include_dir ${PROJECT_NAME} )
set( ${project_name_include_dir}_include_dir ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/ PARENT_SCOPE )

target_link_libraries( ${PROJECT_NAME}
	PRIVATE
	Qt${QT_VERSION_MAJOR}::Core
	Tools
)

STRING( SUBSTRING "${PROJECT_NAME}" 0 1 exportFileStartName )
STRING( SUBSTRING "${PROJECT_NAME}" 1 -1 exportFileName )
STRING( TOUPPER ${exportFileStartName} exportFileStartName )
SET( exportFileName ${CMAKE_CURRENT_SOURCE_DIR}/export/${exportFileStartName}${exportFileName}_export.h )
GENERATE_EXPORT_HEADER( ${PROJECT_NAME}
	EXPORT_FILE_NAME ${exportFileName}
)
TARGET_INCLUDE_DIRECTORIES( ${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} )
