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
 *   date: 20181126
 */

#ifndef SRC_RT_TASK_INCLUDE_RT_MSG_LOOPER_H_
#define SRC_RT_TASK_INCLUDE_RT_MSG_LOOPER_H_

#include "rt_header.h"     // NOLINT
#include "rt_dequeue.h"    // NOLINT
#include "rt_array_list.h" // NOLINT
#include "rt_thread.h"     // NOLINT
#include <string>          // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

struct RTMsgHandler;
struct RTMessage;

struct RTMsgLooper {
 public:
    RTMsgLooper();
    virtual ~RTMsgLooper();

    void setHandler(struct RTMsgHandler* handler) {
        mHandler = handler;
    }

    RT_BOOL msgLoop();
    RT_RET  start(INT32 priority = 0);
    RT_RET  stop();
    RT_RET  flush();
    RT_RET  flush_message(INT32 mWhat);
    RT_RET  post(RTMessage* msg, INT64 delayUs = 0);   // async handler
    RT_RET  send(RTMessage* msg, INT64 delayUs = 0);   //  sync handler
    RT_RET  requestExit();

    static INT64 getNowUs();
    void         setName(const char *name);
    const char*  getName() const {
        return mName.c_str();
    }

 private:
    friend struct RTMessage;       // post()

    std::string          mName;
    RT_BOOL              mExitFlag;
    RT_Deque*            mEventQueue;
    struct RTMsgHandler *mHandler;
    RtThread*            mThread;
    RtMutex*             mDataLock;
    RtMutex*             mSyncLock;
    RtMutex*             mExecLock;
    RtCondition*         mExecCond;

 public:
    RtCondition*         mSyncCond;
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_RT_TASK_INCLUDE_RT_MSG_LOOPER_H_
