cmake_minimum_required(VERSION 3.3)
project(loadlibs)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

include_directories(${LUA_INCLUDE_DIR})

set(SOURCE_FILES
    loadlib.c
    loadlib.h)

add_library(loadlibs SHARED ${SOURCE_FILES})
target_link_libraries(loadlibs ${LUA_LIBRARY_PATH})

set_target_properties(loadlibs PROPERTIES
        COMPILE_FLAGS "-m32"
        LINK_FLAGS "-m32"
        PREFIX "")