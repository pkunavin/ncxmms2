
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-documentation -Wno-c++98-compat -Wno-c++98-compat-pedantic")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-gnu-zero-variadic-macro-arguments -Wno-old-style-cast -Wno-sign-conversion")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-padded -Wno-implicit-fallthrough -Wno-conversion -Wno-covered-switch-default")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors -Wno-global-constructors -Wno-missing-prototypes")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-variable-declarations -Wno-shadow -Wno-switch-enum -Wno-weak-vtables")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-exception-parameter -Wno-zero-length-array -Wno-reserved-id-macro")
endif()

add_subdirectory(lib)

set(SOURCES
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

    filesystembrowser/dir.cpp
    filesystembrowser/filesystembrowser.cpp
    filesystembrowser/serversidebrowser.cpp
    filesystembrowser/serversidebrowsermodel.cpp
    filesystembrowser/localfilesystembrowser.cpp
    filesystembrowser/filesystemmodel.cpp
    filesystembrowser/filesystemitemdelegate.cpp
    filesystembrowser/filesystemwatcher.cpp

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

    filesystembrowser/dir.h
    filesystembrowser/abstractfilesystemmodel.h
    filesystembrowser/filesystembrowser.h
    filesystembrowser/serversidebrowser.h
    filesystembrowser/serversidebrowsermodel.h
    filesystembrowser/localfilesystembrowser.h
    filesystembrowser/filesystemmodel.h
    filesystembrowser/filesystemitemdelegate.h
    filesystembrowser/filesystemwatcher.h

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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(subst ${CMAKE_SOURCE_DIR}/,,$(abspath $<))\"'")

find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB glib-2.0 REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})

pkg_check_modules(XMMS2_C xmms2-client REQUIRED)
include_directories(${XMMS2_C_INCLUDE_DIRS})

pkg_check_modules(XMMS2_C_GLIB xmms2-client-glib REQUIRED)
include_directories(${XMMS2_C_GLIB_INCLUDE_DIRS})

add_library(libncxmms2-app ${SOURCES})
set_target_properties(libncxmms2-app PROPERTIES PREFIX "")
target_link_libraries(libncxmms2-app libncxmms2
                                     ${GLIB_LIBRARIES}
                                     ${XMMS2_C_LIBRARIES}
                                     ${XMMS2_C_GLIB_LIBRARIES})
add_executable(ncxmms2 main.cpp)
target_link_libraries(ncxmms2 libncxmms2-app)

install (TARGETS ncxmms2 DESTINATION bin)
