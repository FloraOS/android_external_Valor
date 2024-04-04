cmake_minimum_required(VERSION 3.29)
project(libvalor)

add_library(libvalor STATIC
        src/db/db.c
        src/crc32/crc32.c
        src/crc32/crc_poly.c
)

target_include_directories(libvalor PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)