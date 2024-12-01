LOCAL_PATH := $(call my-dir)

# rc-file to start on boot
include $(CLEAR_VARS)
LOCAL_MODULE := valord.rc
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)/init
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)


# Database(should be generated in advance or provided by other repo)
ifneq ($(VALOR_HAS_CUSTOM_DB), true)
$(warning VALOR_HAS_CUSTOM_DB is not true. Be sure to define/create valor.db in advance)
include $(CLEAR_VARS)
LOCAL_MODULE := valor.db
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_OUT_ETC)
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)
endif

# Include all module makefiles
include $(call all-makefiles-under, $(LOCAL_PATH))
