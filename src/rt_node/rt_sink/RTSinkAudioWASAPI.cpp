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
 * Author:shika.zhou@rock-chips.com
 *   Date: 2019/03/11
 * Module: output audio pcm with WASAPI
 *
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTSinkAudioWAS"

#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x0

#include "RTSinkAudioWASAPI.h" // NOLINT
#include "rt_metaData.h"       // NOLINT
#include "rt_message.h"        // NOLINT
#include "RTMediaMetaKeys.h"   // NOLINT
#include "RTMediaBuffer.h"     // NOLINT

typedef struct _RTSinkAudioCtx {
    RtMetaData         *mMetaInput;
    RtMutex            *mDataLock;
    RTMsgLooper        *mEventLooper;
    RT_Deque           *mDequeFrame;
    RtThread           *mThread;
    INT32               mVolume;
    RT_BOOL             mMute;
} RTSinkAudioCtx;


void* sink_audio_was_loop(void* ptrNode) {
    RTSinkAudioWASAPI* audiosink = reinterpret_cast<RTSinkAudioWASAPI*>(ptrNode);
    audiosink->runTask();
    return RT_NULL;
}

RTSinkAudioWASAPI::RTSinkAudioWASAPI() {
    RTSinkAudioCtx* ctx = rt_malloc(RTSinkAudioCtx);
    rt_memset(ctx, 0, sizeof(RTSinkAudioCtx));

    ctx->mDataLock = new RtMutex();
    RT_ASSERT(RT_NULL != ctx->mDataLock);

    ctx->mThread = new RtThread(sink_audio_was_loop, reinterpret_cast<void*>(this));
    ctx->mThread->setName("SinkWAS");

    ctx->mDequeFrame = deque_create(10);
    RT_ASSERT(RT_NULL != ctx->mDequeFrame);

    // save private context to mNodeContext
    mNodeContext = ctx;
}

RTSinkAudioWASAPI::~RTSinkAudioWASAPI() {
    release();
    mNodeContext = RT_NULL;
}

// override RTNode methods
RT_RET RTSinkAudioWASAPI::init(RtMetaData *metaData) {
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    ctx->mMetaInput = metaData;
    return RT_OK;
}

RT_RET RTSinkAudioWASAPI::release() {
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    if (RT_NULL != ctx->mDataLock) {
        delete ctx->mDataLock;
        ctx->mDataLock = RT_NULL;
    }

    if (ctx->mThread != RT_NULL) {
        delete ctx->mThread;
        ctx->mThread = RT_NULL;
    }

    if (ctx->mDequeFrame != NULL) {
        deque_destory(&ctx->mDequeFrame);
    }
    rt_safe_free(mNodeContext);
    return RT_OK;
}

RT_RET RTSinkAudioWASAPI::pullBuffer(RTMediaBuffer** media_buf) {
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    *media_buf = RT_NULL;
    RT_RET  err = RT_ERR_NULL_PTR;
    RT_DequeEntry entry = deque_pop(ctx->mDequeFrame);

    if (RT_NULL != entry.data) {
        *media_buf = reinterpret_cast<RTMediaBuffer*>(entry.data);
        err = RT_OK;
    } else {
        err = RT_ERR_NULL_PTR;
    }

    return err;
}

RT_RET RTSinkAudioWASAPI::pushBuffer(RTMediaBuffer* media_buf) {
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    RT_RET  err = RT_ERR_NULL_PTR;

    if (RT_NULL != media_buf) {
        err = deque_push_tail(ctx->mDequeFrame, media_buf);
    }

    return err;
}

