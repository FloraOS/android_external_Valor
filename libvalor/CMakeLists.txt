cmake_minimum_required(VERSION 3.28.1)
project(libvalor)

# libvalor base
add_library(valor STATIC
        src/checksum.c
        src/db/db.c
        src/crc32/crc32.c
        src/crc32/crc_poly.c
        src/array.c
        src/hashtable.c
        src/hashset.c
)

target_include_directories(valor PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Tests
add_executable(test_hashset tests/test_hashset.c)
target_link_libraries(test_hashset valor)
target_link_libraries(test_hashset ubsan)

add_test(NAME test_hashset COMMAND test_hashset)

add_executable(test_stringset tests/test_stringset.c)
target_link_libraries(test_stringset valor)
target_link_libraries(test_stringset ubsan)

add_test(NAME test_stringset COMMAND test_stringset)