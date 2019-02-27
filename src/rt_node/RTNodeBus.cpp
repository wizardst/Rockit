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
#include "RTNodeSink.h"       // NOLINT
#include "RTNodeHeader.h"     // NOLINT

#include "FFNodeDecoder.h"    // NOLINT
#include "FFNodeEncoder.h"    // NOLINT
#include "FFNodeDemuxer.h"    // NOLINT

#ifdef OS_LINUX
#include "RTSinkAudioALSA.h"   // NOLINT
#include "RTNodeSinkAWindow.h" // NOLINT
#include "HWNodeMpiDecoder.h"  // NOLINT
#include "HWNodeMpiEncoder.h"  // NOLINT
#endif

#ifdef OS_WINDOWS
#include "RTNodeSinkGLES.h" // NOLINT
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
    RTNode*         mRootNodes[BUS_LINE_MAX];
    struct RTMsgLooper* mLooper;
    UINT32          mState;
    INT64           mWantSeekTimeUs;
    INT64           mLastSeekTimeUs;
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

    // Message Queue Mechanism
    mBusCtx->mLooper = new RTMsgLooper();
    mBusCtx->mLooper->setName("MsgLooper");
    mBusCtx->mLooper->start();

    setCurState(RTM_PLAYER_IDLE);
}

RTNodeBus::~RTNodeBus() {
    RT_ASSERT(RT_NULL != mBusCtx);

    reset();
    rt_hash_table_destory(mBusCtx->mNodeBus);
    rt_hash_table_destory(mBusCtx->mNodeAll);

    RT_ASSERT(RT_NULL == mBusCtx->mNodeBus);
    RT_ASSERT(RT_NULL == mBusCtx->mNodeAll);

    // NodeBusSetting is released by its producer
    mBusCtx->mSetting = RT_NULL;
    mBusCtx->mDemuxer = RT_NULL;

    // Message Queue Mechanism
    if (RT_NULL != mBusCtx->mLooper) {
        mBusCtx->mLooper->stop();
    }
    rt_safe_delete(mBusCtx->mLooper);

    rt_safe_free(mBusCtx);
}

RT_RET RTNodeBus::autoBuild(NodeBusSetting *setting) {
    // create [demxuer] by setting
    mBusCtx->mDemuxer = reinterpret_cast<RTNodeDemuxer*>(bus_find_and_add_demuxer(this, setting));

    // create [codecs] by meta from demxuer
    RTNode *codec_v = bus_find_and_add_codec(this, mBusCtx->mDemuxer, \
                                       RTTRACK_TYPE_VIDEO, BUS_LINE_VIDEO);
    nodeChainAppend(codec_v, BUS_LINE_VIDEO);

    RTNode *codec_a = bus_find_and_add_codec(this, mBusCtx->mDemuxer, \
                                       RTTRACK_TYPE_AUDIO, BUS_LINE_AUDIO);
    nodeChainAppend(codec_a, BUS_LINE_AUDIO);

    #if TODO_FLAG
    RTNode *codec_s = bus_find_and_add_codec(this, mBusCtx->mDemuxer, \
                                       RTTRACK_TYPE_SUBTITLE, BUS_LINE_SUBTE);
    nodeChainAppend(codec_s, BUS_LINE_SUBTE);
    #endif

    // create [sink] by meta from codecs
    RTNode *sink_v = bus_find_and_add_sink(this, codec_v, BUS_LINE_VIDEO);
    nodeChainAppend(sink_v, BUS_LINE_VIDEO);
    RTNode *sink_a = bus_find_and_add_sink(this, codec_a, BUS_LINE_AUDIO);
    nodeChainAppend(sink_a, BUS_LINE_AUDIO);
    #if TODO_FLAG
    RTNode *sink_s = bus_find_and_add_sink(this, codec_s, BUS_LINE_SUBTE);
    nodeChainAppend(sink_s, BUS_LINE_SUBTE);
    #endif

    nodeChainDumper(BUS_LINE_VIDEO);
    nodeChainDumper(BUS_LINE_AUDIO);
    nodeChainDumper(BUS_LINE_SUBTE);

    this->setCurState(RTM_PLAYER_INITIALIZED);
    RTMessage* msg = new RTMessage(RT_MEDIA_PREPARED, RT_NULL, this);
    mBusCtx->mLooper->send(msg, 0);

    return RT_OK;
}

