CMAKE_MINIMUM_REQUIRED( VERSION 3.19 )

SET( CMAKE_C_STANDARD 99 )
SET( CMAKE_C_STANDARD_REQUIRED ON )
SET( CMAKE_C_VISIBILITY_PRESET hidden )

SET( CMAKE_CXX_STANDARD 17 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )
SET( CMAKE_CXX_VISIBILITY_PRESET hidden )

find_package( CmakeFuncs REQUIRED PATHS "${CMAKE_CURRENT_SOURCE_DIR}/srcs/cmake/CmakeFuncs/" )

INCLUDE( GenerateExportHeader )
INCLUDE( GNUInstallDirs )



get_path_cmake_dir_path( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/libs/ "CMakeLists.txt" )
get_path_cmake_dir_path( out_path_list ${CMAKE_CURRENT_SOURCE_DIR}/srcs/projects/ "CMakeLists.txt" )

foreach( out_dir ${out_path_list} )
	string( REPLACE ${CMAKE_SOURCE_DIR}/ "./" out_path ${out_dir} )
	message( STATUS "正在添加路径 :\t" ${out_dir} )
	ADD_SUBDIRECTORY( ${out_dir} )
endforeach()
