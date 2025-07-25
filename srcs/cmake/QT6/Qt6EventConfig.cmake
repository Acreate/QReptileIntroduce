﻿CMAKE_MINIMUM_REQUIRED( VERSION 3.19 )

SET( tools_dir )

IF( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
	SET( tools_dir "llvm-mingw_64" )
	SET( Clang 1 )
	SET( GNU 0 )
	SET( MSVC 0 )
	SET( builder_tools llvm-mingw_64 )
ELSEIF( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )
	SET( tools_dir "mingw_64" )
	ADD_COMPILE_OPTIONS( -finput-charset=UTF-8 )
	ADD_COMPILE_OPTIONS( -fexec-charset=UTF-8 )
	SET( Clang 0 )
	SET( GNU 1 )
	SET( MSVC 0 )
	SET( builder_tools mingw_64 )
ELSEIF( CMAKE_CXX_COMPILER_ID STREQUAL "Intel" )
ELSEIF( CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )
	SET( builder_tools msvc2022_64 )
	SET( tools_dir "${builder_tools}" )
	ADD_COMPILE_OPTIONS( /utf-8 )
	SET( Clang 0 )
	SET( GNU 0 )
	SET( MSVC 1 )
ENDIF()

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

IF( NOT DEFINED QT_VERSION_MAJOR )
	SET( QT_VERSION_MAJOR 6 )
ENDIF()

IF( NOT DEFINED QT_VERSION )
	SET( QT_VERSION ${QT_VERSION_MAJOR}.8.2 )
ENDIF()

IF( NOT DEFINED QT_QMAKE_EXECUTABLE )
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
ENDIF()


SET( CMAKE_AUTOUIC ON )
SET( CMAKE_AUTOMOC ON )
SET( CMAKE_AUTORCC ON )

MACRO( call_qt_deploy PROJECT_NAME )
	TARGET_LINK_LIBRARIES(
			${PROJECT_NAME}
			PRIVATE
			${ARGN}
	)
	
	SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES
			MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
			MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
			MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
			MACOSX_BUNDLE TRUE
			WIN32_EXECUTABLE TRUE
	)
	
	IF( APPLE )
		SET( executable_path "$<TARGET_FILE_NAME:${PROJECT_NAME}>.app" )
	ELSE()
		SET( executable_path "$<TARGET_FILE_NAME:${PROJECT_NAME}>" )
	ENDIF()
	
	MESSAGE( "设置路径: " ${executable_path} )
	
	SET( deploy_script "${CMAKE_HOME_DIRECTORY}/builder/install_cmake_file/deploy_${PROJECT_NAME}.cmake" )
	
	QT_GENERATE_DEPLOY_SCRIPT( TARGET ${PROJECT_NAME}
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
	INSTALL( TARGETS ${PROJECT_NAME} BUNDLE DESTINATION "${CMAKE_INSTALL_PREFIX}" )
	INSTALL( SCRIPT ${deploy_script} )
ENDMACRO()
