# Variables setting for this project ####################################
# set variable for this project
SET(PROJ_NAME VideoRecord)

file (GLOB ${PROJ_NAME}_HDR "*.h*")
file (GLOB ${PROJ_NAME}_SRC "*.c*")

SET(PROJ_FILES ${${PROJ_NAME}_HDR} ${${PROJ_NAME}_SRC})
SET(PROJ_LIBS ${Boost_LIBRARIES} ${OpenCV_LIBS} getopt)
IF(WIN32)
	list(APPEND PROJ_LIBS OpenCVDeviceEnumerator)
ENDIF(WIN32)

# include directories other libraries
INCLUDE_DIRECTORIES(
	../../lib/getopt
    )
IF(WIN32)
	INCLUDE_DIRECTORIES(../../lib/OpenCVDeviceEnumerator)
ENDIF(WIN32)


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
			file(APPEND ${CMAKE_CURRENT_BINARY_DIR}/${PROJ_NAME}.vcxproj.user "<LocalDebuggerEnvironment>PATH=${OpenCV_CONFIG_PATH}/../bin;${_qt5Widgets_install_prefix}/bin;%PATH%</LocalDebuggerEnvironment>\n")
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
TARGET_LINK_LIBRARIES(${PROJ_NAME} ${PROJ_LIBS})
