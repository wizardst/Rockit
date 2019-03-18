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

#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x0

RTMsgLooper::RTMsgLooper() {
    mEventQueue = deque_create();
    mHandler    = RT_NULL;
    mThread     = RT_NULL;
    mDataLock   = new RtMutex();
    mSyncLock   = new RtMutex();
    mExecLock   = new RtMutex();
    mExecCond   = new RtCondition();
    mSyncCond   = new RtCondition();
    mExitFlag   = RT_FALSE;
    RT_LOGD_IF(DEBUG_FLAG, "+Constructor");
}

RTMsgLooper::~RTMsgLooper() {
    deque_destory(&mEventQueue);
    mEventQueue = RT_NULL;
    mHandler    = RT_NULL;
    rt_safe_delete(mThread);
    rt_safe_delete(mDataLock);
    rt_safe_delete(mSyncLock);
    rt_safe_delete(mExecLock);
    rt_safe_delete(mExecCond);
    rt_safe_delete(mSyncCond);
    RT_LOGD_IF(DEBUG_FLAG, "~Destructor");
}

RT_RET RTMsgLooper::send(struct RTMessage* msg, INT64 delayUs /* = 0 */) {
    // @TODO fix this bug
    msg->mSync = RT_TRUE;
    return post(msg, delayUs);
}

int LooperDoneListener(void* looper, UINT32 what) {
    RTMsgLooper* pLooper = reinterpret_cast<RTMsgLooper*>(looper);
    if (RT_NULL != pLooper->mSyncCond) {
        pLooper->mSyncCond->signal();
        RT_LOGE("mSyncCond->signal()");
    }
    return 0;
}

RT_RET RTMsgLooper::post(struct RTMessage* msg, INT64 delayUs /* = 0 */) {
    INT64 whenUs;
    if (THREAD_EXIT == mThread->getState()) {
        return RT_OK;
    }
    if (delayUs > 0) {
        whenUs = getNowUs() + delayUs;
    } else {
        whenUs = getNowUs();
    }

    do {
        RtMutex::RtAutolock autoLock(mDataLock);
#if TODO_FLAG
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
        msg->setWhenUs(whenUs);
        deque_push_head(mEventQueue, reinterpret_cast<void*>(msg));
#endif
    } while (0);

    mExecCond->broadcast();

    if (RT_TRUE == msg->mSync) {
        RtMutex::RtAutolock autoLock(mSyncLock);
        msg->mDoneListener = LooperDoneListener;
        RT_LOGD_IF(DEBUG_FLAG, "call, condition->wait(lock=%p) for handled", mSyncLock);
        mSyncCond->wait(mSyncLock);
        RT_LOGD_IF(DEBUG_FLAG, "done, condition->wait(lock=%p) for handled", mSyncLock);
        RT_LOGE("done, mSyncCond->wait(mSyncLock)");
    }

    RT_LOGD_IF(DEBUG_FLAG, "done, post message(msg=%p; what=%d)", msg, msg->getWhat());

    return RT_OK;
}

RT_BOOL RTMsgLooper::msgLoop() {
    while (THREAD_LOOP == mThread->getState()) {
        if (RT_NULL == mThread) {
            return RT_FALSE;
        }

        if (0 == deque_size(mEventQueue)) {
            RtMutex::RtAutolock autoLock(mExecLock);
            RT_LOGD_IF(DEBUG_FLAG, "call, condition->wait(exec=%p) for new msg", mExecLock);
            mExecCond->wait(mExecLock);
            RT_LOGD_IF(DEBUG_FLAG, "done, condition->wait(exec=%p) for new msg", mExecLock);
        }

        struct RTMessage* msg = RT_NULL;
        do {
            RtMutex::RtAutolock autoLock(mDataLock);
            RT_DequeEntry* header = deque_head(mEventQueue);
            if (header == RT_NULL) {
                continue;
            }
            // struct RTMessage* msg = reinterpret_cast<struct RTMessage*>deque_data(deque_head(mEventQueue));
            msg = reinterpret_cast<struct RTMessage*>(deque_pop(mEventQueue).data);
        } while (0);

        if (RT_NULL != msg) {
            INT64 whenUs = msg->getWhenUs();
            INT64 nowUs  = getNowUs();
            if (whenUs > nowUs) {
                INT64 delayUs = whenUs - nowUs;
                mExecCond->timedwait(mExecLock, delayUs);
                RT_LOGD_IF(DEBUG_FLAG, "done, condition->wait(exec=%p) for timeout", mExecLock);
            }
        }

        // Handler callback will handle this message
        if (RT_NULL != msg) {
            RT_LOGD_IF(DEBUG_FLAG, "call, deliver message(msg=%p; what=%d)", msg, msg->getWhat());
            if (msg->getTarget() == RT_NULL && mHandler) {
                msg->setTarget(mHandler);
            }
            msg->deliver();
            if (RT_NULL != msg->mDoneListener) {
                 msg->mDoneListener(this, msg->getWhat());
            }
            RT_LOGD_IF(DEBUG_FLAG, "done, deliver message(msg=%p; what=%d)", msg, msg->getWhat());
            rt_safe_delete(msg);
        }
    }

    // NOTE: It's important to note that at this point our "Looper" object
    // may no longer exist (its final reference may have gone away while
    // delivering the message). We have made sure, however, that loop()
    // won't be called again.

    return RT_TRUE;
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


RT_RET RTMsgLooper::flush() {
    RtMutex::RtAutolock autoLock(mDataLock);
    while (deque_size(mEventQueue)) {
        deque_pop(mEventQueue);
    }
    return RT_OK;
}

RT_RET RTMsgLooper::flush_message(UINT32 mWhat) {
    RtMutex::RtAutolock autoLock(mDataLock);
    RT_DequeEntry* head = deque_head(mEventQueue);
    RT_DequeEntry* it   = head;
    UINT32         idx  = 0;

    while (++idx <= deque_size(mEventQueue)) {
        struct RTMessage* msg = reinterpret_cast<struct RTMessage*>deque_data(it);
        if (msg->getWhat() == mWhat)
            break;
        it =  deque_next(it);
    }

    if (idx <= deque_size(mEventQueue))
        deque_del_index(mEventQueue, idx);
    return RT_OK;
}

RT_RET RTMsgLooper::requestExit() {
    mThread->requestInterruption();
    mExecCond->broadcast();
    mSyncCond->broadcast();
    return RT_OK;
}

RT_RET RTMsgLooper::stop() {
    requestExit();
    mThread->join();
    return RT_OK;
}

void RTMsgLooper::setName(const char *name) {
    mName = name;
}

INT64 RTMsgLooper::getNowUs() {
    return RtTime::getNowTimeUs();
}