RT_RET RTSinkAudioWASAPI::runCmd(RT_NODE_CMD cmd, RtMetaData *metaData) {
    RT_RET err = RT_OK;
    switch (cmd) {
    case RT_NODE_CMD_INIT:
        err = this->init(metaData);
        break;
    case RT_NODE_CMD_START:
        err = this->onStart();
        break;
    case RT_NODE_CMD_STOP:
        err = this->onStop();
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

RT_RET RTSinkAudioWASAPI::runTask() {
    RTMediaBuffer *input = NULL;
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    while (THREAD_LOOP == ctx->mThread->getState()) {
        if (!input) {
            this->pullBuffer(&input);
        }

        if (!input || !input->getData()) {
            RtTime::sleepMs(5);
            continue;
        }

        INT32 eos = 0;
        input->getMetaData()->findInt32(kKeyFrameEOS, &eos);

        if (callback_ptr) {
            queueCodecBuffer(callback_ptr, input);
            input = NULL;
        } else {
            RT_LOGE("callback_ptr is NULL!");
        }

        if (eos && (RT_NULL != ctx->mEventLooper)) {
            RT_LOGD("render EOS Flag, post EOS message");
            RTMessage* eosMsg = new RTMessage(RT_MEDIA_PLAYBACK_COMPLETE, nullptr, nullptr);
            ctx->mEventLooper->post(eosMsg);
            return RT_OK;
        }

        // dump AVFrame
        RtTime::sleepMs(5);
    }
    return RT_OK;
}

RT_RET RTSinkAudioWASAPI::setEventLooper(RTMsgLooper* eventLooper) {
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    ctx->mEventLooper = eventLooper;
    return RT_OK;
}

RtMetaData* RTSinkAudioWASAPI::queryFormat(RTPortType port) {
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    if (RT_PORT_INPUT == port) {
        return ctx->mMetaInput;
    }
    return RT_NULL;
}

RTNodeStub* RTSinkAudioWASAPI::queryStub() {
    return &rt_sink_audio_wasapi;
}

// override RTNodeAudioSink methods
RT_RET RTSinkAudioWASAPI::SetVolume(INT32 volume) {
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    ctx->mVolume = volume;
    return RT_OK;
}

// override RTNodeAudioSink methods
INT32 RTSinkAudioWASAPI::GetVolume() {
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    return ctx->mVolume;
}

// override RTNodeAudioSink methods
RT_BOOL RTSinkAudioWASAPI::GetMute() {
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    return ctx->mMute;
}

// override RTNodeAudioSink methods
RT_RET RTSinkAudioWASAPI::Mute(RT_BOOL muted) {
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    ctx->mMute = muted;
    return RT_OK;
}

// override RTNode methods
RT_RET RTSinkAudioWASAPI::onStart() {
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    RT_RET err = RT_OK;
    RT_LOGD("Audio Sink Thread... begin");
    ctx->mThread->start();
    return err;
}

// override RTNode methods
RT_RET RTSinkAudioWASAPI::onStop() {
    RTSinkAudioCtx* ctx = reinterpret_cast<RTSinkAudioCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    RT_RET err = RT_OK;
    ctx->mThread->requestInterruption();
    ctx->mThread->join();
    return err;
}

// override RTNode methods
RT_RET RTSinkAudioWASAPI::onPause() {
    return RT_OK;
}

// override RTNode methods
RT_RET RTSinkAudioWASAPI::onFlush()  {
    RT_RET err = RT_OK;
    RT_LOGD("call, flush packet and frame in audio sink");
    return err;
}

// override RTNode methods
RT_RET RTSinkAudioWASAPI::onReset() {
    RT_LOGD("call, reset and flush in audio sink");
    return onFlush();
}

static RTNode* createSinkAudioWAS() {
    return new RTSinkAudioWASAPI();
}

struct RTNodeStub rt_sink_audio_wasapi {
    .mCreateNode     = createSinkAudioWAS,
    .mNodeType       = RT_NODE_TYPE_SINK,
    .mUsePool        = RT_FALSE,
    .mNodeName       = "rt_sink_wasapi",
    .mNodeVersion    = "v1.0",
};

extern struct RTNodeStub rt_sink_audio_wasapi;
