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
 *   Date: 2019/03/11
 * Module: output audio pcm with WASAPI
 *
 */

#ifndef SRC_RT_NODE_RT_SINK_INCLUDE_RTSINKAUDIOWASAPI_H_
#define SRC_RT_NODE_RT_SINK_INCLUDE_RTSINKAUDIOWASAPI_H_

#include "RTNodeAudioSink.h"    // NOLINT
#include "rt_header.h"          // NOLINT
#include "RTObjectPool.h"       // NOLINT
#include "rt_thread.h"          // NOLINT
#include "rt_dequeue.h"         // NOLINT

class RTSinkAudioWASAPI : public RTNodeAudioSink {
 public:
    RTSinkAudioWASAPI();
    virtual ~RTSinkAudioWASAPI();
    RT_RET runTask();

    // override RTNode methods
    virtual RT_RET init(RtMetaData *metaData);
    virtual RT_RET release();
    virtual RT_RET pullBuffer(RTMediaBuffer** mediaBuf);
    virtual RT_RET pushBuffer(RTMediaBuffer*  mediaBuf);
    virtual RT_RET runCmd(RT_NODE_CMD cmd, RtMetaData *metaData);
    virtual RT_RET setEventLooper(RTMsgLooper* eventLooper);

    virtual RtMetaData* queryFormat(RTPortType port);
    virtual RTNodeStub* queryStub();

 public:
    // override RTNodeAudioSink methods
    virtual RT_RET  SetVolume(int volume);
    virtual INT32   GetVolume();
    virtual RT_BOOL GetMute();
    virtual RT_RET  Mute(RT_BOOL muted);

 protected:
    // override RTNode methods
    virtual RT_RET onStart();
    virtual RT_RET onStop();
    virtual RT_RET onPause();
    virtual RT_RET onFlush();
    virtual RT_RET onReset();
};

extern struct RTNodeStub rt_sink_audio_wasapi;

#endif  // SRC_RT_NODE_RT_SINK_INCLUDE_RTSINKAUDIOWASAPI_H_
