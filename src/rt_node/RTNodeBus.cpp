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
 * Author: martin.cheng@rock-chips.com
 *   Date: 2018/11/03
 *   Task: construct and manage pipeline of media node
 */

#include "rt_string_utils.h"  // NOLINT
#include "RTMediaMetaKeys.h"  // NOLINT
#include "RTNodeBus.h"        // NOLINT
#include "RTNodeDemuxer.h"    // NOLINT
#include "RTNodeHeader.h"     // NOLINT
#include "FFNodeDecoder.h"    // NOLINT
#include "FFNodeEncoder.h"    // NOLINT
#include "FFNodeDemuxer.h"    // NOLINT

#ifdef OS_LINUX
#include "HWNodeMpiDecoder.h" // NOLINT
#include "HWNodeMpiEncoder.h" // NOLINT
#endif

#ifdef OS_WINDOWS
#include "RTSinkDisplayGLES.h" // NOLINT
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTNodeBus"
#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x1

typedef enum _NODE_BUS_STATE {
    NODE_ALL_EMPTY,
    NODE_ALL_OK,
    NODE_BUS_EMPTY,
    NODE_BUS_OK,
    NODE_BUS_MAX,
} NODE_BUS_STATE;

struct NodeBusContext {
    RtHashTable    *mNodeBus;
    RtHashTable    *mNodeAll;
    NodeBusSetting *mSetting;
    RTNodeDemuxer  *mDemuxer;
    RTNode         *mChainVideo;
    RTNode         *mChainAudio;
    RTNode         *mChainSubtitle;
} NodeBusContext;

RT_RET  bus_release_nodes(RtHashTable *pNodeBus);
RTNode* bus_find_and_add_demuxer(RTNodeBus *pNodeBus, NodeBusSetting *setting);
RTNode* bus_find_and_add_codec(RTNodeBus *pNodeBus, RTNode *demuxer, \
                         RTTrackType tType, BUS_LINE_TYPE lType);
RTNode* bus_find_and_add_sink(RTNodeBus *pNodeBus, RTNode *codec, BUS_LINE_TYPE lType);

UINT32  node_hash_func(UINT32 bucktes, const void *key) {
    void *tmp_key = const_cast<void *>(key);
    return *(reinterpret_cast<UINT32 *>(tmp_key)) % (bucktes);
}

RTNodeBus::RTNodeBus() {
    mBusCtx = rt_malloc(struct NodeBusContext);
    rt_memset(mBusCtx, 0, sizeof(struct NodeBusContext));
    mBusCtx->mNodeBus = rt_hash_table_create((RT_NODE_TYPE_MAX - RT_NODE_TYPE_BASE),
                                       hash_ptr_func, hash_ptr_compare);
    mBusCtx->mNodeAll = rt_hash_table_create((RT_NODE_TYPE_MAX - RT_NODE_TYPE_BASE),
                                       hash_ptr_func, hash_ptr_compare);
    mBusCtx->mSetting = RT_NULL;
    mBusCtx->mDemuxer = RT_NULL;
    registerCoreStubs();
}

RTNodeBus::~RTNodeBus() {
    RT_ASSERT(RT_NULL != mBusCtx);

    release();
    rt_hash_table_destory(mBusCtx->mNodeBus);
    rt_hash_table_destory(mBusCtx->mNodeAll);

    RT_ASSERT(RT_NULL == mBusCtx->mNodeBus);
    RT_ASSERT(RT_NULL == mBusCtx->mNodeAll);

    // NodeBusSetting is released by its producer
    mBusCtx->mSetting = RT_NULL;
    mBusCtx->mDemuxer = RT_NULL;

    rt_safe_free(mBusCtx);
}

RT_RET RTNodeBus::autoBuild(NodeBusSetting *setting) {
    // create [demxuer] by setting
    mBusCtx->mDemuxer = reinterpret_cast<RTNodeDemuxer*>(bus_find_and_add_demuxer(this, setting));

    // create [codecs] by meta from demxuer
    RTNode *codec_v = bus_find_and_add_codec(this, mBusCtx->mDemuxer, \
                                       RTTRACK_TYPE_VIDEO, BUS_LINE_VIDEO);
    nodeChainAppend(codec_v, BUS_LINE_VIDEO);
    #if TO_DO_FLAG
    RTNode *codec_a = bus_find_and_add_codec(this, mBusCtx->mDemuxer, \
                                       RTTRACK_TYPE_AUDIO, BUS_LINE_AUDIO);
    nodeChainAppend(codec_a, BUS_LINE_AUDIO);
    RTNode *codec_s = bus_find_and_add_codec(this, mBusCtx->mDemuxer, \
                                       RTTRACK_TYPE_SUBTITLE, BUS_LINE_SUBTITLE);
    nodeChainAppend(codec_s, BUS_LINE_SUBTITLE);
    #endif

    // create [sink] by meta from codecs
    RTNode *sink_v = bus_find_and_add_sink(this, codec_v, BUS_LINE_VIDEO);
    nodeChainAppend(sink_v, BUS_LINE_VIDEO);
    #if TO_DO_FLAG
    RTNode *sink_a = bus_find_and_add_sink(this, codec_a, BUS_LINE_AUDIO);
    nodeChainAppend(sink_a, BUS_LINE_AUDIO);
    RTNode *sink_s = bus_find_and_add_sink(this, codec_s, BUS_LINE_SUBTITLE);
    nodeChainAppend(sink_s, BUS_LINE_SUBTITLE);
    #endif

    nodeChainDumper(BUS_LINE_VIDEO);
    nodeChainDumper(BUS_LINE_AUDIO);
    nodeChainDumper(BUS_LINE_SUBTITLE);

    return RT_OK;
}

