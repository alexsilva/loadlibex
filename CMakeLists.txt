cmake_minimum_required(VERSION 3.3)
project(loadlibs)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

include_directories(${LUA_INCLUDE_DIR})
include_directories(deps)

link_directories(${LUA_LIBRARIES})

set(SOURCE_FILES
        deps/path-join/path-join.c
        deps/str-ends-with/str-ends-with.c
        deps/str-starts-with/str-starts-with.c
        deps/extname/extname.c
        deps/substr/substr.c
        deps/strdup/strdup.c
        deps/trim/trim.c
        loadlib.c
        loadlib.h
        )

add_library(loadlibs MODULE ${SOURCE_FILES})

if (WIN32)
    set_target_properties(loadlibs PROPERTIES PREFIX "")
    target_link_libraries(loadlibs lua32ng lualib32ng)
endif ()

set_target_properties(loadlibs PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")