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

#ifndef SRC_RT_NODE_FF_NODE_INCLUDE_FFNODEDEMUXER_H_
#define SRC_RT_NODE_FF_NODE_INCLUDE_FFNODEDEMUXER_H_

#include "RTNodeDemuxer.h" // NOLINT

#ifdef  __cplusplus
extern "C" {
#endif

class FFNodeDemuxer : public RTNodeDemuxer {
 public:
    FFNodeDemuxer();
    ~FFNodeDemuxer();
    RT_RET runTask();

    // override RTNode public methods
    virtual RT_RET init(RtMetaData *metaData);
    virtual RT_RET release();

    virtual RT_RET pullBuffer(RTMediaBuffer** mediaBuf);
    virtual RT_RET pushBuffer(RTMediaBuffer*  mediaBuf);

    virtual RT_RET runCmd(RT_NODE_CMD cmd, RtMetaData *metaData);
    virtual RT_RET setEventLooper(RTMsgLooper* eventLooper);

    virtual RtMetaData* queryFormat(RTPortType port);
    virtual RTNodeStub* queryStub();

 public:
    // override RTNodeDemuxer methods
    virtual INT32 countTracks(RTTrackType tType);
    virtual INT32 selectTrack(INT32 index, RTTrackType tType);
    virtual RtMetaData* queryTrackMeta(UINT32 index, RTTrackType tType);
    virtual INT32       queryTrackUsed(RTTrackType tType);
    virtual INT64       queryDuration();

 protected:
    // override RTNode protected methods
    virtual RT_RET onStart();
    virtual RT_RET onPause();
    virtual RT_RET onStop();
    virtual RT_RET onReset();
    virtual RT_RET onFlush();
    virtual RT_RET onSeek(RtMetaData *options);
    virtual RT_RET onPrepare();
};

extern struct RTNodeStub ff_node_demuxer;

#ifdef  __cplusplus
}
#endif

#endif  // SRC_RT_NODE_FF_NODE_INCLUDE_FFNODEDEMUXER_H_