RT_RET RTNodeBus::stop() {
    UINT32 curState = getCurState();
    switch (curState) {
      case RTM_PLAYER_STOPPED:
        RT_LOGE("Fail to stop, invalid state(%d)", curState);
        break;
      case RTM_PLAYER_PREPARING:
      case RTM_PLAYER_PREPARED:
      case RTM_PLAYER_STARTED:
      case RTM_PLAYER_PAUSED:
      case RTM_PLAYER_PLAYBACK_COMPLETE:
        // @TODO: do stop player
        this->excuteCommand(RT_NODE_CMD_STOP);
        break;
    }

    return RT_OK;
}

RT_RET RTNodeBus::reset() {
    UINT32 curState = getCurState();
    if (RTM_PLAYER_IDLE == curState) {
        RT_LOGE("Fail to reset, invalid state(%d)", curState);
        return RT_OK;
    }
    if (RTM_PLAYER_STOPPED != curState) {
        RT_LOGE("Need stop() before reset(), state(%d)", curState);
        this->stop();
    }

    // @TODO: do reset player
    this->excuteCommand(RT_NODE_CMD_RESET);

    // release all [RTNodes] in node_bus;
    // but NO NEED to release [NodeStub].
    RT_RET err = bus_release_nodes(mBusCtx->mNodeBus);
    setCurState(RTM_PLAYER_IDLE);

    for (int cnt = 0; cnt < BUS_LINE_MAX; cnt++) {
        mBusCtx->mRootNodes[cnt] = RT_NULL;
    }
    return err;
}

RT_RET RTNodeBus::prepare() {
    UINT32 curState = getCurState();
    if ((RTM_PLAYER_INITIALIZED != curState) && (RTM_PLAYER_STOPPED != curState)) {
        RT_LOGE("Fail to prepare, invalid state(%d)", curState);
        return RT_OK;
    }

    setCurState(RTM_PLAYER_PREPARING);

    // @TODO: do prepare player
    this->excuteCommand(RT_NODE_CMD_PREPARE);

    RTMessage* msg = new RTMessage(RT_MEDIA_PREPARED, RT_NULL, this);
    mBusCtx->mLooper->send(msg, 0);
    return RT_OK;
}

RT_RET RTNodeBus::start() {
    UINT32 curState = getCurState();
    RTMessage* msg  = RT_NULL;
    switch (curState) {
      case RTM_PLAYER_PREPARED:
      case RTM_PLAYER_PAUSED:
        // @TODO: do resume player
        this->excuteCommand(RT_NODE_CMD_START);
        // @TODO: decode proc with thread
        this->startAudioPlayerProc();

        msg = new RTMessage(RT_MEDIA_STARTED, RT_NULL, this);
        mBusCtx->mLooper->send(msg, 0);
        break;
      default:
        RT_LOGE("Fail to start, invalid state(%d)", curState);
        break;
    }
    return RT_OK;
}

RT_RET RTNodeBus::pause() {
    UINT32 curState = getCurState();
    RTMessage* msg  = RT_NULL;
    switch (curState) {
      case RTM_PLAYER_PAUSED:
        RT_LOGE("Fail to pause, invalid state(%d)", curState);
        break;
      case RTM_PLAYER_STARTED:
        // @TODO: do pause player

        msg = new RTMessage(RTM_PLAYER_PAUSED, RT_NULL, this);
        mBusCtx->mLooper->send(msg, 0);
        break;
      default:
        RT_LOGE("Fail to pause, invalid state(%d)", curState);
        break;
    }
    return RT_OK;
}

