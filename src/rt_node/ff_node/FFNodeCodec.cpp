/*
 * Copyright 2018 Rockchip Electronics Co. LTD
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
 * Author: rimon.xu@rock-chips.com
 *   Date: 2018/12/11
 * Module: use ffmpeg as decoder
 *
 * author: martin.cheng@rock-chips.com
 *   Date: 2019/01/04
 */

#include "./include/FFNodeCodec.h"  // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FFNodeCodec"

#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x0

#include "FFNodeCodec.h" // NOLINT
#include "RTObjectPool.h" // NOLINT
#include "FFMPEGAdapter.h" // NOLINT
#include "rt_metadata.h" // NOLINT
#include "rt_metadata_key.h" // NOLINT
#include "rt_thread.h" // NOLINT
#include "rt_dequeue.h" // NOLINT
#include "rt_media_frame.h" // NOLINT
#include "rt_media_packet.h" // NOLINT

#define MAX_RT_PACKET_COUNT    30
#define MAX_RT_FRAME_COUNT     30

typedef struct _FFNodeCodecCtx {
    AVCodecContext *mCodecCtx;
    RtThread       *mThread;
    RT_Deque       *mDequePacket;
    RT_Deque       *mDequeFrame;
    RtMutex        *mLockPacket;
    RtMutex        *mLockFrame;
    RTObjectPool   *mPoolPacket;

    RT_BOOL mRunning;
    UINT32  mCountPull;
    UINT32  mCountPush;
} FFNodeCodecCtx;

void* ff_codec_loop(void* ptr_node) {
    FFNodeCodec* nodeCodec = reinterpret_cast<FFNodeCodec*>(ptr_node);
    nodeCodec->runTask();
    return RT_NULL;
}

RT_RET initPacket(AVPacket *pkt, RtBuffer rt_buffer) {
    RtMetaData *meta = rt_buffer_get_metadata(rt_buffer);
    memset(pkt, 0, sizeof(AVPacket));
    av_init_packet(pkt);

    if (rt_buffer) {
        pkt->data = rt_buffer_get_data(rt_buffer);
        pkt->size = rt_buffer_get_size(rt_buffer);
        meta->findInt64(kKeyTimeStamps, &pkt->pts);
        meta->findInt64(kKeyTimeStamps, &pkt->dts);
    } else {
        // empty packet. like eos.
        pkt->data = NULL;
        pkt->size = 0;
        pkt->pts = AV_NOPTS_VALUE;
        pkt->dts = AV_NOPTS_VALUE;
    }

    RT_LOGD("pkt->data: %p, size: %d pts: %lld", pkt->data, pkt->size, pkt->pts);

    return RT_OK;
}

RT_RET initFFCodec(FFNodeCodecCtx* ctx) {
    int err = 0;
    ctx->mCodecCtx = avcodec_alloc_context3(NULL);
    CHECK_IS_NULL(ctx->mCodecCtx);

    ctx->mCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    ctx->mCodecCtx->codec_id = AV_CODEC_ID_H264;
    ctx->mCodecCtx->extradata_size = 0;
    ctx->mCodecCtx->extradata = NULL;
    ctx->mCodecCtx->width = 352;
    ctx->mCodecCtx->height = 288;

    // find decoder again as codec_id may have changed
    ctx->mCodecCtx->codec = avcodec_find_decoder(ctx->mCodecCtx->codec_id);
    if (NULL == ctx->mCodecCtx->codec) {
        RT_LOGE("ffmpeg decoder codec find failed, id: 0x%x", ctx->mCodecCtx->codec_id);
        return RT_ERR_BAD;
    }

    RT_LOGD("begin to open ffmpeg decoder(%s) now", avcodec_get_name(ctx->mCodecCtx->codec_id));
    err = avcodec_open2(ctx->mCodecCtx, ctx->mCodecCtx->codec, NULL);
    if (err < 0) {
        RT_LOGE("ffmpeg video decoder failed to initialize. (%d)", (err));
        return RT_ERR_BAD;
    }
    RT_LOGD("open ffmpeg video decoder(%s) success", avcodec_get_name(ctx->mCodecCtx->codec_id));

__FAILED:
    return RT_ERR_UNKNOWN;
}

