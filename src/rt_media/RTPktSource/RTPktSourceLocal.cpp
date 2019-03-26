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

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTPktSourceLocal"

#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x0

#include "RTPktSourceLocal.h"       // NOLINT

#define HIGH_WATER_CACHE_DURATION       10 * 1000 * 1000   // 10s
#define HIGH_WATER_CACHE_COUNT          600
#define HIGH_WATER_CACHE_SIZE           30 * 1024 * 1024   // 30MB
#define LOW_WATER_CACHE_DURATION        2 * 1000 * 1000    // 2s

RTPktSourceLocal::RTPktSourceLocal()
        : mVideoPktQ(RT_NULL),
          mAudioPktQ(RT_NULL),
          mMaxCacheSize(HIGH_WATER_CACHE_SIZE) {
    mVideoQLock = new RtMutex();
    RT_ASSERT(RT_NULL != mVideoQLock);

    mAudioQLock = new RtMutex();
    RT_ASSERT(RT_NULL != mAudioQLock);

    mVideoCache = new RTMediaCache();
    RT_ASSERT(RT_NULL != mVideoCache);

    mAudioCache = new RTMediaCache();
    RT_ASSERT(RT_NULL != mAudioCache);

    mCondition = new RtCondition();
    RT_ASSERT(RT_NULL != mCondition);

    mWaitLock = new RtMutex();
    RT_ASSERT(RT_NULL != mWaitLock);
}

RTPktSourceLocal::~RTPktSourceLocal() {
    release();
}

RT_RET RTPktSourceLocal::init(RtMetaData *config) {
    RT_RET ret = RT_OK;
    RT_ASSERT(RT_NULL != config);

    INT64 maxCacheDuration;
    if (!config->findInt64(kKeyMaxCacheDuration, &maxCacheDuration)) {
        maxCacheDuration = HIGH_WATER_CACHE_DURATION;
    }
    INT32 maxCacheCount;
    if (!config->findInt32(kKeyMaxCacheCount, &maxCacheCount)) {
        maxCacheCount = HIGH_WATER_CACHE_COUNT;
    }
    INT32 maxCacheSize;
    if (!config->findInt32(kKeyMaxCacheSize, &maxCacheSize)) {
        maxCacheSize = HIGH_WATER_CACHE_SIZE;
    }

    mVideoCache->mHighWaterCacheCount = maxCacheCount;
    mVideoCache->mHighWaterCacheDuration = maxCacheDuration;
    mAudioCache->mHighWaterCacheCount = maxCacheCount;
    mAudioCache->mHighWaterCacheDuration = maxCacheDuration;

    mMaxCacheSize = maxCacheSize;

    RT_LOGD("init: cache size: %d, count: %d, duration: %lld",
              mMaxCacheSize, maxCacheCount, maxCacheDuration);

    mVideoPktQ = deque_create(maxCacheCount);
    RT_ASSERT(RT_NULL != mVideoPktQ);
    mAudioPktQ = deque_create(maxCacheCount);
    RT_ASSERT(RT_NULL != mAudioPktQ);

    return ret;
}

RT_RET RTPktSourceLocal::release() {
    RT_RET ret = RT_OK;
    if (mVideoPktQ && mAudioPktQ) {
        flush();
    }
    if (mVideoPktQ) {
        deque_destory(&mVideoPktQ);
    }
    if (mAudioPktQ) {
        deque_destory(&mAudioPktQ);
    }

    rt_safe_delete(mVideoQLock);
    rt_safe_delete(mAudioQLock);
    rt_safe_delete(mVideoCache);
    rt_safe_delete(mAudioCache);
    rt_safe_delete(mCondition);
    rt_safe_delete(mWaitLock);

    return ret;
}

RT_RET RTPktSourceLocal::start() {
    RT_RET ret = RT_OK;
    return ret;
}

RT_RET RTPktSourceLocal::stop() {
    RT_RET ret = RT_OK;
    RT_LOGE("this: %p stop", this);
    RtMutex::RtAutolock autoLock(mWaitLock);
    mCondition->signal();
    return ret;
}

RT_RET RTPktSourceLocal::flush() {
    RTPacket *pkt;
    while (deque_size(mVideoPktQ) > 0) {
        RtMutex::RtAutolock autoLock(mVideoQLock);
        RT_DequeEntry entry = deque_pop(mVideoPktQ);
        if (entry.data) {
            pkt = reinterpret_cast<RTPacket *>(entry.data);
        }
        mVideoCache->mCurCacheCount = deque_size(mVideoPktQ);
        mVideoCache->mCurCacheDuration -= pkt->mDuration;
        mVideoCache->mCurCacheSize -= pkt->mSize;
        rt_utils_packet_free(pkt);
        rt_safe_free(pkt);
    }

    while (deque_size(mAudioPktQ) > 0) {
        RtMutex::RtAutolock autoLock(mAudioQLock);
        RT_DequeEntry entry = deque_pop(mAudioPktQ);
        if (entry.data) {
            pkt = reinterpret_cast<RTPacket *>(entry.data);
        }
        mAudioCache->mCurCacheCount = deque_size(mAudioPktQ);
        mAudioCache->mCurCacheDuration -= pkt->mDuration;
        mAudioCache->mCurCacheSize -= pkt->mSize;
        rt_utils_packet_free(pkt);
        rt_safe_free(pkt);
    }
    return RT_OK;
}

INT32 RTPktSourceLocal::getTotalCacheSize() {
    INT32 totalSize = mVideoCache->mCurCacheSize + mAudioCache->mCurCacheSize;
    return totalSize;
}

