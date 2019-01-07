/*
 * Copyright 2018 Rockchip Electronics Co. LTD
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
 * Author: martin.cheng@rock-chips.com
 *   Date: 2018/12/26
 */

#include "RTObjectPool.h" // NOLINT
#include "rt_mutex.h" // NOLINT
#include "rt_thread.h" // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTObjectPool"
#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x1

RTObjectPool::RTObjectPool(AllocListener listener, UINT32 maxNum) {
    RTObject::trace(this->getName(), this, sizeof(RTObjectPool));

    mMaxNum    = (maxNum < 16)? 16 : maxNum;
    mObjNum    = 0;
    mAllocObj  = listener;
    mIdleDeque = deque_create(mMaxNum);
    mLock      = new RtMutex();
}

RTObjectPool::~RTObjectPool() {
    RTObject::untrace(this->getName(), this);

    deque_destory(&mIdleDeque);
    delete mLock;
    mLock  = RT_NULL;
}

RTObject* RTObjectPool::borrowObj() {
    RtAutoMutex autolock(mLock);
    RTObject* result = RT_NULL;
    if (mObjNum < mMaxNum) {
       mObjNum++;
       RT_DequeEntry entry = deque_pop(mIdleDeque);
       if (RT_NULL != entry.data) {
           // return reused object from idle object pool
           result = reinterpret_cast<RTObject*>(entry.data);
       } else {
           // use alloc listener to create new object
           result = (RT_NULL != mAllocObj)? mAllocObj(): RT_NULL;
       }
       RT_LOGD_IF(DEBUG_FLAG, "object = %p", result);
       return result;
    }

    // limit max object number in the pool
    return RT_NULL;
}

RT_RET RTObjectPool::returnObj(RTObject* obj) {
    RtAutoMutex autolock(mLock);
    if (RT_NULL != obj) {
        RT_LOGD_IF(DEBUG_FLAG, "object = %p", obj);
        deque_push_tail(mIdleDeque, reinterpret_cast<void*>(obj));
        mObjNum--;
    }
    return RT_OK;
}

UINT32 RTObjectPool::getNumIdle() {
    return deque_size(mIdleDeque);
}

UINT32 RTObjectPool::getNumUsed() {
    return mObjNum;
}

void RTObjectPool::dump() {
    for (UINT32 idx = 0; idx < deque_size(mIdleDeque); idx++) {
        RT_LOGD_IF(DEBUG_FLAG, "objects[%02d]=%p", idx, deque_get(mIdleDeque, idx));
    }
}
