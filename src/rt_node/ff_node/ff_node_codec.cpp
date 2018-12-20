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
 *   Task: use ffmpeg as decoder
 */

#include "./include/ff_node_codec.h"  // NOLINT
#include "./include/ffmpeg_wrapper.h" // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FFNodeCodec"

#include "rt_node.h" // NOLINT
#include "rt_metadata.h" // NOLINT
#include "rt_metadata_key.h" // NOLINT
#include "rt_thread.h" // NOLINT
#include "rt_dequeue.h" // NOLINT
#include "rt_media_frame.h" // NOLINT
#include "rt_media_packet.h" // NOLINT

#define MAX_RT_PACKET_COUNT             30
#define MAX_RT_FRAME_COUNT              30

typedef struct FFNodeCodecContext {
    AVCodecContext *av_codec_ctx;
    RtThread       *decode_thread;
    RT_Deque       *packet_queue;
    RT_Deque       *frame_queue;
    RtMutex        *packet_queue_lock;
    RtMutex        *frame_queue_lock;

    RT_BOOL runing;

    UINT32 pull_cnt;
    UINT32 push_cnt;

    RT_RET onInit(RtMetaData *metadata);
    RT_RET onStart();
    RT_RET onStop();

    void decodeLooper();
    RT_RET initPacket(AVPacket *pkt, RtBuffer rt_buffer);
} FFNodeCodecCtx;

void* decode_loop(void* ptr_this) {
    FFNodeCodecCtx* decode_ctx = reinterpret_cast<FFNodeCodecCtx*>(ptr_this);
    decode_ctx->decodeLooper();
    return RT_NULL;
}

RT_RET FFNodeCodecCtx::onInit(RtMetaData *metadata) {
    int err = 0;
    av_codec_ctx = avcodec_alloc_context3(NULL);
    CHECK_IS_NULL(av_codec_ctx);

    av_codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    av_codec_ctx->codec_id = AV_CODEC_ID_H264;
    av_codec_ctx->extradata_size = 0;
    av_codec_ctx->extradata = NULL;
    av_codec_ctx->width = 352;
    av_codec_ctx->height = 288;

    // find decoder again as codec_id may have changed
    av_codec_ctx->codec = avcodec_find_decoder(av_codec_ctx->codec_id);
    if (NULL == av_codec_ctx->codec) {
        RT_LOGE("ffmpeg decoder codec find failed, id: 0x%x", av_codec_ctx->codec_id);
        return RT_ERR_BAD;
    }

    RT_LOGD("begin to open ffmpeg decoder(%s) now", avcodec_get_name(av_codec_ctx->codec_id));
    err = avcodec_open2(av_codec_ctx, av_codec_ctx->codec, NULL);
    if (err < 0) {
        RT_LOGE("ffmpeg video decoder failed to initialize. (%d)", (err));
        return RT_ERR_BAD;
    }
    RT_LOGD("open ffmpeg video decoder(%s) success", avcodec_get_name(av_codec_ctx->codec_id));

    decode_thread = new RtThread(decode_loop, reinterpret_cast<void*>(this));
    decode_thread->setName("ffmpeg_decode");

    packet_queue = deque_create(MAX_RT_PACKET_COUNT);
    frame_queue = deque_create(MAX_RT_FRAME_COUNT);
    packet_queue_lock = new RtMutex();
    frame_queue_lock = new RtMutex();

    return RT_OK;

__FAILED:
    return RT_ERR_UNKNOWN;
}

