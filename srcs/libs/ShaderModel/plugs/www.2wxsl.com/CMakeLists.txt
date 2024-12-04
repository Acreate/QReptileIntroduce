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
include_directories( ${NovelInfo_include_dir} )
include_directories( ${UserHread_include_dir} )
include_directories( ${HtmlTools_include_dir} )
include_directories( ${IOStream_include_dir} )
include_directories( ${QtExtendDB_include_dir} )
set( ${project_name_include_dir}_include_dir ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/ PARENT_SCOPE )


set( cmake_definitions_macro_out "${CMAKE_CURRENT_LIST_DIR}/${cmake_auto_sub_dir_name}/macro/cmake_to_c_cpp_header_macro.h" )

# # 合并源码
list( APPEND ${SRC_LIST} ${SRC_UI} ${QM_FILES} ${SRC_DATA} ${cmake_definitions_macro_out} )
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
	Qt${QT_VERSION_MAJOR}::Sql
	NovelInfo
	HtmlTools
	IOStream
	QtExtendDB
)


# 设置变量
set( urlStart "http" )
set( iid \"${PROJECT_NAME}.json.IRequestNetInterfaceExtend\" )
set( get_url \"${urlStart}://${PROJECT_NAME}\" )

string(TOUPPER ${project_name_include_dir} project_head_start)


get_target_property( project_type_property ${PROJECT_NAME} TYPE )

if( "EXECUTABLE" STREQUAL ${project_type_property} )
	set( project_type_property_is_exe true )
else()
	set( project_type_property_is_exe false )
endif()

# ## 		CMAKE_EXECUTABLE_SUFFIX	.exe	string
configure_file( ${cmake_definitions_in} ${cmake_definitions_macro_out} )

unset(SRC_LIST)
unset(SRC_UI)
unset(SRC_DATA)
unset(QM_FILES)
  
