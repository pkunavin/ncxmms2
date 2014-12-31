set(SOURCES json.c)
set(HEADERS json.h)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99")

add_library(json-parser ${SOURCES})
SET_TARGET_PROPERTIES(json-parser PROPERTIES PREFIX "")
