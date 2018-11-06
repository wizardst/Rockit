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
 * author: Hery.Xu@rock-chips.com
 *   date: 20181102
 * author: martin.cheng@rock-chips.com
 *   date: 20181126
 */

#ifndef SRC_RT_TASK_INCLUDE_RT_MESSAGE_H_
#define SRC_RT_TASK_INCLUDE_RT_MESSAGE_H_

#include "rt_header.h" // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

struct RTMsgHandler;
struct RTMsgLooper;

struct RTMessage {
    struct RTMsgData {
        INT32   mWhat;
        UINT64  mWhenUs;
        UINT32  mArgU32;
        UINT64  mArgU64;
        RT_PTR* mArgPtr;
    };

 public:
    RTMessage();
    RTMessage(UINT32 what, struct RTMsgHandler* handler);

    void           setWhat(UINT32 what);
    const UINT32   getWhat();
    void           setWhenUs(UINT64 when);
    const UINT64   getWhenUs();
    void           setTarget(struct RTMsgHandler* handler);
    RT_RET         post(INT64 delayUs = 0);
    RTMessage*     dup();   // performs message deep copy
    const char*    toString();

    struct RTMsgData mData;

 private:
    friend struct        RTMsgLooper;  // deliver()

    struct RTMsgHandler* mHandler;

    RT_RET deliver();
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_RT_TASK_INCLUDE_RT_MESSAGE_H_
