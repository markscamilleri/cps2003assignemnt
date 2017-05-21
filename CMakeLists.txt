cmake_minimum_required(VERSION 3.7)
project(assignment)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        src/server/arena.c
        src/server/arena.h
        src/server/network_server.c
        src/server/network_server.h
        src/server/listnode.h
        src/server/listnode.c
        src/client/network_client.c
        src/client/network_client.h
        src/config.h
        deps/zf_log/zf_log.c
        deps/zf_log/zf_log.h src/main.c)

add_executable(assignment ${SOURCE_FILES})