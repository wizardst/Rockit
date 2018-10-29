/*
 * Copyright 2018 The Rockit Open Source Project
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
 * author: martin.cheng
 *   date: 20180719
 */

 /*
  *  TaskPool Performance Report (windows 10)
  *  Task Count: 400; Task Run: 100ms
  *  43.13s with 01 threads;  22.51s with 02 threads;
  *  17.84s with 03 threads;  11.97s with 04 threads;
  *  08.72s with 05 threads;  07.39s with 06 threads;
  *  06.32s with 07 threads;  05.66s with 08 threads;
  */

#ifndef __RT_TASKPOOL_H__
#define __RT_TASKPOOL_H__

#include "rt_header.h"
#include "rt_dequeue.h"

typedef enum taskpool_state {
    kRunning_State,  // Normal case.  We've been constructed and no one has called wait().
    kWaiting_State,  // wait has been called, but there still might be work to do or being done.
    kHalting_State,  // There's no work to do and no thread is busy.  All threads can shut down.
    kPausing_State,  // Pausing
} RtPoolState;

class RtMutex;
class RtCondition;
class RtTask;
typedef struct rt_taskpool {
    UINT32 cur_task_num;
    UINT32 busy_task_num;
    UINT32 max_task_num;
    UINT32 max_thread_num;
    RT_Deque    *tasks;
    RT_Deque    *pthreads;
    RtMutex     *task_lock;
    RtPoolState state;
} RtTaskPool;

RtTaskPool* rt_taskpool_init(UINT32 max_thread_num, UINT32 max_task_num);
INT8 rt_taskpool_push(RtTaskPool *taskpool, RtTask *task, RT_BOOL tail=RT_TRUE);
INT8 rt_taskpool_push_head(RtTaskPool *taskpool, RtTask *task);
INT8 rt_taskpool_push_tail(RtTaskPool *taskpool, RtTask *task);
void rt_taskpool_pop( RtTaskPool *taskpool);
void rt_taskpool_resume(RtTaskPool *taskpool);
void rt_taskpool_wait(  RtTaskPool *taskpool);
void rt_taskpool_dump(  RtTaskPool *taskpool);
static void* rt_taskpool_loop(void*);

#endif