RT_RET RTNodeBus::release() {
    // release all [RTNodes] in node_bus;
    // but NO NEED to release [NodeStub].
    return bus_release_nodes(mBusCtx->mNodeBus);
}

RT_RET RTNodeBus::summary(INT32 fd, RT_BOOL full /*= RT_FALSE*/) {
    RT_ASSERT((RT_NULL != mBusCtx)&&(mBusCtx->mNodeBus));

    RT_LOGD("dump used nodes in node_bus ...");
    UINT32 num_buckets = rt_hash_table_get_num_buckets(mBusCtx->mNodeBus);
    for (UINT32 idx = 0; idx < num_buckets; idx++) {
        struct rt_hash_node* node = RT_NULL;
        UINT32 num_plugin = 0;
        struct rt_hash_node* root = rt_hash_table_get_bucket(mBusCtx->mNodeBus, idx);
        for (node = root->next; node != root; node = node->next, num_plugin++) {
            RTNode* plugin = reinterpret_cast<RTNode*>(node->data);
            RT_LOGD("%-12s { name:%-18s ptr:0x%p }",
                     rt_node_type_name(plugin->queryStub()->mNodeType),
                     plugin->queryStub()->mNodeName, node->data);
        }
    }
    RT_LOGD("dump used nodes in node_bus ... DONE!!!");

    return RT_OK;
}

RT_RET RTNodeBus::registerCoreStubs() {
    RT_ASSERT(RT_NULL != mBusCtx);
    #ifdef RK_HW_CODEC
    registerStub(&hw_node_mpi_decoder);
    registerStub(&hw_node_mpi_encoder);
    #endif
    registerStub(&ff_node_demuxer);
    registerStub(&ff_node_decoder);
    registerStub(&ff_node_video_encoder);
    #ifdef OS_WINDOWS
    registerStub(&rt_sink_display_gles);
    #endif
    return RT_OK;
}

RTNodeStub* findStub(RT_NODE_TYPE nType) {
    RTNodeStub* stub = RT_NULL;
    switch (nType) {
    case RT_NODE_TYPE_DEMUXER:
        stub = &ff_node_demuxer;
        break;
    case RT_NODE_TYPE_DECODER:
        stub = &ff_node_decoder;
        break;
    case RT_NODE_TYPE_ENCODER:
        stub = &ff_node_video_encoder;
        break;
    case RT_NODE_TYPE_SINK:
        #ifdef OS_WINDOWS
        stub = &rt_sink_display_gles;
        #endif
        break;
    default:
        break;
    }
    return stub;
}

RT_RET RTNodeBus::registerStub(RTNodeStub *nStub) {
    RT_ASSERT((RT_NULL != mBusCtx) && (RT_NULL != nStub));

    INT32 nType = nStub->mNodeType;
    rt_hash_table_insert(mBusCtx->mNodeAll, reinterpret_cast<void*>(nType), nStub);
    return RT_OK;
}

RT_RET RTNodeBus::registerNode(RTNode *pNode) {
    RT_ASSERT((RT_NULL != mBusCtx) && (RT_NULL != pNode));

    INT32 nType = pNode->queryStub()->mNodeType;
    rt_hash_table_insert(mBusCtx->mNodeBus, reinterpret_cast<void*>(nType), pNode);
    return RT_OK;
}

// @TODO find NodeStub by MIME
RTNodeStub* RTNodeBus::findStub(RT_NODE_TYPE nType, BUS_LINE_TYPE lType) {
    RT_ASSERT(RT_NULL != mBusCtx);

    void* data = rt_hash_table_find(mBusCtx->mNodeAll,
                     reinterpret_cast<void *>(nType));
    if (RT_NULL != data) {
        return reinterpret_cast<RTNodeStub*>(data);
    }
    return RT_NULL;
}

