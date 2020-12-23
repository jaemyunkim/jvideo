# CMake powered build system for jVideo
# Create by Jaemyun Kim
# First draft is 29 Oct. 2018

CMAKE_MINIMUM_REQUIRED (VERSION 3.1.0)

PROJECT (jVideo VERSION 0.0.1)

# set output path both execution and library files.
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Detect Boost ##################################################
IF(WIN32 OR MINGW)
	set(Boost_INCLUDE_DIR CACHE PATH "$ENV{Boost_DIR}")

	IF(NOT Boost_INCLUDE_DIR)
		MESSAGE(FATAL_ERROR "Please set Boost_INCLUDE_DIR in Advanced.")
	ELSE(NOT Boost_INCLUDE_DIR)
		set(CMAKE_INCLUDE_CURRENT_DIR ON)
		set(Boost_DIR ${Boost_INCLUDE_DIR})
	ENDIF(NOT Boost_INCLUDE_DIR)
	
	#set(Boost_INCLUDE_DIR ${Boost_DIR})
	#set(CMAKE_PREFIX_PATH ${Boost_DIR})
	
	set(Boost_USE_STATIC_LIBS		ON)
	set(Boost_USE_MULTITHREADED		ON)
	set(Boost_USE_STATIC_RUNTIME	OFF)
	#FIND_PACKAGE(Boost 1.47.0 COMPONENTS ${BOOST_COMPONENTS_NEEDED})\
ELSE(WIN32 OR MINGW)
ENDIF(WIN32 OR MINGW)

FIND_PACKAGE(Boost 1.54 REQUIRED COMPONENTS filesystem system chrono date_time)
IF(Boost_FOUND)
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)


# Detect OpenCV ##################################################
#IF(WIN32)
#	SET(OpenCV_STATIC ON)
#ELSE(WIN32)
#	SET(OpenCV_STATIC OFF)
#ENDIF(WIN32)
FIND_PACKAGE(OpenCV REQUIRED COMPONENTS core imgproc highgui)
IF(OpenCV_FOUND)
	MESSAGE(STATUS "OpenCV version: " ${OpenCV_VERSION})
	MESSAGE(STATUS "Found the following OpenCV libraries: \n  core\n  imgproc\n  highgui")
	IF(OpenCV_VERSION_MAJOR EQUAL 3)
		FIND_PACKAGE(OpenCV REQUIRED COMPONENTS imgcodecs)
		MESSAGE(STATUS "  imgcodecs")
	ENDIF(OpenCV_VERSION_MAJOR EQUAL 3)
	INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})

	IF(WIN32)
		IF(MSVC)
			IF(NOT DEFINED OpenCV_LIB_DIR_DBG)
				SET(OpenCV_LIB_DIR_DBG /Debug)
			ENDIF(NOT DEFINED OpenCV_LIB_DIR_DBG)
			IF(NOT DEFINED OpenCV_LIB_DIR_OPT)
				SET(OpenCV_LIB_DIR_OPT /Release)
			ENDIF(NOT DEFINED OpenCV_LIB_DIR_OPT)
		ENDIF(MSVC)
	ENDIF(WIN32)

ENDIF(OpenCV_FOUND)


# Detect QT ##################################################
OPTION (USE_QT "Use QT for GUI" ON)
IF(USE_QT)
	if(APPLE AND EXISTS /usr/local/opt/qt5)
		# Homebrew installs Qt5 (up to at least 5.9.1) in
		# /usr/local/qt5, ensure it can be found by CMake since
		# it is not in the default /usr/local prefix.
		list(APPEND CMAKE_PREFIX_PATH "/usr/local/opt/qt5")
	endif()

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
ENDIF(USE_QT)


# Detect OpenMP ##################################################
#OPTION (USE_OpenMP "Use OpenMP" ON)
#IF(USE_OpenMP)
	FIND_PACKAGE(OpenMP)
	IF(OPENMP_FOUND)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
		option(USE_OPENMP "Use OpenMP for parallelization" ON)
	ELSE(OPENMP_FOUND)
		option(USE_OPENMP "Use OpenMP for parallelization" OFF)
	ENDIF(OPENMP_FOUND)
#ENDIF(USE_OpenMP)


# prevent warnings on visual studio.
if(WIN32)
	if(MSVC)
		# for _CRT_SECURE_NO_WARNINGS
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)
		
		# for _SCL_SECURE_NO_WARNINGS warning
		add_definitions(-D_SCL_SECURE_NO_WARNINGS)

		# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
		add_definitions(${Qt5Widgets_DEFINITIONS})

		# enable multi-processor compilation
		add_definitions(/MP)
	endif(MSVC)
