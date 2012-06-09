add_subdirectory(lib)

set(SOURCES
    main.cpp
    utils.cpp
    xmms2client.cpp
    settings.cpp
    commandlineoptions.cpp
    song.cpp

    mainwindow/mainwindow.cpp

    statusarea/playbackstatuswindow.cpp
    statusarea/statusarea.cpp
    statusarea/playbackprogressbar.cpp
    statusarea/questionwindow.cpp

    localfilesystembrowser/localfilesystembrowser.cpp

    headerwindow/headerwindow.cpp

    playlistsbrowser/playlistsbrowser.cpp
    playlistsbrowser/playlistslistmodel.cpp
    playlistsbrowser/playlistslistview.cpp

    activeplaylistwindow/activeplaylistwindow.cpp

    playlistview/playlistview.cpp
    playlistview/playlistmodel.cpp
    playlistview/playlistitemdelegate.cpp

    medialibbrowser/medialibbrowser.cpp
    medialibbrowser/artistslistmodel.cpp
    medialibbrowser/albumslistmodel.cpp
    medialibbrowser/songslistmodel.cpp)

set(HEADERS
    utils.h
    xmms2client.h
    settings.h
    commandlineoptions.h
    song.h
    hotkeys.h

    mainwindow/mainwindow.h

    statusarea/playbackstatuswindow.h
    statusarea/statusarea.h
    statusarea/playbackprogressbar.h
    statusarea/questionwindow.h

    localfilesystembrowser/localfilesystembrowser.h

    headerwindow/headerwindow.h

    playlistsbrowser/playlistsbrowser.h
    playlistsbrowser/playlistslistmodel.h
    playlistsbrowser/playlistslistview.h

    activeplaylistwindow/activeplaylistwindow.h

    playlistview/playlistview.h
    playlistview/playlistmodel.h
    playlistview/playlistitemdelegate.h

    medialibbrowser/medialibbrowser.h
    medialibbrowser/artistslistmodel.h
    medialibbrowser/albumslistmodel.h
    medialibbrowser/songslistmodel.h)

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
