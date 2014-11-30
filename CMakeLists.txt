cmake_minimum_required(VERSION 2.6)
project(ncxmms2)

option(USE_SANITIZER "Build with -fsanitize=address,undefined" OFF)

if(USE_SANITIZER)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fsanitize=address,undefined -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address,undefined -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g -fsanitize=address,undefined -fno-omit-frame-pointer" )
endif(USE_SANITIZER)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(3rdparty/libtermkey)
add_subdirectory(3rdparty/json-parser)
add_subdirectory(3rdparty/gtest-1.7.0)

install(FILES ncxmms2.colors DESTINATION share/ncxmms2/)