RT_RET FFNodeCodecCtx::initPacket(AVPacket *pkt, RtBuffer rt_buffer) {
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

void FFNodeCodecCtx::decodeLooper() {
    RtMediaPacket packet = NULL;
    RtBuffer pkt_buffer = NULL;
    AVFrame *frame = NULL;

    while (runing) {
        {
            RtMutex::RtAutolock autoLock(packet_queue_lock);
            packet = deque_pop(packet_queue).data;
        }
        if (packet) {
            pkt_buffer = rt_media_packet_get_buffer(packet);
        } else {
            RtTime::sleepMs(5);
            continue;
        }

        AVPacket pkt;
        initPacket(&pkt, pkt_buffer);
        if (avcodec_send_packet(av_codec_ctx, &pkt) == AVERROR(EAGAIN)) {
            RT_LOGE("send_packet returned EAGAIN, which is an API violation.\n");
            continue;
        }

        if (NULL == frame) {
            frame = av_frame_alloc();
        }
        if (avcodec_receive_frame(av_codec_ctx, frame) == AVERROR(EAGAIN)) {
            RT_LOGE("receive_frame returned EAGAIN, which is an API violation.\n");
            continue;
        }

        if (frame) {
            RtMutex::RtAutolock autoLock(frame_queue_lock);
            deque_push(frame_queue, frame);
            frame = NULL;
        }

        av_packet_unref(&pkt);
    }
}

RT_RET FFNodeCodecCtx::onStart() {
    RT_RET ret = RT_OK;
    runing = RT_TRUE;
    decode_thread->start();
    return ret;
}

RT_RET FFNodeCodecCtx::onStop() {
    RT_RET ret = RT_OK;
    runing = RT_FALSE;
    decode_thread->join();
    return ret;
}

RT_RET ff_node_codec_init(void **ctx) {
    *ctx = rt_malloc(FFNodeCodecCtx);
    rt_memset(*ctx, 0, sizeof(FFNodeCodecCtx));
    return RT_OK;
}

RT_RET ff_node_codec_release(void **ctx) {
    FFNodeCodecCtx* decode_ctx = reinterpret_cast<FFNodeCodecCtx*>(*ctx);
    CHECK_IS_NULL(decode_ctx);
    if (decode_ctx->packet_queue) {
        // clear packet list
        RtMutex::RtAutolock autoLock(decode_ctx->packet_queue_lock);
        deque_destory(&decode_ctx->packet_queue);
        decode_ctx->packet_queue = NULL;
    }
    if (decode_ctx->packet_queue_lock) {
        delete decode_ctx->packet_queue_lock;
        decode_ctx->packet_queue_lock = NULL;
    }

    if (decode_ctx->frame_queue) {
        // clear av frame list
        RtMutex::RtAutolock autoLock(decode_ctx->frame_queue_lock);
        do {
            AVFrame *frame = reinterpret_cast<AVFrame *>
                                 (deque_pop(decode_ctx->frame_queue).data);
            if (frame) {
                av_frame_free(&frame);
                frame = NULL;
            } else {
                break;
            }
        } while (1);
        deque_destory(&decode_ctx->frame_queue);
        decode_ctx->frame_queue = NULL;
    }
    if (decode_ctx->frame_queue_lock) {
        delete decode_ctx->frame_queue_lock;
        decode_ctx->frame_queue_lock = NULL;
    }

    // thread release
    delete(decode_ctx->decode_thread);
    decode_ctx->decode_thread = NULL;
    rt_safe_free(*ctx);

    return RT_OK;

__FAILED:
    return RT_ERR_UNKNOWN;
}

RT_RET ff_node_codec_pull(void *ctx, RtMediaFrame frame, UINT32 *size) {
    (void)size;
    FFNodeCodecCtx* decode_ctx = reinterpret_cast<FFNodeCodecCtx*>(ctx);
    AVFrame *av_frame = NULL;
    RtBuffer frame_buffer = NULL;
    RtMetaData *meta = NULL;
    RT_ASSERT(RT_NULL != decode_ctx);
    decode_ctx->pull_cnt++;

    // get av frame from decoded queue.
    {
        RtMutex::RtAutolock autoLock(decode_ctx->frame_queue_lock);
        av_frame = reinterpret_cast<AVFrame *>
                       (deque_pop(decode_ctx->frame_queue).data);
        if (!av_frame) {
            return RT_ERR_LIST_EMPTY;
        }
    }


    {
        uint8_t *data[4];
        int linesize[4];
        int i;
        int64_t pts = AV_NOPTS_VALUE;
        uint8_t *dst = NULL;

        frame_buffer = rt_media_frame_get_buffer(frame);
        meta = rt_buffer_get_metadata(frame_buffer);
        dst = rt_buffer_get_data(frame_buffer);
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

        rt_buffer_set_range(frame_buffer, 0, av_frame->width * av_frame->height * 3 / 2);
        if (av_frame->pts == AV_NOPTS_VALUE) {
            pts = 0;
        } else {
            pts = av_frame->pts;
        }
        meta->setInt64(kKeyTimeStamps, pts);
        meta->setInt32(kKeyFrameWidth, av_frame->width);
        meta->setInt32(kKeyFrameHeight, av_frame->height);
        av_frame_unref(av_frame);
        rt_media_frame_set_status(frame, RT_MEDIA_FRAME_STATUS_READY);
    }
    return RT_OK;
}

RT_RET ff_node_codec_push(void *ctx, RtMediaPacket packet, UINT32 *size) {
    RT_RET ret = RT_OK;
    FFNodeCodecCtx* decode_ctx = reinterpret_cast<FFNodeCodecCtx*>(ctx);
    RT_ASSERT(RT_NULL != decode_ctx);
    decode_ctx->push_cnt++;

    {
        RtMutex::RtAutolock autoLock(decode_ctx->packet_queue_lock);
        if (packet) {
            ret = deque_push(decode_ctx->packet_queue, packet);
            if (ret) {
                RT_LOGE("push packet to queue failed! ret: 0x%x", ret);
                return ret;
            }
        }
    }
    return RT_OK;
}

RT_RET ff_node_codec_run_cmd(void* ctx, RT_NODE_CMD cmd,
                            RtMetaData *metadata) {
    RT_RET ret = RT_OK;
    FFNodeCodecCtx* decode_ctx = reinterpret_cast<FFNodeCodecCtx*>(ctx);
    RT_ASSERT(RT_NULL != decode_ctx);
    switch (cmd) {
    case RT_NODE_CMD_INIT:
        ret = decode_ctx->onInit(metadata);
        break;
    case RT_NODE_CMD_START:
        ret = decode_ctx->onStart();
        break;
    case RT_NODE_CMD_STOP:
        ret = decode_ctx->onStop();
        break;
    default:
        RT_LOGE("unkown command: %d", cmd);
        ret = RT_ERR_UNKNOWN;
        break;
    }

    return ret;
}

RT_Node ff_node_video_decoder = {
    .type         = RT_NODE_TYPE_DECODER,
    .name         = "ff_node_codec",
    .version      = "v1.0",
    .node_ctx     = RT_NULL,
    .impl_init    = ff_node_codec_init,
    .impl_release = ff_node_codec_release,
    .impl_pull    = ff_node_codec_pull,
    .impl_push    = ff_node_codec_push,
    .impl_run_cmd = ff_node_codec_run_cmd,
    .impl_query_cap = NULL,
};

