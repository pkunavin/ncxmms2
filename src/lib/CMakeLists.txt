
set(SOURCES
    application.cpp
    window.cpp
    stackedwindow.cpp
    abstractitemview.cpp
    timer.cpp
    painter.cpp
    label.cpp
    lineedit.cpp)

set(HEADERS
    application.h
    window.h
    window_p.h
    size.h
    stackedwindow.h
    abstractitemview.h
    timer.h
    colors.h
    painter.h
    keyevent.h
    label.h
    lineedit.h)

add_library(ncxmms2-lib ${SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall")

find_package(Curses REQUIRED)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB glib-2.0 REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})

target_link_libraries(ncxmms2-lib ${CURSES_LIBRARIES}
                                  ${GLIB_LIBRARIES})
