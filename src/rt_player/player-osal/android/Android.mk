LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := rt_player_test

LOCAL_SRC_FILES := \
    rt_player_main.cpp \
    RTMediaPlayer.cpp \
    RTMediaPlayerImpl.cpp

LOCAL_C_INCLUDES += $(LOCAL_PATH) \
                    frameworks/av/include/ \
                    frameworks/av/media/libmedia

LOCAL_MODULE_TAGS :=optional

LOCAL_SHARED_LIBRARIES := \
    libutils \
    liblog \
    libui \
    libgui \
    libmedia \
    libbinder \
    libos_sink

LOCAL_CFLAGS +=-fpermissive

include $(BUILD_EXECUTABLE)
