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

typedef struct _rt_pthread_data {
    pthread_t      fPThread;
    RT_BOOL        fValid;
    pthread_attr_t fAttr;
    void*          fData;
    RtThread::RTThreadProc fThreadProc;
} RtThreadData;

static void* thread_looping(void* arg) {
    RtThreadData* thread_data = static_cast<RtThreadData*>(arg);
    // Call entry point only if thread was not canceled before starting.
    thread_data->fThreadProc(thread_data->fData);
    return NULL;
}

RtThread::RtThread(RTThreadProc entryPoint, void* data) {
    fData = RT_NULL;
    RtThreadData  *thread_data = rt_malloc(RtThreadData);
    if (RT_NULL != thread_data) {
        thread_data->fThreadProc = entryPoint;
        thread_data->fData       = data;
        thread_data->fValid      = RT_FALSE;
        fData                    = thread_data;
    }
}

RtThread::~RtThread() {
    if (RT_NULL != fData) {
        RtThreadData* thread_data = static_cast<RtThreadData*>(fData);
        if (thread_data->fValid) {
            this->join();
        }
        rt_free(thread_data);
        fData = RT_NULL;
    }
}

RT_BOOL RtThread::start() {
    if (RT_NULL != fData) {
        RtThreadData* thread_data = static_cast<RtThreadData*>(fData);
        int err = pthread_create(&(thread_data->fPThread), RT_NULL,
                                 thread_looping, thread_data);
        thread_data->fValid = (0 == err);
        return thread_data->fValid;
    }
    return RT_FALSE;
}

void RtThread::join() {
    if (RT_NULL != fData) {
        RtThreadData* thread_data = static_cast<RtThreadData*>(fData);
        if (!thread_data->fValid) {
            return;
        }

        pthread_join(thread_data->fPThread, RT_NULL);
        thread_data->fPThread = 0;
        thread_data->fValid   = RT_FALSE;
    }
}

INT32 RtThread::get_tid() {
    pthread_t tid = pthread_self();
    return (INT32)tid;
}

#endif
