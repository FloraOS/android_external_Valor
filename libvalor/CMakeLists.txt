cmake_minimum_required(VERSION 3.28.1)
project(libvalor)

add_library(valor STATIC
        src/checksum.c
        src/db/db.c
        src/crc32/crc32.c
        src/crc32/crc_poly.c
        src/array.c
        src/hashtable.c
)

target_include_directories(valor PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)