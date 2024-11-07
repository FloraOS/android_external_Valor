LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
	src/process.c \
	src/util.c \
	src/aassert.c \
	src/valor.c \
	src/log.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/src \
	$(LOCAL_PATH)/../libvalor/include \
	$(LOCAL_PATH)/../../ssdeep/include

LOCAL_SEPOLICY_DIRS := $(LOCAL_PATH)/sepolicy

LOCAL_MODULE := valord
LOCAL_CFLAGS := -O2 -g -W -Wall
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_STATIC_LIBRARIES := libvalor \
			  libfuzzy

LOCAL_REQUIRED_MODULES := \
	valor.db \
	valord.rc

include $(BUILD_EXECUTABLE)
