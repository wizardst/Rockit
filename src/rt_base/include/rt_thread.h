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

#include "rt_header.h"

#ifndef __RT_THREAD_H__
#define __RT_THREAD_H__

struct RtRunnable {
    virtual ~RtRunnable() {};
    virtual void run(void* args) = 0;
};

class RtThread  {
public:
    typedef void* (*RTThreadProc)(void*);

    RtThread(RTThreadProc entryPoint, void* data = NULL);

    /**
     * Non-virtual, do not subclass.
     */
    ~RtThread();

    /**
     * Starts the thread. Returns false if the thread could not be started.
     */
    RT_BOOL start();

    /**
     * Waits for the thread to finish.
     * If the thread has not started, returns immediately.
     */
    void join();
    int mIndex;
public:
    static INT32 get_tid();
private:
    void* fData;
};

#endif // __RT_THREAD_H__
