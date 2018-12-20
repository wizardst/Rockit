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
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FFNodeDemuxer"

#include "rt_node.h" // NOLINT
#include "./include/ff_node_demuxer.h" // NOLINT
#include "rt_mem.h" // NOLINT
#include "rt_array_list.h" // NOLINT
#include "rt_thread.h" // NOLINT
#include "rt_metadata_key.h" // NOLINT
#include "rt_buffer.h" // NOLINT
#include "rt_metadata.h" // NOLINT
#include "rt_common.h" // NOLINT
#include "rt_media_packet.h" // NOLINT
#include "ffmpeg_wrapper.h" // NOLINT

typedef struct FFNodeDemuxerContext {
    AVFormatContext    *format_ctx;
    RtArrayList        *packet_list;
    RtThread           *readpacket_thread;
    int                 read_status;
    RtMutex            *packet_queue_lock;

    UINT32 pull_cnt;
    UINT32 push_cnt;

    RT_RET onInit(RtMetaData *metadata);
    RT_RET onSetDataSource(RtMetaData *metadata);
    RT_RET onStart();
    RT_RET onStop();
} FFNodeDemuxerCtx;


RT_RET FFNodeDemuxerCtx::onInit(RtMetaData *metadata) {
    RT_LOGD("IN");
    format_ctx = NULL;
    readpacket_thread = NULL;
    read_status = RT_TRUE;

    // av_register_all();
    avformat_network_init();

    packet_list = array_list_create();
    RT_ASSERT(RT_NULL != packet_list);

    packet_queue_lock = new RtMutex();
    RT_ASSERT(RT_NULL != packet_queue_lock);
    RT_LOGD("OUT");

    return RT_OK;
}

RT_RET FFNodeDemuxerCtx::onSetDataSource(RtMetaData *metadata) {
    AVDictionary *opts = NULL;
    const char *url;
    metadata->findCString(kKeyUrl, &url);
    RT_ASSERT(RT_NULL != url);

    RT_LOGD("IN, uri=%s", url);

    format_ctx = avformat_alloc_context();
    RT_ASSERT(RT_NULL != format_ctx);
    if (avformat_open_input(&format_ctx, url, NULL, NULL) != 0) {
        RT_LOGE("Couldn't open input stream.\n");
        av_dict_free(&opts);
        if (format_ctx != NULL) {
            av_free(format_ctx);
        }
        return RT_ERR_UNKNOWN;
    }

    if (avformat_find_stream_info(format_ctx, NULL) < 0) {
        RT_LOGE("Couldn't find stream information.\n");
        return RT_ERR_UNKNOWN;
    }
    av_dict_free(&opts);
    RT_LOGD("OUT");

    return RT_OK;
}

void* readPacketLoop(void* arg) {
    RT_LOGD("IN arg=%p", arg);
    FFNodeDemuxerCtx * demuxer_ctx = static_cast<FFNodeDemuxerCtx*>(arg);

    while (demuxer_ctx->read_status) {
        AVPacket *packet = av_packet_alloc();
        av_init_packet(packet);
        int ret = av_read_frame(demuxer_ctx->format_ctx, packet);
        if (ret < 0) {
            RT_LOGD("%s read end", __FUNCTION__);
            demuxer_ctx->read_status = RT_FALSE;
            return NULL;
        }

        if (0) {
            int flags = 0;
            ret = avformat_seek_file(demuxer_ctx->format_ctx, 0/*seekStrmIdx*/,
                                     RT_INT64_MIN, 0/*timeus*/, RT_INT64_MAX, flags);
        }
        do {
            // RT_LOGD("packet %p size = %d", packet, packet->size);
            RtMutex::RtAutolock autoLock(demuxer_ctx->packet_queue_lock);
            array_list_add(demuxer_ctx->packet_list, packet);
        } while (0);
        RtTime::sleepUs(2000);
    }
    RT_LOGD("OUT");
    return NULL;
}

RT_RET FFNodeDemuxerCtx::onStart() {
    RT_LOGD("IN");
    readpacket_thread = new RtThread(readPacketLoop, this);
    readpacket_thread->setName("Demuxer-Read");
    readpacket_thread->start();
    return RT_OK;
}

RT_RET FFNodeDemuxerCtx::onStop() {
    RT_LOGD("IN");
    read_status = RT_FALSE;
    readpacket_thread->join();

    if (packet_list != NULL) {
        while (packet_list->size > 0) {
            RtMutex::RtAutolock autoLock(packet_queue_lock);
            AVPacket *packet = reinterpret_cast<AVPacket *>(array_list_get_data(packet_list, 0));
            av_packet_unref(packet);
            array_list_remove_at(packet_list, 0);
        }
    }

    if (format_ctx != NULL) {
        avformat_close_input(&format_ctx);
    }
    avformat_network_deinit();
    return RT_OK;
}


