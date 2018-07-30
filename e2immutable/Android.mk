LOCAL_PATH := $(call my-dir)
include $(CLEAN_VARS)

LOCAL_CFLAGS := -Wall -Werror -O3 $(EXTRA_FLAGS)
LOCAL_MODULE := e2immutable
LOCAL_MODULE_FILENAME := e2immutable
LOCAL_SRC_FILES := e2immutable.c

include $(BUILD_EXECUTABLE)
