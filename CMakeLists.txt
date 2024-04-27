CMAKE_MINIMUM_REQUIRED( VERSION 3.19 )

SET( CMAKE_C_STANDARD 99 )
SET( CMAKE_C_STANDARD_REQUIRED ON )
SET( CMAKE_C_VISIBILITY_PRESET hidden )

SET( CMAKE_CXX_STANDARD 17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )
SET( CMAKE_CXX_VISIBILITY_PRESET hidden )

if( ANDROID )
	message( SEND_ERROR "无法编译到 android 平台" )
endif()

FIND_PACKAGE( CmakeFuncs REQUIRED PATHS "${CMAKE_CURRENT_SOURCE_DIR}/srcs/cmake/CmakeFuncs/" )

get_current_dir_name( prject_name ${CMAKE_CURRENT_SOURCE_DIR} )
message( "============ ${prject_name}" )
message( "name =" ${prject_name} )
project( ${prject_name} VERSION 0.1 LANGUAGES CXX )
message( "============ ${CURRENT_FOLDER}" )

INCLUDE( GenerateExportHeader )
INCLUDE( GNUInstallDirs )

SET( Project_Run_bin "${CMAKE_HOME_DIRECTORY}/builder/${CMAKE_BUILD_TYPE}_${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}_${CMAKE_CXX_COMPILER_ID}/" )
SET( Project_Install_bin "${Project_Run_bin}" )
SET( Project_Plug_bin "${CMAKE_HOME_DIRECTORY}/builder/${CMAKE_BUILD_TYPE}_${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}_${CMAKE_CXX_COMPILER_ID}/plugs" )

message( "QT_VERSION_MAJOR : ${QT_VERSION_MAJOR}" )

# 如果发生错误，请重新配置 qt 6 环境，该文件位于根目录的 /cmake/QT6/Qt6EventConfig.cmake
FIND_PACKAGE( Qt6Event REQUIRED PATHS "${CMAKE_HOME_DIRECTORY}/srcs/cmake/QT6/" )

FIND_PACKAGE( Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
	Core
	Widgets
	Gui
	Network
	openglwidgets
	Core5Compat
	LinguistTools
	Xml
)

get_current_dir_name( current_dir_name ${CMAKE_CURRENT_SOURCE_DIR} )

SET( translation_file_list

	# # 美语
	"${CMAKE_HOME_DIRECTORY}/srcs/translations/${current_dir_name}_en.ts"

	# # 汉语
	"${CMAKE_HOME_DIRECTORY}/srcs/translations/${current_dir_name}_zh_CN.ts"
)

FILE( GLOB_RECURSE SRC_LIST
	${CMAKE_HOME_DIRECTORY}/srcs/*.cpp
	${CMAKE_HOME_DIRECTORY}/srcs/*.cxx
	${CMAKE_HOME_DIRECTORY}/srcs/*.c
	${CMAKE_HOME_DIRECTORY}/srcs/*.h
	${CMAKE_HOME_DIRECTORY}/srcs/*.hpp
	${CMAKE_HOME_DIRECTORY}/srcs/*.hxx
	${CMAKE_HOME_DIRECTORY}/srcs/*.ui
)

ADD_CUSTOM_TARGET( ${current_dir_name} )
SET_SOURCE_FILES_PROPERTIES( ${translation_file_list}
	PROPERTIES OUTPUT_LOCATION "${CMAKE_HOME_DIRECTORY}/builder/${CMAKE_BUILD_TYPE}_${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}_${CMAKE_CXX_COMPILER_ID}/progress/translations" )
qt_add_translations(
	${current_dir_name}
	TS_FILES ${translation_file_list}
	SOURCES ${SRC_LIST}
	LUPDATE_OPTIONS -no-obsolete
)

# # 根部定义的模板位置
set( cmake_definitions_tmp ${CMAKE_HOME_DIRECTORY}/srcs/cmake/temp )
set( cmake_definitions_in ${cmake_definitions_tmp}/cmake_in/cmake_to_c_cpp_header_macro.h.in )

get_path_cmake_dir_path( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/libs/ShaderModel/plugs "CMakeLists.txt" )

# ###################          拷贝模板-不存在则拷贝
set( target_update_sub_dir_list )
list( APPEND target_update_sub_dir_list
	"RequestDefinition"
	"Request"
	"RequestNetPlugin"
)

foreach( plug_dir ${out_path_list} )
	foreach( target_update_sub_dir ${target_update_sub_dir_list} )
		update_path_files_if( "${cmake_definitions_tmp}/cpp_code/${target_update_sub_dir}" "${plug_dir}/${target_update_sub_dir}" )
	endforeach()
endforeach()

m_set_env( "${Project_Run_bin}" "${Project_Install_bin}" )

# 创建测试写入目录
set( Cache_Path_Dir "${Project_Run_bin}write_cache/" )
file( MAKE_DIRECTORY  ${Cache_Path_Dir} )

get_path_cmake_dir_path( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/git/htmlcpp/srcs "CMakeLists.txt" )
get_path_cmake_dir_path( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/libs/UserHread/defineMorc "CMakeLists.txt" )
get_path_cmake_dir_path( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/libs/ShaderModel/shaders/ "CMakeLists.txt" )

get_path_cmake_dir_path( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/projects/ "CMakeLists.txt" )

foreach( out_dir ${out_path_list} )
	STRING( REPLACE ${CMAKE_SOURCE_DIR}/ "./" out_path ${out_dir} )
	MESSAGE( STATUS "正在添加路径 :\t" ${out_dir} )
	ADD_SUBDIRECTORY( ${out_dir} )
endforeach()
