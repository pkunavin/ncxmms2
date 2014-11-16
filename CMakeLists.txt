cmake_minimum_required(VERSION 2.6)
project(ncxmms2)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(3rdparty/libtermkey)
add_subdirectory(3rdparty/json-parser)
add_subdirectory(3rdparty/gtest-1.7.0)

install(FILES ncxmms2.colors DESTINATION share/ncxmms2/)
