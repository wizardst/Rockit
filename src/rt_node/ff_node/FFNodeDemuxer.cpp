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
 * Author: Hery.xu@rock-chips.com
 *   Date: 2018/12/10
 *   Task: use ffmpeg as demuxer and muxer
 *
 * author: martin.cheng@rock-chips.com
 *   Date: 2019/01/04
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FFNodeDemuxer"

#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x1

#include "FFNodeDemuxer.h" // NOLINT
#include "FFMPEGAdapter.h" // NOLINT
#include "rt_mem.h" // NOLINT
#include "rt_array_list.h" // NOLINT
#include "rt_thread.h" // NOLINT
#include "RTMediaMetaKeys.h" // NOLINT
#include "rt_buffer.h" // NOLINT
#include "rt_metadata.h" // NOLINT
#include "rt_common.h" // NOLINT

typedef struct _FFNodeDemuxerCtx {
    FAFormatContext    *mFormatCtx;
    RtMetaData         *mMetaInput;
    RtMetaData         *mMetaOutput;
    RtArrayList        *mListPacket;
    RtThread           *mThread;
    RtMutex            *mLockPacket;
    RTMsgLooper        *mEventLooper;

    UINT32              mEosFlag;
    UINT32              mCountPull;
    UINT32              mCountPush;

    INT32               mIndexVideo;
    INT32               mIndexAudio;
    INT32               mIndexSubtitle;
} FFNodeDemuxerCtx;

void* ff_demuxer_loop(void* ptr_node) {
    FFNodeDemuxer* nodeDemuxer = reinterpret_cast<FFNodeDemuxer*>(ptr_node);
    nodeDemuxer->runTask();
    return RT_NULL;
}

FFNodeDemuxer::FFNodeDemuxer() {
    FFNodeDemuxerCtx* ctx = rt_malloc(FFNodeDemuxerCtx);
    rt_memset(ctx, 0, sizeof(FFNodeDemuxerCtx));

    ctx->mListPacket = array_list_create();
    RT_ASSERT(RT_NULL != ctx->mListPacket);

    ctx->mLockPacket = new RtMutex();
    RT_ASSERT(RT_NULL != ctx->mLockPacket);

    ctx->mThread = new RtThread(ff_demuxer_loop, reinterpret_cast<void*>(this));
    ctx->mThread->setName("FFDemuxer");

    // save private context to mNodeContext
    mNodeContext = ctx;
}

FFNodeDemuxer::~FFNodeDemuxer() {
    release();
    mNodeContext = RT_NULL;
}


RT_RET FFNodeDemuxer::init(RtMetaData *metadata) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);

    // av_register_all();
    avformat_network_init();

    const char *uri;
    metadata->findCString(kKeyFormatUri, &uri);
    RT_ASSERT(RT_NULL != uri);

    RT_LOGD_IF(DEBUG_FLAG, "uri = %s", uri);

    ctx->mFormatCtx = fa_format_open(uri, FLAG_DEMUXER);

    #if TO_DO_FLAG
    if (avformat_find_stream_info(ctx->mFormatCtx, NULL) < 0) {
        RT_LOGE("Couldn't find stream information.\n");
        return RT_ERR_UNKNOWN;
    }
    #endif
    ctx->mMetaInput = metadata;

    return RT_OK;
}

RT_RET FFNodeDemuxer::release() {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    if (ctx->mListPacket != NULL) {
        array_list_remove_all(ctx->mListPacket);
        array_list_destroy(ctx->mListPacket);
        ctx->mListPacket = RT_NULL;
    }

    if (ctx->mThread != RT_NULL) {
        delete ctx->mThread;
        ctx->mThread = RT_NULL;
    }

    if (ctx->mLockPacket != RT_NULL) {
        delete ctx->mLockPacket;
        ctx->mLockPacket = RT_NULL;
    }

    if (ctx->mMetaInput != RT_NULL) {
        delete ctx->mMetaInput;
        ctx->mMetaInput = RT_NULL;
    }

    if (ctx->mMetaOutput != RT_NULL) {
        delete ctx->mMetaOutput;
        ctx->mMetaOutput = RT_NULL;
    }

    if (ctx->mFormatCtx != RT_NULL) {
        fa_format_close(ctx->mFormatCtx);
        ctx->mFormatCtx = RT_NULL;
    }
    rt_safe_free(ctx);

    return RT_OK;
}

RT_RET FFNodeDemuxer::pullBuffer(RTMediaBuffer** rtPacket) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);
    RT_ASSERT(RT_NULL != rtPacket);

    RtMetaData* metaData = (*rtPacket)->getMetaData();
    void*       raw_pkt  = array_list_get_data(ctx->mListPacket, 0);
    RTPacket    rt_pkt   = {0};

    if (RT_NULL == raw_pkt) {
        RT_LOGD("packet NULL");
        (*rtPacket)->setData(RT_NULL, 0);
        if (ctx->mEosFlag) {
            return RT_OK;
        }
        return RT_ERR_UNKNOWN;
    } else {
        RtMutex::RtAutolock autoLock(ctx->mLockPacket);
        fa_format_parse_packet(raw_pkt, &rt_pkt);
        RT_LOGD(" --> RTPacket(ptr=%p, size=%d)", rt_pkt.mRawPkt, rt_pkt.mSize);
        array_list_remove_at(ctx->mListPacket, 0);
        (*rtPacket)->setData(rt_pkt.mData, rt_pkt.mSize);
        if (RT_NULL != metaData) {
            rt_utils_pkt_to_meta(&rt_pkt, metaData);
        }
        return RT_OK;
    }
    return RT_OK;
}

