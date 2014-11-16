
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/gtest-1.7.0/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../src)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra")

set(SOURCES
    main.cpp
    test_expected.cpp)

add_executable(test_all ${SOURCES})
target_link_libraries(test_all gtest)
