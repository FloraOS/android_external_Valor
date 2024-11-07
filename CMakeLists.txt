cmake_minimum_required(VERSION 3.27)
project(Valor)

if (NOT ANDROID_NDK)
    message(WARNING "Setting android-ndk to /opt/android-ndk")
    set(ANDROID_NDK /opt/android-ndk)
endif ()
set(CMAKE_set (ANDROID_NDK /opt/android-ndk) C_COMPILER "/usr/bin/clang" CACHE STRING "clang compiler" FORCE)
set(ANDROID_NDK /opt/android-ndk)

add_subdirectory(libvalor)
add_subdirectory(dbgen)
#add_subdirectory(valord)