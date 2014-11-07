set(SOURCES json.c)
set(HEADERS json.h)
add_library(json-parser ${SOURCES})
SET_TARGET_PROPERTIES(json-parser PROPERTIES PREFIX "")
