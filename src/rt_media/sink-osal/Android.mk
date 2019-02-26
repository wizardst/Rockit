LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    android/RTAWindowSink.cpp

LOCAL_SHARED_LIBRARIES := \
    libutils \
    liblog \
    libui \
    libgui

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/android \
    $(TOP)/hardware/libhardware/include/ \
    $(TOP)/hardware/libhardware/include/hardware \

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libsink_osal

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := rt_sink_test

LOCAL_SRC_FILES := \
    rt_sink_main.cpp \

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include

LOCAL_MODULE_TAGS :=optional

LOCAL_SHARED_LIBRARIES := \
    libutils \
    liblog \
    libui \
    libgui \
    libbinder \
    libsink_osal

LOCAL_CFLAGS +=-fpermissive

include $(BUILD_EXECUTABLE)
