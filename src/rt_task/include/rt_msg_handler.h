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

#ifndef SRC_RT_TASK_INCLUDE_RT_MSG_HANDLER_H_
#define SRC_RT_TASK_INCLUDE_RT_MSG_HANDLER_H_

#include "rt_header.h" // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

struct RTMessage;
struct RTLooper;

struct RTMsgHandler {
 public:
    RTMsgHandler() : mID(0) {}

 protected:
    virtual ~RTMsgHandler() {}
    virtual void onMessageReceived(struct RTMessage* msg) = 0;

 private:
    friend struct RTMessage;  // deliverMessage()

    UINT32 mID;

    RT_RET deliverMessage(struct RTMessage* msg);
};

#ifdef __cplusplus
}
#endif

#endif  // SRC_RT_TASK_INCLUDE_RT_MSG_HANDLER_H_
