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
 *   date: 20190225
 */

#include "RTMediaData.h"                 // NOLINT
#include "rt_thread.h"                   // NOLINT
#include "RTVideoSink.h"                 // NOLINT
#include "android/RTAWindowSink.h"       // NOLINT
#include "RTNodeSinkAWindow.h"           // NOLINT
#include "rt_time.h"                     // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "NodeSinkAWindow"

typedef struct _RTSinkAWindowCtx {
    RtMetaData         *mMetaInput;
    RtMutex            *mLockPacket;
    RTMsgLooper        *mEventLooper;
    RT_Deque           *mDequePacket;
    RtThread           *mThread;
    RTVideoSink        *mVideoSink;
    RTDisplaySetting   *mDisplaySetting;
    UINT32              mLoop;
} RTSinkAWindowCtx;

void* render_loop(void* ptr_node) {
    RTNodeSinkAWindow* sink = reinterpret_cast<RTNodeSinkAWindow*>(ptr_node);
    if (RT_NULL != sink) {
        sink->runTask();
    }
    return RT_NULL;
}

RT_RET RTNodeSinkAWindow::runTask() {
    RTSinkAWindowCtx* ctx = reinterpret_cast<RTSinkAWindowCtx*>(mNodeContext);

    INT32 video_w = 0;
    INT32 video_h = 0;
    ctx->mMetaInput->findInt32(kKeyFrameW, &video_w);
    ctx->mMetaInput->findInt32(kKeyFrameH, &video_h);

    ctx->mLoop  = RT_TRUE;
    ctx->mVideoSink = new RTAWindowSink();
    ctx->mDisplaySetting->mFrameWidth  = video_w;
    ctx->mDisplaySetting->mFrameHeight = video_h;
    RT_LOGE("DISPLAY_DEV_SW mode of RTNodeSinkAWindow is using.......");
    RT_LOGE("DISPLAY_DEV_HW mode of RTNodeSinkAWindow is developping.......");
    ctx->mDisplaySetting->mDisplayDev  = DISPLAY_DEV_SW;
    ctx->mVideoSink->initRender(ctx->mDisplaySetting);

    do {
        // @TODO Render Frame by timestamp.
        this->onFireFrame();
        RtTime::sleepMs(10*1000);
    }while (ctx->mLoop);

    return RT_OK;
}

RTNodeSinkAWindow::RTNodeSinkAWindow() {
    RTSinkAWindowCtx* ctx = rt_malloc(RTSinkAWindowCtx);
    rt_memset(ctx, 0, sizeof(RTSinkAWindowCtx));

    ctx->mLockPacket = new RtMutex();
    RT_ASSERT(RT_NULL != ctx->mLockPacket);

    ctx->mDequePacket = deque_create(3);
    RT_ASSERT(RT_NULL != ctx->mDequePacket);

    ctx->mDisplaySetting = rt_malloc(RTDisplaySetting);
    rt_memset(ctx->mDisplaySetting, 0, sizeof(RTDisplaySetting));

    // save private context to mNodeContext
    mNodeContext = ctx;
}

RTNodeSinkAWindow::~RTNodeSinkAWindow() {
    release();
    mNodeContext = RT_NULL;
}

// override RTSinkDisplay public methods
RT_RET RTNodeSinkAWindow::onFireFrame() {
    RTMediaBuffer*     media_buf = RT_NULL;
    RTFrame            rt_frame  = {0};
    RTSinkAWindowCtx*  ctx       = reinterpret_cast<RTSinkAWindowCtx*>(mNodeContext);
    pullBuffer(&media_buf);
    if (RT_NULL != media_buf) {
        RTBufferInfo info;
        rt_memset(&info, 0, sizeof(RTBufferInfo));
        rt_mediabuf_goto_frame(media_buf, &rt_frame);

        info.mStatus    = RTBufferInfo::OWNED_BY_US;
        info.mFrameW    = rt_frame.mFrameW;
        info.mFrameH    = rt_frame.mFrameH;
        info.mDisplayW  = rt_frame.mDisplayW;
        info.mDisplayH  = rt_frame.mDisplayH;
        info.mFrameSrc  = rt_frame.mData;
        RT_LOGE("onFireFrame RTFrame(%p - %dx%d - %dx%d)", info.mFrameSrc, \
                             info.mFrameW, info.mFrameH, \
                             info.mDisplayW, info.mDisplayH);
        ctx->mVideoSink->renderFrame(&info, 0/*timestamp*/);
        return RT_OK;
    }
    return RT_ERR_BAD;
}

// override RTNode public methods
RT_RET RTNodeSinkAWindow::init(RtMetaData *metadata) {
    INT32 video_w = 0;
    INT32 video_h = 0;
    RTSinkAWindowCtx* ctx = reinterpret_cast<RTSinkAWindowCtx*>(mNodeContext);
    ctx->mMetaInput = metadata;
    ctx->mMetaInput->findInt32(kKeyFrameW, &video_w);
    ctx->mMetaInput->findInt32(kKeyFrameH, &video_h);

    ctx->mThread = new RtThread(render_loop, reinterpret_cast<void*>(this));
    ctx->mThread->setName("SinkAWindow");
    return RT_OK;
}

