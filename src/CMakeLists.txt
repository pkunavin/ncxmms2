
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -Wno-deprecated-declarations")

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
    Utils.cpp
    Settings.cpp
    CommandLineOptions.cpp
    Song.cpp
    SongDisplayFormatParser.cpp
    Log.cpp

    XmmsUtils/Client.cpp
    XmmsUtils/Types.cpp
    XmmsUtils/Result.cpp

    MainWindow/MainWindow.cpp

    StatusArea/PlaybackStatusWindow.cpp
    StatusArea/StatusArea.cpp
    StatusArea/PlaybackProgressBar.cpp
    StatusArea/QuestionWindow.cpp

    ListViewAppIntegrated/ListViewAppIntegrated.cpp

    FileSystemBrowser/Dir.cpp
    FileSystemBrowser/FileSystemBrowser.cpp
    FileSystemBrowser/ServerSideBrowser.cpp
    FileSystemBrowser/ServerSideBrowserModel.cpp
    FileSystemBrowser/LocalFileSystemBrowser.cpp
    FileSystemBrowser/FileSystemModel.cpp
    FileSystemBrowser/FileSystemItemDelegate.cpp
    FileSystemBrowser/FileSystemWatcher.cpp

    HeaderWindow/HeaderWindow.cpp

    PlaylistsBrowser/PlaylistsBrowser.cpp
    PlaylistsBrowser/PlaylistsListModel.cpp
    PlaylistsBrowser/PlaylistsListView.cpp

    ActivePlaylistWindow/ActivePlaylistWindow.cpp

    HelpBrowser/HelpBrowser.cpp

    PlaylistView/PlaylistView.cpp
    PlaylistView/PlaylistModel.cpp
    PlaylistView/PlaylistItemDelegate.cpp

    MedialibBrowser/MedialibBrowser.cpp
    MedialibBrowser/ArtistsListModel.cpp
    MedialibBrowser/AlbumsListModel.cpp
    MedialibBrowser/SongsListModel.cpp
    MedialibBrowser/TagValueListModel.cpp

    EqualizerWindow/EqualizerWindow.cpp
    EqualizerWindow/EqualizerBandsWindow.cpp
    EqualizerWindow/EqualizerPreampWindow.cpp

    SongInfoWindow/SongInfoWindow.cpp)

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
