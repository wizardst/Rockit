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
 * author: rimon.xu@rock-chips.com
 *   date: 20100321
 * module: local packet source
 */

#ifndef SRC_RT_MEDIA_INCLUDE_RTPKTSOURCELOCAL_H_
#define SRC_RT_MEDIA_INCLUDE_RTPKTSOURCELOCAL_H_

#include "rt_header.h"           // NOLINT
#include "rt_dequeue.h"          // NOLINT
#include "RTPktSourceBase.h"     // NOLINT

class RTPktSourceLocal : public RTPktSourceBase {
 public:
    RTPktSourceLocal();
    ~RTPktSourceLocal();

    virtual RT_RET init(RtMetaData *config);
    virtual RT_RET release();
    virtual RT_RET start();
    virtual RT_RET stop();
    virtual RT_RET flush();

    virtual INT32  getTotalCacheSize();
    virtual INT64  getAudioCacheDuration();
    virtual INT64  getVideoCacheDuration();
    virtual RTPacket *dequeueUnusedPacket(RT_BOOL block = RT_FALSE);
    virtual RT_RET queuePacket(RTPacket *pkt);
    virtual RT_RET queueNullPacket(INT32 streamIndex, RTTrackType type);
    virtual RTPacket *dequeuePacket(RTTrackType type, RT_BOOL block = RT_FALSE);
    virtual RT_RET queueUnusedPacket(RTPacket *pkt);

    // override pure virtual methods of RTObject class
    virtual const char* getName() { return "RTPktSourceLocal"; }
    virtual void summary(INT32 fd) {}

 private:
    RTMediaCache       *mVideoCache;
    RTMediaCache       *mAudioCache;
    RT_Deque           *mVideoPktQ;
    RT_Deque           *mAudioPktQ;
    RtMutex            *mVideoQLock;
    RtMutex            *mAudioQLock;
    RtCondition        *mCondition;
    RtMutex            *mWaitLock;

    INT32               mMaxCacheSize;
};

#endif  // SRC_RT_MEDIA_INCLUDE_RTPKTSOURCELOCAL_H_