// @TODO find RTNode by MIME
RTNode* RTNodeBus::findNode(RT_NODE_TYPE nType, BUS_LINE_TYPE lType) {
    RT_ASSERT(RT_NULL != mBusCtx);

    void* data = rt_hash_table_find(mBusCtx->mNodeBus,
                     reinterpret_cast<void *>(nType));
    if (RT_NULL != data) {
        return reinterpret_cast<RTNode*>(data);
    }
    return RT_NULL;
}

RT_RET RTNodeBus::nodeChainAppend(RTNode *pNode, BUS_LINE_TYPE lType) {
    RT_ASSERT(RT_NULL != mBusCtx);

    RTNode **nRoot = RT_NULL;
    switch (lType) {
    case BUS_LINE_VIDEO:
        nRoot = &(mBusCtx->mChainVideo);
        break;
    case BUS_LINE_AUDIO:
        nRoot = &(mBusCtx->mChainAudio);
        break;
    case BUS_LINE_SUBTITLE:
        nRoot = &(mBusCtx->mChainSubtitle);
        break;
    default:
        RT_LOGE("Ignore this node(%s)", pNode->queryStub()->mNodeName);
    }
    pNode->mNext = RT_NULL;
    if (RT_NULL == *nRoot) {
        *nRoot = pNode;
    } else {
        (*nRoot)->mNext = pNode;
    }

    return RT_OK;
}

RT_RET RTNodeBus::nodeChainDumper(BUS_LINE_TYPE lType) {
    RT_ASSERT(RT_NULL != mBusCtx);

    RTNode* nRoot = RT_NULL;
    switch (lType) {
    case BUS_LINE_VIDEO:
        nRoot = mBusCtx->mChainVideo;
        break;
    case BUS_LINE_AUDIO:
        nRoot = mBusCtx->mChainAudio;
        break;
    case BUS_LINE_SUBTITLE:
        nRoot = mBusCtx->mChainAudio;
        break;
    default:
        break;
    }
    if (RT_NULL != nRoot) {
        RTNode* rtNode = nRoot;
        RT_LOGE("Work Chain[%d]: ----------", lType);
        while (RT_NULL != rtNode) {
            RT_LOGD("%-12s { name:%-18s ptr:0x%p }",
                     rt_node_type_name(rtNode->queryStub()->mNodeType),
                     rtNode->queryStub()->mNodeName, rtNode);
            rtNode = rtNode->mNext;
        }
        RT_LOGE("Work Chain: ---------- ... \r\n");
    }

    return RT_OK;
}

RT_RET RTNodeBus::nodeChainDriver(RTNode *pNode, BUS_LINE_TYPE lType) {
    RTMediaBuffer *pMediaBuf;
    while (RT_NULL != pNode) {
        RTNodeAdapter::pushBuffer(pNode, pMediaBuf);
        if (RT_NULL != pNode->mNext) {
            RTNodeAdapter::pullBuffer(pNode->mNext, &pMediaBuf);
        }
        pNode = pNode->mNext;
    }
    return RT_OK;
}

RT_RET RTNodeBus::coreLoopDriver() {
    excuteCommand(RT_NODE_CMD_START);
    RTMediaBuffer *pMediaBuf = RT_NULL;
    do {
        RTNodeAdapter::pullBuffer(mBusCtx->mDemuxer, &pMediaBuf);

        // NO packet, retry more
        if (RT_NULL == pMediaBuf) {
            RtTime::sleepMs(3);
            continue;
        }
        nodeChainDriver(mBusCtx->mChainVideo,    BUS_LINE_VIDEO);
        nodeChainDriver(mBusCtx->mChainAudio,    BUS_LINE_AUDIO);
        nodeChainDriver(mBusCtx->mChainSubtitle, BUS_LINE_SUBTITLE);
        RtTime::sleepMs(20);
    } while (true);
}

RT_RET RTNodeBus::excuteCommand(RT_NODE_CMD cmd) {
    RTNodeAdapter::runCmd(mBusCtx->mDemuxer,       cmd, RT_NULL);
    RTNodeAdapter::runCmd(mBusCtx->mChainVideo,    cmd, RT_NULL);
    RTNodeAdapter::runCmd(mBusCtx->mChainAudio,    cmd, RT_NULL);
    RTNodeAdapter::runCmd(mBusCtx->mChainSubtitle, cmd, RT_NULL);
    return RT_OK;
}

RT_BOOL check_setting(NodeBusSetting *setting) {
    // @TODO NEED more checks...
    if (RT_NULL == setting->mUri) {
        return RT_FALSE;
    }
    return RT_TRUE;
}

