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

#ifndef SRC_RT_MEDIA_INCLUDE_RTBUFFER_H_
#define SRC_RT_MEDIA_INCLUDE_RTBUFFER_H_

#include "rt_header.h"          // NOLINT

class RTBuffer {
 public:
    RTBuffer() {}
    ~RTBuffer() {}
    explicit RTBuffer(void *data, UINT32 size, INT32 handle, INT32 fd);

    void*  getData() const;
    UINT32 getSize() const;
    UINT32 getOffset() const;
    UINT32 getLength() const;
    INT32  getFd() const;
    INT32  getHandle() const;
    void   setData(void* data, UINT32 size);
    void   setRange(UINT32 offset, UINT32 length);

 private:
    void*   mData;
    UINT32  mSize;
    INT32   mFd;
    INT32   mHandle;
    UINT32  mRangeOffset;
    UINT32  mRangeLength;
};

#endif  // SRC_RT_MEDIA_INCLUDE_RTBUFFER_H_