RT_RET FFNodeDemuxer::pushBuffer(RTMediaBuffer* data) {
    return RT_ERR_UNIMPLIMENTED;
}

RT_RET FFNodeDemuxer::runCmd(RT_NODE_CMD cmd, RtMetaData *metadata) {
    // RT_LOGD(" cmd = %d; metadata = %p", cmd, metadata);
    switch (cmd) {
    case RT_NODE_CMD_INIT:
        this->init(metadata);
        break;
    case RT_NODE_CMD_START:
        this->onStart();
        break;
    case RT_NODE_CMD_STOP:
        this->onStop();
        break;
    case RT_NODE_CMD_SEEK:
        break;
    default:
        RT_LOGE("demuxer not support the cmd\n");
        break;
    }
    return RT_OK;
}

RT_RET FFNodeDemuxer::setEventLooper(RTMsgLooper* eventLooper) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    ctx->mEventLooper = eventLooper;
}

RtMetaData* FFNodeDemuxer::queryFormat(RTPortType port) {
    return RT_NULL;
}

RTNodeStub* FFNodeDemuxer::queryStub() {
    return &ff_node_demuxer;
}

INT32 FFNodeDemuxer::countTracks(RTTrackType tType) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    fa_format_count_tracks(ctx->mFormatCtx, tType);
}

INT32 FFNodeDemuxer::selectTrack(INT32 index, RTTrackType tType) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);

    switch (tType) {
    case RTTRACK_TYPE_VIDEO:
        ctx->mIndexVideo = index;
        break;
    case RTTRACK_TYPE_AUDIO:
        ctx->mIndexAudio = index;
        break;
    case RTTRACK_TYPE_SUBTITLE:
        ctx->mIndexSubtitle = index;
        break;
    default:
        break;
    }
}

RtMetaData* FFNodeDemuxer::queryTrack(UINT32 index) {
    return RT_NULL;
}

RT_RET FFNodeDemuxer::onStart() {
    RT_RET            err = RT_OK;
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    ctx->mEosFlag = RT_FALSE;
    ctx->mThread->start();
    return err;
}

RT_RET FFNodeDemuxer::onStop() {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    ctx->mEosFlag = RT_TRUE;
    ctx->mThread->join();

    // flush all packets in the caches
    onFlush();

    if (ctx->mFormatCtx != RT_NULL) {
        fa_format_close(ctx->mFormatCtx);
        ctx->mFormatCtx = RT_NULL;
    }

    avformat_network_deinit();
    return RT_OK;
}

RT_RET FFNodeDemuxer::onPause() {
    return RT_OK;
}

RT_RET FFNodeDemuxer::onReset() {
    return RT_OK;
}

RT_RET FFNodeDemuxer::onFlush() {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    RTPacket rt_pkt;
    if (ctx->mListPacket != NULL) {
        while (ctx->mListPacket->size > 0) {
            RtMutex::RtAutolock autoLock(ctx->mLockPacket);
            void* raw_pkt = array_list_get_data(ctx->mListPacket, 0);
            fa_format_parse_packet(raw_pkt, &rt_pkt);
            rt_utils_packet_free(&rt_pkt);
            array_list_remove_at(ctx->mListPacket, 0);
        }
    }
    return RT_OK;
}

RT_RET FFNodeDemuxer::runTask() {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    void* raw_pkt = RT_NULL;
    while (RT_FALSE == ctx->mEosFlag) {
        INT32 err = fa_format_read_packet(ctx->mFormatCtx, &raw_pkt);
        if (err < 0) {
            RT_LOGE("%s read end", __FUNCTION__);
            ctx->mEosFlag = RT_TRUE;
            continue;
        }

        #if TO_DO_FLAG
            int flags = 0;
            ret = avformat_seek_file(ctx->mFormatCtx, 0/*seekStrmIdx*/,
                                     RT_INT64_MIN, 0/*timeus*/, RT_INT64_MAX, flags);
        #endif
        do {
            RtMutex::RtAutolock autoLock(ctx->mLockPacket);
            array_list_add(ctx->mListPacket, raw_pkt);
        } while (0);
        RtTime::sleepUs(2000);
    }
    RT_LOGD_IF(DEBUG_FLAG, "thread done");
    return RT_OK;
}

static RTNode* createFFDemuxer() {
    return new FFNodeDemuxer();
}

struct RTNodeStub ff_node_demuxer {
    .mCreateNode     = createFFDemuxer,
    .mNodeType       = RT_NODE_TYPE_DEMUXER,
    .mUsePool        = RT_FALSE,
    .mNodeName       = "ff_node_demuxer",
    .mNodeVersion    = "v1.0",
};
