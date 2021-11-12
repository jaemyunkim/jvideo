
add_subdirectory(VideoRecord)
add_subdirectory(MultiVideoCapture)
IF(USE_QT)
	add_subdirectory(VideoRecord_gui)
ENDIF(USE_QT)
