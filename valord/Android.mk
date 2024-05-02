include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
	src/valord/process.c \
	src/valord/util.c \
	src/valord/checksum.c \
	src/valord/aassert.c \
	src/valord/valor.c \
	src/valord/log.c

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH) \
	$(LOCAL_PATH)/src

LOCAL_SEPOLICY_DIRS := $(LOCAL_PATH)/sepolicy

LOCAL_MODULE := valord
LOCAL_CFLAGS := -O2 -g -W -Wall
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_STATIC_LIBRARIES := libvalor

LOCAL_REQUIRED_MODULES := \
	valor.db \
	valord.rc

include $(BUILD_EXECUTABLE)
