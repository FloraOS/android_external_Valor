LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
	src/valord/process.c \
	src/valord/util.c \
	src/valord/checksum.c \
	src/valord/aassert.c \
	src/valord/valor.c \
	src/valord/db/db.c \
	src/valord/crc32/crc32.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)

LOCAL_MODULE := valord
LOCAL_CFLAGS := -O2 -g -W -Wall 

include $(BUILD_EXECUTABLE)
