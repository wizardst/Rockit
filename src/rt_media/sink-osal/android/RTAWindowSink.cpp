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
 *
 * int native_window_set_buffers_geometry(ANativeWindow*, int, int, int)' is deprecated 
 * (declared at system/core/include/system/window.h:734)
 * int Surface::queueBuffer(ANativeWindowBuffer*, int)' is protected
 * (declared at frameworks/native/include/gui/Surface.h)
 */

#include <utils/Trace.h>
#include <utils/CallStack.h>
#include <utils/RefBase.h>
#include <utils/Errors.h>
#include <assert.h>

#include <gui/IGraphicBufferProducer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>

#include <ui/GraphicBufferMapper.h>
#include <ui/GraphicBuffer.h>

using namespace android;      // NOLINT

#include "rt_header.h"        // NOLINT
#include "RTAWindowSink.h"    // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTAWindowSink"

#define DEBUG_FLAG   0x1

#define RT_LOGD_IF(condition, format, ...) if (condition > 0) ALOGD(format, ##__VA_ARGS__)
#define RT_LOGE_IF(condition, format, ...) if (condition > 0) ALOGE(format, ##__VA_ARGS__)

enum AState {
    DISCONNECTED = 0,
    CONNECTED,
    ALLOCATED,
};

struct RTVideoSinkContext {
    sp<ANativeWindow>     mAWindow;
    RTDisplaySetting*     mSetting;
    Vector<RTBufferInfo>* mRenderBuffers;
    AState                mState;
};

#define MIN_REF_BUFFER_COUNT 10

int32_t checkRenderSetting(RTDisplaySetting* setting) {
    if ( NULL == setting ) {
        ALOGE("%24s: make sure setting is valid, please check!", __FUNCTION__);
        return -1;
    }
    if ((setting->mFrameWidth == 0) || (setting->mFrameHeight == 0)) {
        ALOGE("%24s: INVALID FrameDimens(%dx%d), use default(1280x720)",
                 __FUNCTION__, setting->mFrameWidth, setting->mFrameHeight);
        setting->mFrameWidth  = 1280;
        setting->mFrameHeight = 720;
    }
    if ((setting->mDisplayWidth == 0) || (setting->mDisplayHeight == 0)) {
        ALOGE("%24s: INVALID DisplayDimens(%dx%d), use FrameDimens",
                 __FUNCTION__, setting->mDisplayWidth, setting->mDisplayHeight);
        setting->mDisplayWidth  = setting->mFrameWidth;
        setting->mDisplayHeight = setting->mFrameHeight;
    }

    if (0 == setting->mUsage) {
        ALOGE("%24s: INVALID Usage(%d), use default...",
                 __FUNCTION__, setting->mUsage);
        setting->mUsage = GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_HW_TEXTURE | GRALLOC_USAGE_EXTERNAL_DISP;
    }
    if (setting->mMaxBufCount < MIN_REF_BUFFER_COUNT) {
       if (DISPLAY_DEV_HW == setting->mDisplayDev) {
            setting->mMaxBufCount = MIN_REF_BUFFER_COUNT;
            setting->mFormat      = HAL_PIXEL_FORMAT_YCrCb_NV12;
       } else {
            setting->mMaxBufCount = 3;
            setting->mFormat      = HAL_PIXEL_FORMAT_YV12;
            setting->mUsage      |= GRALLOC_USAGE_SW_READ_NEVER;
       }
       ALOGE("%24s: INVALID MaxBufferCount(%d), use %d instead...",
                __FUNCTION__, setting->mMaxBufCount, setting->mMaxBufCount);
    }
    if (setting->mDisplayDev >= DISPLAY_DEV_MAX) {
        ALOGE("%24s: INVALID DisplayDev(%d), use DISPLAY_DEV_HW instead",
                 __FUNCTION__, setting->mDisplayDev);
        setting->mDisplayDev = DISPLAY_DEV_HW;
    }
    if (setting->mDisplayFmt >= DFMT_MAX) {
        ALOGE("%24s: INVALID DisplayFmt(%d), use DFMT_OVERLAY instead",
                 __FUNCTION__, setting->mDisplayFmt);
        setting->mDisplayFmt = DFMT_OVERLAY;
    }
    if (NULL == setting->mHandler) {
        ALOGE("%24s: INVALID Surface, createNativeSurface(%dx%d)", \
                 __FUNCTION__, setting->mFrameWidth, setting->mFrameHeight);
        setting->mHandler = createNativeSurface(setting);
        ALOGE("setting->mHandler = Surface(%p)", setting->mHandler);
    }

    return 0;
}

