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
 *
 * author: martin.cheng@rock-chips.com
 *   date: 20180719
 */

#ifdef  HAS_PTHREAD

#include <pthread.h>

#include "rt_thread.h" // NOLINT
#include "rt_log.h" // NOLINT
#include "rt_mem.h" // NOLINT
#include "rt_string_utils.h" // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "os_pthread"

#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x1

#define MAX_THREAD_NAME_LEN 12

#include <string.h>

typedef struct _rt_pthread_data {
    pthread_t      mTid;
    pthread_attr_t mAttr;
    void*          mPtrData;
    ThreadState    mLoopState;
    char           mName[MAX_THREAD_NAME_LEN];
    RtRunnable*    mRunnable;
    RtThread::RtTaskSlot mTaskSlot;
} RtThreadData;

static void* thread_looping(void* arg) {
    RtThread*     thread = static_cast<RtThread*>(arg);
    RtThreadData* data   = static_cast<RtThreadData*>(thread->mData);
    // Call entry point only if thread was not canceled before starting.
    INT64 tid = (INT64)(data->mTid);
    data->mLoopState = THREAD_LOOP;
    RT_LOGD_IF(DEBUG_FLAG, "call, pthread_looper(name:%-010s tid:%lld)", data->mName, tid%10000);

    if (RT_NULL != data->mTaskSlot) {
        data->mTaskSlot(data->mPtrData);
    }

    if (RT_NULL != data->mRunnable) {
        data->mRunnable->run(data->mPtrData);
    }

    RT_LOGD_IF(DEBUG_FLAG, "done, pthread_looper(name:%-010s tid:%lld)", data->mName, tid%10000);
    pthread_exit(0);
    return NULL;
}

RtThreadData* createThreadData(RtThread::RtTaskSlot task_slot, RtRunnable* runnable, void* ptr_data) {
    RtThreadData* data = rt_malloc(RtThreadData);

    // error: rt_memset will make string illegal address
    // rt_memset(data, 0, sizeof(RtThreadData));
    if (RT_NULL != data) {
        data->mTid        = 0;
        data->mTaskSlot   = task_slot;
        data->mRunnable   = runnable;
        data->mPtrData    = ptr_data;
        data->mLoopState  = THREAD_IDLE;
        return data;
    }
    return RT_NULL;
}

RtThread::RtThread(RtTaskSlot task_slot, void* ptr_data) {
    mData = createThreadData(task_slot, RT_NULL, ptr_data);
    this->setName("thread-?");
}

RtThread::RtThread(RtRunnable* runnable, void* ptr_data) {
    mData = createThreadData(RT_NULL, runnable, ptr_data);
    this->setName("thread-?");
}

RtThread::~RtThread() {
    if (RT_NULL != mData) {
        RtThreadData* data = static_cast<RtThreadData*>(mData);
        if (data->mTid > 0) {
            this->join();
        }
        rt_safe_free(mData);
    }
}

RT_BOOL RtThread::start() {
    int err = RT_OK;
    if (RT_NULL != mData) {
        RtThreadData* data = static_cast<RtThreadData*>(mData);
        err = pthread_create(&(data->mTid), RT_NULL, thread_looping, this);
        RT_LOGD("done, pthread_create with name=%s, error=%s",
                   data->mName, strerror(err));
        if (err != RT_OK) {
            data->mLoopState  = THREAD_IDLE;
            data->mTid        = 0;
        }
    }
}

INT32 RtThread::getState() {
    if (RT_NULL != mData) {
        RtThreadData* data = static_cast<RtThreadData*>(mData);
        return data->mLoopState;
    }
    return THREAD_IDLE;
}

void RtThread::join() {
    if (RT_NULL != mData) {
        RtThreadData* data = static_cast<RtThreadData*>(mData);
        INT64 tid = (INT64)(data->mTid);
        RT_LOGD_IF(DEBUG_FLAG, "call, pthread_join(name:%-010s tid:%lld)", data->mName, tid%10000);
        pthread_join(data->mTid, RT_NULL);
        RT_LOGD_IF(DEBUG_FLAG, "done, pthread_join(name:%-010s tid:%lld)", data->mName, tid%10000);
        data->mLoopState =  THREAD_EXIT;
        data->mTid   = 0;
    }
}

void RtThread::requestInterruption() {
    if (RT_NULL != mData) {
        RtThreadData* data = static_cast<RtThreadData*>(mData);
        data->mLoopState   = THREAD_EXIT;
    }
}

void RtThread::setName(const char* name) {
    if (RT_NULL != mData) {
        RtThreadData* data = static_cast<RtThreadData*>(mData);
        rt_str_snprintf(data->mName, MAX_THREAD_NAME_LEN, "%s", name);
    }
}

const char* RtThread::getName() {
    if (RT_NULL != mData) {
        RtThreadData* data = static_cast<RtThreadData*>(mData);
        return data->mName;
    }
    return "name-??";
}

INT32 RtThread::getThreadID() {
    pthread_t tid = pthread_self();
    return (INT32)tid;
}

#endif