RTObject* AllocMediaBuffer() {
    return new RTMediaBuffer(RT_NULL, 0);
}

#define MAX_MEDIA_BUFFER_NUM 32

FFNodeCodec::FFNodeCodec(FFCodecType type/*= FFCodecDecoder*/) {
    mCodecType = type;
    FFNodeCodecCtx* ctx = rt_malloc(FFNodeCodecCtx);
    rt_memset(ctx, 0, sizeof(FFNodeCodecCtx));

    const char* name = (mCodecType == FFCodecDecoder)?"FFDecoder":"FFEncoder";
    ctx->mThread = new RtThread(ff_codec_loop, reinterpret_cast<void*>(this));
    ctx->mThread->setName(name);

    ctx->mDequePacket = deque_create(MAX_RT_PACKET_COUNT);
    ctx->mDequeFrame  = deque_create(MAX_RT_FRAME_COUNT);
    ctx->mLockPacket  = new RtMutex();
    ctx->mLockFrame   = new RtMutex();

    const RTNodeStub* stub = RT_NULL;
    switch (mCodecType) {
    case FFCodecDecoder:
        stub = &ff_node_decoder;
        break;
    case FFCodecEncoder:
        stub = &ff_node_encoder;
        break;
    default:
        break;
    }
    ctx->mPoolPacket  = RT_NULL;
    if (RT_TRUE == stub->mUsePool) {
        ctx->mPoolPacket = new RTObjectPool(AllocMediaBuffer, 32);
    }

    mNodeContext = ctx;
}

FFNodeCodec::~FFNodeCodec() {
    release();
    mNodeContext = RT_NULL;
}

RT_RET FFNodeCodec::init(RtMetaData *metadata) {
    FFNodeCodecCtx* ctx = reinterpret_cast<FFNodeCodecCtx*>(mNodeContext);

    initFFCodec(ctx);

    return RT_OK;
}

RT_RET FFNodeCodec::release() {
    FFNodeCodecCtx* ctx = reinterpret_cast<FFNodeCodecCtx*>(mNodeContext);
    CHECK_IS_NULL(ctx);
    if (ctx->mDequePacket) {
        // clear packet list
        RtMutex::RtAutolock autoLock(ctx->mLockPacket);
        deque_destory(&ctx->mDequePacket);
        ctx->mDequePacket = RT_NULL;
    }
    if (ctx->mLockPacket) {
        delete ctx->mLockPacket;
        ctx->mLockPacket = RT_NULL;
    }

    if (ctx->mDequeFrame) {
        // clear av frame list
        RtMutex::RtAutolock autoLock(ctx->mLockFrame);
        AVFrame *frame = RT_NULL;
        do {
            AVFrame *frame = reinterpret_cast<AVFrame *>
                                 (deque_pop(ctx->mDequeFrame).data);
            if (frame) {
                av_frame_free(&frame);
                frame = RT_NULL;
            }
        } while (RT_NULL != frame);
        deque_destory(&ctx->mDequeFrame);
        ctx->mDequeFrame = RT_NULL;
    }
    if (ctx->mLockFrame) {
        delete ctx->mLockFrame;
        ctx->mLockFrame = RT_NULL;
    }

    // thread release
    delete(ctx->mThread);
    ctx->mThread = RT_NULL;

    rt_safe_free(ctx);

    return RT_OK;

__FAILED:
    return RT_ERR_UNKNOWN;
}

