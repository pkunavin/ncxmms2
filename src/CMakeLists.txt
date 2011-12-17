
set(SOURCES
    main.cpp
    mainwindow.cpp
    playbackstatuswindow.cpp
    utils.cpp
    xmms2client.cpp
    playlistwindow.cpp
    localfilesystembrowser.cpp
    headerwindow.cpp
    settings.cpp
    commandlineoptions.cpp
    song.cpp
    playlistsbrowser.cpp
    playlistslistview.cpp
    playlistviewer.cpp
    statusarea.cpp
    playbackprogressbar.cpp
    questionwindow.cpp)

set(HEADERS
    mainwindow.h
    utils.h
    playbackstatuswindow.h
    xmms2client.h
    playlistwindow.h
    localfilesystembrowser.h
    headerwindow.h
    hotkeys.h
    settings.h
    commandlineoptions.h
    song.h
    playlistsbrowser.h
    playlistslistview.h
    playlistviewer.h
    statusarea.h
    playbackprogressbar.h
    questionwindow.)

add_executable(ncxmms2 ${SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB glib-2.0 REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})

pkg_check_modules(XMMS2_CPP xmms2-client-cpp REQUIRED)
include_directories(${XMMS2_CPP_INCLUDE_DIRS})

pkg_check_modules(XMMS2_CPP_GLIB xmms2-client-cpp-glib REQUIRED)
include_directories(${XMMS2_CPP_GLIB_INCLUDE_DIRS})

add_subdirectory(lib)

target_link_libraries(ncxmms2 ncxmms2-lib
                              ${GLIB_LIBRARIES}
                              ${XMMS2_CPP_LIBRARIES}
                              ${XMMS2_CPP_GLIB_LIBRARIES})

install (TARGETS ncxmms2 DESTINATION bin)
