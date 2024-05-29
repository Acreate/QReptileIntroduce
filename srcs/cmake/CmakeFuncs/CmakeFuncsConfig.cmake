﻿CMAKE_MINIMUM_REQUIRED( VERSION 3.19 )

macro( m_set_env bin_ptah install_path )
	SET( PROJECT_BINARY_DIR "${bin_ptah}" CACHE STRING "" FORCE )

	SET( CMAKE_CURRENT_BINARY_DIR "${bin_ptah}" CACHE STRING "" FORCE )
	SET( EXECUTABLE_OUTPUT_PATH "${bin_ptah}" CACHE STRING "" FORCE )

	SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY "${bin_ptah}" CACHE STRING "" FORCE )
	SET( CMAKE_LIBRARY_OUTPUT_DIRECTORY "${bin_ptah}" CACHE STRING "" FORCE )

	SET( CMAKE_INSTALL_BINDIR "${install_path}" CACHE PATH "" FORCE )
	SET( CMAKE_INSTALL_LIBDIR "${install_path}" CACHE PATH "" FORCE )
	SET( CMAKE_INSTALL_PREFIX "${install_path}" CACHE PATH "" FORCE )

	SET( QT_DEPLOY_BIN_DIR "${install_path}" CACHE STRING "" FORCE )
endmacro()

macro( m_all_set_env all_path )
	m_set_env( ${all_path} ${all_path} )
endmacro()

# ## 根据目录获取一个文件夹名称
function( get_current_dir_name out_name in_path )
	STRING( REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER ${in_path} )
	set( ${out_name} ${CURRENT_FOLDER} PARENT_SCOPE )
endfunction()

# ## 在目录中查找目录，并且在目标目录中找到 file_name 的文件名
# ## 若存在，则返回所在目录的路径
# ## 文件名不区分大小写
function( get_path_cmake_dir_path out_list check_path_dir file_name )
	set( for_each_list_dirs ${${out_list}} )

	if( IS_DIRECTORY "${check_path_dir}" )
		# # 获取所有目录
		file( GLOB_RECURSE file_get_file_paths "${check_path_dir}/*" )

		foreach( current_path_file ${file_get_file_paths} )
			STRING( REGEX REPLACE ".+/(.*)" "\\1" out_file_name ${current_path_file} )

			if( "${file_name}" STREQUAL "${out_file_name}" )
				STRING( REGEX REPLACE "(.+)/.*" "\\1" out_path_file ${current_path_file} )
				list( APPEND for_each_list_dirs "${out_path_file}" )
			endif()
		endforeach()

		set( ${out_list} ${for_each_list_dirs} PARENT_SCOPE )
	endif()
endfunction()

# # 拷贝目录到指定路径
function( copy_dir_path builder_target copy_dir_target paste_dir_target )
	add_custom_command(
		TARGET ${builder_target} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${copy_dir_target}/" "${paste_dir_target}"
	)
endfunction()

# # 查找指定路径的 *.cpp *.c *.hpp *.h 文件
# # check_path : 路径
# # out_file_list : 返回值
function( get_path_cxx_and_c_sources check_path out_file_list )
	set( for_each_list_dirs ${${out_file_list}} )
	FILE( GLOB_RECURSE file_get_names
		"${check_path}/*.cpp"
		"${check_path}/*.c"
		"${check_path}/*.hpp"
		"${check_path}/*.h"
	)

	foreach( get_file_name ${file_get_names} )
		FILE( REAL_PATH "${get_file_name}" out_value_path )
		list( APPEND for_each_file_list ${out_value_path} )
	endforeach()

	set( ${out_file_list} ${for_each_file_list} PARENT_SCOPE )
endfunction()

# ## 查找匹配拓展名的文件
# ## check_path : 检查路径
# ## find_expansion : 拓展名列表
# ## out_file_list : 返回输出
function( get_path_sources check_path find_expansion out_file_list )
	set( for_each_list_dirs ${${out_file_list}} )

	foreach( types_name ${find_expansion} )
		# message( "检测源码路径 : ${check_path}/${types_name}" )
		FILE( GLOB_RECURSE file_get_names "${check_path}/*.${types_name}" )

		foreach( get_file_name ${file_get_names} )
			# message( "发现文件 : ${get_file_name}" )
			list( APPEND for_each_file_list ${get_file_name} )
		endforeach()
	endforeach()

	set( ${out_file_list} ${for_each_file_list} PARENT_SCOPE )
endfunction()

# 更新路径所有的文件与文件夹
# 该操作会直接覆盖已经存在的文件，并且把源存在的文件拷贝到目标目录上
# src_path : 源，提供更新的参考文件
# des_path : 目标，提供有效的目标信息
function( update_path_files src_path des_path )
	# message( "update_path_files :" "\n\t*\t" ${src_path} "\n\t*\t" ${des_path} )
	file( GLOB_RECURSE file_get_src_file_paths RELATIVE "${src_path}" "${src_path}/*" )

	foreach( file_path ${file_get_src_file_paths} )
		set( src_file_path "${src_path}/${file_path}" )
		message( "update_path_files => 源 (" ${src_path}/${file_path} ") 拷贝到 [" ${des_path}"]" )
		file( COPY "${src_file_path}"
			DESTINATION "${des_path}" )
	endforeach()
endfunction()

# 如果不存在文件，则更新路径所有的文件与文件夹
# 该操作会直接覆盖已经存在的文件，并且把源存在的文件拷贝到目标目录上
# src_path : 源，提供更新的参考文件
# des_path : 目标，提供有效的目标信息
function( update_path_files_if src_path des_path )
	# message( "update_path_files :" "\n\t*\t" ${src_path} "\n\t*\t" ${des_path} )
	file( GLOB_RECURSE file_get_src_file_paths RELATIVE "${src_path}" "${src_path}/*" )
	file( GLOB_RECURSE file_get_des_file_paths RELATIVE "${des_path}" "${des_path}/*" )

	foreach( file_path ${file_get_src_file_paths} )
		set( src_file_path "${src_path}/${file_path}" )

		if( NOT EXISTS "${des_path}/${file_path}" )
			message( "update_path_files_if => 源 (" ${src_path}/${file_path} ") 拷贝到 [" ${des_path}"]" )
			file( COPY "${src_file_path}"
				DESTINATION "${des_path}" )
		endif()
	endforeach()
endfunction()