endif(WIN32)


# use c++11 version
set(CMAKE_CXX_STANDARD 11) # C++11...
set(CMAKE_CXX_STANDARD_REQUIRED ON) #...is required...
set(CMAKE_CXX_EXTENSIONS OFF) #...without compiler extensions like gnu++11


# include sub-directories. Target directories have to have "CMakeLists.txt" file.
ADD_SUBDIRECTORY(src)


SET(CMAKE_VERBOSE_MAKEFILE ON)


## set compile environment
#include(CheckCXXCompilerFlag)
#CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
#CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
#if(COMPILER_SUPPORTS_CXX11)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#elseif(COMPILER_SUPPORTS_CXX0X)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
#else()
#   message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
#endif()


# # copy running script files.
# SET(SOURCE_PATH bin)
# FILE(GLOB_RECURSE RUNNING_SCRIPTS ${SOURCE_PATH}/*)
# MESSAGE(STATUS "Check running script files:")
# FOREACH(ORIGIN_FILE ${RUNNING_SCRIPTS})
# 	#MESSAGE(STATUS "  current_file : " ${ORIGIN_FILE})
# 	FILE(RELATIVE_PATH EACH_FILE ${CMAKE_SOURCE_DIR} ${ORIGIN_FILE})
# 	#MESSAGE(STATUS "  abs_path script file1 : " ${ORIGIN_FILE})
# 	#MESSAGE(STATUS "  rel_path script file2 : " ${EACH_FILE})
# 	SET(DESTINATION_FILE ${CMAKE_CURRENT_BINARY_DIR}/${EACH_FILE})
# 	GET_FILENAME_COMPONENT(DESTINATION_PATH ${DESTINATION_FILE} DIRECTORY)
# 	#MESSAGE(STATUS "  destinat script file3 : " ${DESTINATION_FILE})
# 	IF(NOT EXISTS ${DESTINATION_FILE})
# 		#MESSAGE(STATUS "  " ${DESTINATION_PATH})
# 		FILE(COPY ${ORIGIN_FILE} DESTINATION ${DESTINATION_PATH})
# 		#MESSAGE(STATUS "  " ${DESTINATION_FILE})
# 		IF(NOT IS_DIRECTORY ${ORIGIN_FILE})
# 			MESSAGE(STATUS "  copy script file: " ${DESTINATION_FILE})
# 			#MESSAGE(STATUS "  copy script file: " ${DESTINATION_PATH})
# 		ENDIF(NOT IS_DIRECTORY ${ORIGIN_FILE})
# 	ENDIF(NOT EXISTS ${DESTINATION_FILE})
# ENDFOREACH(ORIGIN_FILE)

# # copy configuration files.
# SET(SOURCE_PATH conf)
# FILE(GLOB_RECURSE RUNNING_SCRIPTS ${SOURCE_PATH}/*)
# MESSAGE(STATUS "Check running configuration files:")
# FOREACH(ORIGIN_FILE ${RUNNING_SCRIPTS})
# 	#MESSAGE(STATUS "  current_file : " ${ORIGIN_FILE})
# 	FILE(RELATIVE_PATH EACH_FILE ${CMAKE_SOURCE_DIR} ${ORIGIN_FILE})
# 	#MESSAGE(STATUS "  abs_path conf file1 : " ${ORIGIN_FILE})
# 	#MESSAGE(STATUS "  rel_path conf file2 : " ${EACH_FILE})
# 	SET(DESTINATION_FILE ${CMAKE_CURRENT_BINARY_DIR}/${EACH_FILE})
# 	GET_FILENAME_COMPONENT(DESTINATION_PATH ${DESTINATION_FILE} DIRECTORY)
# 	#MESSAGE(STATUS "  destinat conf file3 : " ${DESTINATION_FILE})
# 	IF(NOT EXISTS ${DESTINATION_FILE})
# 		#MESSAGE(STATUS "  " ${DESTINATION_PATH})
# 		FILE(COPY ${ORIGIN_FILE} DESTINATION ${DESTINATION_PATH})
# 		#MESSAGE(STATUS "  " ${DESTINATION_FILE})
# 		IF(NOT IS_DIRECTORY ${ORIGIN_FILE})
# 			MESSAGE(STATUS "  copy conf file: " ${DESTINATION_FILE})
# 			#MESSAGE(STATUS "  copy conf file: " ${DESTINATION_PATH})
# 		ENDIF(NOT IS_DIRECTORY ${ORIGIN_FILE})
# 	ENDIF(NOT EXISTS ${DESTINATION_FILE})
# ENDFOREACH(ORIGIN_FILE)
