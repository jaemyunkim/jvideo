
# list sources files
FILE(GLOB GET_OPT_C "./*.c")
FILE(GLOB GET_OPT_H "./*.h")

ADD_LIBRARY(getopt
	${GET_OPT_C}
	${GET_OPT_H}
	)