RT_RET RTNodeBus::seekTo(INT64 usec) {
    UINT32 curState = getCurState();
    RTMessage* msg  = RT_NULL;
    switch (curState) {
      case RTM_PLAYER_PREPARING:
      case RTM_PLAYER_PREPARED:
        mBusCtx->mWantSeekTimeUs = usec;
        RT_LOGE("Fail to seekTo, invalid state(%d), save only", curState);
        break;
      case RTM_PLAYER_PAUSED:
      case RTM_PLAYER_STARTED:
        // @TODO: do pause player
        mBusCtx->mWantSeekTimeUs = usec;
        // async seek message
        msg = new RTMessage(RT_MEDIA_SEEK_ASYNC, 0, usec, this);
        mBusCtx->mLooper->post(msg, 0);
        break;
      default:
        RT_LOGE("Fail to seekTo, invalid state(%d)", curState);
        break;
    }

    return RT_OK;
}

RT_RET RTNodeBus::RTNodeBus::seekToAsync(INT64 usec) {
    // @TODO: do seek player
    // ...

    RTMessage* msg = new RTMessage(RT_MEDIA_SEEK_COMPLETE, RT_NULL, this);
    mBusCtx->mLooper->post(msg, 0);
    return RT_OK;
}

RT_RET RTNodeBus::setCurState(UINT32 newState) {
    RT_ASSERT((RT_NULL != mBusCtx)&&(mBusCtx->mNodeBus));
    mBusCtx->mState = newState;
    return RT_OK;
}

UINT32 RTNodeBus::getCurState() {
    RT_ASSERT((RT_NULL != mBusCtx)&&(mBusCtx->mNodeBus));
    return mBusCtx->mState;
}

void RTNodeBus::onMessageReceived(struct RTMessage* msg) {
    const char* msgName = mEventNames[msg->getWhat()].name;
    RT_LOGE("RTMessage(ptr=%p, what=%d, name=%s) is Received", msg, msg->getWhat(), msgName);
    switch (msg->getWhat()) {
      case RT_MEDIA_PREPARED:
        setCurState(RTM_PLAYER_PREPARED);
        break;
      case RT_MEDIA_PLAYBACK_COMPLETE:
        setCurState(RTM_PLAYER_PLAYBACK_COMPLETE);
        break;
      case RT_MEDIA_STARTED:
        setCurState(RTM_PLAYER_STARTED);
        break;
      case RT_MEDIA_PAUSED:
        setCurState(RTM_PLAYER_PAUSED);
        break;
      case RT_MEDIA_ERROR:
        setCurState(RTM_PLAYER_STATE_ERROR);
        break;
      case RT_MEDIA_STOPPED:
        setCurState(RTM_PLAYER_STOPPED);
        break;
      case RT_MEDIA_BUFFERING_UPDATE:
        break;
      case RT_MEDIA_SEEK_COMPLETE:
        break;
      case RT_MEDIA_SET_VIDEO_SIZE:
        break;
      case RT_MEDIA_SKIPPED:
        break;
      case RT_MEDIA_TIMED_TEXT:
        break;
      case RT_MEDIA_INFO:
        break;
      case RT_MEDIA_SUBTITLE_DATA:
        break;
      case RT_MEDIA_SEEK_ASYNC:
        seekToAsync(msg->mData.mArgU64);
        break;
      default:
        break;
    }
}

RT_RET RTNodeBus::summary(INT32 fd, RT_BOOL full /*= RT_FALSE*/) {
    RT_ASSERT((RT_NULL != mBusCtx)&&(mBusCtx->mNodeBus));

    RT_LOGD("dump used nodes in node_bus ...");
    struct rt_hash_node *list, *node;
    UINT32 num_buckets = rt_hash_table_get_num_buckets(mBusCtx->mNodeBus);
    for (UINT32 bucket = 0; bucket < num_buckets; bucket++) {
        list = rt_hash_table_get_bucket(mBusCtx->mNodeBus, bucket);
        for (node = list->next; node != RT_NULL; node = node->next) {
            if ((RT_NULL != node) && (RT_NULL != node->data)) {
                RTNode* plugin = reinterpret_cast<RTNode*>(node->data);
                RT_LOGD("%-16s RTNode(name=%s, ptr=%p) hash=%p next=%p",
                           rt_node_type_name(plugin->queryStub()->mNodeType),
                           plugin->queryStub()->mNodeName, plugin, node, node->next);
            }
        }
    }
    RT_LOGD("dump used nodes in node_bus ... DONE!!!");

    return RT_OK;
}

