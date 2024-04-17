get_current_dir_name( prject_name ${CMAKE_CURRENT_SOURCE_DIR} )
message( "============ ${prject_name}" )
message( "name =" ${prject_name} )
project( ${prject_name} VERSION 0.1 LANGUAGES CXX )
message( "============ ${CURRENT_FOLDER}" )


SET( Project_Plug_bin_Path  "${Project_Plug_bin}/${prject_name}/")
m_set_env( "${Project_Plug_bin_Path}" "${Project_Plug_bin_Path}" )

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

set( cmake_definitions_out ${CMAKE_CURRENT_LIST_DIR}/macro/cmake_to_c_cpp_header_macro.h )
configure_file( ${cmake_definitions_in} ${cmake_definitions_out} )
