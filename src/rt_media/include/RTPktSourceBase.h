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
 *   date: 20190320
 * module: media packet source base
 */

#ifndef SRC_RT_MEDIA_INCLUDE_RTPKTSOURCEBASE_H_
#define SRC_RT_MEDIA_INCLUDE_RTPKTSOURCEBASE_H_

#include "rt_header.h"      // NOLINT
#include "RTObject.h"       // NOLINT
#include "RTMediaData.h"    // NOLINT

class RTMediaCache : public RTObject {
 public:
    RTMediaCache();
    virtual ~RTMediaCache();

    RT_BOOL isFull();
    RT_BOOL isInsufficient();

    INT64   mCurCacheDuration;
    INT32   mCurCacheSize;
    INT32   mCurCacheCount;
    INT64   mHighWaterCacheDuration;
    INT32   mHighWaterCacheCount;
    INT64   mLowWaterCacheDuration;
    INT32   mLowWaterCacheCount;

    // override pure virtual methods of RTObject class
    virtual const char* getName() { return "RTMediaCache"; }
    virtual void summary(INT32 fd) {}
};

class RTPktSourceBase : public RTObject {
 public:
    RTPktSourceBase();
    virtual ~RTPktSourceBase();

    virtual RT_RET init(RtMetaData *config) = 0;
    virtual RT_RET release() = 0;
    virtual RT_RET start() = 0;
    virtual RT_RET stop() = 0;
    virtual RT_RET flush() = 0;

    virtual INT32  getTotalCacheSize() = 0;
    virtual INT64  getAudioCacheDuration() = 0;
    virtual INT64  getVideoCacheDuration() = 0;
    virtual RTPacket *dequeueUnusedPacket(RT_BOOL block = RT_TRUE) = 0;
    virtual RT_RET queuePacket(RTPacket *pkt) = 0;
    virtual RT_RET queueNullPacket(INT32 streamIndex, RTTrackType type) = 0;
    virtual RTPacket *dequeuePacket(RTTrackType type, RT_BOOL block = RT_TRUE) = 0;
    virtual RT_RET queueUnusedPacket(RTPacket *pkt) = 0;

    // override pure virtual methods of RTObject class
    virtual const char* getName() { return "RTPktSourceBase"; }
};

#endif  // SRC_RT_MEDIA_INCLUDE_RTPKTSOURCEBASE_H_