RT_RET node_ff_demuxer_init(void **ctx) {
    RT_LOGD("IN");
    *ctx = rt_malloc(FFNodeDemuxerCtx);
    rt_memset(*ctx, 0, sizeof(FFNodeDemuxerCtx));
    return RT_OK;
}

RT_RET node_ff_demuxer_release(void **ctx) {
    RT_LOGD("IN");
    FFNodeDemuxerCtx* demuxer_ctx = reinterpret_cast<FFNodeDemuxerCtx*>(*ctx);
    RT_ASSERT(RT_NULL != demuxer_ctx);

    if (demuxer_ctx->packet_list != NULL) {
        array_list_remove_all(demuxer_ctx->packet_list);
        array_list_destroy(demuxer_ctx->packet_list);
        demuxer_ctx->packet_list = NULL;
    }

    if (demuxer_ctx->readpacket_thread != NULL) {
        delete demuxer_ctx->readpacket_thread;
        demuxer_ctx->readpacket_thread = NULL;
    }

    if (demuxer_ctx->packet_queue_lock != NULL) {
        delete demuxer_ctx->packet_queue_lock;
        demuxer_ctx->packet_queue_lock = NULL;
    }
    rt_safe_free(*ctx);
    return RT_OK;
}

RT_RET node_ff_demuxer_pull(void *ctx, RtMediaPacket packet, UINT32 *size) {
    FFNodeDemuxerCtx* demuxer_ctx = reinterpret_cast<FFNodeDemuxerCtx*>(ctx);
    RT_ASSERT(RT_NULL != demuxer_ctx);
    RT_ASSERT(RT_NULL != packet);

    RtBuffer    prt_buffer    = rt_media_packet_get_buffer(packet);
    RtMetaData *prt_metadata  = rt_buffer_get_metadata(prt_buffer);
    AVPacket   *prt_packet    = reinterpret_cast<AVPacket *>(array_list_get_data(demuxer_ctx->packet_list, 0));
    if (prt_packet == NULL) {
        RT_LOGD("packet NULL");
        *size = 0;
        if (demuxer_ctx->read_status) {
            return RT_OK;
        }
        return RT_ERR_UNKNOWN;
    } else {
        *size = prt_packet->size;
        // RT_LOGD("%s prt_packet size = %d", __FUNCTION__, prt_packet->size);
        RtMutex::RtAutolock autoLock(demuxer_ctx->packet_queue_lock);
        array_list_remove_at(demuxer_ctx->packet_list, 0);
        prt_metadata->setPointer(kKeyAVPacket, prt_packet);
        prt_metadata->setInt64(kKeyAVPktPts, prt_packet->pts);
        prt_metadata->setInt64(kKeyAVPktDts, prt_packet->dts);
        prt_metadata->setInt32(kKeyAVPktSize, prt_packet->size);
        prt_metadata->setInt32(kKeyAVPktFlag, prt_packet->flags);
        prt_metadata->setInt32(kKeyAVPktIndex, prt_packet->stream_index);
        prt_metadata->setPointer(kKeyAVPktData, prt_packet->data);
        return RT_OK;
    }
    return RT_OK;
}

RT_RET node_ff_demuxer_push(void *ctx, RtMediaPacket packet, UINT32 *size) {
    return RT_OK;
}

RT_RET node_ff_demuxer_run_cmd(void* ctx, RT_NODE_CMD cmd,
                            RtMetaData *metadata) {
    FFNodeDemuxerCtx* demuxer_ctx = reinterpret_cast<FFNodeDemuxerCtx*>(ctx);
    RT_ASSERT(RT_NULL != demuxer_ctx);

    RT_LOGD("CMD = %d", cmd);
    switch (cmd) {
    case RT_NODE_CMD_INIT: {
        demuxer_ctx->onInit(metadata);
        break;
    }
    case RT_NODE_CMD_SET_DATASOURCE: {
        demuxer_ctx->onSetDataSource(metadata);
        break;
    }
    case RT_NODE_CMD_START: {
        demuxer_ctx->onStart();
        break;
    }
    case RT_NODE_CMD_STOP: {
        demuxer_ctx->onStop();
        break;
    }
    case RT_NODE_CMD_SEEK: {
        break;
    }
    default: {
        RT_LOGE("demuxer not support the cmd\n");
        break;
    }
    }
    return RT_OK;
}

RT_Node ff_node_demuxer = {
    .type         = RT_NODE_TYPE_DEMUXER,
    .name         = "ff_demuxer",
    .version      = "v1.0",
    .node_ctx     = RT_NULL,
    .impl_init    = node_ff_demuxer_init,
    .impl_release = node_ff_demuxer_release,
    .impl_pull    = node_ff_demuxer_pull,
    .impl_push    = node_ff_demuxer_push,
    .impl_run_cmd = node_ff_demuxer_run_cmd,
    .impl_query_cap = NULL,
};
