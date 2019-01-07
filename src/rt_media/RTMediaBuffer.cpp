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

#include "RTMediaBuffer.h"

RTMediaBuffer::RTMediaBuffer(void* data, UINT32 size) {
    RTObject::trace(this->getName(), this, sizeof(RTMediaBuffer));

    mMetaData = RT_NULL;
    setData(data, size);
}

RTMediaBuffer::RTMediaBuffer(UINT32 size) {
    RTObject::trace(this->getName(), this, sizeof(RTMediaBuffer));

    mMetaData = RT_NULL;
    mData = rt_malloc_size(void*, size);
    setData(mData, size);
    mOwnsData = RT_TRUE;
}

RTMediaBuffer::RTMediaBuffer(const RTMediaBuffer* data) {
    RTObject::trace(this->getName(), this, sizeof(RTMediaBuffer));

    mMetaData = RT_NULL;
    setData(data->getData(), data->getSize());
    setRange(data->getOffset(), data->getLength());
}

RTMediaBuffer::~RTMediaBuffer() {
    RTObject::untrace(this->getName(), this);

    if (mOwnsData && (RT_NULL != mData)) {
        rt_safe_free(mData);
    }
    if (RT_NULL != mMetaData) {
        mMetaData->clear();
        delete mMetaData;
        mMetaData =  RT_NULL;
    }
}

void RTMediaBuffer::summary(INT32 fd) {
    RT_LOGD("data=%p, size=%d", mData, mSize);
}

void RTMediaBuffer::setData(void* data, UINT32 size) {
    mData = data; mSize = size;
    setRange(0, size);
    mOwnsData = RT_FALSE;
    if (RT_NULL != mMetaData) {
        mMetaData->clear();
    } else {
        mMetaData = new RtMetaData();
    }
}

void RTMediaBuffer::setRange(UINT32 offset, UINT32 length) {
    if (offset + length <= mSize) {
        mRangeOffset = offset;
        mRangeLength = length;
    }
}

void*  RTMediaBuffer::getData() const {
    return mData;
}

UINT32 RTMediaBuffer::getSize() const {
    return mSize;
}

UINT32 RTMediaBuffer::getOffset() const {
    return mRangeOffset;
}

UINT32 RTMediaBuffer::getLength() const {
    return mRangeLength;
}

RtMetaData* RTMediaBuffer::getMetaData() {
    return mMetaData;
}

void RTMediaBuffer::reset() {
    mMetaData->clear();
    setRange(0, mSize);
}