RTNode* bus_find_and_add_demuxer(RTNodeBus *pNodeBus, NodeBusSetting *setting) {
    RTNodeStub *nStub   = &ff_node_demuxer;
    RTNode     *demuxer = RT_NULL;
    RtMetaData *nMeta   = RT_NULL;

    // init node demuxer
    if (RT_TRUE == check_setting(setting)) {
        demuxer = nStub->mCreateNode();
        nMeta   =  new RtMetaData();
        nMeta->setCString(kKeyFormatUri, setting->mUri);
        nMeta->setCString(kKeyUserAgent, setting->mUserAgent);
        RTNodeAdapter::init(demuxer, nMeta);
        pNodeBus->registerNode(demuxer);
    } else {
        RT_LOGE("Fail to create demxuer(uri=0x%p), invalid par...", setting->mUri);
    }

    return demuxer;
}

RTNode* bus_find_and_add_codec(RTNodeBus *pNodeBus, RTNode *demuxer, \
                         RTTrackType tType, BUS_LINE_TYPE lType) {
    if ((RT_NULL == pNodeBus) || (RT_NULL == demuxer)) {
        RT_LOGE("Fail to create codec(type=%d), demuxer is NULL", tType);
        return RT_NULL;
    }
    RTNodeDemuxer *nDemuxer = reinterpret_cast<RTNodeDemuxer*>(demuxer);
    INT32       track_idx   = nDemuxer->queryTrackUsed(tType);
    RtMetaData *node_meta   = RT_NULL;
    RTNodeStub *node_stub   = RT_NULL;
    RTNode     *node_codec  = RT_NULL;

    if (track_idx >= 0) {
        node_meta = nDemuxer->queryTrackMeta(track_idx, tType);
        rt_utils_dump_track(node_meta);
    }

    if (RT_NULL != node_meta) {
        // @TODO create codec by MIME
        node_stub = pNodeBus->findStub(RT_NODE_TYPE_DECODER, lType);
    }

    if (RT_NULL != node_stub) {
        node_codec = node_stub->mCreateNode();
    }

    if (RT_NULL != node_codec) {
        RTNodeAdapter::init(node_codec, node_meta);
        pNodeBus->registerNode(node_codec);
    } else {
        switch (lType) {
        case BUS_LINE_VIDEO:
            RT_LOGE("Found no Track(%d) or codec for _Video_", track_idx);
            break;
        case BUS_LINE_AUDIO:
            RT_LOGE("Found no Track(%d) or codec for _Audio_", track_idx);
            break;
        case BUS_LINE_SUBTITLE:
            RT_LOGE("Found no Track(%d) or codec for Caption", track_idx);
            break;
        default:
            break;
        }
    }
    return node_codec;
}

RTNode* bus_find_and_add_sink(RTNodeBus *pNodeBus, RTNode *codec, BUS_LINE_TYPE lType) {
    if ((RT_NULL == pNodeBus) || (RT_NULL == codec)) {
        RT_LOGE("Fail to create sink(type=%d), codec is NULL", lType);
        return RT_NULL;
    }

    RtMetaData *nMeta = codec->queryFormat(RT_PORT_INPUT);

    // @TODO create codec by MIME
    RTNodeStub *nStub = pNodeBus->findStub(RT_NODE_TYPE_SINK, lType);
    RTNode     *nSink = (RT_NULL != nStub)?nStub->mCreateNode():RT_NULL;

    if ((RT_NULL != nSink) && (RT_NULL != nMeta)) {
        RTNodeAdapter::init(nSink, nMeta);
        pNodeBus->registerNode(nSink);
        rt_utils_dump_track(nMeta);
    } else {
        const char* name = codec->queryStub()->mNodeName;
        RT_LOGE("Found no Sink for Codec(%s, 0x%p) ", name, codec);
    }
    return nSink;
}

RT_RET bus_release_nodes(RtHashTable *pNodeBus) {
    struct rt_hash_node *node, *next, *list;
    RTNode *plugin = NULL;

    for (UINT32 idx = 0; idx < rt_hash_table_get_num_buckets(pNodeBus); idx++) {
        list = rt_hash_table_get_bucket(pNodeBus, idx);
        for (node = (list)->next; node != list; node = next) {
            next = (node)->next;
            plugin = reinterpret_cast<RTNode*>(node->data);
            if (RT_NULL != plugin) {
                RTNodeAdapter::runCmd(plugin, RT_NODE_CMD_STOP, \
                                      reinterpret_cast<RtMetaData *>(NULL));
                RT_LOGD("%-12s { name:%-18s ptr:0x%p }",
                        rt_node_type_name(plugin->queryStub()->mNodeType),
                        plugin->queryStub()->mNodeName, node->data);
                rt_safe_delete(plugin);
            }
        }
    }
    rt_hash_table_clear(pNodeBus);
    return RT_OK;
}
