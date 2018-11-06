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

#include "rt_msg_looper.h"    // NOLINT
#include "rt_message.h"       // NOLINT
#include "rt_time.h"          // NOLINT
#include "rt_string_utils.h"  // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "msg_looper"

#ifdef LOG_FLAG
#undef LOG_FLAG
#endif
#define LOG_FLAG 0x1

RTMsgLooper::RTMsgLooper() {
    mEventQueue = deque_create();
    mHandlers   = array_list_create();
    mThread     = RT_NULL;
    mLock       = new RtMutex();
    mExecCond   = new RtCondition();
    mExitFlag   = RT_FALSE;
    RT_LOGX(LOG_FLAG, "+Constructor");
}

RTMsgLooper::~RTMsgLooper() {
    deque_destory(&mEventQueue);
    array_list_destroy(mHandlers);
    mEventQueue = RT_NULL;
    mHandlers   = RT_NULL;
    rt_safe_delete(mThread);
    rt_safe_delete(mLock);
    rt_safe_delete(mExecCond);
    RT_LOGX(LOG_FLAG, "~Destructor");
}

RT_RET RTMsgLooper::post(struct RTMessage* msg, INT64 delayUs /* = 0 */) {
    RtMutex::RtAutolock autoLock(mLock);

#if TO_DO_FLAG
    INT64 whenUs;
    if (delayUs > 0) {
        whenUs = getNowUs() + delayUs;
    } else {
        whenUs = getNowUs();
    }

    RT_DequeEntry* head = deque_head(mEventQueue);
    RT_DequeEntry* tail = deque_tail(mEventQueue);
    RT_DequeEntry* it   = head;
    UINT32 idx = -1;
    while (it != tail) {
        struct RTEvent* event = reinterpret_cast<struct RTEvent*>(deque_data(it));
        if (event->mWhenUs <= whenUs) {
            idx++;
            it =  deque_next(it);
        } else {
            break;
        }
    }

    msg->setWhenUs(whenUs);

    if (0 == deque_size(mEventQueue)) {
        deque_push_head(mEventQueue, reinterpret_cast<void*>(msg));
    } else {
        deque_insert(mEventQueue, it, reinterpret_cast<void*>(msg));
    }
#else
    deque_push_head(mEventQueue, reinterpret_cast<void*>(msg));
#endif
    mExecCond->broadcast();

    RT_LOGX(LOG_FLAG, "message(msg=%p; what=%d) posted to msg-queue", msg, msg->getWhat());

    return RT_OK;
}

RT_BOOL RTMsgLooper::msgLoop() {
    mExitFlag = RT_FALSE;
    while (RT_FALSE == mExitFlag) {
        RtMutex::RtAutolock autoLock(mLock);

        if (RT_NULL == mThread) {
            return RT_FALSE;
        }

        if (0 == deque_size(mEventQueue)) {
            RT_LOGX(LOG_FLAG, "[0]wait condition ...pid(%d)", mThread->get_tid());
            mExecCond->wait(mLock);
        }

        struct RTMessage* msg = reinterpret_cast<struct RTMessage*>deque_data(deque_head(mEventQueue));
        INT64 whenUs = msg->getWhenUs();
        INT64 nowUs  = getNowUs();

        if (whenUs > nowUs) {
            INT64 delayUs = whenUs - nowUs;
            mExecCond->timedwait(mLock, delayUs);
            RT_LOGX(LOG_FLAG, "[0]timedwait condition ...pid(%d)", mThread->get_tid());
        }

        msg = reinterpret_cast<struct RTMessage*>(deque_pop(mEventQueue).data);

        // Handler callback will handle this message
        RT_LOGX(LOG_FLAG, "message(msg=%p; what=%d) delivering ...", msg, msg->getWhat());
        if (RT_NULL != msg) {
            msg->deliver();
            rt_safe_delete(msg);
        }
    }

    // NOTE: It's important to note that at this point our "Looper" object
    // may no longer exist (its final reference may have gone away while
    // delivering the message). We have made sure, however, that loop()
    // won't be called again.

    return RT_TRUE;
}

void RTMsgLooper::registerHandler(struct RTMsgHandler* handler) {
    array_list_add(mHandlers, reinterpret_cast<void*>(handler));
}

void RTMsgLooper::unregisterHandler(struct RTMsgHandler* handler) {
    array_list_remove(mHandlers, reinterpret_cast<void*>(handler));
}

void* rt_thread_looper(void* ptr_this) {
    struct RTMsgLooper* looper = reinterpret_cast<struct RTMsgLooper*>(ptr_this);
    looper->msgLoop();
    return RT_NULL;
}

RT_RET RTMsgLooper::start(INT32 priority) {
    mThread = new RtThread(rt_thread_looper, reinterpret_cast<void*>(this));
    mThread->setName(rt_str_to_char(mName));
    mThread->start();
    return RT_OK;
}

RT_RET RTMsgLooper::stop() {
    mExitFlag = RT_TRUE;
    mThread->join();
    return RT_OK;
}

void RTMsgLooper::setName(const char *name) {
    mName = name;
}

INT64 RTMsgLooper::getNowUs() {
    return RtTime::getNowTimeUs();
}