static sp<IGraphicBufferProducer> pBufferProducer = NULL;
static sp<SurfaceComposerClient>  pComposerClient = NULL;
static sp<SurfaceControl>         pSurfaceControl = NULL;
static sp<Surface>                pSurface        = NULL;

void* createNativeSurface(RTDisplaySetting* setting) {
    pComposerClient = new SurfaceComposerClient;
    pComposerClient->initCheck();

    pSurfaceControl = pComposerClient->createSurface(String8("DisplaySink"),
                        setting->mFrameWidth, setting->mFrameHeight,
                        PIXEL_FORMAT_RGBA_8888, 0);
    SurfaceComposerClient::openGlobalTransaction();
    pSurfaceControl->setLayer(0x20000000);
    pSurfaceControl->setPosition(0, 0);
    pSurfaceControl->setSize(setting->mFrameWidth, setting->mFrameHeight);
    pSurfaceControl->show();
    SurfaceComposerClient::closeGlobalTransaction();
    pSurface = pSurfaceControl->getSurface();
    pBufferProducer = pSurface->getIGraphicBufferProducer();
    return pBufferProducer.get();
}

int32_t checkRenderContext(RTVideoSinkContext* context) {
    if ((NULL == context) || (NULL == context->mSetting)) {
        ALOGE("%24s: NO render context and setting Found...", __FUNCTION__);
        return -1;
    }

    if (context->mState < CONNECTED) {
        ALOGE("%24s: Called when in bad state: %d", __FUNCTION__, context->mState);
        return -1;
    }
    return 0;
}

RTAWindowSink::RTAWindowSink() {
    mContext        = reinterpret_cast<RTVideoSinkContext*>(malloc(sizeof(RTVideoSinkContext)));
    memset(mContext, 0, sizeof(RTVideoSinkContext));
    mContext->mAWindow       = NULL;
    mContext->mRenderBuffers = new Vector<RTBufferInfo>();
    mBufferListener = NULL;
    mDisplaySetting = NULL;
}

RTAWindowSink::~RTAWindowSink() {
    mContext->mAWindow = NULL;
    if (NULL != mContext->mRenderBuffers) {
        mContext->mRenderBuffers->clear();
        delete mContext->mRenderBuffers;
        mContext->mRenderBuffers = NULL;
    }
    if (NULL != mContext) {
       free(mContext);
       mContext = NULL;
    }
    // listener is owned and released by external entity, set NULL only
    mBufferListener = NULL;
}

int32_t dumpError(const char* func_name, int32_t err,
                  const char* op_name,   int32_t param1, int32_t param2 = 0) {
    if (err != NO_ERROR) {
        ALOGE("%24s: Fail to connect to %s(p1=%d; p2=%d)",
                __FUNCTION__, op_name, param1, param2);
        return err;
    } else {
        RT_LOGD_IF(DEBUG_FLAG, "%24s: Success to %s(p1=%d; p2=%d)",
                   func_name, op_name, param1, param2);
    }
    return err;
}

