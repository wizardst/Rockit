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

#include "RTNodeSinkGLES.h"     // NOLINT
#include "RTMediaData.h"        // NOLINT
#include "rt_thread.h"          // NOLINT
#include "RTGLApp.h"            // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "NodeSinkAWindow"

typedef struct _RTSinkGLESCtx {
    RtMetaData         *mMetaInput;
    RtMutex            *mLockPacket;
    RTMsgLooper        *mEventLooper;
    RT_Deque           *mDequePacket;
    RtThread           *mThread;
    RTGLApp            *mGLApp;
    UINT32              mLoop;
} RTSinkGLESCtx;

void* render_loop(void* ptr_node) {
    RTNodeSinkGLES* sink = reinterpret_cast<RTNodeSinkGLES*>(ptr_node);
    if (RT_NULL != sink) {
        sink->runTask();
    }
    return RT_NULL;
}

void FrameUpdate(void* ptr_node, RT_FLOAT interval) {
    RTNodeSinkGLES* sink = reinterpret_cast<RTNodeSinkGLES*>(ptr_node);
    if (RT_NULL != sink) {
        sink->onFireFrame();
    }
}

RT_RET RTNodeSinkGLES::runTask() {
    RTSinkGLESCtx* ctx = reinterpret_cast<RTSinkGLESCtx*>(mNodeContext);

    INT32 video_w = 0;
    INT32 video_h = 0;
    ctx->mMetaInput->findInt32(kKeyFrameW, &video_w);
    ctx->mMetaInput->findInt32(kKeyFrameH, &video_h);

    ctx->mGLApp = new RTGLApp();
    ctx->mLoop  = RT_TRUE;
    ctx->mGLApp->createWindow("Rockit", video_w, video_h);
    ctx->mGLApp->setVideoScheduler(FrameUpdate, this);
    ctx->mGLApp->eventLoop();
    return RT_OK;
}

RTNodeSinkGLES::RTNodeSinkGLES() {
    RTSinkGLESCtx* ctx = rt_malloc(RTSinkGLESCtx);
    rt_memset(ctx, 0, sizeof(RTSinkGLESCtx));

    ctx->mLockPacket = new RtMutex();
    RT_ASSERT(RT_NULL != ctx->mLockPacket);

    ctx->mDequePacket = deque_create(3);
    RT_ASSERT(RT_NULL != ctx->mDequePacket);

    // save private context to mNodeContext
    mNodeContext = ctx;
}

RTNodeSinkGLES::~RTNodeSinkGLES() {
    release();
    mNodeContext = RT_NULL;
}

// override RTSinkDisplay public methods
RT_RET RTNodeSinkGLES::onFireFrame() {
    RTMediaBuffer*     media_buf = RT_NULL;
    RTFrame            rt_frame  = {0};
    RTSinkGLESCtx*  ctx       = reinterpret_cast<RTSinkGLESCtx*>(mNodeContext);
    pullBuffer(&media_buf);
    if (RT_NULL != media_buf) {
        rt_mediabuf_goto_frame(media_buf, &rt_frame);
        // RT_LOGE("RTFrame(%p) %dx%d", rt_frame.mData, rt_frame.mFrameW, rt_frame.mFrameH);
        ctx->mGLApp->updateFrame(reinterpret_cast<unsigned char*>(rt_frame.mData), rt_frame.mFrameW, rt_frame.mFrameH);
        return RT_OK;
    }
    return RT_ERR_BAD;
}

// override RTNode public methods
RT_RET RTNodeSinkGLES::init(RtMetaData *metadata) {
    INT32 video_w = 0;
    INT32 video_h = 0;
    RTSinkGLESCtx* ctx = reinterpret_cast<RTSinkGLESCtx*>(mNodeContext);
    ctx->mMetaInput = metadata;
    ctx->mMetaInput->findInt32(kKeyFrameW, &video_w);
    ctx->mMetaInput->findInt32(kKeyFrameH, &video_h);

    ctx->mThread = new RtThread(render_loop, reinterpret_cast<void*>(this));
    ctx->mThread->setName("SinkGLES2.0");
    return RT_OK;
}

