add_subdirectory(lib)

set(SOURCES
    main.cpp
    utils.cpp
    xmmsutils.cpp
    settings.cpp
    commandlineoptions.cpp
    song.cpp
    log.cpp

    mainwindow/mainwindow.cpp

    statusarea/playbackstatuswindow.cpp
    statusarea/statusarea.cpp
    statusarea/playbackprogressbar.cpp
    statusarea/questionwindow.cpp

    localfilesystembrowser/localfilesystembrowser.cpp
    localfilesystembrowser/filesystemmodel.cpp
    localfilesystembrowser/filesystemitemdelegate.cpp

    headerwindow/headerwindow.cpp

    playlistsbrowser/playlistsbrowser.cpp
    playlistsbrowser/playlistslistmodel.cpp
    playlistsbrowser/playlistslistview.cpp

    activeplaylistwindow/activeplaylistwindow.cpp

    helpbrowser/helpbrowser.cpp

    playlistview/playlistview.cpp
    playlistview/playlistmodel.cpp
    playlistview/playlistitemdelegate.cpp

    medialibbrowser/medialibbrowser.cpp
    medialibbrowser/artistslistmodel.cpp
    medialibbrowser/albumslistmodel.cpp
    medialibbrowser/songslistmodel.cpp)

set(HEADERS
    utils.h
    xmmsutils.h
    settings.h
    commandlineoptions.h
    song.h
    hotkeys.h
    log.h

    mainwindow/mainwindow.h

    statusarea/playbackstatuswindow.h
    statusarea/statusarea.h
    statusarea/playbackprogressbar.h
    statusarea/questionwindow.h

    localfilesystembrowser/localfilesystembrowser.h
    localfilesystembrowser/filesystemmodel.h
    localfilesystembrowser/filesystemitemdelegate.h

    headerwindow/headerwindow.h

    playlistsbrowser/playlistsbrowser.h
    playlistsbrowser/playlistslistmodel.h
    playlistsbrowser/playlistslistview.h

    activeplaylistwindow/activeplaylistwindow.h

    helpbrowser/helpbrowser.h

    playlistview/playlistview.h
    playlistview/playlistmodel.h
    playlistview/playlistitemdelegate.h

    medialibbrowser/medialibbrowser.h
    medialibbrowser/artistslistmodel.h
    medialibbrowser/albumslistmodel.h
    medialibbrowser/songslistmodel.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR}/src)

add_executable(ncxmms2 ${SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra")

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB glib-2.0 REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})

pkg_check_modules(XMMS2_CPP xmms2-client-cpp REQUIRED)
include_directories(${XMMS2_CPP_INCLUDE_DIRS})

pkg_check_modules(XMMS2_CPP_GLIB xmms2-client-cpp-glib REQUIRED)
include_directories(${XMMS2_CPP_GLIB_INCLUDE_DIRS})

target_link_libraries(ncxmms2 ncxmms2-lib
                              ${GLIB_LIBRARIES}
                              ${XMMS2_CPP_LIBRARIES}
                              ${XMMS2_CPP_GLIB_LIBRARIES})

install (TARGETS ncxmms2 DESTINATION bin)