int32_t RTAWindowSink::initRender(RTDisplaySetting* setting) {
    // Verify legality and correct bad values.
    if (-1 == checkRenderSetting(setting)) {
        mContext->mState = DISCONNECTED;
        return -1;
    }

    int32_t err = 0;
    android_native_rect_t crop;

    if (NULL == setting->mHandler) {
        ALOGE("%24s: INVALID Surface(%p)", __FUNCTION__, setting->mHandler);
        return -1;
    }
    mContext->mAWindow = new Surface(reinterpret_cast<IGraphicBufferProducer*>(setting->mHandler));
    if (NULL == mContext->mAWindow.get()) {
        ALOGE("%24s: INVALID NativeWindow(%p)", __FUNCTION__, mContext->mAWindow.get());
        return -1;
    }
    RT_LOGD_IF(DEBUG_FLAG, "%24s: pSurface      = %p", __FUNCTION__, setting->mHandler);
    RT_LOGD_IF(DEBUG_FLAG, "%24s: pNativeWindow = %p", __FUNCTION__, mContext->mAWindow.get());

    mContext->mSetting      = setting;
    mContext->mState        = DISCONNECTED;

    err = native_window_api_connect(mContext->mAWindow.get(),
            NATIVE_WINDOW_API_MEDIA);
    if (NO_ERROR != dumpError("initRender", err, "window_api_connect", 0)) {
        return err;
    }

    err = native_window_set_scaling_mode(
            mContext->mAWindow.get(), NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW);
    if (NO_ERROR != dumpError("initRender", err, "window_set_scaling_mode", 0)) {
        return err;
    }

    crop.left = 0;
    crop.top  = 0;
    crop.right  = setting->mDisplayWidth;
    crop.bottom = setting->mDisplayHeight;
    err = native_window_set_crop(mContext->mAWindow.get(), &crop);
    if (NO_ERROR != dumpError("initRender", err,
                      "window_set_crop", crop.right, crop.bottom)) {
        return err;
    }

    if (setting->mTransform != 0) {
        uint32_t halTransform = 0;
        switch (setting->mTransform) {
            case   0: halTransform = 0; break;
            case  90: halTransform = HAL_TRANSFORM_ROT_90; break;
            case 180: halTransform = HAL_TRANSFORM_ROT_180; break;
            case 270: halTransform = HAL_TRANSFORM_ROT_270; break;
            default : halTransform = 0; break;
        }

        if (halTransform > 0) {
            err = native_window_set_buffers_transform(
                    mContext->mAWindow.get(), halTransform);
            if (NO_ERROR != dumpError("initRender", err,
                              "window_set_crop", halTransform)) {
                return err;
            }
        }
    }

    err = native_window_set_usage(mContext->mAWindow.get(), setting->mUsage);
    if (NO_ERROR != dumpError("initRender", err, "window_set_usage", setting->mUsage)) {
        return err;
    }

    /*
     * set_buffers_geometry is deprecated
     * use set_buffers_dimensions and set_buffers_format instead
     */
    err = native_window_set_buffers_dimensions(
            mContext->mAWindow.get(),
            setting->mFrameWidth,
            setting->mFrameHeight);
    if (NO_ERROR != dumpError("initRender", err,
                      "window_set_dimensions",
                      setting->mFrameWidth, setting->mFrameHeight)) {
        return err;
    }

    err = native_window_set_buffers_format(mContext->mAWindow.get(),
            setting->mFormat);
    if (NO_ERROR != dumpError("initRender", err, "window_set_format", setting->mFormat)) {
        return err;
    }

    int queuesToNativeWindow = 0;
    err = mContext->mAWindow->query(mContext->mAWindow.get(),
                               NATIVE_WINDOW_QUEUES_TO_WINDOW_COMPOSER, &queuesToNativeWindow);
    if (NO_ERROR != dumpError("initRender", err,
                     "authenticate native window", queuesToNativeWindow)) {
        return err;
    }

    err = native_window_set_buffer_count(mContext->mAWindow.get(),
            setting->mMaxBufCount);
    if (NO_ERROR != dumpError("initRender", err,
                     "window_set_buffer_count", setting->mMaxBufCount)) {
        return err;
    }
    mDisplaySetting  = setting;
    mContext->mState = CONNECTED;
    return err;
}

int32_t RTAWindowSink::releaseRender() {
    if (NULL == mContext) {
        return -1;
    }

    int32_t err = NO_ERROR;
    if (mContext->mState >= CONNECTED) {
        err = native_window_api_disconnect(mContext->mAWindow.get(),
                NATIVE_WINDOW_API_CAMERA);
        if (NO_ERROR != dumpError("releaseRender", err,
                         "window_api_disconnect", 0)) {
            return err;
        }
    }
    mContext->mState = DISCONNECTED;
    return err;
}

int32_t RTAWindowSink::setBufferListener(RTBufferListener* listener) {
    if ((NULL != mBufferListener)&&(mBufferListener != listener)) {
        ALOGE("Check, don't set difference BufferListener...");
    }
    mBufferListener = listener;
    return NO_ERROR;
}

