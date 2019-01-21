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
 * Module:
 *    Why: avoid malloc and free memory frequently
 *  How-1: if your create and destroy some objects frequently,
 *         the best practice is using object pool technique,
 *         When used, gets an object from the object pool;
 *         when unused, puts the object into the object pool.
 *  How-2: When the class contains at least one virtual function,
           the typeid operator returns dynamic type of the object,
           it is typical RTTI(Run-Time Type Identification);
           otherwise, the typeid operator returns static type of the object,
           it is typical CTTI(Compile-Time Type Identification).
 */

#ifndef SRC_RT_MEDIA_INCLUDE_RTOBJECTPOOL_H_
#define SRC_RT_MEDIA_INCLUDE_RTOBJECTPOOL_H_

#include <typeinfo>

#include "RTObject.h"   // NOLINT
#include "rt_type.h"    // NOLINT
#include "rt_dequeue.h" // NOLINT

typedef RTObject* (*AllocListener)(void *);

class RtMutex;
class RTObjectPool : public RTObject {
 public:
    RTObjectPool(AllocListener listener, UINT32 maxNum, void *listener_ctx = RT_NULL);
    virtual ~RTObjectPool();
    RTObject* borrowObj();
    RTObject* useObj();
    RT_RET    returnObj(RTObject* obj);
    UINT32    getNumIdle();
    UINT32    getNumUsed();

 public:
    // override pure virtual methods of RTObject class
    virtual void summary(INT32 fd) {}
    virtual const char* getName() { return "RTObject/RTObjectPool"; }
    virtual void dump();

 private:
    INT32    mMaxNum;
    INT32    mObjNum;
    RT_Deque* mUsedDeque;
    RT_Deque* mIdleDeque;
    AllocListener mAllocObj;
    void* mListenerCtx;
    RtMutex*  mLock;
};



#endif  // SRC_RT_MEDIA_INCLUDE_RTOBJECTPOOL_H_
