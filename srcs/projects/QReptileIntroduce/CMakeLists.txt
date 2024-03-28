get_current_dir_name( prject_name ${CMAKE_CURRENT_SOURCE_DIR} )
message( "============ ${prject_name}" )
message( "name =" ${prject_name} )
project( ${prject_name} VERSION 0.1 LANGUAGES CXX )
message( "============ ${CURRENT_FOLDER}" )


# 如果发生错误，请重新配置 qt 6 环境，该文件位于根目录的 /cmake/QT6/Qt6EventConfig.cmake
find_package( Qt6Event REQUIRED PATHS "${CMAKE_HOME_DIRECTORY}/srcs/cmake/QT6/" )

FIND_PACKAGE( Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Gui LinguistTools Network openglwidgets )


qt_standard_project_setup()

SET( code_check_path "${CMAKE_CURRENT_LIST_DIR}/src/codes")
SET( ui_check_path "${CMAKE_CURRENT_LIST_DIR}/src/ui")

SET( cpp_check_path "${code_check_path}/*.cpp")
SET( c_check_path "${code_check_path}/*.c")
SET( hpp_check_path "${code_check_path}/*.hpp")
SET( h_check_path "${code_check_path}/*.h")
SET( ui_check_path "${ui_check_path}/*.ui")
# 当前目录下也有代码
FILE( GLOB SRC_LIST
	"${cpp_check_path}"
	"${c_check_path}"
	"${hpp_check_path}"
	"${h_check_path}"
	"${ui_check_path}"
)

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
		add_executable( ${PROJECT_NAME} ${SRC_LIST} )
	endif()
endif()

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Network
	Qt${QT_VERSION_MAJOR}::OpenGLWidgets
)

set_target_properties( ${PROJECT_NAME} PROPERTIES
	MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE TRUE
)

set( MY_APP ${PROJECT_NAME} )

if( APPLE )
	set( executable_path "$<TARGET_FILE_NAME:${MY_APP}>.app" )
else()
	set( executable_path "\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:${MY_APP}>" )
endif()

set( deploy_script "${CMAKE_HOME_DIRECTORY}/builder/install_cmake_file/deploy_${MY_APP}.cmake" )

file( GENERATE OUTPUT ${deploy_script} CONTENT "
message(\"=================\")
message(\"============	> 执行 : ${executable_path}\")
message(\"=================\")
include(\"${QT_DEPLOY_SUPPORT}\")
qt_deploy_runtime_dependencies(
    EXECUTABLE \"${executable_path}\"
    PLUGINS_DIR \${QT_DEPLOY_BIN_DIR}
)
" )
install( TARGETS ${MY_APP} DESTINATION "${CMAKE_INSTALL_PREFIX}" )
install( SCRIPT ${deploy_script} )