int32_t RTAWindowSink::allocRenderBuffers() {
    if (0 != checkRenderContext(mContext)) {
        return -1;
    }

    uint32_t bufferCount = mContext->mSetting->mMaxBufCount;
    int32_t  err = NO_ERROR;
    mContext->mRenderBuffers->clear();
    // Dequeue and register buffers to external entity by callback
    for (uint32_t idx = 0; idx < bufferCount; idx++) {
        ANativeWindowBuffer *buf;
        err = native_window_dequeue_buffer_and_wait(mContext->mAWindow.get(), &buf);
        if (NO_ERROR != dumpError("allocRenderBuffers", err,
                         "window_dequeue_buffer", idx)) {
            return err;
        }

        RTBufferInfo info;
        info.mStatus   = RTBufferInfo::OWNED_BY_US;
        info.mFrameDst = buf;
        ALOGE("add %p to RenderBuffers(%p)", &info, &(mContext->mRenderBuffers));
        mContext->mRenderBuffers->push(info);

        if (NULL != mBufferListener) {
            mBufferListener->onRegisterBuffer(&info);
            RT_LOGD_IF(DEBUG_FLAG, "[%24s]: Registered graphic buffer with ID %d (pointer = %p)",
                    __FUNCTION__, idx, info.mFrameDst);
        } else {
            ALOGE("[%24s]: No entity to register graphic buffer with ID %d (pointer = %p)",
                    __FUNCTION__, idx, info.mFrameDst);
        }
    }
    return err;
}

int32_t RTAWindowSink::dequeBuffer(RTBufferInfo* pInfo) {
    int32_t err              = NO_ERROR;
    void*   dst              = NULL;
    ANativeWindowBuffer* buf = NULL;
    pInfo->mStatus           = RTBufferInfo::OWNED_BY_US;
    pInfo->mFrameDst         = NULL;
    err = native_window_dequeue_buffer_and_wait(mContext->mAWindow.get(), &buf);
    if (NO_ERROR != dumpError("dequeBuffer", err, "window_dequeue_buffer", 0)) {
        return err;
    }

    pInfo->mFrameDst = buf;
    return err;
}

int32_t RTAWindowSink::queueBuffer(RTBufferInfo*  pInfo) {
    if ((NULL == pInfo) ||(NULL == pInfo->mFrameDst)) {
        return -1;
    }
    int32_t              err = NO_ERROR;
    ANativeWindowBuffer* buf    = reinterpret_cast<ANativeWindowBuffer*>(pInfo->mFrameDst);
    GraphicBufferMapper &mapper = GraphicBufferMapper::get();
    mapper.unlock(buf->handle);
    err = mContext->mAWindow->queueBuffer(mContext->mAWindow.get(), buf, -1);
    if (NO_ERROR != dumpError("queueBuffer", err, "window_queue_buffer", 0)) {
        return err;
    }
    return err;
}

int32_t RTAWindowSink::renderHWFrame(RTBufferInfo* pInfo, int64_t timestamp) {
    int32_t err = NO_ERROR;
    err = native_window_set_buffers_timestamp(mContext->mAWindow.get(), timestamp);
    if (err != NO_ERROR) return err;

    void* dst = NULL;
    RT_DISPLAY_FORMAT dFmt = mDisplaySetting->mDisplayFmt;
    if (NO_ERROR == dequeBuffer(pInfo)) {
        Rect bounds(pInfo->mDisplayW, pInfo->mDisplayH);
        ANativeWindowBuffer* buf    = reinterpret_cast<ANativeWindowBuffer*>(pInfo->mFrameDst);
        GraphicBufferMapper &mapper = GraphicBufferMapper::get();
        if (NULL != buf) {
           mapper.lock(buf->handle, GRALLOC_USAGE_SW_WRITE_OFTEN, bounds, &dst);
           switch (dFmt) {
              case DFMT_OVERLAY:
                // YUV420 frame allocated by HW Decoder and dst display is overlay
                memcpy(dst, pInfo->mFrameSrc, pInfo->mDisplayW * pInfo->mDisplayH * 3/2);
                break;
              case DFMT_GLES_YUV420SP:
                // YUV420 frame allocated by HW Decoder and dst display is GLES
                // @TODO fix this bug
                memcpy(dst, pInfo->mFrameSrc, pInfo->mDisplayW * pInfo->mDisplayH * 3/2);
                break;
              case DFMT_GLES_RGB565:
                // YUV420 frame allocated by HW Decoder and dst display is GLES
                // @TODO fix this bug
                memcpy(dst, pInfo->mFrameSrc, pInfo->mDisplayW * pInfo->mDisplayH * 3/2);
                break;
              case DFMT_GLES_ARGB8888:
                // YUV420 frame allocated by HW Decoder and dst display is GLES
                // @TODO fix this bug
                memcpy(dst, pInfo->mFrameSrc, pInfo->mDisplayW * pInfo->mDisplayH * 3/2);
                break;
              default:
                break;
           }
           queueBuffer(pInfo);
        }
    }
    return err;
}

