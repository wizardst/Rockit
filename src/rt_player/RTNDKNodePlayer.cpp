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
#include "rt_header.h"        // NOLINT
#include "rt_hash_table.h"    // NOLINT
#include "rt_array_list.h"    // NOLINT
#include "rt_message.h"       // NOLINT
#include "rt_msg_handler.h"   // NOLINT
#include "rt_msg_looper.h"    // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTNDKNodePlayer"
#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x0

struct NodePlayerContext {
    RTNodeBus*          mNodeBus;
    RTMediaDirector*    mDirector;
    // thread used for data transfer between plugins
    RtThread*           mDeliverThread;
    struct RTMsgLooper* mLooper;
    UINT32              mState;
    RTSeekType          mSeekFlag;
    RT_BOOL             mLooping;
    RtMetaData         *mCmdOptions;
    INT64               mSaveSeekTimeUs;
    INT64               mWantSeekTimeUs;
    INT64               mCurTimeUs;
    INT64               mDuration;
    RT_CALLBACK_T       mRT_Callback;
    INT32               mRT_Callback_Type;
    void *              mRT_Callback_Data;
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

RTNDKNodePlayer::RTNDKNodePlayer() {
    mPlayerCtx = rt_malloc(NodePlayerContext);
    rt_memset(mPlayerCtx, 0, sizeof(NodePlayerContext));

    // node-bus manages node plugins
    mNodeBus = new RTNodeBus();

    // Message Queue Mechanism
    mPlayerCtx->mLooper  = new RTMsgLooper();
    mPlayerCtx->mLooper->setHandler(this);  // message handler is player

    // param config and performance collection
    mPlayerCtx->mDirector = new RTMediaDirector();

    // thread used for data transfer between plugins
    mPlayerCtx->mDeliverThread = NULL;
    mPlayerCtx->mRT_Callback   = NULL;
    mPlayerCtx->mLooping       = RT_FALSE;
    mPlayerCtx->mCmdOptions = new RtMetaData();

    init();

    setCurState(RT_STATE_IDLE);
}

RTNDKNodePlayer::~RTNDKNodePlayer() {
    this->release();
    rt_safe_free(mNodeBus);
    RT_LOGD("done, ~RTNDKNodePlayer()");
}

RT_RET RTNDKNodePlayer::release() {
    RT_RET err = checkRuntime("release");
    if (RT_OK != err) {
        return err;
    }

    // @review: release resources in player context
    mPlayerCtx->mLooper->stop();
    rt_safe_delete(mPlayerCtx->mLooper);
    rt_safe_delete(mPlayerCtx->mDirector);
    rt_safe_delete(mPlayerCtx->mCmdOptions);
    rt_safe_free(mPlayerCtx);

    // @review: release node bus
    rt_safe_delete(mNodeBus);

    return err;
}

RT_RET RTNDKNodePlayer::init() {
    RT_RET err = checkRuntime("init");
    if (RT_OK != err) {
        return err;
    }

    if (RT_NULL != mPlayerCtx->mLooper) {
        mPlayerCtx->mLooper->setName("MsgQueue");
        mPlayerCtx->mLooper->start();
    }
    return err;
}

RT_RET RTNDKNodePlayer::reset() {
    RT_RET err = checkRuntime("reset");
    if (RT_OK != err) {
        return err;
    }

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
    err = mNodeBus->releaseNodes();
    this->setCurState(RT_STATE_IDLE);
    return err;
}

RT_RET RTNDKNodePlayer::setDataSource(RTMediaUri *mediaUri) {
    RT_RET err = checkRuntime("setDataSource");
    if (RT_OK != err) {
        return err;
    }

    if (mediaUri->mUri[0] == RT_NULL) {
        this->setCurState(RT_STATE_INITIALIZED);
    } else {
        err = mNodeBus->autoBuild(mediaUri);
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
    }
    return err;
}

RT_RET RTNDKNodePlayer::prepare() {
    RT_RET err = checkRuntime("prepare");
    if (RT_OK != err) {
        return err;
    }

    UINT32 curState = this->getCurState();
    if ((RT_STATE_INITIALIZED != curState) && (RT_STATE_STOPPED != curState)) {
        RTStateUtil::dumpStateError(curState, __FUNCTION__);
        return RT_OK;
    }

    if (RT_STATE_INITIALIZED == curState) {
        err = mNodeBus->autoBuildCodecSink();
        if (RT_OK != err) {
            RT_LOGE("fail to use node-bus to build codec sink");
        }
    }

    setCurState(RT_STATE_PREPARING);

    // @TODO: do prepare player
    mNodeBus->excuteCommand(RT_NODE_CMD_PREPARE);

    RTMessage* msg = new RTMessage(RT_MEDIA_PREPARED, RT_NULL, this);
    mPlayerCtx->mLooper->send(msg, 0);
    return RT_OK;
}

RT_RET RTNDKNodePlayer::start() {
    RT_RET err = checkRuntime("start");
    if (RT_OK != err) {
        return err;
    }

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
    return err;
}

RT_RET RTNDKNodePlayer::pause() {
    RT_RET err = checkRuntime("pause");
    if (RT_OK != err) {
        return err;
    }

    UINT32 curState = this->getCurState();
    RTMessage* msg  = RT_NULL;
    switch (curState) {
      case RT_STATE_PAUSED:
        RTStateUtil::dumpStateError(curState, __FUNCTION__);
        break;
      case RT_STATE_STARTED:
        // @TODO: do pause player
        mNodeBus->excuteCommand(RT_NODE_CMD_PAUSE);

        msg = new RTMessage(RT_MEDIA_PAUSED, RT_NULL, this);
        mPlayerCtx->mLooper->send(msg, 0);
        break;
      default:
        RTStateUtil::dumpStateError(curState, __FUNCTION__);
        break;
    }
    return err;
}

RT_RET RTNDKNodePlayer::stop() {
    RT_RET err = checkRuntime("stop");
    if (RT_OK != err) {
        return err;
    }

    UINT32 curState = this->getCurState();
    RTMessage* msg  = RT_NULL;
    switch (curState) {
      case RT_STATE_STOPPED:
        RTStateUtil::dumpStateError(curState, __FUNCTION__);
        break;
      case RT_STATE_PREPARING:
      case RT_STATE_PREPARED:
      case RT_STATE_STARTED:
      case RT_STATE_PAUSED:
      case RT_STATE_COMPLETE:
      case RT_STATE_ERROR:
        // @TODO: do stop player
        mNodeBus->excuteCommand(RT_NODE_CMD_STOP);
        mPlayerCtx->mLooper->flush();
        msg = new RTMessage(RT_MEDIA_STOPPED, RT_NULL, this);
        mPlayerCtx->mLooper->send(msg, 0);
        mPlayerCtx->mCurTimeUs = 0;
        mPlayerCtx->mDuration  = 0;
        break;
    }

    return err;
}

RT_RET RTNDKNodePlayer::wait(int64_t timeUs) {
    UINT32 curState = RT_STATE_IDLE;
    UINT32 loopFlag = 1;
    timeUs = (timeUs < 1000000)? 20000000:timeUs;
    do {
        curState = this->getCurState();
        switch (curState) {
          case RT_STATE_IDLE:
          case RT_STATE_ERROR:
          case RT_STATE_COMPLETE:
            loopFlag = 0;
            break;
          default:
            RtTime::sleepMs(5);
            break;
        }
    } while ((1 == loopFlag)&&(mPlayerCtx->mCurTimeUs < timeUs));
    RT_LOGE("done, ndk-node-player completed playback! current:%lldms, timeout:%lldms",
             mPlayerCtx->mCurTimeUs/1000, timeUs/1000);

    return RT_OK;
}

RT_RET RTNDKNodePlayer::setLooping(RT_BOOL loop) {
    RT_RET err = checkRuntime("setLooping");
    if (RT_OK != err) {
        return err;
    }
    mPlayerCtx->mLooping = loop;
    return err;
}

RT_RET RTNDKNodePlayer::seekTo(INT64 usec) {
    RT_RET err = checkRuntime("seekTo");
    if (RT_OK != err) {
        return err;
    }

    RT_LOGE("seek %lld us", usec);
    UINT32 curState = this->getCurState();
    switch (curState) {
      case RT_STATE_IDLE:
      case RT_STATE_INITIALIZED:
      case RT_STATE_PREPARING:
        mPlayerCtx->mWantSeekTimeUs = -1;
        mPlayerCtx->mSaveSeekTimeUs = usec;
        RT_LOGE("seek %lld us", usec);
        RTStateUtil::dumpStateError(curState, "seekTo, save only");
        break;
      case RT_STATE_PREPARED:
      case RT_STATE_PAUSED:
      case RT_STATE_STARTED:
      case RT_STATE_COMPLETE:
        // @TODO: do pause player
        RT_LOGE("seek %lld us mPlayerCtx: %p mSeekFlag %d", usec, mPlayerCtx, mPlayerCtx->mSeekFlag);
        switch (mPlayerCtx->mSeekFlag) {
          case RT_SEEK_DOING:
            RT_LOGE("seek %lld us", usec);
            mPlayerCtx->mWantSeekTimeUs = -1;
            mPlayerCtx->mSaveSeekTimeUs = usec;
            break;
          default:
            RT_LOGE("seek %lld us", usec);
            mPlayerCtx->mSaveSeekTimeUs = -1;
            mPlayerCtx->mWantSeekTimeUs = usec;
            postSeekIfNecessary();
            break;
        }
        break;
      default:
        RTStateUtil::dumpStateError(curState, __FUNCTION__);
        break;
    }

    return err;
}

RT_RET RTNDKNodePlayer::postSeekIfNecessary() {
    RT_RET err = checkRuntime("postSeekIfNecessary");
    if (RT_OK != err) {
        return err;
    }

    // restore seek position if necessary
    if (-1 != mPlayerCtx->mSaveSeekTimeUs) {
         mPlayerCtx->mWantSeekTimeUs = mPlayerCtx->mSaveSeekTimeUs;
    }
    if (-1 == mPlayerCtx->mWantSeekTimeUs) {
        return RT_OK;
    }
    // mini seek margin is 500ms
    INT64 seekDelta = RT_ABS(mPlayerCtx->mWantSeekTimeUs - mPlayerCtx->mCurTimeUs);
    RT_LOGE("seek want seek %lld us, mCurTimeUs: %lld us",
             mPlayerCtx->mWantSeekTimeUs, mPlayerCtx->mCurTimeUs);
    if (seekDelta > 500*100) {
        // async seek message
        RTMessage* msg = new RTMessage(RT_MEDIA_SEEK_ASYNC, 0, mPlayerCtx->mWantSeekTimeUs, this);
        mPlayerCtx->mLooper->flush_message(RT_MEDIA_SEEK_ASYNC);
        mPlayerCtx->mLooper->post(msg, 0);
        mPlayerCtx->mSeekFlag = RT_SEEK_DOING;
        RT_LOGE("seek seekDelta %lld us mPlayerCtx: %p mSeekFlag: %d", seekDelta, mPlayerCtx, mPlayerCtx->mSeekFlag);
    }

    mPlayerCtx->mWantSeekTimeUs = -1;
    mPlayerCtx->mSaveSeekTimeUs = -1;

    return err;
}

RT_RET RTNDKNodePlayer::summary(INT32 fd) {
    RT_RET err = checkRuntime("summary");
    if (RT_OK != err) {
        return err;
    }

    mNodeBus->summary(0);
    return RT_OK;
}

RT_RET RTNDKNodePlayer::getCurrentPosition(int64_t *usec) {
    *usec = 0;
    RT_RET err = checkRuntime("getCurrentPosition");
    if (RT_OK != err) {
        return err;
    }
    *usec = mPlayerCtx->mCurTimeUs;
    return err;
}

RT_RET RTNDKNodePlayer::getDuration(int64_t *usec) {
    *usec = 0;
    RT_RET err = checkRuntime("getDuration");
    if (RT_OK != err) {
        return err;
    }
    *usec = mPlayerCtx->mDuration;
    return err;
}

RT_RET RTNDKNodePlayer::setCurState(UINT32 newState) {
    RT_RET err = checkRuntime("setCurState");
    if (RT_OK != err) {
        return err;
    }

    RT_LOGE("done, switch state:%s to state:%s", \
             RTStateUtil::getStateName(mPlayerCtx->mState), \
             RTStateUtil::getStateName(newState));
    mPlayerCtx->mState = newState;
    return err;
}

UINT32 RTNDKNodePlayer::getCurState() {
    RT_ASSERT(RT_NULL != mPlayerCtx);
    if (RT_NULL != mPlayerCtx) {
        return mPlayerCtx->mState;
    }
    return RT_STATE_ERROR;
}

RT_RET RTNDKNodePlayer::onSeekTo(INT64 usec) {
    RTMessage* msg  = RT_NULL;
    RT_RET err = checkRuntime("onSeekTo");
    if (RT_OK != err) {
        return err;
    }

    // workflow: pause flush (cache maybe) start
    #if 0
    mNodeBus->excuteCommand(RT_NODE_CMD_PAUSE);
    mNodeBus->excuteCommand(RT_NODE_CMD_FLUSH);
    mNodeBus->excuteCommand(RT_NODE_CMD_START);
    #else
    mPlayerCtx->mCmdOptions->clear();
    mPlayerCtx->mCmdOptions->setInt64(kKeySeekTimeUs, usec);
    mNodeBus->excuteCommand(RT_NODE_CMD_SEEK, mPlayerCtx->mCmdOptions);
    msg = new RTMessage(RT_MEDIA_SEEK_COMPLETE, RT_NULL, this);
    mPlayerCtx->mLooper->post(msg, 0);
    RT_LOGE("done, seek to target:%lldms", usec/1000);
    #endif
    return err;
}

RT_RET RTNDKNodePlayer::onPlaybackDone() {
    RT_RET err = checkRuntime("onPlaybackDone");
    if (RT_OK != err) {
        return err;
    }
    // workflow: pause flush (cache maybe) reset
    mNodeBus->excuteCommand(RT_NODE_CMD_PAUSE);
    mNodeBus->excuteCommand(RT_NODE_CMD_FLUSH);
    mNodeBus->excuteCommand(RT_NODE_CMD_RESET);
    if (mPlayerCtx->mRT_Callback != RT_NULL) {
        mPlayerCtx->mRT_Callback(mPlayerCtx->mRT_Callback_Type, mPlayerCtx->mRT_Callback_Data);
    }
    RT_LOGE("done, onPlaybackDone");

    return RT_OK;
}

RT_RET RTNDKNodePlayer::onPreparedDone() {
    RT_RET err = checkRuntime("onPreparedDone");
    if (RT_OK != err) {
        return err;
    }

    RTNode*          root      = mNodeBus->getRootNode(BUS_LINE_ROOT);
    RTNodeDemuxer*   demuxer   = reinterpret_cast<RTNodeDemuxer*>(root);
    if (RT_NULL != demuxer) {
        mPlayerCtx->mDuration = demuxer->queryDuration();
    }
}

RT_RET RTNDKNodePlayer::checkRuntime(const char* caller) {
    if ((RT_NULL == mPlayerCtx) || (RT_NULL == mNodeBus)) {
        RT_LOGE("fail to %s, context in null", caller);
        return RT_ERR_BAD;
    }
    return RT_OK;
}

/* looper functions or callback of thread */
void   RTNDKNodePlayer::onMessageReceived(struct RTMessage* msg) {
    const char* msgName = mEventNames[msg->getWhat()].name;
    RT_LOGE("done, received messsage(ptr=%p, what=%d, name=%s)", \
             msg, msg->getWhat(), msgName);
    switch (msg->getWhat()) {
      case RT_MEDIA_PREPARED:
        setCurState(RT_STATE_PREPARED);
        this->onPreparedDone();
        postSeekIfNecessary();
        break;
      case RT_MEDIA_PLAYBACK_COMPLETE:
        if (!mPlayerCtx->mLooping) {
            onPlaybackDone();
            setCurState(RT_STATE_COMPLETE);
        } else {
            seekTo(0ll);
        }
        break;
      case RT_MEDIA_STARTED:
        setCurState(RT_STATE_STARTED);
        break;
      case RT_MEDIA_PAUSED:
        setCurState(RT_STATE_PAUSED);
        break;
      case RT_MEDIA_ERROR:
        setCurState(RT_STATE_ERROR);
        break;
      case RT_MEDIA_STOPPED:
        setCurState(RT_STATE_STOPPED);
        break;
      case RT_MEDIA_BUFFERING_UPDATE:
        break;
      case RT_MEDIA_SEEK_COMPLETE:
        mPlayerCtx->mSeekFlag = RT_SEEK_NO;
        RT_LOGD("seek complete mPlayerCtx: %p mSeekFlag: %d", mPlayerCtx, mPlayerCtx->mSeekFlag);
        mPlayerCtx->mCurTimeUs = mPlayerCtx->mWantSeekTimeUs;
        postSeekIfNecessary();
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
        onSeekTo(msg->mData.mArgU64);
        break;
      default:
        break;
    }
}

RT_RET RTNDKNodePlayer::startDataLooper() {
    mNodeBus->excuteCommand(RT_NODE_CMD_START);
    return RT_OK;
}

RT_RET RTNDKNodePlayer::writeData(const char * data, const UINT32 length, int flag, int type) {
    RT_ASSERT(RT_NULL != mPlayerCtx);
    if (RT_WRITEDATA_PCM == flag) {
        RTMediaBuffer* esPacket;
        RTNode* decoder   = mNodeBus->getRootNode(BUS_LINE_AUDIO);
        if (decoder != RT_NULL) {
            RTNodeAdapter::dequeCodecBuffer(decoder, &esPacket, RT_PORT_INPUT);
            if (RT_NULL != esPacket) {
                char *tempdata = rt_malloc_size(char, length);
                rt_memcpy(tempdata, data, length);
                esPacket->setData(tempdata, length);
                RTNodeAdapter::pushBuffer(decoder, esPacket);
            } else {
                RT_LOGD("writeData list null");
            }
        } else {
            RT_LOGD("writeData decoder null");
        }
    } else if (RT_WRITEDATA_TS == flag) {
    } else {
        if (RTTRACK_TYPE_VIDEO == type) {
        } else {
        }
    }
    return RT_OK;
}

RT_RET RTNDKNodePlayer::setCallBack(RT_CALLBACK_T callback, int p_event, void *p_data) {
    RT_ASSERT(RT_NULL != mPlayerCtx);
    mPlayerCtx->mRT_Callback = callback;
    mPlayerCtx->mRT_Callback_Type = p_event;
    mPlayerCtx->mRT_Callback_Data = p_data;
    return RT_OK;
}

RT_RET RTNDKNodePlayer::startAudioPlayerProc() {
    RT_RET           err       = RT_OK;
    INT32            eosFlag   = 0;
    RTNode*          root      = mNodeBus->getRootNode(BUS_LINE_ROOT);
    RTNodeDemuxer*   demuxer   = reinterpret_cast<RTNodeDemuxer*>(root);
    RTNode*          decoder   = mNodeBus->getRootNode(BUS_LINE_AUDIO);
    RTNodeAudioSink* audiosink = RT_NULL;
    if (RT_NULL != decoder) {
        audiosink = reinterpret_cast<RTNodeAudioSink*>(decoder->mNext);
        decoder->setEventLooper(mPlayerCtx->mLooper);
        audiosink->setEventLooper(mPlayerCtx->mLooper);
    }

    if ((RT_NULL == decoder) || (RT_NULL == audiosink)) {
        return RT_ERR_BAD;
    }

    RTMediaBuffer* frame   = RT_NULL;
    RTMediaBuffer* esPacket = RT_NULL;

    while (mPlayerCtx->mDeliverThread->getState() == THREAD_LOOP) {
        UINT32 curState = this->getCurState();
        if (RT_STATE_STARTED != curState) {
            RtTime::sleepMs(2);
            continue;
        }

        if (demuxer != RT_NULL) {
            // deqeue buffer from object pool
            INT32 audio_idx  = demuxer->queryTrackUsed(RTTRACK_TYPE_AUDIO);
            while (RT_NULL == esPacket) {
                RTNodeAdapter::dequeCodecBuffer(decoder, &esPacket, RT_PORT_INPUT);
                RtTime::sleepMs(2);
            }

            RT_BOOL validPkt = RT_FALSE;
            do {
                do  {
                    // save es-packet to buffer
                    err      = RTNodeAdapter::pullBuffer(demuxer, &esPacket);
                    curState = this->getCurState();
                    if (RT_STATE_STARTED != curState) {
                        break;
                    } else {
                        RtTime::sleepMs(2);
                    }
                } while ((RT_OK != err) && (RT_STATE_STARTED == curState) && !eosFlag);

                INT32 track_index = 0;
                esPacket->getMetaData()->findInt32(kKeyPacketIndex, &track_index);

                if (track_index == audio_idx) {
                    UINT32 size = esPacket->getSize();
                    INT32 eos = 0;
                    esPacket->getMetaData()->findInt32(kKeyFrameEOS, &eos);
                    if (eos) {
                        eosFlag = RT_TRUE;
                    }
                    RT_LOGD_IF(DEBUG_FLAG, "audio es-packet(ptr=0x%p, size=%d, eosFlag=%d)", esPacket, size, eosFlag);
                    validPkt = RT_TRUE;
                } else {
                    /* pass other packet */
                    INT32 eos = 0;
                    esPacket->getMetaData()->findInt32(kKeyFrameEOS, &eos);
                    esPacket->release(RT_TRUE);
                    if (eos) {
                        RT_LOGD("receive eos , break");
                        break;
                    }
                    continue;
                }
            } while (!validPkt && !eosFlag);

            if (validPkt) {
                // push es-packet to decoder
                RTNodeAdapter::pushBuffer(decoder, esPacket);
                esPacket = RT_NULL;
            }
        }

        // pull av-frame from decoder
        RTNodeAdapter::pullBuffer(decoder, &frame);

        if (frame) {
            if (frame->getStatus() == RT_MEDIA_BUFFER_STATUS_READY) {
                INT64 timeUs = 0ll;
                frame->getMetaData()->findInt64(kKeyFramePts, &timeUs);
                RT_LOGD_IF(DEBUG_FLAG, "audio frame(ptr=0x%p, size=%d, timeUs=%lldms)",
                        frame->getData(), frame->getLength(), timeUs/1000);

                RTNodeAdapter::pushBuffer(audiosink, frame);
                INT32 eos = 0;
                frame->getMetaData()->findInt32(kKeyFrameEOS, &eos);
                if (eos) {
                    // get eos frame, reset eos flags
                    RT_LOGE("eos %d", eos);
                    eosFlag = RT_FALSE;
                } else {
                    mPlayerCtx->mCurTimeUs = timeUs;
                }
            }

            frame = NULL;
        }

        // dump AVFrame
        RtTime::sleepMs(5);
    }

    return RT_OK;
}
