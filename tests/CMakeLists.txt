
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/gtest-1.7.0/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra")

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB glib-2.0 REQUIRED)
include_directories(${GLIB_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    test_stringalgo.cpp
    test_expected.cpp
    test_dir.cpp)

add_executable(test_all ${SOURCES})
target_link_libraries(test_all gtest libncxmms2-app)
