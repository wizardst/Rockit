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

#include "RTMediaBuffer.h"          // NOLINT
#include "RTAllocatorBase.h"        // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTMediaBuffer"

RTMediaBuffer::RTMediaBuffer(void* data, UINT32 size) {
    this->baseInit();

    setData(data, size);
}

RTMediaBuffer::RTMediaBuffer(
        void* data,
        UINT32 size,
        INT32 handle,
        INT32 fd,
        RTAllocator *alloctor) {
    this->baseInit();

    mHandle    = handle;
    mFd        = fd;
    mAllocator = alloctor;
    setData(data, size);
}

RTMediaBuffer::RTMediaBuffer(UINT32 size) {
    this->baseInit();

    mData = rt_malloc_size(void*, size);
    setData(mData, size);
    mOwnsData = RT_TRUE;
}

RTMediaBuffer::RTMediaBuffer(const RTMediaBuffer* data) {
    this->baseInit();

    setData(data->getData(), data->getSize());
    setRange(data->getOffset(), data->getLength());
}

void RTMediaBuffer::baseInit() {
    RTObject::trace(this->getName(), this, sizeof(RTMediaBuffer));

    mData      = RT_NULL;
    mSize      = 0;
    mRangeOffset = 0;
    mRangeLength = 0;
    mHandle    = 0;
    mFd        = 0;
    mPhyAddr   = 0;
    mOwnsData  = RT_FALSE;
    mMetaData  = RT_NULL;
    mAllocator = RT_NULL;
    mRefCount  = 0;
    mFuncFree  = RT_NULL;
    mStatus    = RT_MEDIA_BUFFER_STATUS_UNKONN;
    mObserver  = RT_NULL;
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
    mData = data;
    mSize = size;
    setRange(0, size);
    mOwnsData = RT_FALSE;
    if (RT_NULL != mMetaData) {
        mMetaData->clear();
    } else {
        mMetaData = new RtMetaData();
    }
}

void RTMediaBuffer::setData(void *data, UINT32 size, RT_RAW_FREE freeFunc) {
    mData = data;
    mSize = size;
    setRange(0, size);
    mOwnsData = RT_FALSE;
    if (RT_NULL != mMetaData) {
        mMetaData->clear();
    } else {
        mMetaData = new RtMetaData();
    }
    mFuncFree = freeFunc;
}

void RTMediaBuffer::setPhyAddr(UINT32 phyaddr) {
    mPhyAddr = phyaddr;
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

INT32  RTMediaBuffer::getFd() const {
    return mFd;
}
INT32  RTMediaBuffer::getHandle() const {
    return mHandle;
}

UINT32  RTMediaBuffer::getPhyAddr() const {
    return mPhyAddr;
}

RtMetaData* RTMediaBuffer::getMetaData() {
    return mMetaData;
}

void RTMediaBuffer::reset() {
    mMetaData->clear();
    setRange(0, mSize);
}

void RTMediaBuffer::setStatus(RtMediaBufferStatus status) {
    mStatus = status;
}

RtMediaBufferStatus RTMediaBuffer::getStatus() {
    return mStatus;
}

void RTMediaBuffer::addRefs() {
    (void) __sync_fetch_and_add(&mRefCount, 1);
}

INT32 RTMediaBuffer::refsCount() {
    return mRefCount;
}

void RTMediaBuffer::release(bool debug) {
    if (debug) {
        RT_LOGE("this: %p, mAllocator: %p mObserver=%p, mFuncFree=%p, refs=%d",
                 this, mAllocator, mObserver, mFuncFree, mRefCount);
    }
    if (mObserver == RT_NULL && mRefCount == 0) {
        if (mAllocator != RT_NULL) {
            RTMediaBuffer *this_tmp = this;
            mAllocator->freeBuffer(&this_tmp);
        } else if (mFuncFree != RT_NULL) {
            void *raw_ptr = RT_NULL;
            mMetaData->findPointer(kKeyPacketPtr, &raw_ptr);
            if (raw_ptr) {
                mFuncFree(raw_ptr);
                mMetaData->setPointer(kKeyPacketPtr, RT_NULL);
            }
            delete this;
        } else {
            delete this;
        }
        return;
    }
    if (mRefCount == 0) {
        RT_LOGE("refs count is 0, should not release.");
        return;
    }

    int prevCount = __sync_fetch_and_sub(&mRefCount, 1);
    if (prevCount == 1) {
         if (mFuncFree != RT_NULL) {
            void *raw_ptr = RT_NULL;
            mMetaData->findPointer(kKeyPacketPtr, &raw_ptr);
            if (raw_ptr) {
                mFuncFree(raw_ptr);
                mMetaData->setPointer(kKeyPacketPtr, RT_NULL);
            }
        }
        if (mObserver == RT_NULL) {
            if (mAllocator != RT_NULL) {
                RTMediaBuffer *this_tmp = this;
                mAllocator->freeBuffer(&this_tmp);
            }
            delete this;
            return;
        }
        mObserver->signalBufferReturned(this);
    }
}

void RTMediaBuffer::setObserver(RTMediaBufferObserver *observer) {
    RT_ASSERT(observer != RT_NULL);
    mObserver = observer;
}



