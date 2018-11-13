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
 *   date: 2018/11/05
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "test_taskpool"

#include "rt_task.h" // NOLINT
#include "rt_taskpool.h" // NOLINT
#include "rt_task_tests.h" // NOLINT

class UnitTestTask : public RtTask {
 public:
     explicit UnitTestTask(UINT32 idx) {
         mPriority = TASK_PRIOTRY_FIFO;
         mID       = idx;
         // RT_LOGT("Task(%p,Id=%d) ++++Constructor", this, mID);
     }

     ~UnitTestTask() {
         // RT_LOGT("Task(%p,Id=%d) ----Destructor", this, mID);
     }

     virtual void run_impl(void* args) {
        RtTime::sleepMs(100);
     }

     virtual void* get_args() {
        return this;
     }

     virtual char* get_name() {
         return const_cast<char*>("Task: UnitTestTask");
     }

     virtual bool shouldSkip() const {return false;}
};

UINT8 unit_test_task() {
    UnitTestTask *task = new UnitTestTask(0);
    task->run(task);
    delete task;
    RT_LOGT("done");
    return RT_OK;
}

UINT8 unit_test_taskpool_inner(
        UINT32 max_thread,
        UINT32 max_task,
        UINT32 num_task) {
    RtTaskPool* taskpool = rt_taskpool_init(max_thread, max_task);
    for (UINT32 idx = 0; idx < num_task; idx++) {
        rt_taskpool_push_tail(taskpool, new UnitTestTask(idx+1));
    }
    RT_LOGE("------rt_taskpool_wait-------");
    rt_taskpool_wait(taskpool);
    RT_LOGT("done");
    return RT_OK;
}

RT_RET  unit_test_taskpool(INT32 index, INT32 total) {
    unit_test_taskpool_inner(10, 100, 80);
    unit_test_taskpool_inner(10, 100, 200);
    return RT_OK;
}

