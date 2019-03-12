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
 *   date: 2019/03/07
 * module: RTMediaBuffer Pool
 */

#ifndef SRC_RT_MEDIA_INCLUDE_RTMEDIABUFFERPOOL_H_
#define SRC_RT_MEDIA_INCLUDE_RTMEDIABUFFERPOOL_H_

#include "rt_header.h"      // NOLINT

class RTMediaBuffer;

class RTMediaBufferObserver {
 public:
    RTMediaBufferObserver() {}
    virtual ~RTMediaBufferObserver() {}

    virtual void signalBufferReturned(RTMediaBuffer *buffer) = 0;

 private:
    RTMediaBufferObserver(const RTMediaBufferObserver &);
    RTMediaBufferObserver &operator=(const RTMediaBufferObserver &);
};

class RTMediaBufferPool : public RTMediaBufferObserver {
 public:
    /* create for external buffer pool */
    explicit RTMediaBufferPool(UINT32 max_buffer_count = 0);
    /* create for internal buffer pool */
    RTMediaBufferPool(UINT32 max_buffer_count, UINT32 buffer_size);

    ~RTMediaBufferPool();

    RT_RET registerBuffer(RTMediaBuffer *buffer);

    RT_BOOL hasBuffer();

    RT_RET acquireBuffer(
               RTMediaBuffer **out,
               RT_BOOL block = RT_FALSE,
               UINT32 request_size = 0);

    RT_RET releaseAllBuffers();

    virtual void signalBufferReturned(RTMediaBuffer *buffer);

 private:
    struct RTBufferList;
    RTBufferList *mBufferList;

    RTMediaBufferPool(const RTMediaBufferPool &);
    RTMediaBufferPool &operator=(const RTMediaBufferPool &);
};

#endif  // SRC_RT_MEDIA_INCLUDE_RTMEDIABUFFERPOOL_H_

