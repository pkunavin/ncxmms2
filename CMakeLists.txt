cmake_minimum_required(VERSION 2.6)
project(ncxmms2)
add_subdirectory(src)
add_subdirectory(3rdparty/libtermkey)
add_subdirectory(3rdparty/json-parser)

install(FILES ncxmms2.colors DESTINATION share/ncxmms2/)
