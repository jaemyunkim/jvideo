# Variables setting for this project ####################################
# set variable for this project
SET(PARENT_PROJ_NAME VideoRecord)
SET(PROJ_NAME ${PARENT_PROJ_NAME}_gui)

file(GLOB ${PROJ_NAME}_HDR "*.h*")
file(GLOB ${PROJ_NAME}_SRC "*.c*")
file(GLOB ${PROJ_NAME}_QT_UI "*.ui")
file(GLOB ${PARENT_PROJ_NAME}_HDR "../${PARENT_PROJ_NAME}/*.h*")
file(GLOB ${PARENT_PROJ_NAME}_SRC "../${PARENT_PROJ_NAME}/*.c*")
list(REMOVE_ITEM ${PARENT_PROJ_NAME}_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../${PARENT_PROJ_NAME}/main.cpp")


# Detect QT ##################################################
FIND_PACKAGE(Qt5Widgets CONFIG REQUIRED Concurrent)

IF(Qt5Widgets_FOUND)
	MESSAGE(STATUS "Qt5Widgets version: " ${Qt5Widgets_VERSION})
	INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
	# MESSAGE(STATUS "  *1*CMAKE_CURRENT_LIST_DIR : " ${imported_implib})
	# MESSAGE(STATUS "  *2*CMAKE_CURRENT_LIST_DIR : " ${Qt5Widgets_LIBRARIES})

	# # Find includes in corresponding build directories
	# set(CMAKE_INCLUDE_CURRENT_DIR ON)
	# # Instruct CMake to run moc automatically when needed
	# set(CMAKE_AUTOMOC ON)
	# # Create code from a list of Qt designer ui files
	# set(CMAKE_AUTOUIC ON)
ENDIF(Qt5Widgets_FOUND)

#find_package(Qt5Concurrent 5.0.0 REQUIRED)
FIND_PACKAGE(Qt5Concurrent REQUIRED PATHS ${Qt5Widgets_DIR}/../Qt5Concurrent)
IF(Qt5Concurrent_FOUND)
	MESSAGE(STATUS "Qt5Concurrent version: " ${Qt5Concurrent_VERSION})
ENDIF(Qt5Concurrent_FOUND)

# Detect QT ##################################################


SET(PROJ_FILES ${${PROJ_NAME}_HDR} ${${PROJ_NAME}_SRC} ${${PROJ_NAME}_QT_UI}
	${${PARENT_PROJ_NAME}_HDR} ${${PARENT_PROJ_NAME}_SRC})
SET(PROJ_LIBS ${Boost_LIBRARIES} ${OpenCV_LIBS} ${Qt5Widgets_LIBRARIES} getopt)
IF(WIN32)
	list(APPEND PROJ_LIBS OpenCVDeviceEnumerator)
ENDIF(WIN32)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# qt5_wrap_cpp(${PROJ_NAME}_hdr_moc ${${PROJ_NAME}_HDR})
# qt5_wrap_ui(${PROJ_NAME}_ui_moc ${${PROJ_NAME}_QT_UI})

# include directories other libraries
INCLUDE_DIRECTORIES(
	../${PARENT_PROJ_NAME}
	../../lib/getopt
    ${Qt5Concurrent_INCLUDE_DIRS}
    )
IF(WIN32)
	INCLUDE_DIRECTORIES(../../lib/OpenCVDeviceEnumerator)
ENDIF(WIN32)

add_definitions(
    ${Qt5Concurrent_DEFINITIONS}
    )

# OpenCV path config visual studio user file #############################################################
if(WIN32)
	if(NOT _OpenCV_LIB_PATH)
		if(NOT OpenCV_LIB_PATH)
			if(OpenCV_STATIC AND EXISTS "${OpenCV_CONFIG_PATH}/${OpenCV_ARCH}/${OpenCV_RUNTIME}/staticlib")
				if(OpenCV_CUDA AND EXISTS "${OpenCV_CONFIG_PATH}/gpu/${OpenCV_ARCH}/${OpenCV_RUNTIME}/staticlib/OpenCVConfig.cmake")
					set(OpenCV_LIB_PATH "${OpenCV_CONFIG_PATH}/gpu/${OpenCV_ARCH}/${OpenCV_RUNTIME}/staticlib")
				else()
					set(OpenCV_LIB_PATH "${OpenCV_CONFIG_PATH}/${OpenCV_ARCH}/${OpenCV_RUNTIME}/staticlib")
				endif()
			elseif(EXISTS "${OpenCV_CONFIG_PATH}/${OpenCV_ARCH}/${OpenCV_RUNTIME}/lib")
				if(OpenCV_CUDA AND EXISTS "${OpenCV_CONFIG_PATH}/gpu/${OpenCV_ARCH}/${OpenCV_RUNTIME}/lib/OpenCVConfig.cmake")
					set(OpenCV_LIB_PATH "${OpenCV_CONFIG_PATH}/gpu/${OpenCV_ARCH}/${OpenCV_RUNTIME}/lib")
				else()
					set(OpenCV_LIB_PATH "${OpenCV_CONFIG_PATH}/${OpenCV_ARCH}/${OpenCV_RUNTIME}/lib")
				endif()
			endif()
		endif()

		#message(STATUS "Found OpenCV ${OpenCV_VERSION} in ${OpenCV_LIB_PATH}")
		if(NOT OpenCV_LIB_PATH MATCHES "/staticlib")
			get_filename_component(_OpenCV_LIB_PATH "${OpenCV_LIB_PATH}/../bin" ABSOLUTE)
			file(TO_NATIVE_PATH "${_OpenCV_LIB_PATH}" _OpenCV_LIB_PATH)
			#message(STATUS "You might need to add ${_OpenCV_LIB_PATH} to your PATH to be able to run your applications.")
			if(OpenCV_LIB_PATH MATCHES "/gpu/")
				string(REPLACE "\\gpu" "" _OpenCV_LIB_PATH2 "${_OpenCV_LIB_PATH}")
				#message(STATUS "GPU support is enabled so you might also need ${_OpenCV_LIB_PATH2} in your PATH (it must go after the ${_OpenCV_LIB_PATH}).")
			endif()
		endif()
	endif()

	#MESSAGE(STATUS "*** win32 | " ${_OpenCV_LIB_PATH})

#	if(MSVC10)
	if(MSVC)
		if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
			set(USER_PLATFORM x64)
		else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
			set(USER_PLATFORM Win32)
		endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
		file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.vcxproj.user "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
		file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.vcxproj.user "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")

		#MESSAGE(STATUS "*** win32 | " ${_OpenCV_LIB_PATH})
		#MESSAGE(STATUS "*** win32 | " ${_OpenCV_LIB_PATH}${OpenCV_LIB_DIR_DBG})
		#MESSAGE(STATUS "*** win32 | " ${_OpenCV_LIB_PATH}${OpenCV_LIB_DIR_OPT})

		foreach(f ${CMAKE_CONFIGURATION_TYPES})
			file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.vcxproj.user "<PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='${f}|${USER_PLATFORM}'\">\n")
			file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.vcxproj.user "<LocalDebuggerEnvironment>PATH=${_OpenCV_LIB_PATH};${_OpenCV_LIB_PATH}${OpenCV_LIB_DIR_DBG};${_OpenCV_LIB_PATH}${OpenCV_LIB_DIR_OPT};${_qt5Widgets_install_prefix}/bin;%PATH%</LocalDebuggerEnvironment>\n")
			file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.vcxproj.user "<DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>\n")
			file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.vcxproj.user "<LocalDebuggerWorkingDirectory>${CMAKE_BINARY_DIR}/bin</LocalDebuggerWorkingDirectory>\n")
			file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.vcxproj.user "</PropertyGroup>\n")
		endforeach(f)

		file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.vcxproj.user "</Project>")
#	endif(MSVC10)
	endif(MSVC)
endif(WIN32)


# set build target #############################################
SET_SOURCE_FILES_PROPERTIES(${PROJ_FILES}
	PROPERTIES
	COMPILE_FLAGS "-D__NO_UI__ -D_CRT_SECURE_NO_WARNINGS")
ADD_EXECUTABLE(${PROJ_NAME} ${PROJ_FILES})
# ADD_EXECUTABLE(${PROJ_NAME} WIN32 ${PROJ_FILES})
TARGET_LINK_LIBRARIES(${PROJ_NAME} ${PROJ_LIBS} ${Qt5Concurrent_LIBRARIES})

# Use the Widgets module from Qt 5.
# qt5_use_modules(helloworld QtConcurrent)