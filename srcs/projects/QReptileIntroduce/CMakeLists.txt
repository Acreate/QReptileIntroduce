get_current_dir_name( prject_name ${CMAKE_CURRENT_SOURCE_DIR} )
message( "============ ${prject_name}" )
message( "name =" ${prject_name} )
project( ${prject_name} VERSION 0.1 LANGUAGES CXX )
message( "============ ${CURRENT_FOLDER}" )

# 如果发生错误，请重新配置 qt 6 环境，该文件位于根目录的 /cmake/QT6/Qt6EventConfig.cmake
find_package( Qt6Event REQUIRED PATHS "${CMAKE_HOME_DIRECTORY}/srcs/cmake/QT6/" )

FIND_PACKAGE( Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
	Core
	Widgets
	Gui
	LinguistTools
	Network
	openglwidgets
	Core5Compat
	LinguistTools
)

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
## 旧 无法工作
## qt_create_translation( QM_FILES "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ini/progress/translations/" ${PROJECT_NAME}_en.ts ${PROJECT_NAME}_de.ts )


# # 合并源码
list( APPEND ${SRC_LIST} ${SRC_UI} ${QM_FILES} )

if( ${QT_VERSION_MAJOR} GREATER_EQUAL 6 )
	qt_add_executable( ${PROJECT_NAME}
		MANUAL_FINALIZATION
		${SRC_LIST}
	)
else()
	if( ANDROID )
		add_library( ${PROJECT_NAME} SHARED
			${SRC_LIST}
		)
	else()
		add_executable( ${PROJECT_NAME}
			${SRC_LIST}
		)
	endif()
endif()

set(translation_file_list    
	"${CMAKE_CURRENT_LIST_DIR}/translations/${PROJECT_NAME}_en.ts"
    "${CMAKE_CURRENT_LIST_DIR}/translations/${PROJECT_NAME}_zh_CN.ts"
)


# set_source_files_properties(${translation_file_list}
    # PROPERTIES OUTPUT_LOCATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ini/progress/translations/"
# )

qt_add_translations(
    ${PROJECT_NAME}
    TS_FILES
    ${translation_file_list}
	QM_FILES_OUTPUT_VARIABLE qm_files
)
install(FILES ${qm_files} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ini/progress/translations/")

call_qt_deploy( ${PROJECT_NAME}
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Network
	Qt${QT_VERSION_MAJOR}::OpenGLWidgets
	Qt${QT_VERSION_MAJOR}::Core5Compat
)

copy_dir_path( "${CMAKE_CURRENT_LIST_DIR}/res" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/" )
