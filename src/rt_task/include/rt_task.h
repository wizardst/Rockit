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

#ifndef __RT_TASK_H__
#define __RT_TASK_H__

#define TASK_PRIOTRY_FIFO 0
#define TASK_PRIOTRY_RT   1   /*message, audio, vsync*/

#include "rt_header.h"

template <typename T>
struct RtTRunnable {
    virtual ~RtTRunnable() {};
    virtual void run(T&);
    virtual void run_impl(T&) = 0;
};

template <>
struct RtTRunnable<void> {
    virtual ~RtTRunnable() {};
    virtual void run() = 0;
};

struct RtTask {
    virtual ~RtTask() {};
    virtual void run(void* args);
    virtual void run_impl(void* args) = 0;
    virtual void* get_args() = 0;
    virtual char* get_name() = 0;
    virtual UINT32 get_id() {return mID;};
    virtual UINT32 get_priority(){return mPriority;};
    UINT32 mPriority;
    UINT32 mID;
};

#endif
