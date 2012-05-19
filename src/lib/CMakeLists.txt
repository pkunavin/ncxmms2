
set(SOURCES
    application.cpp
    object.cpp
    window.cpp
    stackedwindow.cpp
    abstractitemview.cpp
    timer.cpp
    painter.cpp
    label.cpp
    lineedit.cpp
    listmodel.cpp
    listview.cpp
    listmodelitemdelegate.cpp)

set(HEADERS
    signals.h
    object.h
    application.h
    window.h
    window_p.h
    exceptions.h
    point.h
    size.h
    stackedwindow.h
    abstractitemview.h
    timer.h
    colors.h
    painter.h
    keyevent.h
    label.h
    lineedit.h
    rectangle.h
    listmodel.h
    listview.h
    listmodelitemdelegate.h
    listitempaintoptions.h)

add_library(ncxmms2-lib ${SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)

pkg_check_modules(NCURSESW ncursesw REQUIRED)
include_directories(${NCURSESW_INCLUDE_DIRS})

pkg_check_modules(GLIB glib-2.0 REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})

target_link_libraries(ncxmms2-lib ${NCURSESW_LIBRARIES}
                                  ${GLIB_LIBRARIES})
