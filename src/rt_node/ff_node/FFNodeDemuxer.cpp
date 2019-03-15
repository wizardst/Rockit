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
#define DEBUG_FLAG 0x0

#include "FFNodeDemuxer.h"   // NOLINT
#include "FFMPEGAdapter.h"   // NOLINT
#include "rt_mem.h"          // NOLINT
#include "rt_array_list.h"   // NOLINT
#include "rt_thread.h"       // NOLINT
#include "RTMediaMetaKeys.h" // NOLINT
#include "rt_buffer.h"       // NOLINT
#include "rt_metadata.h"     // NOLINT
#include "rt_common.h"       // NOLINT

typedef struct _FFNodeDemuxerCtx {
    FAFormatContext    *mFormatCtx;
    RtMetaData         *mMetaInput;
    RtMetaData         *mMetaOutput;
    RtArrayList        *mVideoPktList;
    RtArrayList        *mAudioPktList;
    RtThread           *mThread;
    RtMutex            *mLockVideoPacket;
    RtMutex            *mLockAudioPacket;
    RTMsgLooper        *mEventLooper;

    UINT32              mEosFlag;
    UINT32              mCountPull;
    UINT32              mCountPush;

    INT32               mIndexVideo;
    INT32               mIndexAudio;
    INT32               mIndexSubtitle;

    INT32               mNeedSeek;
    INT64               mSeekTimeUs;
} FFNodeDemuxerCtx;

void* ff_demuxer_loop(void* ptr_node) {
    FFNodeDemuxer* nodeDemuxer = reinterpret_cast<FFNodeDemuxer*>(ptr_node);
    nodeDemuxer->runTask();
    return RT_NULL;
}

FFNodeDemuxer::FFNodeDemuxer() {
    FFNodeDemuxerCtx* ctx = rt_malloc(FFNodeDemuxerCtx);
    rt_memset(ctx, 0, sizeof(FFNodeDemuxerCtx));

    ctx->mVideoPktList = array_list_create();
    RT_ASSERT(RT_NULL != ctx->mVideoPktList);

    ctx->mAudioPktList = array_list_create();
    RT_ASSERT(RT_NULL != ctx->mAudioPktList);

    ctx->mLockVideoPacket = new RtMutex();
    RT_ASSERT(RT_NULL != ctx->mLockVideoPacket);

    ctx->mLockAudioPacket = new RtMutex();
    RT_ASSERT(RT_NULL != ctx->mLockAudioPacket);

    ctx->mThread = new RtThread(ff_demuxer_loop, reinterpret_cast<void*>(this));
    ctx->mThread->setName("FFDemuxer");

    // save private context to mNodeContext
    mNodeContext = ctx;
}

FFNodeDemuxer::~FFNodeDemuxer() {
    this->release();
    mNodeContext = RT_NULL;
    RT_LOGD("done ~FFNodeDemuxer()");
}

INT32 updateDefaultTrack(FAFormatContext* fa_ctx, RTTrackType tType) {
    INT32 bestIndex = fa_format_find_best_track(fa_ctx, tType);
    if (bestIndex < 0) {
        bestIndex = -1;
    }
    return bestIndex;
}

RT_RET FFNodeDemuxer::init(RtMetaData *metaData) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);

    const char *uri;
    metaData->findCString(kKeyFormatUri, &uri);
    RT_ASSERT(RT_NULL != uri);

    ctx->mMetaInput = metaData;
    ctx->mFormatCtx = fa_format_open(uri, FLAG_DEMUXER);
    if (RT_NULL == ctx->mFormatCtx) {
        RT_LOGE("demuxer open url err.\n");
        return RT_ERR_UNKNOWN;
    }

    ctx->mIndexVideo    = updateDefaultTrack(ctx->mFormatCtx, RTTRACK_TYPE_VIDEO);
    ctx->mIndexAudio    = updateDefaultTrack(ctx->mFormatCtx, RTTRACK_TYPE_AUDIO);
    ctx->mIndexSubtitle = updateDefaultTrack(ctx->mFormatCtx, RTTRACK_TYPE_SUBTITLE);

    return RT_OK;
}

