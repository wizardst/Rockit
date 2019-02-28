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
 *   date: 2018/11/27
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "test_msgqueue"

#include "rt_task_tests.h"  // NOLINT
#include "rt_message.h"     // NOLINT
#include "rt_msg_handler.h" // NOLINT
#include "rt_msg_looper.h"  // NOLINT

#define MAX_TEST_COUNT 100

enum CTL_CMD {
    CMD_INIT = 0x0,
    CMD_PLAY,
    CMD_PAUSE,
    CMD_STOP,
    CMD_STEP,
    CMD_RELEASE,
    CMD_MAX,
};

typedef struct cmd_name_t {
    UINT32      cmd;
    const char *name;
} rt_cmd_name;

static const rt_cmd_name mCmdNames[] = {
        { CMD_INIT,    "CMD_INIT" },
        { CMD_PLAY,    "CMD_PLAY" },
        { CMD_PAUSE,   "CMD_PAUSE" },
        { CMD_STOP,    "CMD_STOP" },
        { CMD_STEP,    "CMD_STEP" },
        { CMD_RELEASE, "CMD_RELEASE" },
        { CMD_MAX,     "CMD_MAX" },
};

struct CMDPlayer: public RTMsgHandler {
 public:
    CMDPlayer();
    ~CMDPlayer();
    void postRandomMessage(UINT32 msg_cnt);
    void onMessageReceived(struct RTMessage* msg);
    struct RTMsgLooper* mLooper;
};

CMDPlayer::CMDPlayer() {
    mLooper = new RTMsgLooper();
    mLooper->setName("CMDLooper");
    mLooper->start();
}

CMDPlayer::~CMDPlayer() {
    if (RT_NULL != mLooper) {
        mLooper->stop();
    }
    rt_safe_delete(mLooper);
}

void CMDPlayer::postRandomMessage(UINT32 msg_cnt) {
    for (UINT32 idx = 0; idx < msg_cnt; idx++) {
        enum CTL_CMD cmd = (enum CTL_CMD)(idx%CMD_MAX);
        RTMessage* msg = new RTMessage(cmd, RT_NULL, this);
        RT_LOGE("message(msg=%p; what=%d; name=%s) was created", msg, cmd, mCmdNames[cmd].name);
        mLooper->post(msg, 0);
        RtTime::sleepMs(3000);
    }
}

void CMDPlayer::onMessageReceived(struct RTMessage* msg) {
    RT_LOGE("message(msg=%p; what=%d) received", msg, msg->getWhat());
}

RT_RET unit_test_msgqueue(INT32 index, INT32 total) {
    struct CMDPlayer* cmd_player =  new CMDPlayer();
    cmd_player->postRandomMessage(MAX_TEST_COUNT);
    rt_safe_delete(cmd_player);
    return RT_OK;
}

RT_RET unit_test_message(INT32 index, INT32 total) {
    RT_Deque* msg_queue = deque_create();
    for (UINT32 cmd = 0; cmd < MAX_TEST_COUNT; cmd++) {
        if (cmd % 3 == 0) {
            struct RTMessage* msg = new RTMessage(cmd, 0, 0, RT_NULL);
            // deque_push_head(msg_queue, reinterpret_cast<void*>(msg));
            deque_push_tail(msg_queue, reinterpret_cast<void*>(msg));
        } else {
            RT_DequeEntry entry = deque_pop(msg_queue);
            struct RTMessage* msg = (struct RTMessage*)(entry.data);
            if (RT_NULL != msg) {
                RT_LOGE("message(msg=%p; what=%d", msg, msg->getWhat());
                rt_safe_delete(msg);
            }
        }
    }
    return RT_OK;
}
