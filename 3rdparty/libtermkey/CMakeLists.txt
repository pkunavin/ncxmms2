
set(SOURCES
    driver-csi.c
    driver-ti.c
    termkey.c)

set(HEADERS
    termkey.h
    termkey-internal.h)

add_library(libtermkey ${SOURCES})
SET_TARGET_PROPERTIES(libtermkey PROPERTIES PREFIX "")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99")

find_package(PkgConfig REQUIRED)

pkg_check_modules(NCURSESW ncursesw REQUIRED)
include_directories(${NCURSESW_INCLUDE_DIRS})

target_link_libraries(libtermkey ${NCURSESW_LIBRARIES})