RT_RET FFNodeDemuxer::release() {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    if (ctx->mThread != RT_NULL) {
        delete ctx->mThread;
        ctx->mThread = RT_NULL;
    }

    // @review: code redundancy but logically reasonable
    // @review: flush video&audio&subtitle packets and close av_format
    this->onReset();

    if (ctx->mLockVideoPacket != RT_NULL) {
        delete ctx->mLockVideoPacket;
        ctx->mLockVideoPacket = RT_NULL;
    }
    if (ctx->mLockAudioPacket != RT_NULL) {
        delete ctx->mLockAudioPacket;
        ctx->mLockAudioPacket = RT_NULL;
    }

    if (ctx->mVideoPktList != RT_NULL) {
        array_list_destroy(ctx->mVideoPktList);
        ctx->mVideoPktList = RT_NULL;
    }

    if (ctx->mAudioPktList != RT_NULL) {
        array_list_destroy(ctx->mAudioPktList);
        ctx->mAudioPktList = RT_NULL;
    }

    if (ctx->mMetaInput != RT_NULL) {
        delete ctx->mMetaInput;
        ctx->mMetaInput = RT_NULL;
    }

    if (ctx->mMetaOutput != RT_NULL) {
        delete ctx->mMetaOutput;
        ctx->mMetaOutput = RT_NULL;
    }

    // @review: release memory of node context
    rt_safe_free(ctx);

    return RT_OK;
}

/* read/pull RTPacket from NodeDemuxer */
RT_RET FFNodeDemuxer::pullBuffer(RTMediaBuffer** mediaBuf) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);
    RT_ASSERT(RT_NULL != mediaBuf);

    void*        raw_pkt  = RT_NULL;
    RTPacket     rt_pkt   = {0};
    RtMetaData*  meta     = (*mediaBuf)->getMetaData();
    RtArrayList* pktList  = RT_NULL;
    RtMutex*     pktLock  = RT_NULL;
    RTTrackType type;
    if (!meta->findInt32(kKeyCodecType, reinterpret_cast<INT32*>(&type))) {
        RT_LOGE("track  type is unset!!");
        return RT_ERR_UNKNOWN;
    }

    if (type == RTTRACK_TYPE_VIDEO) {
        pktLock = ctx->mLockVideoPacket;
        pktList = ctx->mVideoPktList;
    } else if (type == RTTRACK_TYPE_AUDIO) {
        pktLock = ctx->mLockAudioPacket;
        pktList = ctx->mAudioPktList;
    }
    raw_pkt = array_list_get_data(pktList, 0);
    if (RT_NULL == raw_pkt) {
        if (ctx->mEosFlag) {
            RT_LOGD("receive EOS buffer.");
            (*mediaBuf)->setData(RT_NULL, 0);
            meta->setInt32(kKeyFrameEOS, 1);
            return RT_OK;
        }
        return RT_ERR_UNKNOWN;
    } else {
        RtMutex::RtAutolock autoLock(pktLock);
        if (RT_NULL != *mediaBuf) {
            fa_format_packet_parse(ctx->mFormatCtx, raw_pkt, &rt_pkt);
            array_list_remove_at(pktList, 0);

            if (rt_pkt.mSize <= 0) {
                return RT_ERR_UNKNOWN;
            }

             RT_LOGD_IF(DEBUG_FLAG, "RTPacket(ptr=0x%p, size=%d) MediaBuffer=0x%p type: %d", \
                        rt_pkt.mRawPtr, rt_pkt.mSize, *mediaBuf, type);
            (*mediaBuf)->setData(rt_pkt.mData, rt_pkt.mSize, rt_pkt.mFuncFree);
            rt_mediabuf_from_packet(*mediaBuf, &rt_pkt);
        }
        return RT_OK;
    }
    return RT_OK;
}

RT_RET FFNodeDemuxer::pushBuffer(RTMediaBuffer* data) {
    return RT_ERR_UNIMPLIMENTED;
}

