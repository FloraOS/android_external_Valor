LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := dbgen

LOCAL_SRC_FILES := \
    arguments.c \
    dbgen.c\
    message.c

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)

LOCAL_STATIC_LIBRARIES := libvalor

LOCAL_CFLAGS := -Wall -Wextra

include $(BUILD_HOST_EXECUTABLE)