int32_t RTAWindowSink::renderSWFrame(RTBufferInfo* pInfo, int64_t timestamp) {
    void*   dst = NULL;
    int32_t err = NO_ERROR;
    if (NO_ERROR == dequeBuffer(pInfo)) {
        Rect bounds(pInfo->mDisplayW, pInfo->mDisplayH);
        ANativeWindowBuffer* buf    = reinterpret_cast<ANativeWindowBuffer*>(pInfo->mFrameDst);
        GraphicBufferMapper &mapper = GraphicBufferMapper::get();
        if (NULL != buf) {
           mapper.lock(buf->handle, GRALLOC_USAGE_SW_WRITE_OFTEN, bounds, &dst);
           // YUV420 frame allocated by Software Decoder
           memcpy(dst, pInfo->mFrameSrc, pInfo->mDisplayW * pInfo->mDisplayH * 3/2);
           ALOGE("renderSWFrame RTFrame(%p - %dx%d - %dx%d)", pInfo->mFrameSrc, \
                             pInfo->mFrameW,   pInfo->mFrameH, \
                             pInfo->mDisplayW, pInfo->mDisplayH);
           queueBuffer(pInfo);
           return err;
        }
    }

    return -1;
}

int32_t RTAWindowSink::renderFrame(RTBufferInfo* pInfo, int64_t timestamp) {
    if (0 != checkRenderContext(mContext)) {
        return -1;
    }

    if ((0 == pInfo->mDisplayW) || (0 == pInfo->mDisplayH)) {
        pInfo->mDisplayW = pInfo->mFrameW;
        pInfo->mDisplayH = pInfo->mFrameH;
    }

    int32_t err = 0;
    switch (mDisplaySetting->mDisplayDev) {
      case DISPLAY_DEV_HW:
        err = renderHWFrame(pInfo, timestamp);
        break;
      case DISPLAY_DEV_SW:
        err = renderSWFrame(pInfo, timestamp);
        break;
      default:
        break;
    }

    return err;
}

int32_t RTAWindowSink::renderBlank() {
    if (0 != checkRenderContext(mContext)) {
        return -1;
    }

    int32_t err = 0;
    switch (mDisplaySetting->mDisplayDev) {
      case DISPLAY_DEV_HW:
        err = renderHWBlank();
        break;
      case DISPLAY_DEV_SW:
        err = renderSWBlank();
        break;
      default:
        break;
    }

    return err;
}

int32_t RTAWindowSink::renderSWBlank() {
    return -1;
}

