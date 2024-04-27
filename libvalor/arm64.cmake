# Arm64 library binding for libvalor
set(ANDROID_NATIVE_API_LEVEL 26)
set(ANDROID_ABI "arm64-v8a")
include(${ANDROID_NDK}/build/cmake/android.toolchain.cmake)

add_library(valor_arm64-v8a STATIC
        ${LIBVALOR_DIR}/src/checksum.c
        ${LIBVALOR_DIR}/src/db/db.c
        ${LIBVALOR_DIR}/src/crc32/crc32.c
        ${LIBVALOR_DIR}/src/crc32/crc_poly.c
        ${LIBVALOR_DIR}/src/array.c
        ${LIBVALOR_DIR}/src/hashtable.c
        ${LIBVALOR_DIR}/src/hashset.c
)

target_include_directories(valor_arm64-v8a PUBLIC
        $<BUILD_INTERFACE:${LIBVALOR_DIR}/include>
        $<INSTALL_INTERFACE:include>
)