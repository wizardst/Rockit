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
 * author: martin.cheng@rock-chips.com
 *   date: 2018/12/28
 */

#include <typeinfo>

#include "rt_media_tests.h"  // NOLINT
#include "RTObject.h"   // NOLINT
#include "RTObjectPool.h"   // NOLINT

#include "rt_mutex.h" // NOLINT
#include "rt_thread.h" // NOLINT

class UnitTestObject : public RTObject {
 public:
     UnitTestObject() { mCount = 0; }
     virtual ~UnitTestObject() {}
     virtual const char* getName() { return typeid(this).name(); }
     virtual void    summary(INT32 fd) {}
     virtual void    unitTest(UINT32 used, UINT32 idle);
 private:
     UINT32 mCount;
};

void UnitTestObject::unitTest(UINT32 used, UINT32 idle) {
    RT_LOGE("Thread[%d] reuse object(%p) %02d times, ObjPool(used:%02d, idle:%02d)",
            RtThread::getThreadID(), this, ++mCount, used, idle);
}

RT_RET unit_test_object(INT32 index, INT32 total_index) {
    UnitTestObject* unitObj = new UnitTestObject();
    RTObject*       rtObj   = RT_NULL;

    RT_LOGE("ObjName  = %s", typeid(UnitTestObject).name());
    RT_LOGE("ObjName  = %s", typeid(unitObj).name());
    rtObj = reinterpret_cast<RTObject*>(unitObj);
    RT_LOGE("ObjName  = %s", typeid(rtObj).name());
    RT_LOGE("hashCode = %d", unitObj->hashCode());
    RT_LOGE("hashCode = %d",   rtObj->hashCode());
    return RT_OK;
}

RTObject* ObjectAllocator(void *) {
    return new UnitTestObject();
}

#define MAX_ALLOC_NUM 8
void* unit_test_obj_malloc_reuse(void* ptr) {
    RTObjectPool* objPool = reinterpret_cast<RTObjectPool*>(ptr);
    UINT32 idx = 0;
    for (UINT32 ii = 0; ii < 5; ii++) {
        UnitTestObject* objs[MAX_ALLOC_NUM] = {RT_NULL};

        rt_memset(objs, RT_NULL, sizeof(UnitTestObject*)*MAX_ALLOC_NUM);

        UINT32 objNum = ii + 1;
        RT_LOGE("Thread[%d] Pitch(%02d) ObjNum(%02d)", RtThread::getThreadID(), ii, objNum);
        for (idx = 0; idx < objNum; idx++) {
            objs[idx] = reinterpret_cast<UnitTestObject*>(objPool->borrowObj());
            if (RT_NULL != objs[idx]) {
                objs[idx]->unitTest(objPool->getNumUsed(), objPool->getNumIdle());
                // usleep(200*1000);
            }
        }
        // objPool->dump();
        for (idx = 0; idx < objNum; idx++) {
            if (RT_NULL != objs[idx]) {
                objPool->returnObj(objs[idx]);
            }
        }
        // objPool->dump();
        rt_memset(objs, RT_NULL, sizeof(UnitTestObject*)*MAX_ALLOC_NUM);
    }
    return NULL;
}

RT_RET unit_test_object_pool(INT32 index, INT32 total_index) {
    RTObjectPool* objectPool = new RTObjectPool(ObjectAllocator, 16);
    #if TODO_FLAG
    RtThread* test_a = new RtThread(unit_test_obj_malloc_reuse, objectPool);
    RtThread* test_b = new RtThread(unit_test_obj_malloc_reuse, objectPool);
    test_a->start();
    test_b->start();
    test_a->join();
    RT_LOGE("RTObjectPool test_a thread done");
    test_b->join();
    RT_LOGE("RTObjectPool test_b thread done");
    #else
    unit_test_obj_malloc_reuse(objectPool);
    #endif
    delete objectPool;
    return RT_OK;
}
