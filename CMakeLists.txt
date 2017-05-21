cmake_minimum_required(VERSION 3.7)
project(assignment)

set(CMAKE_CXX_STANDARD 11)
set(ZF_LOG_ANDROID_LOG ON)
add_subdirectory(deps/zf_log)

set(SOURCE_FILES
        src/server/arena.c
        src/server/arena.h
        src/server/network.c
        src/server/network.h
        src/server/listnode.h
        src/server/listnode.c)

add_executable(assignment ${SOURCE_FILES})