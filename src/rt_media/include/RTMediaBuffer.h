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
 *   date: 2019/01/04
 * module: RTMediaBuffer
 */

#ifndef SRC_RT_MEDIA_INCLUDE_RTMEDIABUFFER_H_
#define SRC_RT_MEDIA_INCLUDE_RTMEDIABUFFER_H_

#include "RTObject.h"   // NOLINT
#include "rt_metadata.h"  // NOLINT

enum RtMediaBufferStatus {
    RT_MEDIA_BUFFER_STATUS_UNKONN = 0,
    RT_MEDIA_BUFFER_STATUS_UNUSED,
    RT_MEDIA_BUFFER_STATUS_READY,
    RT_MEDIA_BUFFER_STATUS_USING,
    RT_MEDIA_BUFFER_STATUS_INFO_CHANGE,
    RT_MEDIA_BUFFER_STATUS_BOTTON,
};

class RTMediaBuffer : public RTObject {
 public:
    // The underlying data remains the responsibility of the caller!
    explicit RTMediaBuffer(void* data, UINT32 size);
    explicit RTMediaBuffer(void* data, UINT32 size, INT32 handle, INT32 fd);
    explicit RTMediaBuffer(UINT32 size);
    explicit RTMediaBuffer(const RTMediaBuffer* data);
    virtual ~RTMediaBuffer();

    // override RTObject methods
    const char* getName() { return "RTMediaBuffer"; }
    void  summary(INT32 fd);

 public:
    void   release();
    void*  getData() const;
    UINT32 getSize() const;
    UINT32 getOffset() const;
    UINT32 getLength() const;
    INT32  getFd() const;
    INT32  getHandle() const;
    void   setData(void* data, UINT32 size);
    void   setRange(UINT32 offset, UINT32 length);
    void   setStatus(RtMediaBufferStatus status);
    RtMediaBufferStatus getStatus();
    RtMetaData* getMetaData();

    // Clears meta data and resets the range to the full extent.
    void reset();

 private:
    void*   mData;
    UINT32  mSize, mRangeOffset, mRangeLength;
    INT32 mHandle;
    INT32 mFd;
    RT_BOOL mOwnsData;

    RtMetaData* mMetaData;
    RtMediaBufferStatus mStatus;
};

#endif  // SRC_RT_MEDIA_INCLUDE_RTMEDIABUFFER_H_
