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
 *   date: 2018/07/19
 */

#ifndef __TEST_BASE_THREAD_H__
#define __TEST_BASE_THREAD_H__

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "base_thread"

#include <unistd.h> /*rand and usleep*/

#include "rt_mutex.h"
#include "rt_thread.h"
#include "rt_base_tests.h"

RT_RET unit_test_mutex(INT32 index, INT32 total_index) {
    RtMutex *lock = new RtMutex();
    lock->lock();
    lock->unlock();
    delete lock;
    return RT_OK;
}

void* unit_test_thread_loop(void*) {
   int rand_sleep = (rand()%1000)*10000;
   usleep(rand_sleep);
   RT_LOGE("done");
   return NULL;
}

RT_RET unit_test_thread(INT32 index, INT32 total_index) {
    RtThread* td = new RtThread(unit_test_thread_loop, NULL);
    td->start();
    td->join();
    RT_LOGE("done");
    return RT_OK;
}

#endif
