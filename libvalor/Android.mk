LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := valor
LOCAL_SRC_FILES := \
    src/checksum.c \
	src/db/db.c \
	src/crc32/crc32.c \
	src/crc32/crc_poly.c \
	src/array.c \
	src/hashtable.c \
	src/hashset.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include


LOCAL_CFLAGS := -Wall -Werror

include $(BUILD_STATIC_LIBRARY)
