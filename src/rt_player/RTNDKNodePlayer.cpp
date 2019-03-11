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
 * Author: martin.cheng@rock-chips.com
 *   Date: 2019/03/10
 *   Task: build player streamline with node bus.
 */

#include "RTNDKNodePlayer.h"
#include "RTNode.h"           // NOLINT
#include "RTNodeDemuxer.h"    // NOLINT
#include "RTNodeAudioSink.h"  // NOLINT
#include "rt_header.h"       // NOLINT
#include "rt_hash_table.h"   // NOLINT
#include "rt_array_list.h"   // NOLINT
#include "rt_message.h"      // NOLINT
#include "rt_msg_handler.h"  // NOLINT
#include "rt_msg_looper.h"   // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTNDKNodePlayer"
#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x1

struct NodePlayerContext {
    RTNodeBus*          mNodeBus;
    RTMediaDirector*    mDirector;
    // thread used for data transfer between plugins
    RtThread*           mDeliverThread;
    struct RTMsgLooper* mLooper;
    UINT32              mState;
    INT64               mWantSeekTimeUs;
    INT64               mLastSeekTimeUs;
};

void* thread_deliver_proc(void* pNodePlayer) {
    RTNDKNodePlayer* pPlayer = reinterpret_cast<RTNDKNodePlayer*>(pNodePlayer);
    if (RT_NULL != pPlayer) {
        // @TODO data transferring between plugins
        // @THIS data transferring for audio player
        pPlayer->startAudioPlayerProc();
    }
    return RT_NULL;
}

RT_VOID audio_sink_feed_callback(RTNode* pNode, RTMediaBuffer* data) {
    RTNodeAdapter::queueCodecBuffer(pNode, data, RT_PORT_OUTPUT);
}

RTNDKNodePlayer::RTNDKNodePlayer() {
    mPlayerCtx = rt_malloc(NodePlayerContext);
    mNodeBus = new RTNodeBus();

    // Message Queue Mechanism
    mPlayerCtx->mLooper  = new RTMsgLooper();

    // param config and performance collection
    mPlayerCtx->mDirector = new RTMediaDirector();

    // thread used for data transfer between plugins
    mPlayerCtx->mDeliverThread = NULL;

    init();

    setCurState(RT_STATE_IDLE);
}

RTNDKNodePlayer::~RTNDKNodePlayer() {
    RT_ASSERT(RT_NULL != mNodeBus);
    this->release();
    rt_safe_free(mNodeBus);
    RT_LOGD("done, ~RTNDKNodePlayer()");
}

RT_RET RTNDKNodePlayer::release() {
    RT_ASSERT(RT_NULL != mPlayerCtx);
    RT_ASSERT(RT_NULL != mNodeBus);
    RT_ASSERT(RT_NULL != mPlayerCtx->mLooper);

    rt_safe_delete(mNodeBus);
    if (RT_NULL != mPlayerCtx->mLooper) {
        mPlayerCtx->mLooper->stop();
        rt_safe_delete(mPlayerCtx->mLooper);
    }
    if (RT_NULL != mPlayerCtx->mDirector) {
        rt_safe_delete(mPlayerCtx->mDirector);
    }
    rt_safe_free(mPlayerCtx);
    return RT_OK;
}

RT_RET RTNDKNodePlayer::init() {
    RT_ASSERT(RT_NULL != mPlayerCtx);
    RT_ASSERT(RT_NULL != mNodeBus);
    RT_ASSERT(RT_NULL != mPlayerCtx->mLooper);
    if (RT_NULL != mPlayerCtx->mLooper) {
        mPlayerCtx->mLooper->setName("MsgQueue");
        mPlayerCtx->mLooper->start();
    }
    return RT_OK;
}

RT_RET RTNDKNodePlayer::reset() {
    UINT32 curState = this->getCurState();
    if (RT_STATE_IDLE == curState) {
        RTStateUtil::dumpStateError(curState, __FUNCTION__);
        return RT_OK;
    }
    if (RT_STATE_STOPPED != curState) {
        RT_LOGE("Need stop() before reset(), state(%d)", curState);
        this->stop();
    }

    // shutdown thread of data delivering between plugins
    if (RT_NULL != mPlayerCtx->mDeliverThread) {
        mPlayerCtx->mDeliverThread->requestInterruption();
        mPlayerCtx->mDeliverThread->join();
        rt_safe_delete(mPlayerCtx->mDeliverThread);
        mPlayerCtx->mDeliverThread = RT_NULL;
    }

    // @TODO: do reset player
    mNodeBus->excuteCommand(RT_NODE_CMD_RESET);

    // release all [RTNodes] in node_bus;
    // but NO NEED to release [NodeStub].
    RT_RET err = mNodeBus->releaseNodes();
    this->setCurState(RT_STATE_IDLE);
    return err;
}

