cmake_minimum_required(VERSION 3.27)
project(Valor)

set(ANDROID_NDK /opt/android-ndk)
set(CMAKE_C_COMPILER "/usr/bin/clang" CACHE STRING "clang compiler" FORCE)

add_subdirectory(libvalor)
add_subdirectory(dbgen)
add_subdirectory(valord)