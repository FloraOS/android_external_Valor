cmake_minimum_required(VERSION 3.27)
project(dbgen C)

set(CMAKE_C_STANDARD 11)



add_executable(dbgen
        src/arguments.c
        src/arguments.h
        src/array.c
        src/array.h
        src/dbgen.c
        src/hashtable.c
        src/hashtable.h
        src/message.c
        src/message.h)

target_link_libraries(dbgen valor)