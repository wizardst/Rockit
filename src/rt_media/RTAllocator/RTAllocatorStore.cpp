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
 * author: rimon.xu@rock-chips.com
 *   date: 2019/01/16
 * module: allocator store
 */

#include "RTAllocatorStore.h"       // NOLINT
#include "RTAllocatorDrm.h"         // NOLINT
#include "RTAllocatorIon.h"         // NOLINT
#include "RTAllocatorGralloc.h"     // NOLINT
#include "RTAllocatorMalloc.h"      // NOLINT

RTAllocatorStore::RTAllocatorStore() {
}

RTAllocatorStore::~RTAllocatorStore() {
}

RT_RET RTAllocatorStore::fetchAllocator(
        RTAllocatorType type,
        RtMetaData* config,
        RTAllocator **allocator) {
    RT_RET ret = RT_OK;
    switch (type) {
        case RT_ALLOC_TYPE_ION:
            *allocator = fetchIonAllocator(config);
            break;
        case RT_ALLOC_TYPE_DRM:
            *allocator = fetchDrmAllocator(config);
            break;
        case RT_ALLOC_TYPE_GRAPHIC:
            *allocator = fetchGrallocAllocator(config);
            break;
        default:
            RT_LOGE("unsupported allocator type: %d", type);
            ret = RT_ERR_UNKNOWN;
            break;
    }
    return ret;
}

RT_RET RTAllocatorStore::priorAvailLinearAllocator(RtMetaData* config, RTAllocator **allocator) {
    RT_RET ret = RT_OK;
    RTAllocator* rt_allocator = NULL;
    rt_allocator = fetchIonAllocator(config);
    if (rt_allocator) {
        *allocator = rt_allocator;
        return ret;
    }

    rt_allocator = fetchDrmAllocator(config);
    if (rt_allocator) {
        *allocator = rt_allocator;
        return ret;
    }

    rt_allocator = fetchMallocAllocator(config);
    if (rt_allocator) {
        *allocator = rt_allocator;
        return ret;
    }

    return RT_ERR_UNKNOWN;
}

RTAllocator *RTAllocatorStore::fetchIonAllocator(RtMetaData *config) {
    RTAllocator *allocator = NULL;
    RT_BOOL avail = RTAllocatorIon::checkAvail();
    if (avail) {
        allocator = new RTAllocatorIon(config);
    }
    return allocator;
}

RTAllocator* RTAllocatorStore::fetchDrmAllocator(RtMetaData *config) {
    RTAllocator* allocator = NULL;
    RT_BOOL avail = RTAllocatorDrm::checkAvail();
    if (avail) {
        #ifdef OS_LINUX
        allocator = new RTAllocatorDrm(config);
        #endif
    }
    return allocator;
}

RTAllocator* RTAllocatorStore::fetchGrallocAllocator(RtMetaData *config) {
    #if TODO_FLAG
    RTAllocator* allocator = NULL;
    RT_BOOL avail = RTAllocatorGralloc::checkAvail();
    if (avail) {
        allocator = new RTAllocatorGralloc(config);
    }
    return allocator;
    // TODO(graphic allocator)
    #else
    return RT_NULL;
    #endif
}

RTAllocator* RTAllocatorStore::fetchMallocAllocator(RtMetaData *config) {
    RTAllocator* allocator = NULL;
    RT_BOOL avail = RTAllocatorMalloc::checkAvail();
    if (avail) {
        allocator = new RTAllocatorMalloc(config);
    }
    return allocator;
}

