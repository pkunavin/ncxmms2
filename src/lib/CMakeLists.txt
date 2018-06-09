
set(SOURCES
    Application.cpp
    Signals.cpp
    Object.cpp
    Window.cpp
    StackedWindow.cpp
    Timer.cpp
    Painter.cpp
    KeyEvent.cpp
    MouseEvent.cpp
    Label.cpp
    LineEdit.cpp
    ListModel.cpp
    ListView.cpp
    ListModelItemDelegate.cpp
    Utf.cpp
    TextView.cpp
    IniParser.cpp
    Palette.cpp
    CheckBox.cpp
    RadioButtonGroupBox.cpp
    HtmlParser.cpp
    StringAlgo.cpp)

add_library(libncxmms2 ${SOURCES})
set_target_properties(libncxmms2 PROPERTIES PREFIX "")

find_package(PkgConfig REQUIRED)

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

pkg_check_modules(GLIB glib-2.0 REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})

target_link_libraries(libncxmms2 libtermkey
                                  json-parser
                                  ${NCURSESW_LIBRARIES}
                                  ${GLIB_LIBRARIES})
