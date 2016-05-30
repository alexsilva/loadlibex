cmake_minimum_required(VERSION 3.3)
project(loadlibs)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

include_directories(${LUA_INCLUDE_DIR})

link_directories(${LUA_LIBRARIES})

set(SOURCE_FILES loadlib.c loadlib.h)

add_library(loadlibs SHARED ${SOURCE_FILES})

if (WIN32)
    set_target_properties(loadlibs PROPERTIES PREFIX "")
    target_link_libraries(loadlibs lua32ng lualib32ng)
endif ()

set_target_properties(loadlibs PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")