RT_RET FFNodeDemuxer::runCmd(RT_NODE_CMD cmd, RtMetaData *metaData) {
    // RT_LOGD(" cmd = %d; metaData = %p", cmd, metaData);
    switch (cmd) {
    case RT_NODE_CMD_INIT:
        this->init(metaData);
        break;
    case RT_NODE_CMD_START:
        this->onStart();
        break;
    case RT_NODE_CMD_STOP:
        this->onStop();
        break;
    case RT_NODE_CMD_PAUSE:
        this->onPause();
        break;
    case RT_NODE_CMD_SEEK:
        RT_LOGD("seek to xxxx");
        this->onSeek(metaData);
        break;
    case RT_NODE_CMD_RESET:
        this->onReset();
        break;
    default:
        RT_LOGE("demuxer not support the cmd\n");
        break;
    }
    return RT_OK;
}

RT_RET FFNodeDemuxer::onSeek(RtMetaData *options) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    INT64 seekTimeUs = 0ll;
    if (!options->findInt64(kKeySeekTimeUs, &seekTimeUs)) {
        RT_LOGE("seek timeUs get failed, seek failed");
        return RT_ERR_UNKNOWN;
    }

    ctx->mNeedSeek   = 1;
    ctx->mSeekTimeUs = seekTimeUs;
    return RT_OK;
}

RT_RET FFNodeDemuxer::setEventLooper(RTMsgLooper* eventLooper) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    ctx->mEventLooper     = eventLooper;
    return RT_OK;
}

RtMetaData* FFNodeDemuxer::queryFormat(RTPortType port) {
    FFNodeDemuxerCtx* ctx  = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    RtMetaData*       meta = RT_NULL;

    switch (port) {
    case RT_PORT_INPUT:
        meta = ctx->mMetaInput;
        break;
    case RT_PORT_OUTPUT:
        meta = ctx->mMetaOutput;
        break;
    default:
        break;
    }
    return meta;
}

RTNodeStub* FFNodeDemuxer::queryStub() {
    return &ff_node_demuxer;
}

INT32 FFNodeDemuxer::countTracks(RTTrackType tType) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    return fa_format_count_tracks(ctx->mFormatCtx, tType);
}

INT32 FFNodeDemuxer::selectTrack(INT32 index, RTTrackType tType) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

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

    return index;
}

RtMetaData* FFNodeDemuxer::queryTrackMeta(UINT32 index, RTTrackType tType) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    RTTrackParms  track_par;
    rt_memset(&track_par, 0, sizeof(RTTrackParms));
    RtMetaData   *track_meta = new RtMetaData();
    fa_format_query_track(ctx->mFormatCtx, index, tType, &track_par);
    rt_medatdata_from_trackpar(track_meta, &track_par);
    return track_meta;
}

INT32 FFNodeDemuxer::queryTrackUsed(RTTrackType tType) {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    INT32 used_idx = -1;
    switch (tType) {
    case RTTRACK_TYPE_VIDEO:
        used_idx = ctx->mIndexVideo;
        break;
    case RTTRACK_TYPE_AUDIO:
        used_idx = ctx->mIndexAudio;
        break;
    case RTTRACK_TYPE_SUBTITLE:
        used_idx = ctx->mIndexSubtitle;
        break;
    default:
        break;
    }
    return used_idx;
}

RT_RET FFNodeDemuxer::onStart() {
    RT_RET            err = RT_OK;
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    ctx->mEosFlag = RT_FALSE;
    if (THREAD_LOOP != ctx->mThread->getState()) {
        ctx->mThread->start();
    }
    return err;
}

RT_RET FFNodeDemuxer::onStop() {
    RT_LOGD_IF(DEBUG_FLAG, "call, stop");
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    ctx->mEosFlag = RT_TRUE;
    ctx->mThread->requestInterruption();
    ctx->mThread->join();

    // flush all packets in the caches
    onFlush();

    if (ctx->mFormatCtx != RT_NULL) {
        fa_format_close(ctx->mFormatCtx);
        ctx->mFormatCtx = RT_NULL;
    }

    RT_LOGD_IF(DEBUG_FLAG, "done, stop");
    return RT_OK;
}

