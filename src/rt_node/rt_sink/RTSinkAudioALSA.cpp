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
 * Author: shika.zhou@rock-chips.com
 *   Date: 2019/2/21
 * Module: audio pcm data use alsa-lib output
 *
 *
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTSinkAudioALSA"

#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x0

#include "RTSinkAudioALSA.h" // NOLINT
#include "rt_metadata.h" // NOLINT
#include "RTMediaMetaKeys.h" // NOLINT
#include "RTMediaBuffer.h"

void* sink_audio_alsa_loop(void* ptr_node) {
    RTSinkAudioALSA* audiosink = reinterpret_cast<RTSinkAudioALSA*>(ptr_node);
    audiosink->runTask();
    return RT_NULL;
}

RTSinkAudioALSA::RTSinkAudioALSA() {
    const char* name = "RTSinkAudioALSA";
    mThread = new RtThread(sink_audio_alsa_loop, reinterpret_cast<void*>(this));
    mThread->setName(name);
    mDeque = deque_create(10);
    RT_ASSERT(RT_NULL != mDeque);
}

RTSinkAudioALSA::~RTSinkAudioALSA() {
    release();
    mNodeContext = RT_NULL;
}

RT_RET RTSinkAudioALSA::init(RtMetaData *metadata) {
    mHDMICard = 1;
    openSoundCard(mHDMICard, 0, metadata);
    return RT_OK;
}

RT_RET RTSinkAudioALSA::release() {
    delete(mThread);
    mThread = RT_NULL;

    if (mDeque != NULL) {
        deque_destory(&mDeque);
    }

    closeSoundCard();
    return RT_OK;
}

RT_RET RTSinkAudioALSA::pullBuffer(RTMediaBuffer** media_buf) {
    *media_buf = RT_NULL;
    RT_RET  err = RT_ERR_NULL_PTR;
    RT_DequeEntry entry = deque_pop(mDeque);

    if (RT_NULL != entry.data) {
        *media_buf = reinterpret_cast<RTMediaBuffer*>(entry.data);
        err = RT_OK;
    } else {
        err = RT_ERR_NULL_PTR;
    }

    return err;
}

RT_RET RTSinkAudioALSA::pushBuffer(RTMediaBuffer* media_buf) {
    mCountPush++;
    RT_RET  err = RT_ERR_NULL_PTR;

    if (RT_NULL != media_buf) {
        err = deque_push_tail(mDeque, media_buf);
    }

    return err;
}

RT_RET RTSinkAudioALSA::runCmd(RT_NODE_CMD cmd, RtMetaData *metadata) {
    RT_RET err = RT_OK;

    switch (cmd) {
    case RT_NODE_CMD_INIT:
        err = this->init(metadata);
        break;
    case RT_NODE_CMD_START:
        err = this->onStart();
        break;
    case RT_NODE_CMD_STOP:
        err = this->onStop();
        break;
    case RT_NODE_CMD_FLUSH:
        err = this->onFlush();
        break;
    case RT_NODE_CMD_PAUSE:
        err = this->onPause();
        break;
    case RT_NODE_CMD_RESET:
        err = this->onReset();
        break;
    default:
        RT_LOGE("unkown command: %d", cmd);
        err = RT_ERR_UNKNOWN;
        break;
    }

    return err;
}

RT_RET RTSinkAudioALSA::setEventLooper(RTMsgLooper* eventLooper) {
    mEventLooper = eventLooper;
    return RT_OK;
}

RtMetaData* RTSinkAudioALSA::queryFormat(RTPortType port) {
    return RT_NULL;
}

RTNodeStub* RTSinkAudioALSA::queryStub() {
    return &rt_sink_audio_alsa;
}

RT_RET RTSinkAudioALSA::onStart() {
    RT_RET err = RT_OK;
    mStart = RT_TRUE;
    mThread->start();
    return err;
}

RT_RET RTSinkAudioALSA::onStop() {
    RT_RET err = RT_OK;
    mStart = RT_FALSE;
    mThread->join();
    return err;
}

RT_RET RTSinkAudioALSA::onPause() {
    return RT_OK;
}

RT_RET RTSinkAudioALSA::onFlush() {
    return RT_OK;
}

RT_RET RTSinkAudioALSA::onReset() {
    return RT_OK;
}

RT_VOID RTSinkAudioALSA::openSoundCard(int card, int devices, RtMetaData *metadata) {
    RT_RET err = RT_OK;
    char devicename[10] = "";
    snprintf(devicename, sizeof(devicename), "hw:%d,%d", card, devices);

    mALSASinkCtx = alsa_snd_create(devicename, metadata);

    if (!mALSASinkCtx) {
        RT_LOGE("alsa_snd_create failed");
    }

    err = alsa_set_snd_hw_params(mALSASinkCtx, 0);

    if (err == RT_ERR_INIT) {
        RT_LOGE("Failed to set HW parameters!");

        if (mALSASinkCtx)
            alsa_snd_destroy(mALSASinkCtx);
    }

    err = alsa_set_snd_sw_params(mALSASinkCtx);

    if (err == RT_ERR_INIT) {
        RT_LOGE("Failed to set SW parameters!");

        if (mALSASinkCtx)
            alsa_snd_destroy(mALSASinkCtx);
    }
}

RT_VOID RTSinkAudioALSA::closeSoundCard() {
    if (mALSASinkCtx) {
        alsa_snd_destroy(mALSASinkCtx);
    }
}

RT_RET RTSinkAudioALSA::runTask() {
    RTMediaBuffer *input = NULL;

    while (mStart) {
        RT_RET err = RT_OK;
        if (!input) {
            pullBuffer(&input);
        }

        if (!input || !input->getData()) {
            continue;
        }

        if (mALSASinkCtx) {
            alsa_snd_write_data(mALSASinkCtx, reinterpret_cast<void *>(input->getData()), input->getLength());
        }

        if (callback_ptr) {
            queueCodecBuffer(callback_ptr, input);
        } else {
            RT_LOGE("callback_ptr is NULL!");
        }

        input = NULL;
    }
    return RT_OK;
}

static RTNode* createSinkAudioALSA() {
    return new RTSinkAudioALSA();
}

struct RTNodeStub rt_sink_audio_alsa {
    .mCreateNode   = createSinkAudioALSA,
    .mNodeType     = RT_NODE_TYPE_AUDIO_SINK,
    .mUsePool      = RT_TRUE,
    .mNodeName     = "rt_sink_audio_alsa",
    .mNodeVersion  = "v1.0",
};
