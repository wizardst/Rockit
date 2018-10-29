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

#ifndef __RT_MUTEX_H__
#define __RT_MUTEX_H__

typedef void *(*RTPthreadCallback)(void *);

class RtMutex;
class RtCondition;

/*
 * for shorter type name and function name
 */
class RtMutex
{
public:
    RtMutex();
    ~RtMutex();

    void lock();
    void unlock();
    int  trylock();

    class RtAutolock
    {
    public:
        inline RtAutolock(RtMutex& RtMutex) : mLock(RtMutex)  { mLock.lock(); }
        inline RtAutolock(RtMutex* RtMutex) : mLock(*RtMutex) { mLock.lock(); }
        inline ~RtAutolock() { mLock.unlock(); }
    private:
        RtMutex& mLock;
    };

private:
    friend class RtCondition;

    void* mData;

    RtMutex(const RtMutex &);
    RtMutex &operator = (const RtMutex&);
};

typedef RtMutex::RtAutolock RtAutoMutex;

/*
 * for shorter type name and function name
 */
class RtCondition
{
public:
    RtCondition();
    RtCondition(int type);
    ~RtCondition();

    /*These functions atomically release mutex,
      but block on the condition variable*/
    INT32 wait(RtMutex& RtMutex);
    INT32 wait(RtMutex* RtMutex);

    /*returns with the timeout error*/
    INT32 timedwait(RtMutex& RtMutex, UINT64 timeout);
    INT32 timedwait(RtMutex* RtMutex, UINT64 timeout);

    /*This wakes up at least one thread blocked on the condition variable*/
    INT32 signal();

    /*This wakes up all of the threads blocked on the condition variable*/
    INT32 broadcast();

private:
    void* mData;
};

#endif
