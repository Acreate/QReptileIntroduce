CMAKE_MINIMUM_REQUIRED( VERSION 3.19 )

if(NOT DEFINED QT_VERSION_MAJOR)
	SET( QT_VERSION_MAJOR 6 )
endif()

if(NOT DEFINED QT_VERSION)
	SET( QT_VERSION 6.6.2 )
endif()


if(NOT DEFINED builder_tools)
	SET( builder_tools msvc2019_64 )
endif()

SET( DEPLOY_QT_HOME "C:/Qt/${QT_VERSION}/${builder_tools}/" )
SET( CMAKE_PREFIX_PATH "${DEPLOY_QT_HOME}" )

SET( Qt_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}" )
SET( Qt6_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}" )

SET( Qt${QT_VERSION_MAJOR}CoreTools_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}CoreTools")
SET( Qt${QT_VERSION_MAJOR}GuiTools_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}GuiTools" )
SET( Qt${QT_VERSION_MAJOR}WidgetsTools_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}WidgetsTools" )
SET( Qt${QT_VERSION_MAJOR}Widgets_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}Widgets" )
SET( Qt${QT_VERSION_MAJOR}ZlibPrivate_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}ZlibPrivate" )
SET( Qt${QT_VERSION_MAJOR}Core_DIR "${DEPLOY_QT_HOME}/lib/cmake/Qt${QT_VERSION_MAJOR}Core" )

SET( WINDEPLOYQT_EXECUTABLE "${DEPLOY_QT_HOME}/bin/windeployqt.exe" )
SET( QT_QMAKE_EXECUTABLE "${DEPLOY_QT_HOME}/bin/qmake.exe" )

SET( CMAKE_AUTOUIC ON )
SET( CMAKE_AUTOMOC ON )
SET( CMAKE_AUTORCC ON )


function(call_qt_deploy PROJECT_NAME )
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
		set( executable_path "\${QT_DEPLOY_BIN_DIR}/$<TARGET_FILE_NAME:${PROJECT_NAME}>" )
	endif()

	set( deploy_script "${CMAKE_HOME_DIRECTORY}/builder/install_cmake_file/deploy_${PROJECT_NAME}.cmake" )

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
	install( TARGETS ${PROJECT_NAME} DESTINATION "${CMAKE_INSTALL_PREFIX}" )
	install( SCRIPT ${deploy_script} )
endfunction()

