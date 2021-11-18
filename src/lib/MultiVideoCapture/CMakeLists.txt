
# set project
set(PROJ_NAME MultiVideoCapture)

file(GLOB ${PROJ_NAME}_HDR
    *.h
    *.hpp
)
file(GLOB ${PROJ_NAME}_SRC
    *.cpp
)

set(PROJ_FILES ${${PROJ_NAME}_HDR} ${${PROJ_NAME}_SRC})
set(PROJ_LIBS_DEBUG ${Boost_LIBRARIES} ${OpenCV_LIBS})
set(PROJ_LIBS_RELEASE ${Boost_LIBRARIES} ${OpenCV_LIBS})


# set build target ####################################################
set(CMAKE_DEBUG_POSTFIX d)
set_source_files_properties(${PROJ_FILES}
    PROPERTIES
    COMPILE_FLAGS "-D__NO_UI__ -D_CRT_SECURE_NO_WARNINGS")
add_library(${PROJ_NAME} SHARED #dynamic linking library (.so, .dll)
   ${PROJ_FILES}
)
target_compile_definitions(${PROJ_NAME} PRIVATE -DDLL_EXPORTS)  # add preprocessors. DLL_EXPORTS is for dll exporting.
target_link_libraries(${PROJ_NAME}
    debug ${PROJ_LIBS_DEBUG}
    optimized ${PROJ_LIBS_RELEASE}
)


# other settings for visual studio ####################################
if(WIN32)
    if(MSVC)
        # set working directory
        set_target_properties(${PROJ_NAME}
            PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${Configuration}"
        )

        # "Enable C++ Exceptions" - "Yes with SEH Exceptions (/EHa)"
        set(compile_flags /EHa)
        set_target_properties(${PROJ_NAME} 
            PROPERTIES COMPILE_FLAGS ${compile_flags}
        )

        # OpenCV path config in visual studio user file
        set_target_properties(${PROJ_NAME}
            PROPERTIES VS_DEBUGGER_ENVIRONMENT
                "PATH=\
${_OpenCV_LIB_PATH};\
$<$<CONFIG:Debug>:${_OpenCV_LIB_PATH}${OpenCV_LIB_DIR_DBG};>$<$<NOT:$<CONFIG:Debug>>:${_OpenCV_LIB_PATH}${OpenCV_LIB_DIR_OPT};>\
%PATH%"
        )
    endif(MSVC)
endif(WIN32)


# install output files ################################################
# set default install prefix
set(CMAKE_INSTALL_PREFIX "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/install" CACHE PATH "Installation Directory" FORCE)

# copy binaries
install(TARGETS     ${PROJ_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJ_NAME}/
)

# copy headers for dll exports
install(FILES       FrameType.hpp
                    MultiVideoCapture.hpp
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJ_NAME}/include/
)

# get opencv dlls
if(WIN32)
    if(NOT DEFINED __opencv_dll_dbg)
        get_target_property(__opencv_dll_dbg opencv_world IMPORTED_LOCATION_DEBUG)
    endif()
    if(NOT DEFINED __opencv_dll_release)
        get_target_property(__opencv_dll_release opencv_world IMPORTED_LOCATION_RELEASE)
    endif()
endif()

# copy dlls
install(FILES       $<$<CONFIG:Debug>:${__opencv_dll_dbg}>  # opencv dlls
                    $<$<NOT:$<CONFIG:Debug>>:${__opencv_dll_release}>   # opencv dlls
                    $<$<CONFIG:Debug>:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/install/MultiVideoCapture/MultiVideoCaptured.lib>
                    $<$<NOT:$<CONFIG:Debug>>:${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/install/MultiVideoCapture/MultiVideoCapture.lib>
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJ_NAME}/
)