RT_RET RTNDKNodePlayer::setDataSource(RTMediaUri *mediaUri) {
    RT_ASSERT(RT_NULL != mNodeBus);
    RT_RET err = mNodeBus->autoBuild(mediaUri);
    if (RT_OK != err) {
        if (RT_NULL == mNodeBus->getRootNode(BUS_LINE_ROOT)) {
            RT_LOGE("fail to init demuxer");
            mPlayerCtx->mLooper->flush();
            RTMessage* msg = new RTMessage(RT_MEDIA_ERROR, RT_NULL, this);
            mPlayerCtx->mLooper->send(msg, 0);
            mPlayerCtx->mLooper->requestExit();
            return RT_ERR_UNKNOWN;
        }
    } else {
        this->setCurState(RT_STATE_INITIALIZED);
    }

    return err;
}

RT_RET RTNDKNodePlayer::prepare() {
    UINT32 curState = this->getCurState();
    if ((RT_STATE_INITIALIZED != curState) && (RT_STATE_STOPPED != curState)) {
        RTStateUtil::dumpStateError(curState, __FUNCTION__);
        return RT_OK;
    }

    setCurState(RT_STATE_PREPARING);

    // @TODO: do prepare player
    mNodeBus->excuteCommand(RT_NODE_CMD_PREPARE);

    RTMessage* msg = new RTMessage(RT_MEDIA_PREPARED, RT_NULL, this);
    mPlayerCtx->mLooper->send(msg, 0);
    return RT_OK;
}

RT_RET RTNDKNodePlayer::start() {
    UINT32 curState = getCurState();
    RTMessage* msg  = RT_NULL;
    switch (curState) {
      case RT_STATE_PREPARED:
      case RT_STATE_PAUSED:
      case RT_STATE_COMPLETE:
        // @TODO: do resume player
        mNodeBus->excuteCommand(RT_NODE_CMD_START);

        // thread used for data transferring between plugins
        if (RT_NULL == mPlayerCtx->mDeliverThread) {
            mPlayerCtx->mDeliverThread = new RtThread(thread_deliver_proc, this);
            mPlayerCtx->mDeliverThread->setName("BusDataProc");
            mPlayerCtx->mDeliverThread->start();
        }

        msg = new RTMessage(RT_MEDIA_STARTED, RT_NULL, this);
        mPlayerCtx->mLooper->send(msg, 0);
        break;
      default:
        RTStateUtil::dumpStateError(curState, __FUNCTION__);
        break;
    }
    return RT_OK;
}

RT_RET RTNDKNodePlayer::pause() {
    UINT32 curState = this->getCurState();
    RTMessage* msg  = RT_NULL;
    switch (curState) {
      case RT_STATE_PAUSED:
        RTStateUtil::dumpStateError(curState, __FUNCTION__);
        break;
      case RT_STATE_STARTED:
        // @TODO: do pause player

        msg = new RTMessage(RT_STATE_PAUSED, RT_NULL, this);
        mPlayerCtx->mLooper->send(msg, 0);
        break;
      default:
        RTStateUtil::dumpStateError(curState, __FUNCTION__);
        break;
    }
    return RT_OK;
}

RT_RET RTNDKNodePlayer::stop() {
    UINT32 curState = this->getCurState();
    switch (curState) {
      case RT_STATE_STOPPED:
        RTStateUtil::dumpStateError(curState, __FUNCTION__);
        break;
      case RT_STATE_PREPARING:
      case RT_STATE_PREPARED:
      case RT_STATE_STARTED:
      case RT_STATE_PAUSED:
      case RT_STATE_COMPLETE:
      case RT_STATE_STATE_ERROR:
        // @TODO: do stop player
        mNodeBus->excuteCommand(RT_NODE_CMD_STOP);
        mPlayerCtx->mLooper->flush();
        break;
    }

    return RT_OK;
}

RT_RET RTNDKNodePlayer::wait() {
    UINT32 curState = RT_STATE_IDLE;
    UINT32 loopFlag = 1;
    do {
        curState = this->getCurState();
        switch (curState) {
          case RT_STATE_STATE_ERROR:
          case RT_STATE_COMPLETE:
            loopFlag = 0;
            break;
          default:
            RtTime::sleepMs(5);
            break;
        }
    } while (1 == loopFlag);
    return RT_OK;
    RT_LOGE("done, ndk-node-player completed playback!");
}

