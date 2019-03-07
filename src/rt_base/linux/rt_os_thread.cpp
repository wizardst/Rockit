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

typedef struct _rt_pthread_data {
    pthread_t      mTid;
    RT_BOOL        mValid;
    pthread_attr_t mAttr;
    void*          mPtrData;
    ThreadState    mLoopState;
    char           mName[MAX_THREAD_NAME_LEN];
    RtThread::RTThreadProc mLoopProc;
} RtThreadData;

static void* thread_looping(void* arg) {
    RtThreadData* data = static_cast<RtThreadData*>(arg);
    // Call entry point only if thread was not canceled before starting.
    INT64 tid = (INT64)(data->mTid);
    data->mLoopState = THREAD_LOOP;
    RT_LOGD_IF(DEBUG_FLAG, "pthread(name:%-010s tid:%lld) Loop Start...", data->mName, tid%10000);
    data->mLoopProc(data->mPtrData);
    RT_LOGD_IF(DEBUG_FLAG, "pthread(name:%-010s tid:%lld) Loop DONE!", data->mName, tid%10000);
    return NULL;
}

RtThread::RtThread(RTThreadProc entryPoint, void* ptr_data) {
    mData = RT_NULL;
    RtThreadData  *data = rt_malloc(RtThreadData);
    if (RT_NULL != data) {
        data->mLoopProc   = entryPoint;
        data->mPtrData    = ptr_data;
        data->mValid      = RT_FALSE;
        mData             = data;
    }
    data->mLoopState = THREAD_IDLE;
    this->setName("name-??");
}

RtThread::~RtThread() {
    if (RT_NULL != mData) {
        RtThreadData* data = static_cast<RtThreadData*>(mData);
        if (data->mValid) {
            this->join();
        }
        rt_safe_free(mData);
    }
}

RT_BOOL RtThread::start() {
    if (RT_NULL != mData) {
        RtThreadData* data = static_cast<RtThreadData*>(mData);
        int err = pthread_create(&(data->mTid), RT_NULL,
                                 thread_looping, data);
        data->mValid = (0 == err);
        return data->mValid;
    }
    return RT_FALSE;
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
        data->mLoopState =  THREAD_EXIT;
        if (!data->mValid) {
            return;
        }
        data->mLoopState =  THREAD_EXIT;
        INT64 tid = (INT64)(data->mTid);
        RT_LOGD_IF(DEBUG_FLAG, "pthread(name:%-010s tid:lld) Joining...", data->mName, tid%1000);
        pthread_join(data->mTid, RT_NULL);
        RT_LOGD_IF(DEBUG_FLAG, "pthread(name:%-010s tid:lld) Join DONE",  data->mName, tid%10000);
        data->mTid   = 0;
        data->mValid = RT_FALSE;
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

INT32 RtThread::get_tid() {
    pthread_t tid = pthread_self();
    return (INT32)tid;
}

#endif