RT_RET RTNodeBus::registerCoreStubs() {
    RT_ASSERT(RT_NULL != mBusCtx);
    #ifdef HAVE_MPI
    registerStub(&hw_node_mpi_decoder);
    registerStub(&hw_node_mpi_encoder);
    #endif

    registerStub(&ff_node_demuxer);
    registerStub(&ff_node_decoder);
    registerStub(&ff_node_video_encoder);
    #ifdef OS_WINDOWS
    registerStub(&rt_sink_display_gles);
    #endif
    #ifdef OS_LINUX
    registerStub(&rt_sink_audio_alsa);
    #endif
    return RT_OK;
}

RTNodeStub* findStub(RT_NODE_TYPE nType, BUS_LINE_TYPE lType) {
    RTNodeStub* stub = RT_NULL;
    switch (nType) {
    case RT_NODE_TYPE_DEMUXER:
        stub = &ff_node_demuxer;
        break;
    case RT_NODE_TYPE_DECODER:
        if (lType == BUS_LINE_AUDIO) {
            stub = &ff_node_decoder;
        } else if (lType == BUS_LINE_VIDEO) {
            #ifdef HAVE_MPI
            stub = &hw_node_mpi_decoder;
            #endif
            #ifdef OS_WINDOWS
            stub = &ff_node_decoder;
            #endif
        }
        break;
    case RT_NODE_TYPE_ENCODER:
        stub = &ff_node_video_encoder;
        break;
    case RT_NODE_TYPE_SINK:
        switch (lType) {
          case BUS_LINE_VIDEO:
            #ifdef OS_WINDOWS
            stub = &rt_sink_display_gles;
            #endif
            break;
          case BUS_LINE_AUDIO:
            #ifdef OS_LINUX
            stub = &rt_sink_audio_alsa;
            #endif
            break;
          default:
            break;
        }
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

    INT32 nType  = pNode->queryStub()->mNodeType;
    pNode->mNext = RT_NULL;
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
    if ((RT_NULL == mBusCtx) || (NULL == pNode)) {
        // RT_LOGE("%-16s -> invalid RTNode(0x%p)", mBusLineNames[lType].name, pNode);
        return RT_ERR_NULL_PTR;
    }

    RTNode **nRoot = RT_NULL;
    nRoot = &(mBusCtx->mRootNodes[lType]);
    pNode->mNext = RT_NULL;
    if (RT_NULL == *nRoot) {
        *nRoot = pNode;
    } else {
        (*nRoot)->mNext = pNode;
    }
    RT_LOGE("%-16s -> add RTNode(ptr=0x%p, name=%s)", mBusLineNames[lType].name,
               pNode, pNode->queryStub()->mNodeName);

    return RT_OK;
}

RT_RET RTNodeBus::nodeChainDumper(BUS_LINE_TYPE lType) {
    RT_ASSERT(RT_NULL != mBusCtx);

    RTNode* nRoot = mBusCtx->mRootNodes[lType];
    if (RT_NULL != nRoot) {
        RTNode* rtNode = nRoot;
        RT_LOGE("%-16s -> dump all nodes", mBusLineNames[lType].name);
        while (RT_NULL != rtNode) {
            RT_LOGD("%-12s { name:%-18s ptr:0x%p }",
                     rt_node_type_name(rtNode->queryStub()->mNodeType),
                     rtNode->queryStub()->mNodeName, rtNode);
            rtNode = rtNode->mNext;
        }
        RT_LOGE("\r\n...");
    }

    return RT_OK;
}

RT_VOID audio_sink_feed_callback(RTNode* pNode, RTMediaBuffer* data) {
    RTNodeAdapter::queueCodecBuffer(pNode, data, RT_PORT_OUTPUT);
}

RT_RET RTNodeBus::startAudioPlayerProc() {
    RTNodeDemuxer *demuxer = mBusCtx->mDemuxer;
    RTNode        *decoder = mBusCtx->mRootNodes[BUS_LINE_AUDIO];
    RTNodeSink    *audiosink = RT_NULL;
    if (RT_NULL != decoder) {
        audiosink = reinterpret_cast<RTNodeSink*>(decoder->mNext);
        audiosink->queueCodecBuffer = audio_sink_feed_callback;
        audiosink->callback_ptr     = decoder;
    }
    INT32 audio_idx  = demuxer->queryTrackUsed(RTTRACK_TYPE_AUDIO);


    if ((RT_NULL != demuxer)&&(RT_NULL != decoder)) {
        RTMediaBuffer *frame = RT_NULL;
        RTMediaBuffer* esPacket;

        do {
            // deqeue buffer from object pool
            RTNodeAdapter::dequeCodecBuffer(decoder, &esPacket, RT_PORT_INPUT);
            if (RT_NULL != esPacket) {
                RT_BOOL got_pkt = RT_FALSE;
                while (!got_pkt) {
                    // save es-packet to buffer
                    while (RTNodeAdapter::pullBuffer(demuxer, &esPacket) != RT_OK) {
                        RtTime::sleepMs(10);
                    }
                    INT32 track_index = 0;
                    esPacket->getMetaData()->findInt32(kKeyPacketIndex, &track_index);

                    RT_LOGD("track_index: %d, audio_idx: %d", track_index, audio_idx);
                    if (track_index == audio_idx) {
                        UINT8 *data = reinterpret_cast<UINT8 *>(esPacket->getData());
                        UINT32 size = esPacket->getSize();
                        RT_LOGD("NEW audio MediaBuffer(ptr=0x%p, size=%d)", esPacket, size);
                        got_pkt = RT_TRUE;
                    } else {
                        /* pass other packet */
                        INT32 eos = 0;
                        esPacket->getMetaData()->findInt32(kKeyFrameEOS, &eos);
                        if (eos) {
                            RT_LOGD("receive eos , break");
                            break;
                        }
                        continue;
                    }
                }
             } else {
                 continue;
             }
            // push es-packet to decoder
            RTNodeAdapter::pushBuffer(decoder, esPacket);

            // pull av-frame from decoder
            RTNodeAdapter::pullBuffer(decoder, &frame);

            if (frame) {
                if (frame->getStatus() == RT_MEDIA_BUFFER_STATUS_READY) {
                    RT_LOGD("NEW Frame(ptr=0x%p, size=%d)", frame->getData(), frame->getLength());
                    char* data = reinterpret_cast<char *>(frame->getData());

                    RTNodeAdapter::pushBuffer(audiosink, frame);
                }
              //  RTNodeAdapter::queueCodecBuffer(decoder, frame, RT_PORT_OUTPUT);
                INT32 eos = 0;
                frame->getMetaData()->findInt32(kKeyFrameEOS, &eos);
                if (eos) {
                    RT_LOGD("receive eos , break");
                    break;
                }
                frame = NULL;
            }

            // dump AVFrame
            RtTime::sleepMs(5);
        } while (true);
    }
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

RT_RET RTNodeBus::startDataLooper() {
    excuteCommand(RT_NODE_CMD_START);
    RTMediaBuffer *pMediaBuf = RT_NULL;
    do {
        RTNodeAdapter::pullBuffer(mBusCtx->mDemuxer, &pMediaBuf);

        // NO packet, retry more
        if (RT_NULL == pMediaBuf) {
            RtTime::sleepMs(3);
            continue;
        }

        nodeChainDriver(mBusCtx->mRootNodes[BUS_LINE_VIDEO], BUS_LINE_VIDEO);
        nodeChainDriver(mBusCtx->mRootNodes[BUS_LINE_AUDIO], BUS_LINE_AUDIO);
        nodeChainDriver(mBusCtx->mRootNodes[BUS_LINE_SUBTE], BUS_LINE_SUBTE);
        RtTime::sleepMs(20);
    } while (true);
}

RT_RET RTNodeBus::excuteCommand(RT_NODE_CMD cmd) {
    RT_LOGD("node_bus delivers %s to active nodes", rt_node_cmd_name(cmd));
    RTNodeAdapter::runCmd(mBusCtx->mRootNodes[BUS_LINE_SOURCE], cmd, RT_NULL);
    RTNodeAdapter::runCmd(mBusCtx->mRootNodes[BUS_LINE_VIDEO],  cmd, RT_NULL);
    RTNodeAdapter::runCmd(mBusCtx->mRootNodes[BUS_LINE_AUDIO],  cmd, RT_NULL);
    RTNodeAdapter::runCmd(mBusCtx->mRootNodes[BUS_LINE_SUBTE],  cmd, RT_NULL);
    RT_LOGD("node_bus delivers %s to active nodes done!!!!!!\r\n", rt_node_cmd_name(cmd));
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
        RT_LOGE("%-16s -> invalid demuxer, can't create codec", mBusLineNames[lType].name);
        return RT_NULL;
    }
    RTNodeDemuxer *pDemuxer = reinterpret_cast<RTNodeDemuxer*>(demuxer);
    INT32       track_idx   = pDemuxer->queryTrackUsed(tType);
    RtMetaData *node_meta   = RT_NULL;
    RTNodeStub *node_stub   = RT_NULL;
    RTNode     *node_codec  = RT_NULL;

    if (BUS_LINE_AUDIO == lType) {
        RT_LOGE("%-16s -> no audio track, fix this bug!", mBusLineNames[lType].name);
        track_idx = 0;
    }

    if (track_idx >= 0) {
        node_meta = pDemuxer->queryTrackMeta(track_idx, tType);
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
        RT_LOGE("%-16s -> invalid codec(track=%d)", mBusLineNames[lType].name, track_idx);
    }
    return node_codec;
}

RTNode* bus_find_and_add_sink(RTNodeBus *pNodeBus, RTNode *codec, BUS_LINE_TYPE lType) {
    if ((RT_NULL == pNodeBus) || (RT_NULL == codec)) {
        RT_LOGE("%-16s -> invalid codec, can't create sink", mBusLineNames[lType].name);
        return RT_NULL;
    }

    RtMetaData *nMeta = codec->queryFormat(RT_PORT_INPUT);

    // @TODO create codec by MIME
    RTNodeStub *nStub = findStub(RT_NODE_TYPE_SINK, lType);
    RTNode     *nSink = (RT_NULL != nStub)?nStub->mCreateNode():RT_NULL;

    if ((RT_NULL != nSink) && (RT_NULL != nMeta)) {
        RTNodeAdapter::init(nSink, nMeta);
        pNodeBus->registerNode(nSink);
        rt_utils_dump_track(nMeta);
    } else {
        RT_LOGE("%-16s -> valid codec, but found no sink", mBusLineNames[lType].name);
    }
    return nSink;
}

RT_RET bus_release_nodes(RtHashTable *pNodeBus) {
    struct rt_hash_node *list, *node;
    RTNode *plugin = NULL;

    for (UINT32 idx = 0; idx < rt_hash_table_get_num_buckets(pNodeBus); idx++) {
        list = rt_hash_table_get_bucket(pNodeBus, idx);
        for (node = list->next; node != RT_NULL; node = node->next) {
            plugin = reinterpret_cast<RTNode*>(node->data);
            if (RT_NULL != plugin) {
                RTNodeAdapter::runCmd(plugin, RT_NODE_CMD_STOP, \
                                      reinterpret_cast<RtMetaData *>(NULL));
                RT_LOGD("%-16s RTNode(name=%s, ptr=%p)",
                        rt_node_type_name(plugin->queryStub()->mNodeType),
                        plugin->queryStub()->mNodeName, node->data);
                rt_safe_delete(plugin);
            }
        }
    }
    rt_hash_table_clear(pNodeBus);
    return RT_OK;
}