int32_t RTAWindowSink::renderHWBlank() {
    status_t err = NO_ERROR;
    ANativeWindowBuffer* anb = NULL;
    int numBufs = 0;
    int minUndequeuedBufs = 0;

    // We need to reconnect to the ANativeWindow as a CPU client to ensure that
    // no frames get dropped by SurfaceFlinger assuming that these are video
    // frames.
    err = native_window_api_disconnect(mContext->mAWindow.get(),
            NATIVE_WINDOW_API_MEDIA);
    if (NO_ERROR != dumpError("renderBlank", err,
                     "window_api_disconnect", 0)) {
        return err;
    }

    err = native_window_api_connect(mContext->mAWindow.get(),
            NATIVE_WINDOW_API_CPU);
    if (NO_ERROR != dumpError("renderBlank", err,
                     "window_api_connect", NATIVE_WINDOW_API_CPU)) {
        return err;
    }

    err = native_window_set_buffers_geometry(mContext->mAWindow.get(), 1, 1,
            HAL_PIXEL_FORMAT_RGBX_8888);
    if (NO_ERROR != dumpError("renderBlank", err,
                     "window_set_geometry", 1, 1)) {
        goto error;
    }

    err = native_window_set_scaling_mode(mContext->mAWindow.get(),
                NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW);
    if (NO_ERROR != dumpError("renderBlank", err,
                     "window_set_scaling_mode", 0)) {
        goto error;
    }

    err = native_window_set_usage(mContext->mAWindow.get(),
            GRALLOC_USAGE_SW_WRITE_OFTEN);
    if (NO_ERROR != dumpError("renderBlank", err,
                     "window_set_usage", GRALLOC_USAGE_SW_WRITE_OFTEN)) {
        goto error;
    }

    err = mContext->mAWindow->query(mContext->mAWindow.get(),
            NATIVE_WINDOW_MIN_UNDEQUEUED_BUFFERS, &minUndequeuedBufs);
    if (NO_ERROR != dumpError("renderBlank", err,
                     "query MIN_UNDEQUEUED_BUFFERS", minUndequeuedBufs)) {
        goto error;
    }

    numBufs = minUndequeuedBufs + 1;
    err = native_window_set_buffer_count(mContext->mAWindow.get(), numBufs);
    if (NO_ERROR != dumpError("renderBlank", err,
                     "window_set_buffer_count", numBufs)) {
        goto error;
    }

    // We  push numBufs + 1 buffers to ensure that we've drawn into the same
    // buffer twice.  This should guarantee that the buffer has been displayed
    // on the screen and then been replaced, so an previous video frames are
    // guaranteed NOT to be currently displayed.
    for (int idx = 0; idx < numBufs + 1; idx++) {
        int fenceFd = -1;
        err = native_window_dequeue_buffer_and_wait(mContext->mAWindow.get(), &anb);
        if (NO_ERROR != dumpError("renderBlank", err,
                         "window_dequeue_buffer", idx)) {
            goto error;
        }

        sp<GraphicBuffer> buf(new GraphicBuffer(anb, false));

        // Fill the buffer with the a 1x1 checkerboard pattern ;)
        uint32_t* img = NULL;
        err = buf->lock(GRALLOC_USAGE_SW_WRITE_OFTEN, reinterpret_cast<void**>(&img));
        if (NO_ERROR != dumpError("renderBlank", err,
                         "lockBuffer", idx)) {
            goto error;
        }

        *img = 0;

        err = buf->unlock();
        if (NO_ERROR != dumpError("renderBlank", err,
                         "unlockBuffer", idx)) {
            goto error;
        }

        err = mContext->mAWindow->queueBuffer(mContext->mAWindow.get(), buf->getNativeBuffer(), -1);
        if (NO_ERROR != dumpError("renderBlank", err,
                         "queueBuffer", idx)) {
            goto error;
        }

        anb = NULL;
    }
    return NO_ERROR;

error:

    if (err != NO_ERROR) {
        // Clean up after an error.
        if (anb != NULL) {
            mContext->mAWindow->cancelBuffer(mContext->mAWindow.get(), anb, -1);
        }

        native_window_api_disconnect(mContext->mAWindow.get(),
                NATIVE_WINDOW_API_CPU);
        native_window_api_connect(mContext->mAWindow.get(),
                NATIVE_WINDOW_API_MEDIA);

        return err;
    } else {
        // Clean up after success.
        err = native_window_api_disconnect(mContext->mAWindow.get(),
                NATIVE_WINDOW_API_CPU);
        if (NO_ERROR != dumpError("renderBlank", err,
                         "window_api_disconnect", NATIVE_WINDOW_API_CPU)) {
            return err;
        }

        err = native_window_api_connect(mContext->mAWindow.get(),
                NATIVE_WINDOW_API_MEDIA);
        if (NO_ERROR != dumpError("renderBlank", err,
                         "window_api_connect", NATIVE_WINDOW_API_MEDIA)) {
            return err;
        }

        return NO_ERROR;
    }
}

/*
int32_t RTAWindowSink::alignWidth(int32_t width) {
    // @TODO fix this bug
    return width;
}

int32_t RTAWindowSink::alignHeight(int32_t height) {
    // @TODO fix this bug
    return height;
}
*/

