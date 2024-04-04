cmake_minimum_required(VERSION 3.27)
project(Valor C)

set(CMAKE_C_STANDARD 11)

include_directories(src/dbgen)
include_directories(src/dbgen/crc32)
include_directories(src/dbgen/db)
include_directories(src/libvalor/include)

add_executable(Valor
        src/dbgen/crc32/crc32.c
        src/dbgen/crc32/crc32.h
        src/dbgen/crc32/crc_poly.c
        src/dbgen/crc32/crc_poly.h
        src/dbgen/db/db.c
        src/dbgen/db/db.h
        src/dbgen/arguments.c
        src/dbgen/arguments.h
        src/dbgen/array.c
        src/dbgen/array.h
        src/dbgen/checksum.c
        src/dbgen/checksum.h
        src/dbgen/config.h
        src/dbgen/dbgen.c
        src/dbgen/hashtable.c
        src/dbgen/hashtable.h
        src/dbgen/message.c
        src/dbgen/message.h)