RT_RET FFNodeCodec::pullBuffer(RTMediaBuffer* frame) {
    RT_RET err          = RT_OK;
    FFNodeCodecCtx* ctx = reinterpret_cast<FFNodeCodecCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);
    ctx->mCountPull++;

    AVFrame    *av_frame  = NULL;
    RtBuffer    rt_buffer = NULL;
    RtMetaData *meta      = NULL;

    // get av frame from decoded queue.
    if (RT_NULL != frame) {
        RtMutex::RtAutolock autoLock(ctx->mLockFrame);
        av_frame = reinterpret_cast<AVFrame *>(deque_pop(ctx->mDequeFrame).data);
        if (!av_frame) {
            return RT_ERR_LIST_EMPTY;
        }
    }

    int      i;
    uint8_t *data[4];
    int      linesize[4];
    int64_t  pts = AV_NOPTS_VALUE;
    uint8_t *dst = NULL;

    rt_buffer = rt_media_frame_get_buffer(frame);
    meta = rt_buffer_get_metadata(rt_buffer);
    dst = rt_buffer_get_data(rt_buffer);
    data[0] = dst;
    data[1] = dst + av_frame->width * av_frame->height;
    data[2] = data[1] + (av_frame->width / 2  * av_frame->height / 2);
    linesize[0] = av_frame->width;
    linesize[1] = av_frame->width / 2;
    linesize[2] = av_frame->width / 2;

    RT_LOGD("frame_width=%d frame_height=%d timstamps: %lld",
            av_frame->width, av_frame->height, av_frame->pts);

    // TODO(format) just used yuv420
    for (i = 0; i < av_frame->height; i++) {
       rt_memcpy(dst, av_frame->data[0] + av_frame->linesize[0] * i, av_frame->width);
       dst += av_frame->width;
    }

    for (i = 0; i < av_frame->height / 2; i++) {
        rt_memcpy(dst, av_frame->data[1] + av_frame->linesize[1] * i, av_frame->width / 2);
        dst += av_frame->width / 2;
    }
    for (i = 0; i < av_frame->height / 2; i++) {
        rt_memcpy(dst, av_frame->data[2] + av_frame->linesize[2] * i, av_frame->width / 2);
        dst += av_frame->width / 2;
    }

    rt_buffer_set_range(rt_buffer, 0, av_frame->width * av_frame->height * 3 / 2);
    if (av_frame->pts == AV_NOPTS_VALUE) {
        pts = 0;
    } else {
        pts = av_frame->pts;
    }
    meta->setInt64(kKeyTimeStamps,  pts);
    meta->setInt32(kKeyFrameWidth,  av_frame->width);
    meta->setInt32(kKeyFrameHeight, av_frame->height);
    av_frame_unref(av_frame);
    rt_media_frame_set_status(frame, RT_MEDIA_FRAME_STATUS_READY);

    return err;
}

RT_RET FFNodeCodec::dequePoolBuffer(RTMediaBuffer** data) {
    RT_RET err = RT_OK;
    FFNodeCodecCtx* ctx = reinterpret_cast<FFNodeCodecCtx*>(mNodeContext);
    if (RT_NULL != ctx->mPoolPacket) {
        *data = reinterpret_cast<RTMediaBuffer*>(ctx->mPoolPacket->borrowObj());
        err   = (RT_NULL != *data)? RT_OK : RT_ERR_NULL_PTR;
    }
    return err;
}

RT_RET FFNodeCodec::pushBuffer(RTMediaBuffer* packet) {
    RT_RET err          = RT_OK;
    FFNodeCodecCtx* ctx = reinterpret_cast<FFNodeCodecCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);
    ctx->mCountPush++;

    if (packet) {
        RtMutex::RtAutolock autoLock(ctx->mLockPacket);
        err = ctx->mPoolPacket->returnObj(packet);
        // err = deque_push(ctx->mDequePacket, packet);
        if (err) {
            RT_LOGE("push packet to queue failed! ret: 0x%x", err);
            return err;
        }
    }
    return err;
}

RT_RET FFNodeCodec::runCmd(RT_NODE_CMD cmd, RtMetaData *metadata) {
    RT_RET err = RT_OK;
    switch (cmd) {
    case RT_NODE_CMD_INIT:
        err = this->init(metadata);
        break;
    case RT_NODE_CMD_START:
        err = this->onStart();
        break;
    case RT_NODE_CMD_STOP:
        err = this->onStop();
        break;
    case RT_NODE_CMD_FLUSH:
        err = this->onFlush();
        break;
    case RT_NODE_CMD_PAUSE:
        err = this->onPause();
        break;
    default:
        RT_LOGE("unkown command: %d", cmd);
        err = RT_ERR_UNKNOWN;
        break;
    }

    return err;
}

