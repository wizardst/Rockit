#ifndef __TEST_TASK_THREAD_POOL_H__
#define __TEST_TASK_THREAD_POOL_H__

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "test_taskpool"

#include "rt_task.h"
#include "rt_taskpool.h"

#include <unistd.h> /*rand and usleep*/
#include <windows.h>

class UnitTestTask : public RtTask {
public:
     UnitTestTask(UINT32 idx){
         mPriority = TASK_PRIOTRY_FIFO;
         mID       = idx;
         // RT_LOGT("Task(%p,Id=%d) ++++Constructor", this, mID);
     }

     ~UnitTestTask( ){
         // RT_LOGT("Task(%p,Id=%d) ----Destructor", this, mID);
     }

     virtual void run_impl(void* args) {
        RtTime::sleepMs(100);
     }

     virtual void* get_args() {
        return this;
     }

     virtual char* get_name() {
         return "Task: UnitTestTask";
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

UINT8 unit_test_taskpool(UINT32 max_thread, UINT32 max_task, UINT32 num_task) {
    RtTaskPool* taskpool = rt_taskpool_init(max_thread, max_task);
    for(UINT32 idx = 0; idx < num_task; idx++) {
        rt_taskpool_push_tail(taskpool, new UnitTestTask(idx+1));
    }
    RT_LOGE("------rt_taskpool_wait-------");
    rt_taskpool_wait(taskpool);
    RT_LOGT("done");
    return RT_OK;
}

#endif // __TEST_TASK_THREAD_POOL_H__