RT_RET FFNodeDemuxer::onPause() {
    return RT_OK;
}

RT_RET FFNodeDemuxer::onReset() {
    RT_LOGD_IF(DEBUG_FLAG, "call, reset");
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    RT_ASSERT(RT_NULL != ctx);

    // @review: code redundancy but logically reasonable
    // @review: flush video&audio&subtitle packets
    this->onFlush();
    if (ctx->mFormatCtx != RT_NULL) {
        fa_format_close(ctx->mFormatCtx);
        ctx->mFormatCtx = RT_NULL;
    }
    RT_LOGD_IF(DEBUG_FLAG, "done, reset");
    return RT_OK;
}

RT_RET FFNodeDemuxer::onFlush() {
    FFNodeDemuxerCtx* ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    RTPacket rt_pkt;
    if (ctx->mVideoPktList != NULL) {
        while (ctx->mVideoPktList->size > 0) {
            RtMutex::RtAutolock autoLock(ctx->mLockVideoPacket);
            void* raw_pkt = array_list_get_data(ctx->mVideoPktList, 0);
            fa_format_packet_parse(ctx->mFormatCtx, raw_pkt, &rt_pkt);
            rt_utils_packet_free(&rt_pkt);
            array_list_remove_at(ctx->mVideoPktList, 0);
        }
    }
    if (ctx->mAudioPktList != NULL) {
        while (ctx->mAudioPktList->size > 0) {
            RtMutex::RtAutolock autoLock(ctx->mLockAudioPacket);
            void* raw_pkt = array_list_get_data(ctx->mAudioPktList, 0);
            fa_format_packet_parse(ctx->mFormatCtx, raw_pkt, &rt_pkt);
            rt_utils_packet_free(&rt_pkt);
            array_list_remove_at(ctx->mAudioPktList, 0);
        }
    }
    RT_LOGD_IF(DEBUG_FLAG, "done, flush");

    return RT_OK;
}

RT_RET FFNodeDemuxer::runTask() {
    FFNodeDemuxerCtx    *ctx = reinterpret_cast<FFNodeDemuxerCtx*>(mNodeContext);
    void                *raw_pkt = RT_NULL;
    INT32                err = 0;

    RT_LOGD_IF(DEBUG_FLAG, "task begin");
    while (THREAD_LOOP == ctx->mThread->getState()) {
        if (ctx->mNeedSeek > 0) {
            RT_LOGD("do seek, seek to %lld us", ctx->mSeekTimeUs);
            int flags = 0;
            fa_format_seek_to(ctx->mFormatCtx, -1, ctx->mSeekTimeUs, flags);
            onFlush();
            RT_LOGD("flush compelete");
            ctx->mEosFlag = RT_FALSE;
            ctx->mNeedSeek = 0;
        }

        if (!ctx->mEosFlag && array_list_get_size(ctx->mAudioPktList) < 30) {
            err = fa_format_packet_read(ctx->mFormatCtx, &raw_pkt);
            if (err < 0) {
                RT_LOGE("%s read end", __FUNCTION__);
                ctx->mEosFlag = RT_TRUE;
                continue;
            }

            do {
                INT32 stream_index = fa_format_packet_type(raw_pkt);
                if (stream_index == ctx->mIndexVideo) {
                    RtMutex::RtAutolock autoLock(ctx->mLockVideoPacket);
                    array_list_add(ctx->mVideoPktList, raw_pkt);
                } else if (stream_index == ctx->mIndexAudio) {
                    RtMutex::RtAutolock autoLock(ctx->mLockAudioPacket);
                    array_list_add(ctx->mAudioPktList, raw_pkt);
                } else {
                    RT_LOGE("FFPacket(0x%p), track=%d, but select(v=%d,a=%d)",
                             raw_pkt, stream_index, ctx->mIndexVideo, ctx->mIndexAudio);
                }
            } while (0);
        }
        RtTime::sleepUs(2000);
    }
    RT_LOGD_IF(DEBUG_FLAG, "cache_thread done");
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
