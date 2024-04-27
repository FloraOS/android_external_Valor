cmake_minimum_required(VERSION 3.27)
project(valord C)

set(CMAKE_C_STANDARD 11)

set(ANDROID_NATIVE_API_LEVEL 26)
set(ANDROID_ABI "arm64-v8a")
include(${ANDROID_NDK}/build/cmake/android.toolchain.cmake)

set(LIBVALOR_DIR ${PROJECT_SOURCE_DIR}/../libvalor/)
include("../libvalor/arm64.cmake")


add_executable(valord
        src/aassert.c
        src/process.c
        src/util.c
        src/valor.c)

target_link_libraries(valord valor_arm64-v8a)
target_link_libraries(valord android)
target_link_libraries(valord log)