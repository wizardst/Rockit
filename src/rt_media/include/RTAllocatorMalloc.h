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
 * author: martin.cheng@rock-chips.com
 *   date: 2019/03/13
 * module: default malloc allocator
 */

#ifndef SRC_RT_MEDIA_INCLUDE_RTALLOCATORMALLOC_H_
#define SRC_RT_MEDIA_INCLUDE_RTALLOCATORMALLOC_H_

#include "rt_header.h"  // NOLINT
#include "RTAllocatorBase.h" // NOLINT

class RTAllocatorMalloc : public RTAllocator {
 public:
    explicit RTAllocatorMalloc(RtMetaData *config);
    ~RTAllocatorMalloc();

    virtual const char* getName() { return "RTAllocatorMalloc"; }
    virtual void summary(INT32 fd) {}

    static RT_BOOL checkAvail();
    virtual RT_RET newBuffer(UINT32 capacity, RTMediaBuffer **buffer);
    virtual RT_RET newBuffer(UINT32 width,
                             UINT32 height,
                             UINT32 format,
                             RTMediaBuffer **buffer);

    virtual RT_RET freeBuffer(RTMediaBuffer **buffer);

    RT_RET init(RtMetaData *meta);
    RT_RET deinit();

 private:
    INT32               mDrmFd;
    INT32               mAlign;
    INT32               mFlags;
    INT32               mHeapMask;
    INT32               mUsage;
};

#endif  // SRC_RT_MEDIA_INCLUDE_RTALLOCATORMALLOC_H_

