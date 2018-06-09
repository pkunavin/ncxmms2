
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

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

target_link_libraries(libtermkey ${NCURSESW_LIBRARIES})