RT_RET RTNodeSinkAWindow::release() {
    RTSinkAWindowCtx* ctx = reinterpret_cast<RTSinkAWindowCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    if (RT_NULL != ctx->mLockPacket) {
        delete ctx->mLockPacket;
        ctx->mLockPacket = RT_NULL;
    }

    if (ctx->mThread != RT_NULL) {
        delete ctx->mThread;
        ctx->mThread = RT_NULL;
    }

    if (ctx->mDequePacket != NULL) {
        deque_destory(&ctx->mDequePacket);
    }
    rt_safe_free(ctx->mDisplaySetting);
    rt_safe_free(mNodeContext);
    return RT_OK;
}

RT_RET RTNodeSinkAWindow::pullBuffer(RTMediaBuffer **media_buf) {
    // make media_buf RT_NULL only.NodeSink doesn't allocate buffer
    *media_buf  = RT_NULL;
    RT_RET  err = RT_ERR_NULL_PTR;
    RTSinkAWindowCtx*  ctx   = reinterpret_cast<RTSinkAWindowCtx*>(mNodeContext);
    RT_DequeEntry   entry = deque_pop(ctx->mDequePacket);
    if (RT_NULL != entry.data) {
         *media_buf = reinterpret_cast<RTMediaBuffer*>(entry.data);
         err = RT_OK;
    }
    return err;
}

RT_RET RTNodeSinkAWindow::pushBuffer(RTMediaBuffer*  media_buf) {
    RTFrame rt_frame;
    RT_RET  err = RT_ERR_NULL_PTR;
    RTSinkAWindowCtx* ctx = reinterpret_cast<RTSinkAWindowCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    if (RT_NULL != media_buf) {
        rt_mediabuf_goto_frame(media_buf, &rt_frame);
        RT_LOGE("rt_frame(%p - %dx%d)\n", rt_frame.mData,
                  rt_frame.mFrameW,  rt_frame.mFrameH);
        err = deque_push_tail(ctx->mDequePacket, media_buf);
    }

    return err;
}

RT_RET RTNodeSinkAWindow::runCmd(RT_NODE_CMD cmd, RtMetaData *metadata) {
    RT_RET err = RT_OK;
    switch (cmd) {
    case RT_NODE_CMD_INIT:
        err = this->init(metadata);
        break;
    case RT_NODE_CMD_START:
        err = this->onStart();
        break;
    case RT_NODE_CMD_STOP:
        err = this->onPause();
        break;
    case RT_NODE_CMD_RESET:
        err = this->onReset();
        break;
    case RT_NODE_CMD_FLUSH:
        err = this->onFlush();
        break;
    case RT_NODE_CMD_PAUSE:
        err = this->onPause();
        break;
    default:
        break;
    }
    return err;
}

RT_RET RTNodeSinkAWindow::setEventLooper(RTMsgLooper* eventLooper) {
    RTSinkAWindowCtx* ctx = reinterpret_cast<RTSinkAWindowCtx*>(mNodeContext);
    ctx->mEventLooper = eventLooper;
    return RT_OK;
}

RtMetaData* RTNodeSinkAWindow::queryFormat(RTPortType port) {
    RTSinkAWindowCtx* ctx = reinterpret_cast<RTSinkAWindowCtx*>(mNodeContext);
    if (RT_PORT_INPUT == port) {
        return ctx->mMetaInput;
    }
    return RT_NULL;
}

RTNodeStub* RTNodeSinkAWindow::queryStub() {
    return &rt_sink_display_awindow;
}

// override RTNode protected method
RT_RET RTNodeSinkAWindow::onStart() {
    RT_RET            err = RT_OK;
    RTSinkAWindowCtx* ctx = reinterpret_cast<RTSinkAWindowCtx*>(mNodeContext);
    ctx->mLoop = RT_FALSE;
    ctx->mThread->start();
    return err;
}

RT_RET RTNodeSinkAWindow::onPause() {
    return RT_ERR_UNIMPLIMENTED;
}

RT_RET RTNodeSinkAWindow::onStop() {
    RTSinkAWindowCtx* ctx = reinterpret_cast<RTSinkAWindowCtx*>(mNodeContext);
    ctx->mThread->join();
    return RT_OK;
}

RT_RET RTNodeSinkAWindow::onReset() {
    return RT_ERR_UNIMPLIMENTED;
}

RT_RET RTNodeSinkAWindow::onFlush() {
    return RT_ERR_UNIMPLIMENTED;
}

static RTNode* createSinkDisplay() {
    return new RTNodeSinkAWindow();
}

struct RTNodeStub rt_sink_display_awindow {
    .mCreateNode     = createSinkDisplay,
    .mNodeType       = RT_NODE_TYPE_SINK,
    .mUsePool        = RT_FALSE,
    .mNodeName       = "rt_sink_awindow",
    .mNodeVersion    = "v1.0",
};
