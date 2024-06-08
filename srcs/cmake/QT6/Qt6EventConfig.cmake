CMAKE_MINIMUM_REQUIRED( VERSION 3.19 )

set(tools_dir )

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(tools_dir "llvm-mingw_64")
	set(Clang 1)
	set(GNU 0)
	set(MSVC 0)
	SET( builder_tools llvm-mingw_64 )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(tools_dir "mingw_64")
    add_compile_options( -finput-charset=UTF-8  )
    add_compile_options( -fexec-charset=UTF-8  )
	set(Clang 0)
	set(GNU 1)
	set(MSVC 0)
	SET( builder_tools mingw_64 )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(tools_dir "msvc2019_64")
    add_compile_options( /utf-8 )
	set(Clang 0)
	set(GNU 0)
	set(MSVC 1)
	SET( builder_tools msvc2019_64 )
endif()

#set(root_path "C:/Qt")
#if( NOT DEFINED QT_VERSION_MAJOR )
#	SET( QT_VERSION_MAJOR 6 )
#endif()
#
#if( NOT DEFINED QT_VERSION )
#	SET( QT_VERSION ${QT_VERSION_MAJOR}.7.1 )
#endif()
#list(APPEND CMAKE_PREFIX_PATH "${root_path}/${QT_VERSION}/${tools_dir}/")
#

if( NOT DEFINED QT_VERSION_MAJOR )
	SET( QT_VERSION_MAJOR 6 )
endif()

if( NOT DEFINED QT_VERSION )
	SET( QT_VERSION ${QT_VERSION_MAJOR}.7.1 )
endif()

if( NOT DEFINED	QT_QMAKE_EXECUTABLE)
        
    
    SET( DEPLOY_QT_HOME "C:/Qt/${QT_VERSION}/${builder_tools}/" )
    SET( CMAKE_PREFIX_PATH "${DEPLOY_QT_HOME}" )
    
    SET( Qt_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}" )
    SET( Qt6_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}" )
    
    SET( Qt${QT_VERSION_MAJOR}CoreTools_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}CoreTools" )
    SET( Qt${QT_VERSION_MAJOR}GuiTools_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}GuiTools" )
    SET( Qt${QT_VERSION_MAJOR}WidgetsTools_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}WidgetsTools" )
    SET( Qt${QT_VERSION_MAJOR}Widgets_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}Widgets" )
    SET( Qt${QT_VERSION_MAJOR}ZlibPrivate_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}ZlibPrivate" )
    SET( Qt${QT_VERSION_MAJOR}Core_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}Core" )
    
    SET( WINDEPLOYQT_EXECUTABLE "${DEPLOY_QT_HOME}/bin/windeployqt.exe" )
    SET( QT_QMAKE_EXECUTABLE "${DEPLOY_QT_HOME}/bin/qmake.exe" )
    
    
endif()


SET( CMAKE_AUTOUIC ON )
SET( CMAKE_AUTOMOC ON )
SET( CMAKE_AUTORCC ON )

macro( call_qt_deploy PROJECT_NAME )
	target_link_libraries(
		${PROJECT_NAME}
		PRIVATE
		${ARGN}
	)

	set_target_properties( ${PROJECT_NAME} PROPERTIES
		MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
		MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
		MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
		MACOSX_BUNDLE TRUE
		WIN32_EXECUTABLE TRUE
	)

	if( APPLE )
		set( executable_path "$<TARGET_FILE_NAME:${PROJECT_NAME}>.app" )
	else()
		set( executable_path "$<TARGET_FILE_NAME:${PROJECT_NAME}>" )
	endif()

	message( "设置路径: " ${executable_path} )

	set( deploy_script "${CMAKE_HOME_DIRECTORY}/builder/install_cmake_file/deploy_${PROJECT_NAME}.cmake" )

	qt_generate_deploy_script( TARGET ${PROJECT_NAME}
		OUTPUT_SCRIPT deploy_script
		CONTENT "
include(\"${QT_DEPLOY_SUPPORT}\")
qt_deploy_runtime_dependencies(
    EXECUTABLE \"${PROJECT_BINARY_DIR}/${executable_path}\"
	PLUGINS_DIR \".\"
	LIB_DIR \".\"
	BIN_DIR \".\"
)
	" )
	install( TARGETS ${PROJECT_NAME} BUNDLE DESTINATION "${CMAKE_INSTALL_PREFIX}" )
	install( SCRIPT ${deploy_script} )
endmacro()