RT_RET RTNDKNodePlayer::seekToAsync(INT64 usec) {
    // @TODO: do seek player
    // ...

    RTMessage* msg = new RTMessage(RT_MEDIA_SEEK_COMPLETE, RT_NULL, this);
    mPlayerCtx->mLooper->post(msg, 0);
    return RT_OK;
}

RT_RET RTNDKNodePlayer::seekTo(INT64 usec) {
    UINT32 curState = this->getCurState();
    RTMessage* msg  = RT_NULL;
    switch (curState) {
      case RT_STATE_PREPARING:
      case RT_STATE_PREPARED:
        mPlayerCtx->mWantSeekTimeUs = usec;
        RTStateUtil::dumpStateError(curState, "seekTo, save only");
        break;
      case RT_STATE_PAUSED:
      case RT_STATE_STARTED:
      case RT_STATE_COMPLETE:
        // @TODO: do pause player
        mPlayerCtx->mWantSeekTimeUs = usec;
        mPlayerCtx->mLooper->flush_message(RT_MEDIA_SEEK_ASYNC);
        // async seek message
        msg = new RTMessage(RT_MEDIA_SEEK_ASYNC, 0, usec, this);
        mPlayerCtx->mLooper->post(msg, 0);
        break;
      default:
        RTStateUtil::dumpStateError(curState, __FUNCTION__);
        break;
    }

    return RT_OK;
}

RT_RET RTNDKNodePlayer::summary(INT32 fd) {
    mNodeBus->summary(0);
    return RT_OK;
}

RT_RET RTNDKNodePlayer::setCurState(UINT32 newState) {
    RT_ASSERT(RT_NULL != mPlayerCtx);
    RT_LOGE("done, switch state:%s to state:%s", \
             RTStateUtil::getStateName(mPlayerCtx->mState), \
             RTStateUtil::getStateName(newState));
    mPlayerCtx->mState = newState;
    return RT_OK;
}

UINT32 RTNDKNodePlayer::getCurState() {
    RT_ASSERT(RT_NULL != mPlayerCtx);
    return mPlayerCtx->mState;
}

/* looper functions or callback of thread */
void   RTNDKNodePlayer::onMessageReceived(struct RTMessage* msg) {
    const char* msgName = mEventNames[msg->getWhat()].name;
    RT_LOGE("done, received messsage(ptr=%p, what=%d, name=%s)", \
             msg, msg->getWhat(), msgName);
    switch (msg->getWhat()) {
      case RT_MEDIA_PREPARED:
        setCurState(RT_STATE_PREPARED);
        break;
      case RT_MEDIA_PLAYBACK_COMPLETE:
        setCurState(RT_STATE_COMPLETE);
        break;
      case RT_MEDIA_STARTED:
        setCurState(RT_STATE_STARTED);
        break;
      case RT_MEDIA_PAUSED:
        setCurState(RT_STATE_PAUSED);
        break;
      case RT_MEDIA_ERROR:
        setCurState(RT_STATE_STATE_ERROR);
        break;
      case RT_MEDIA_STOPPED:
        setCurState(RT_STATE_STOPPED);
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

RT_RET RTNDKNodePlayer::startDataLooper() {
    mNodeBus->excuteCommand(RT_NODE_CMD_START);
    return RT_OK;
}

RT_RET RTNDKNodePlayer::startAudioPlayerProc() {
    RTNode*          root      = mNodeBus->getRootNode(BUS_LINE_ROOT);
    RTNodeDemuxer*   demuxer   = reinterpret_cast<RTNodeDemuxer*>(root);
    RTNode*          decoder   = mNodeBus->getRootNode(BUS_LINE_AUDIO);
    RTNodeAudioSink* audiosink = RT_NULL;
    if (RT_NULL != decoder) {
        audiosink = reinterpret_cast<RTNodeAudioSink*>(decoder->mNext);
        audiosink->queueCodecBuffer = audio_sink_feed_callback;
        audiosink->callback_ptr     = decoder;
    }
    INT32 audio_idx  = demuxer->queryTrackUsed(RTTRACK_TYPE_AUDIO);

    if ((RT_NULL != demuxer)&&(RT_NULL != decoder)) {
        RTMediaBuffer *frame = RT_NULL;
        RTMediaBuffer* esPacket;

        while (mPlayerCtx->mDeliverThread->getState() == THREAD_LOOP) {
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
                        // UINT8 *data = reinterpret_cast<UINT8 *>(esPacket->getData());
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
                    // char* data = reinterpret_cast<char *>(frame->getData());

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
        }
    }
    return RT_OK;
}
