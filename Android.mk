LOCAL_PATH := $(call my-dir)
include $(CLEAN_VARS)

LOCAL_CFLAGS := -Wall -Werror -O3 $(EXTRA_FLAGS)
LOCAL_MODULE := attrtools
LOCAL_MODULE_FILENAME := attrtools
LOCAL_SRC_FILES := attrtools.c libe2.c

include $(BUILD_EXECUTABLE)
