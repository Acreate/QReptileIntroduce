CMAKE_MINIMUM_REQUIRED( VERSION 3.19 )




SET( CMAKE_C_STANDARD 99 )
SET( CMAKE_C_STANDARD_REQUIRED ON )
SET( CMAKE_C_VISIBILITY_PRESET hidden )

SET( CMAKE_CXX_STANDARD 17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )
SET( CMAKE_CXX_VISIBILITY_PRESET hidden )

FIND_PACKAGE( CmakeFuncs REQUIRED PATHS "${CMAKE_CURRENT_SOURCE_DIR}/srcs/cmake/CmakeFuncs/" )

INCLUDE( GenerateExportHeader )
INCLUDE( GNUInstallDirs )



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
)
get_current_dir_name( current_dir_name ${CMAKE_CURRENT_SOURCE_DIR} )

SET( translation_file_list
	# # 美语
	"${CMAKE_HOME_DIRECTORY}/srcs/translations/${current_dir_name}_en.ts"
	# # 汉语
	"${CMAKE_HOME_DIRECTORY}/srcs/translations/${current_dir_name}_zh_CN.ts"
)

FILE(GLOB_RECURSE SRC_LIST  
	${CMAKE_HOME_DIRECTORY}/srcs/*.cpp
	${CMAKE_HOME_DIRECTORY}/srcs/*.cxx
	${CMAKE_HOME_DIRECTORY}/srcs/*.c
	${CMAKE_HOME_DIRECTORY}/srcs/*.h
	${CMAKE_HOME_DIRECTORY}/srcs/*.hpp
	${CMAKE_HOME_DIRECTORY}/srcs/*.hxx
	${CMAKE_HOME_DIRECTORY}/srcs/*.ui
)

ADD_CUSTOM_TARGET(${current_dir_name})
SET_SOURCE_FILES_PROPERTIES(${translation_file_list}
     PROPERTIES OUTPUT_LOCATION "${CMAKE_HOME_DIRECTORY}/builder/${CMAKE_BUILD_TYPE}_${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}_${CMAKE_CXX_COMPILER_ID}/progress/translations")
qt_add_translations(
	${current_dir_name}
	TS_FILES ${translation_file_list}
	SOURCES ${SRC_LIST}
	LUPDATE_OPTIONS -no-obsolete
)


get_path_cmake_dir_path( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/libs/ "CMakeLists.txt" )
get_path_cmake_dir_path( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/projects/ "CMakeLists.txt" )

foreach( out_dir ${out_path_list} )
	STRING( REPLACE ${CMAKE_SOURCE_DIR}/ "./" out_path ${out_dir} )
	MESSAGE( STATUS "正在添加路径 :\t" ${out_dir} )
	ADD_SUBDIRECTORY( ${out_dir} )
endforeach()