INT64 RTPktSourceLocal::getAudioCacheDuration() {
    return mAudioCache->mCurCacheDuration;
}

INT64 RTPktSourceLocal::getVideoCacheDuration() {
    return mVideoCache->mCurCacheDuration;
}

RTPacket *RTPktSourceLocal::dequeueUnusedPacket(RT_BOOL block) {
    RTPacket *pkt = RT_NULL;
    while (getTotalCacheSize() >= mMaxCacheSize
            || mVideoCache->isFull()
            || mAudioCache->isFull()) {
        RT_LOGD_IF(DEBUG_FLAG, "cache is full, total size{cur: %d max: %d}, \n"
                "video{duration(cur: %lld max: %lld), count(cur: %d max: %d)}, \n"
                "audio{duration(cur: %lld max: %lld), count(cur: %d max: %d)}",
                 getTotalCacheSize(), mMaxCacheSize,
                 mVideoCache->mCurCacheDuration, mVideoCache->mHighWaterCacheDuration,
                 mVideoCache->mCurCacheCount, mVideoCache->mHighWaterCacheCount,
                 mAudioCache->mCurCacheDuration, mAudioCache->mHighWaterCacheDuration,
                 mAudioCache->mCurCacheCount, mAudioCache->mHighWaterCacheCount);

        if (block) {
            RtMutex::RtAutolock autoLock(mWaitLock);
            mCondition->wait(mWaitLock);
            return RT_NULL;
        } else {
            return RT_NULL;
        }
    }

    pkt = rt_malloc(RTPacket);
    RT_ASSERT(RT_NULL != pkt);
    rt_memset(pkt, 0, sizeof(pkt));
    return pkt;
}

RT_RET RTPktSourceLocal::queuePacket(RTPacket *pkt) {
    RT_RET ret = RT_OK;
    switch (pkt->mType) {
    case RTTRACK_TYPE_VIDEO: {
        RtMutex::RtAutolock autoLock(mVideoQLock);
        ret = deque_push(mVideoPktQ, pkt);
        if (ret == RT_OK) {
            mVideoCache->mCurCacheCount = deque_size(mVideoPktQ);
            mVideoCache->mCurCacheDuration += pkt->mDuration;
            mVideoCache->mCurCacheSize += pkt->mSize;
        }
    } break;
    case RTTRACK_TYPE_AUDIO: {
        RtMutex::RtAutolock autoLock(mAudioQLock);
        ret = deque_push(mAudioPktQ, pkt);
        if (ret == RT_OK) {
            mAudioCache->mCurCacheCount = deque_size(mAudioPktQ);
            mAudioCache->mCurCacheDuration += pkt->mDuration;
            mAudioCache->mCurCacheSize += pkt->mSize;
        }
    } break;
    default:
        RT_LOGE("unknown type: %d, free pkt", pkt->mType);
        if (pkt->mFuncFree) {
            pkt->mFuncFree(pkt->mRawPtr);
        }
        queueUnusedPacket(pkt);
        ret = RT_ERR_UNKNOWN;
    }
    return ret;
}

RTPacket *RTPktSourceLocal::dequeuePacket(RTTrackType type, RT_BOOL block) {
    RTPacket *pkt = RT_NULL;
    switch (type) {
    case RTTRACK_TYPE_VIDEO: {
        RtMutex::RtAutolock autoLock(mVideoQLock);
        if (deque_size(mVideoPktQ) > 0) {
            RT_DequeEntry entry = deque_pop(mVideoPktQ);
            if (entry.data) {
                pkt = reinterpret_cast<RTPacket *>(entry.data);
                mVideoCache->mCurCacheCount = deque_size(mVideoPktQ);
                mVideoCache->mCurCacheDuration -= pkt->mDuration;
                mVideoCache->mCurCacheSize -= pkt->mSize;
            }
        }
    } break;
    case RTTRACK_TYPE_AUDIO: {
        RtMutex::RtAutolock autoLock(mAudioQLock);
        if (deque_size(mAudioPktQ) > 0) {
            RT_DequeEntry entry = deque_pop(mAudioPktQ);
            if (entry.data) {
                pkt = reinterpret_cast<RTPacket *>(entry.data);
                mAudioCache->mCurCacheCount = deque_size(mAudioPktQ);
                mAudioCache->mCurCacheDuration -= pkt->mDuration;
                mAudioCache->mCurCacheSize -= pkt->mSize;
            }
        }
    } break;
    default:
        RT_LOGE("unknown type: %d, free pkt", pkt->mType);
    }
    if (pkt) {
        RtMutex::RtAutolock autoLock(mWaitLock);
        mCondition->signal();
    }
    return pkt;
}

RT_RET RTPktSourceLocal::queueNullPacket(INT32 streamIndex, RTTrackType type) {
    RTPacket       *pkt = RT_NULL;
    RT_RET          ret = RT_OK;

    pkt = rt_malloc(RTPacket);
    RT_ASSERT(RT_NULL != pkt);
    rt_memset(pkt, 0, sizeof(pkt));
    pkt->mType = type;
    pkt->mTrackIndex = streamIndex;
    pkt->mDuration = 0ll;
    pkt->mSize = 0;
    pkt->mPts = 0ll;
    pkt->mDts = 0ll;
    pkt->mPos = 0ll;
    pkt->mFlags = 0;
    pkt->mData = RT_NULL;
    pkt->mRawPtr = RT_NULL;
    pkt->mFuncFree = RT_NULL;
    ret = queuePacket(pkt);
    return ret;
}

RT_RET RTPktSourceLocal::queueUnusedPacket(RTPacket *pkt) {
    rt_safe_free(pkt);
    return RT_OK;
}