RtMetaData* FFNodeCodec::queryCap() {
    return RT_NULL;
}

RTNodeStub* FFNodeCodec::queryInfo() {
    RTNodeStub* info = RT_NULL;
    switch (mCodecType) {
    case FFCodecDecoder:
        info = &ff_node_decoder;
        break;
    case FFCodecEncoder:
        info = &ff_node_encoder;
        break;
    default:
        break;
    }
    return info;
}

RT_RET FFNodeCodec::runTask() {
    RtMediaPacket packet     = NULL;
    RtBuffer      pkt_buffer = NULL;
    AVFrame      *frame      = NULL;
    FFNodeCodecCtx* nodeCtx  = reinterpret_cast<FFNodeCodecCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != nodeCtx);

    while (nodeCtx->mRunning) {
        do {
            RtMutex::RtAutolock autoLock(nodeCtx->mLockPacket);
            packet = deque_pop(nodeCtx->mDequePacket).data;
        } while (0);

        if (packet) {
            pkt_buffer = rt_media_packet_get_buffer(packet);
        } else {
            RtTime::sleepMs(5);
            continue;
        }

        AVPacket pkt;
        initPacket(&pkt, pkt_buffer);
        if (avcodec_send_packet(nodeCtx->mCodecCtx, &pkt) == AVERROR(EAGAIN)) {
            RT_LOGE("send_packet returned EAGAIN, which is an API violation.\n");
            continue;
        }

        if (RT_NULL == frame) {
            frame = av_frame_alloc();
        }
        if (avcodec_receive_frame(nodeCtx->mCodecCtx, frame) == AVERROR(EAGAIN)) {
            RT_LOGE("receive_frame returned EAGAIN, which is an API violation.\n");
            continue;
        }

        if (RT_NULL != frame) {
            RtMutex::RtAutolock autoLock(nodeCtx->mLockFrame);
            deque_push(nodeCtx->mDequeFrame, frame);
            frame = NULL;
        }

        av_packet_unref(&pkt);
    }
    return RT_OK;
}

RT_RET FFNodeCodec::onStart() {
    RT_RET          err = RT_OK;
    FFNodeCodecCtx* ctx = reinterpret_cast<FFNodeCodecCtx*>(mNodeContext);
    ctx->mRunning = RT_TRUE;
    ctx->mThread->start();
    return err;
}

RT_RET FFNodeCodec::onStop() {
    RT_RET          err = RT_OK;
    FFNodeCodecCtx* ctx = reinterpret_cast<FFNodeCodecCtx*>(mNodeContext);
    ctx->mRunning = RT_FALSE;
    ctx->mThread->join();
    return err;
}

RT_RET FFNodeCodec::onPause() {
    return RT_OK;
}

RT_RET FFNodeCodec::onFlush() {
    return RT_OK;
}

static RTNode* createFFDecoder() {
    return new FFNodeCodec(FFCodecDecoder);
}

static RTNode* createFFEncoder() {
    return new FFNodeCodec(FFCodecEncoder);
}

struct RTNodeStub ff_node_decoder {
    .mCreateNode   = createFFDecoder,
    .mNodeType     = RT_NODE_TYPE_DECODER,
    .mUsePool      = RT_TRUE,
    .mNodeName     = "ff_node_decoder",
    .mNodeVersion  = "v1.0",
};

struct RTNodeStub ff_node_encoder {
    .mCreateNode   = createFFEncoder,
    .mNodeType     = RT_NODE_TYPE_ENCODER,
    .mUsePool      = RT_TRUE,
    .mNodeName     = "ff_node_encoder",
    .mNodeVersion  = "v1.0",
};

