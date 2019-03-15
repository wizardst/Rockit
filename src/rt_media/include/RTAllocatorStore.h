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

#ifndef SRC_RT_MEDIA_INCLUDE_RTALLOCATORSTORE_H_
#define SRC_RT_MEDIA_INCLUDE_RTALLOCATORSTORE_H_

#include "rt_header.h"  // NOLINT
#include "RTObject.h"  // NOLINT


class RTAllocator;

class RTAllocatorStore : public RTObject {
 public:
    typedef enum RTAllocatorTypeEnum {
        RT_ALLOC_TYPE_DEFAULT_BASE = 0,
        RT_ALLOC_TYPE_GRAPHIC = 0,

        /*
         * allocator by platform,
         * eg: linux. android. windows
         */
        RT_ALLOC_TYPE_PLATFORM_BASE = 0x100,

        /*
         * allocator by vendor,
         * eg: rockchip or ...
         */
        RT_ALLOC_TYPE_VENDOR_BASE = 0x200,
        RT_ALLOC_TYPE_ION,
        RT_ALLOC_TYPE_DRM,
        RT_ALLOC_TYPE_MAX,
    } RTAllocatorType;

    virtual const char* getName() { return "RTAllocatorStore"; }
    virtual void summary(INT32 fd) {}

    RTAllocatorStore();
    ~RTAllocatorStore();

    RT_RET fetchAllocator(RTAllocatorType type, RtMetaData* config, RTAllocator **allocator);
    static RT_RET priorAvailLinearAllocator(RtMetaData* config, RTAllocator **allocator);

 private:
    static RTAllocator *fetchIonAllocator(RtMetaData *config);
    static RTAllocator *fetchDrmAllocator(RtMetaData *config);
    static RTAllocator *fetchGrallocAllocator(RtMetaData *config);
    static RTAllocator *fetchMallocAllocator(RtMetaData* config);
};

#endif  // SRC_RT_MEDIA_INCLUDE_RTALLOCATORSTORE_H_

