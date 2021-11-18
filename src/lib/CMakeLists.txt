
# Paths which including "CMakeList.txt" file are added for this project.
add_subdirectory(getopt)
add_subdirectory(MultiVideoCapture)
IF(WIN32)
	add_subdirectory(OpenCVDeviceEnumerator)
ENDIF(WIN32)