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

#include "RTPktSourceBase.h"     // NOLINT

RTMediaCache::RTMediaCache()
    : mCurCacheDuration(0ll),
      mCurCacheSize(0),
      mCurCacheCount(0),
      mHighWaterCacheDuration(0ll),
      mHighWaterCacheCount(0),
      mLowWaterCacheDuration(0ll),
      mLowWaterCacheCount(0) {
    RTObject::trace(getName(), this, sizeof(RTMediaCache));
}

RTMediaCache::~RTMediaCache() {
    RTObject::untrace(getName(), this);
}

RT_BOOL RTMediaCache::isFull() {
    return (mCurCacheDuration >= mHighWaterCacheDuration)
            || (mCurCacheCount > mHighWaterCacheCount);
}

RT_BOOL RTMediaCache::isInsufficient() {
    return (mCurCacheDuration <= mLowWaterCacheDuration)
            || (mCurCacheCount <= mLowWaterCacheCount);
}

RTPktSourceBase::RTPktSourceBase() {
    RTObject::trace(getName(), this, sizeof(RTPktSourceBase));
}

RTPktSourceBase::~RTPktSourceBase() {
    RTObject::untrace(getName(), this);
}

