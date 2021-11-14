
# set project
set(PROJ_NAME MultiVideoCapture)

file(GLOB ${PROJ_NAME}_HDR
    *.h
    *.hpp
)
file(GLOB ${PROJ_NAME}_SRC
    *.cpp
)
list(REMOVE_ITEM ${PROJ_NAME}_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)
file(GLOB ${PROJ_NAME}_TESTER_HDR
    MultiVideoCapture.hpp
    FrameType.hpp
)
file(GLOB ${PROJ_NAME}_TESTER_SRC
    main.cpp
)

set(PROJ_FILES ${${PROJ_NAME}_HDR} ${${PROJ_NAME}_SRC})
set(PROJ_TESTER_FILES ${${PROJ_NAME}_TESTER_HDR} ${${PROJ_NAME}_TESTER_SRC})
set(PROJ_LIBS_DEBUG ${Boost_LIBRARIES} ${OpenCV_LIBS})
set(PROJ_LIBS_RELEASE ${Boost_LIBRARIES} ${OpenCV_LIBS})


# set build target ####################################################
set_source_files_properties(${PROJ_FILES}
    PROPERTIES
    COMPILE_FLAGS "-D__NO_UI__ -D_CRT_SECURE_NO_WARNINGS")
add_library(${PROJ_NAME} SHARED #dynamic linking library (.so, .dll)
   ${PROJ_FILES}
)
target_compile_definitions(${PROJ_NAME} PUBLIC -DDLL_EXPORTS)  # add preprocessors. DLL_EXPORTS is for dll exporting.
target_link_libraries(${PROJ_NAME}
    debug ${PROJ_LIBS_DEBUG} 
    optimized ${PROJ_LIBS_RELEASE}
)

set_source_files_properties(${PROJ_TESTER_FILES}
    PROPERTIES
    COMPILE_FLAGS "-D__NO_UI__ -D_CRT_SECURE_NO_WARNINGS")
add_executable(${PROJ_NAME}_test ${PROJ_TESTER_FILES})
target_link_libraries(${PROJ_NAME}_test
    debug ${PROJ_LIBS_DEBUG} MultiVideoCapture 
    optimized ${PROJ_LIBS_RELEASE} MultiVideoCapture
)


# other settings for visual studio ####################################
if(WIN32)
    if(MSVC)
        # set working directory
        set_target_properties(${PROJ_NAME}
            PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${Configuration}"
        )

        # "Enable C++ Exceptions" - "Yes with SEH Exceptions (/EHa)"
        # set(compile_flags /EHa)
        # set_target_properties(${PROJ_NAME} 
        #     PROPERTIES COMPILE_FLAGS ${compile_flags}
        # )

        # OpenCV path config in visual studio user file
        set_target_properties(${PROJ_NAME}
            PROPERTIES VS_DEBUGGER_ENVIRONMENT
                "PATH=\
${_OpenCV_LIB_PATH};\
$<$<CONFIG:Debug>:${_OpenCV_LIB_PATH}${OpenCV_LIB_DIR_DBG};>$<$<NOT:$<CONFIG:Debug>>:${_OpenCV_LIB_PATH}${OpenCV_LIB_DIR_OPT};>\
%PATH%"
        )
        set_target_properties(${PROJ_NAME}_test
            PROPERTIES VS_DEBUGGER_ENVIRONMENT
                "PATH=\
${_OpenCV_LIB_PATH};\
$<$<CONFIG:Debug>:${_OpenCV_LIB_PATH}${OpenCV_LIB_DIR_DBG};>$<$<NOT:$<CONFIG:Debug>>:${_OpenCV_LIB_PATH}${OpenCV_LIB_DIR_OPT};>\
%PATH%"
        )

        # get opencv dlls
        get_target_property(__opencv_dll_dbg opencv_world IMPORTED_LOCATION_DEBUG)
        get_target_property(__opencv_dll_release opencv_world IMPORTED__LOCATION_RELEASE)
    endif(MSVC)
endif(WIN32)


# install output files ################################################
# set default install prefix
set(CMAKE_INSTALL_PREFIX "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/install" CACHE PATH "Installation Directory" FORCE)

install(TARGETS     ${PROJ_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJNAME}/
)

if(WIN32)
    if(MSVC)
        # copy dlls
        install(FILES       ${__opencv_dll_release} # opencv dlls
                DESTINATION ${CMAKE_INSTALL_PREFIX}/${PROJ_NAME}/
        )
    endif(MSVC)
endif(WIN32)
