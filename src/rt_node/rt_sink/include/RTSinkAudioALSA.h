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
 *   Date: 2019/02/21
 * Module: output audio pcm with ALSA
 */

#ifndef SRC_RT_NODE_RT_SINK_INCLUDE_RTSINKAUDIOALSA_H_
#define SRC_RT_NODE_RT_SINK_INCLUDE_RTSINKAUDIOALSA_H_

#include "RTNodeAudioSink.h"
#include "rt_header.h" // NOLINT
#if OS_LINUX
#include "ALSAAapterImpl.h"
#endif
#include "RTObjectPool.h" // NOLINT
#include "rt_thread.h" // NOLINT
#include "rt_dequeue.h" // NOLINT
#include "ALSAVolumeManager.h"

class RTSinkAudioALSA : public RTNodeAudioSink {
 public:
    RTSinkAudioALSA();
    virtual ~RTSinkAudioALSA();
    RT_RET runTask();
    // override RTNode methods
    virtual RT_RET init(RtMetaData *metaData);
    virtual RT_RET release();
    virtual RT_RET pullBuffer(RTMediaBuffer** mediaBuf);
    virtual RT_RET pushBuffer(RTMediaBuffer*  mediaBuf);

    virtual RT_RET setEventLooper(RTMsgLooper* eventLooper);
    virtual RT_RET runCmd(RT_NODE_CMD cmd, RtMetaData *metaData);

    virtual RtMetaData* queryFormat(RTPortType port);
    virtual RTNodeStub* queryStub();

 public:
    // override RTNodeAudioSink methods
    virtual RT_RET   setVolume(int volume);
    virtual INT32    getVolume();
    virtual RT_BOOL  getMute();
    virtual RT_RET   setMute(RT_BOOL muted);

 protected:
    // override RTNode methods
    virtual RT_RET onStart();
    virtual RT_RET onStop();
    virtual RT_RET onPause();
    virtual RT_RET onFlush();
    virtual RT_RET onReset();

 private:
    RT_RET openSoundCard(RtMetaData *metadata);
    RT_RET closeSoundCard();
    RT_VOID usleepData(INT32 samplerate, INT32 channels, INT32 bytes);

    RT_Deque          *mDeque;
    ALSASinkContext   *mALSASinkCtx;
    RtThread          *mThread;
    RtMutex           *mLockBuffer;
    RT_Deque          *mQueueBuffer;
    RTObjectPool      *mPoolBuffer;
    INT32              mCodecId;
    INT32              mProfile;
    UINT32             mSize;
    RT_BOOL            mStart;
    UINT32             mCountPull;
    UINT32             mCountPush;
    RTMsgLooper       *mEventLooper;
    ALSAVolumeManager *mVolManager;
    INT32              mSampleRate;
    INT32              mChannels;
    INT32              mDataSize;

    typedef enum _audio_play_status {
        PLAY_STOPPED = 0,  // Playback stopped or has not started yet.
        PLAY_START,        // add for start
        PLAY_PLAYING,      // Playback started & player is actively decoding.
        PLAY_PAUSED,       // Playback paused; player is actively decoding but no new data is given out.
        PLAY_CLOSED,       // Playback is stopped and player is closing.
    } AudioPlayStatus;
    AudioPlayStatus    mPlayStatus;
};

extern struct RTNodeStub rt_sink_audio_alsa;

#endif  // SRC_RT_NODE_RT_SINK_INCLUDE_RTSINKAUDIOALSA_H_

