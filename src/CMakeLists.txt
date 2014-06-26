add_subdirectory(lib)

set(SOURCES
    main.cpp
    utils.cpp
    settings.cpp
    commandlineoptions.cpp
    song.cpp
    songdisplayformatparser.cpp
    log.cpp

    xmmsutils/client.cpp
    xmmsutils/types.cpp
    xmmsutils/result.cpp

    mainwindow/mainwindow.cpp

    statusarea/playbackstatuswindow.cpp
    statusarea/statusarea.cpp
    statusarea/playbackprogressbar.cpp
    statusarea/questionwindow.cpp

    listviewappintegrated/listviewappintegrated.cpp

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
    medialibbrowser/songslistmodel.cpp

    equalizerwindow/equalizerwindow.cpp
    equalizerwindow/equalizerbandswindow.cpp
    equalizerwindow/equalizerpreampwindow.cpp

    songinfowindow/songinfowindow.cpp)

set(HEADERS
    utils.h
    settings.h
    commandlineoptions.h
    song.h
    songdisplayformatparser.h
    hotkeys.h
    log.h

    xmmsutils/client.h
    xmmsutils/types.h
    xmmsutils/result.h

    mainwindow/mainwindow.h

    statusarea/playbackstatuswindow.h
    statusarea/statusarea.h
    statusarea/playbackprogressbar.h
    statusarea/questionwindow.h

    listviewappintegrated/listviewappintegrated.h

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
    medialibbrowser/songslistmodel.h

    equalizerwindow/equalizerwindow.h
    equalizerwindow/equalizerbandswindow.h
    equalizerwindow/equalizerpreampwindow.h

    songinfowindow/songinfowindow.h)

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

pkg_check_modules(XMMS2_C xmms2-client REQUIRED)
include_directories(${XMMS2_C_INCLUDE_DIRS})

pkg_check_modules(XMMS2_C_GLIB xmms2-client-glib REQUIRED)
include_directories(${XMMS2_C_GLIB_INCLUDE_DIRS})

target_link_libraries(ncxmms2 ncxmms2-lib
                              ${GLIB_LIBRARIES}
                              ${XMMS2_C_LIBRARIES}
                              ${XMMS2_C_GLIB_LIBRARIES})

install (TARGETS ncxmms2 DESTINATION bin)

