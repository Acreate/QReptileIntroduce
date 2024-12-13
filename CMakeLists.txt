CMAKE_MINIMUM_REQUIRED( VERSION 3.19 )

SET( CMAKE_C_STANDARD 99 )
SET( CMAKE_C_STANDARD_REQUIRED ON )
SET( CMAKE_C_VISIBILITY_PRESET hidden )

SET( CMAKE_CXX_STANDARD 17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )
SET( CMAKE_CXX_VISIBILITY_PRESET hidden )

IF( ANDROID )
	MESSAGE( SEND_ERROR "无法编译到 android 平台" )
ENDIF()

FIND_PACKAGE( CmakeFuncs REQUIRED PATHS "${CMAKE_CURRENT_SOURCE_DIR}/srcs/cmake/CmakeFuncs/" )

GET_CURRENT_DIR_NAME( prject_name ${CMAKE_CURRENT_SOURCE_DIR} )
MESSAGE( "============ ${prject_name}" )
MESSAGE( "name =" ${prject_name} )
PROJECT( ${prject_name} VERSION 0.1 LANGUAGES CXX )
MESSAGE( "============ ${CURRENT_FOLDER}" )

INCLUDE( GenerateExportHeader )
INCLUDE( GNUInstallDirs )

SET( Project_Run_bin "${CMAKE_HOME_DIRECTORY}/builder/${CMAKE_BUILD_TYPE}_${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}_${CMAKE_CXX_COMPILER_ID}/" )
SET( Project_Install_bin "${Project_Run_bin}" )
SET( Project_Plug_bin "${Project_Install_bin}/plug_lib" )

MESSAGE( "QT_VERSION_MAJOR : ${QT_VERSION_MAJOR}" )

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
		Sql
)

GET_CURRENT_DIR_NAME( current_dir_name ${CMAKE_CURRENT_SOURCE_DIR} )

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
QT_ADD_TRANSLATIONS(
		${current_dir_name}
		TS_FILES ${translation_file_list}
		SOURCES ${SRC_LIST}
		LUPDATE_OPTIONS -no-obsolete
)

# # 根部定义的模板位置
SET( cmake_definitions_tmp ${CMAKE_HOME_DIRECTORY}/srcs/cmake/temp )
SET( cmake_definitions_macro_in ${cmake_definitions_tmp}/cmake_in/cmake_to_c_cpp_header_macro.h.in )
SET( cmake_definitions_env_in ${cmake_definitions_tmp}/cmake_in/cmake_to_c_cpp_header_env.h.in )
SET( cmake_auto_sub_dir_name auto_generate_files )
SET( plug_path_list )

GET_PATH_CMAKE_DIR_PATH( plug_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/libs/ShaderModel/plugs "CMakeLists.txt" )

# ###################          拷贝模板-不存在则拷贝
SET( target_update_sub_dir_list )
LIST( APPEND target_update_sub_dir_list
		"RequestDefinition"
		"RequestNet"
		"RequestNetPlugin"
		"NovelInfo"
		"instance_function"
)

FOREACH( plug_dir ${plug_path_list} )
	FOREACH( target_update_sub_dir ${target_update_sub_dir_list} )
		UPDATE_PATH_FILES_IF( "${cmake_definitions_tmp}/cpp_code/${target_update_sub_dir}" "${plug_dir}/${target_update_sub_dir}" )
	ENDFOREACH()
ENDFOREACH()

M_SET_ENV( "${Project_Run_bin}" "${Project_Install_bin}" )

# 创建测试写入目录
SET( Cache_Path_Dir "${Project_Run_bin}write_cache/" )
FILE( MAKE_DIRECTORY ${Cache_Path_Dir} )


GET_PATH_CMAKE_DIR_PATH( git_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/git/argParser/srcs/argParser "CMakeLists.txt" )
GET_PATH_CMAKE_DIR_PATH( git_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/git/htmlcpp/srcs/HtmlTools "CMakeLists.txt" )
GET_PATH_CMAKE_DIR_PATH( git_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/git/QtExtendDB/srcs/git/DataCheck/srcs/DataCheck "CMakeLists.txt" )
GET_PATH_CMAKE_DIR_PATH( git_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/git/QtExtendDB/srcs/QtExtendDB "CMakeLists.txt" )
LIST( APPEND out_path_list ${git_path_list} )
GET_PATH_CMAKE_DIR_PATH( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/libs/ShaderModel/shaders/Path_Tool "CMakeLists.txt" )
GET_PATH_CMAKE_DIR_PATH( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/libs/ShaderModel/shaders/IOStream "CMakeLists.txt" )
LIST( APPEND out_path_list ${plug_path_list} )
GET_PATH_CMAKE_DIR_PATH( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/libs/UserHread/defineMorc "CMakeLists.txt" )
GET_PATH_CMAKE_DIR_PATH( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/libs/ShaderModel/shaders/NovelInfo "CMakeLists.txt" )
GET_PATH_CMAKE_DIR_PATH( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/libs/ShaderModel/shaders/Plug "CMakeLists.txt" )
GET_PATH_CMAKE_DIR_PATH( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/libs/ShaderModel/shaders/Tools "CMakeLists.txt" )

GET_PATH_CMAKE_DIR_PATH( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/projects/ "CMakeLists.txt" )
 
FOREACH( out_dir ${out_path_list} )
	STRING( REPLACE ${CMAKE_SOURCE_DIR}/ "./" out_path ${out_dir} )
	MESSAGE( STATUS "正在添加路径 :\t" ${out_dir} )
	ADD_SUBDIRECTORY( ${out_dir} )
ENDFOREACH()