RT_RET RTNodeSinkGLES::release() {
    RTSinkGLESCtx* ctx = reinterpret_cast<RTSinkGLESCtx*>(mNodeContext);
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
    rt_safe_free(mNodeContext);
    return RT_OK;
}

RT_RET RTNodeSinkGLES::pullBuffer(RTMediaBuffer **media_buf) {
    // make media_buf RT_NULL only.NodeSink doesn't allocate buffer
    *media_buf  = RT_NULL;
    RT_RET  err = RT_ERR_NULL_PTR;
    RTSinkGLESCtx*  ctx   = reinterpret_cast<RTSinkGLESCtx*>(mNodeContext);
    RT_DequeEntry   entry = deque_pop(ctx->mDequePacket);
    if (RT_NULL != entry.data) {
         *media_buf = reinterpret_cast<RTMediaBuffer*>(entry.data);
         err = RT_OK;
    }
    return err;
}

RT_RET RTNodeSinkGLES::pushBuffer(RTMediaBuffer*  media_buf) {
    RTFrame rt_frame;
    RT_RET  err = RT_ERR_NULL_PTR;
    RTSinkGLESCtx* ctx = reinterpret_cast<RTSinkGLESCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    if (RT_NULL != media_buf) {
        rt_mediabuf_goto_frame(media_buf, &rt_frame);
        RT_LOGE("rt_frame(%p - %dx%d)\n", rt_frame.mData,
                  rt_frame.mFrameW,  rt_frame.mFrameH);
        err = deque_push_tail(ctx->mDequePacket, media_buf);
    }

    return err;
}

RT_RET RTNodeSinkGLES::runCmd(RT_NODE_CMD cmd, RtMetaData *metadata) {
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

RT_RET RTNodeSinkGLES::setEventLooper(RTMsgLooper* eventLooper) {
    RTSinkGLESCtx* ctx = reinterpret_cast<RTSinkGLESCtx*>(mNodeContext);
    ctx->mEventLooper = eventLooper;
    return RT_OK;
}

RtMetaData* RTNodeSinkGLES::queryFormat(RTPortType port) {
    RTSinkGLESCtx* ctx = reinterpret_cast<RTSinkGLESCtx*>(mNodeContext);
    if (RT_PORT_INPUT == port) {
        return ctx->mMetaInput;
    }
    return RT_NULL;
}

RTNodeStub* RTNodeSinkGLES::queryStub() {
    return &rt_sink_display_gles;
}

// override RTNode protected method
RT_RET RTNodeSinkGLES::onStart() {
    RT_RET            err = RT_OK;
    RTSinkGLESCtx* ctx = reinterpret_cast<RTSinkGLESCtx*>(mNodeContext);
    ctx->mLoop = RT_FALSE;
    ctx->mThread->start();
    return err;
}

RT_RET RTNodeSinkGLES::onPause() {
    return RT_ERR_UNIMPLIMENTED;
}

RT_RET RTNodeSinkGLES::onStop() {
    RTSinkGLESCtx* ctx = reinterpret_cast<RTSinkGLESCtx*>(mNodeContext);
    ctx->mThread->join();
    return RT_OK;
}

RT_RET RTNodeSinkGLES::onReset() {
    return RT_ERR_UNIMPLIMENTED;
}

RT_RET RTNodeSinkGLES::onFlush() {
    return RT_ERR_UNIMPLIMENTED;
}

static RTNode* createSinkDisplay() {
    return new RTNodeSinkGLES();
}

struct RTNodeStub rt_sink_display_gles {
    .mCreateNode     = createSinkDisplay,
    .mNodeType       = RT_NODE_TYPE_SINK,
    .mUsePool        = RT_FALSE,
    .mNodeName       = "rt_sink_gles2.0",
    .mNodeVersion    = "v1.0",
};
