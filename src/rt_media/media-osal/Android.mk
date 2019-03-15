LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    android/RTAWindowSink.cpp \
    android/RTMediaSync.cpp \
    android/Vsync.cpp

LOCAL_SHARED_LIBRARIES := \
    libutils \
    liblog \
    libui \
    libgui

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/android \
    $(TOP)/hardware/libhardware/include/ \
    $(TOP)/hardware/libhardware/include/hardware \
    $(TOP)/frameworks/av/include/ \
    $(TOP)/frameworks/av/media/libmedia

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE := libmedia_osal

include $(BUILD_SHARED_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := rt_media_test

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
    libmedia_osal

LOCAL_CFLAGS +=-fpermissive

include $(BUILD_EXECUTABLE)
