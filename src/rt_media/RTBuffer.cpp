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

#include "RTBuffer.h"

RTBuffer::RTBuffer(void *data, UINT32 size, INT32 handle, INT32 fd) {
    mData = data;
    mSize = size;
    mHandle = handle;
    mFd = fd;
}

void*  RTBuffer::getData() const {
    return mData;
}
UINT32 RTBuffer::getSize() const {
    return mSize;
}
UINT32 RTBuffer::getOffset() const {
    return mRangeOffset;
}
UINT32 RTBuffer::getLength() const {
    return mRangeLength;
}
INT32  RTBuffer::getFd() const {
    return mFd;
}
INT32  RTBuffer::getHandle() const {
    return mHandle;
}
void   RTBuffer::setData(void* data, UINT32 size) {
    mData = data;
    mSize = size;
}
void   RTBuffer::setRange(UINT32 offset, UINT32 length) {
    mRangeOffset = offset;
    mRangeLength = length;
}


