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
 * module: RTBuffer
 */

#ifndef SRC_RT_MEDIA_INCLUDE_RTALLOCATORBASE_H_
#define SRC_RT_MEDIA_INCLUDE_RTALLOCATORBASE_H_

#include "rt_header.h"  // NOLINT
#include "RTObject.h"  // NOLINT

class RTMediaBuffer;

class RTAllocator : public RTObject {
 public:
    typedef struct RT_BUFFER_USAGE {
        // TODO(buffer usage): buffer usage should be define.
        UINT32 reserve;
    } RTBufferUsage;

    RTAllocator() {}
    ~RTAllocator() {}

    virtual const char* getName() { return "RTAllocatorBase"; }
    virtual void summary(INT32 fd) {}

    virtual RT_RET newBuffer(UINT32 capacity, RTMediaBuffer **buffer) = 0;
    virtual RT_RET newBuffer(UINT32 width,
                             UINT32 height,
                             UINT32 format,
                             RTMediaBuffer **buffer) = 0;

    virtual RT_RET freeBuffer(RTMediaBuffer **buffer) = 0;
};

#endif  // SRC_RT_MEDIA_INCLUDE_RTALLOCATORBASE_H_


