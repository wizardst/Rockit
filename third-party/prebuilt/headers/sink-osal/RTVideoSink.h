/*
 * Copyright 2019 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * author: martin.cheng@rock-chips.com
 *   date: 2019/02/18
 * module: RTVideoSink
 */

#ifndef SRC_RT_MEDIA_RTOSSINK_RTVIDEOSINK_H_
#define SRC_RT_MEDIA_RTOSSINK_RTVIDEOSINK_H_

#include <inttypes.h>

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _RTBufferInfo {
    enum Status {
        OWNED_BY_US,
        OWNED_BY_DEV_CODEC,      // platform codec hardware
        OWNED_BY_DEV_PP,         // post process
        OWNED_BY_NATIVE_WINDOW,  // surface/NativeWindow
        UNRECOGNIZED,            // not a tracked buffer
    };

    Status   mStatus;
    bool     mOwnedByClient;
    
    int32_t  mFrameW;
    int32_t  mFrameH;
    int32_t  mDisplayW;
    int32_t  mDisplayH;
    void*    mFrameSrc;  // YUV420 or VPUFrame 
    void*    mFrameDst;  // ANativeWindowBuffer
} RTBufferInfo;

class RTBufferListener {
 public:
    int32_t onRegisterBuffer(RTBufferInfo* bufferInfo) { return 0; }
    int32_t onCancelBuffer(RTBufferInfo* bufferInfo)   { return 0; }
};

typedef enum _RT_DISPLAY_DEVICE{
    DISPLAY_DEV_HW = 0,
    DISPLAY_DEV_SW,
    DISPLAY_DEV_MAX,
} RT_DISPLAY_DEVICE;

typedef enum _RT_DISPLAY_FORMAT{
    DFMT_GLES_YUV420SP   = 0,         /* YYYY... UV...    */
    DFMT_GLES_RGB565,                 /* 16-bit RGB       */
    DFMT_GLES_ARGB8888,               /* 32-bit RGB       */
    DFMT_OVERLAY,
    DFMT_MAX,
} RT_DISPLAY_FORMAT;

typedef struct _RTDisplaySetting {
    int32_t mFrameWidth;
    int32_t mFrameHeight;
    int32_t mDisplayWidth;
    int32_t mDisplayHeight;
    int32_t mTransform;
    int32_t mUsage;
    int32_t mFormat;
    int32_t mMaxBufCount;
    RT_DISPLAY_DEVICE mDisplayDev;
    RT_DISPLAY_FORMAT mDisplayFmt;
    void*          mHandler;
} RTDisplaySetting;

struct RTVideoSinkContext;
class RTVideoSink {
 public:
    virtual ~RTVideoSink() {};
    virtual int32_t initRender(RTDisplaySetting* setting) = 0;
    virtual int32_t allocRenderBuffers() = 0;
    virtual int32_t releaseRender() = 0;
    virtual int32_t setBufferListener(RTBufferListener* listener) = 0;
    virtual int32_t renderFrame(RTBufferInfo* pInfo, int64_t timestamp) = 0;
    virtual int32_t renderBlank() = 0;

 public:
    RTDisplaySetting* getSetting() { return mDisplaySetting; }

 protected:
    struct RTVideoSinkContext*   mContext;
    RTBufferListener*            mBufferListener;
    RTDisplaySetting*            mDisplaySetting;
};

#ifdef  __cplusplus
}
#endif

#endif  // SRC_RT_MEDIA_RTOSSINK_RTVIDEOSINK_H_
