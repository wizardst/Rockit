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
 * Author: rimon.xu@rock-chips.com
 *   Date: 2018/12/11
 * Module: use ffmpeg as encoder
 *
 * author: martin.cheng@rock-chips.com
 *   Date: 2019/01/04
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FFNodeEncoder"

#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x0

#include "FFNodeEncoder.h" // NOLINT
#include "RTObjectPool.h" // NOLINT
#include "FFMPEGAdapter.h" // NOLINT
#include "rt_metadata.h" // NOLINT
#include "RTMediaMetaKeys.h" // NOLINT
#include "rt_thread.h" // NOLINT
#include "rt_dequeue.h" // NOLINT
#include "RTMediaBuffer.h" // NOLINT
#include "FFAdapterCodec.h" // NOLINT

#define MAX_INPUT_BUFFER_COUNT      8
#define MAX_OUTPUT_BUFFER_COUNT     8

void* ff_encode_loop(void* ptr_node) {
    FFNodeEncoder* node = reinterpret_cast<FFNodeEncoder*>(ptr_node);
    node->runTask();
    return RT_NULL;
}

RTObject *allocEncInputBuffer(void *) {
    return new RTMediaBuffer(1920 * 1088 * 3 / 2);
}

RTObject *allocEncOutputBuffer(void *) {
    return new RTMediaBuffer(1920 * 1088 * 3 / 2);
}


FFNodeEncoder::FFNodeEncoder() {
    mProcThread = new RtThread(ff_encode_loop, reinterpret_cast<void*>(this));
    mProcThread->setName("FFEncoder");

    mUnusedInputPort  = RT_NULL;
    mUsedInputPort = RT_NULL;
    mUnusedOutputPort = RT_NULL;
    mUsedOutputPort = RT_NULL;
    mUnusedInputPort = new RTObjectPool(allocEncInputBuffer, MAX_INPUT_BUFFER_COUNT);
    mUsedInputPort = new RTObjectPool(RT_NULL, MAX_INPUT_BUFFER_COUNT * 4);
    mUnusedOutputPort = new RTObjectPool(allocEncOutputBuffer, MAX_OUTPUT_BUFFER_COUNT);
    mUsedOutputPort = new RTObjectPool(RT_NULL, MAX_OUTPUT_BUFFER_COUNT * 4);
}

FFNodeEncoder::~FFNodeEncoder() {
    release();
    mNodeContext = RT_NULL;
}

RT_RET FFNodeEncoder::init(RtMetaData *metadata) {
    mFFCodec = fa_video_encode_create(metadata);
    if (!mFFCodec) {
        RT_LOGE("fa_video_decode_open failed");
        return RT_ERR_UNKNOWN;
    }

    allocateBuffersOnPort(RT_PORT_INPUT);
    allocateBuffersOnPort(RT_PORT_OUTPUT);

    return RT_OK;
}

RT_RET FFNodeEncoder::allocateBuffersOnPort(RTPortType port) {
    UINT32 i = 0;
    switch (port) {
        case RT_PORT_INPUT: {
            RTMediaBuffer *buffer[MAX_INPUT_BUFFER_COUNT];
            for (i = 0; i < MAX_INPUT_BUFFER_COUNT; i++) {
                buffer[i] = reinterpret_cast<RTMediaBuffer *>(mUnusedInputPort->borrowObj());
            }
            for (i = 0; i < MAX_INPUT_BUFFER_COUNT; i++) {
                mUnusedInputPort->returnObj(buffer[i]);
            }
        }
            break;
        case RT_PORT_OUTPUT: {
            RTMediaBuffer *buffer[MAX_OUTPUT_BUFFER_COUNT];
            for (i = 0; i < MAX_OUTPUT_BUFFER_COUNT; i++) {
                buffer[i] = reinterpret_cast<RTMediaBuffer *>(mUnusedOutputPort->borrowObj());
            }
            for (i = 0; i < MAX_OUTPUT_BUFFER_COUNT; i++) {
                mUnusedOutputPort->returnObj(buffer[i]);
            }
        }
            break;
        default:
            RT_LOGE("unknown port! port: %d", port);
            return RT_ERR_UNKNOWN;
    }

    return RT_OK;
}

RT_RET FFNodeEncoder::release() {
    fa_video_encode_destroy(&mFFCodec);

    if (mUnusedInputPort) {
        delete mUnusedInputPort;
        mUnusedInputPort = NULL;
    }
    if (mUsedInputPort) {
        delete mUsedInputPort;
        mUsedInputPort = NULL;
    }

    if (mUnusedOutputPort) {
        delete mUnusedOutputPort;
        mUnusedOutputPort = NULL;
    }

    if (mUsedOutputPort) {
        delete mUsedOutputPort;
        mUsedOutputPort = NULL;
    }

    // thread release
    delete(mProcThread);
    mProcThread = RT_NULL;

    return RT_OK;
}

RT_RET FFNodeEncoder::dequeBuffer(RTMediaBuffer **buffer, RTPortType type) {
    switch (type) {
        case RT_PORT_INPUT:
            *buffer = reinterpret_cast<RTMediaBuffer *>(mUnusedInputPort->borrowObj());
            break;
        case RT_PORT_OUTPUT:
            *buffer = reinterpret_cast<RTMediaBuffer *>(mUsedOutputPort->borrowObj());
            break;
        default:
            RT_LOGE("unknown port! port: %d", type);
            return RT_ERR_UNKNOWN;
    }
    if (!(*buffer)) {
        return RT_ERR_UNKNOWN;
    }

    return RT_OK;
}

RT_RET FFNodeEncoder::queueBuffer(RTMediaBuffer *buffer, RTPortType type) {
    RT_RET ret = RT_OK;
    switch (type) {
        case RT_PORT_INPUT:
            ret = mUsedInputPort->returnObj(buffer);
            break;
        case RT_PORT_OUTPUT:
            ret = mUnusedOutputPort->returnObj(buffer);
            break;
        default:
            RT_LOGE("unknown port! port: %d", type);
            return RT_ERR_UNKNOWN;
    }
    if (!buffer) {
        return ret;
    }

    return RT_OK;
}

RT_RET FFNodeEncoder::pullBuffer(RTMediaBuffer** data) {
    mCountPull++;
    return dequeBuffer(data, RT_PORT_OUTPUT);
}

RT_RET FFNodeEncoder::pushBuffer(RTMediaBuffer*  data) {
    mCountPush++;
    return queueBuffer(data, RT_PORT_INPUT);
}

RT_RET FFNodeEncoder::runCmd(RT_NODE_CMD cmd, RtMetaData *metadata) {
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
    default:
        RT_LOGE("unkown command: %d", cmd);
        err = RT_ERR_UNKNOWN;
        break;
    }

    return err;
}

RT_RET FFNodeEncoder::setEventLooper(RTMsgLooper* eventLooper) {
    mEventLooper = eventLooper;
    return RT_OK;
}

RtMetaData* FFNodeEncoder::queryFormat(RTPortType port) {
    return RT_NULL;
}

RTNodeStub* FFNodeEncoder::queryStub() {
    return &ff_node_video_encoder;
}

RT_RET FFNodeEncoder::runTask() {
    RTMediaBuffer *input = NULL;
    RTMediaBuffer *output = NULL;

    while (THREAD_LOOP == mProcThread->getState()) {
        RT_RET err = RT_OK;
        if (!input) {
            input = reinterpret_cast<RTMediaBuffer *>(mUsedInputPort->borrowObj());
        }
        if (!output) {
            output = reinterpret_cast<RTMediaBuffer *>(mUnusedOutputPort->borrowObj());
        }

        if (!input || !output) {
            RtTime::sleepMs(10);
            continue;
        }

        RT_LOGD("input and output ready, go to encode!");
        err = fa_encode_send_frame(mFFCodec, input);
        if (err) {
            if (err == RT_ERR_TIMEOUT) {
                continue;
            }
        }
        err = fa_encode_get_packet(mFFCodec, output);
        if (err) {
            continue;
        } else {
            mUsedOutputPort->returnObj(output);
            mUnusedInputPort->returnObj(input);
            input = NULL;
            output = NULL;
        }
    }
    return RT_OK;
}

RT_RET FFNodeEncoder::onStart() {
    RT_RET err = RT_OK;
    mProcThread->start();
    return err;
}

RT_RET FFNodeEncoder::onPause() {
    return RT_OK;
}

RT_RET FFNodeEncoder::onStop() {
    RT_RET err = RT_OK;
    mProcThread->join();
    return err;
}

RT_RET FFNodeEncoder::onReset() {
    return RT_ERR_UNIMPLIMENTED;
}

RT_RET FFNodeEncoder::onFlush() {
    return RT_ERR_UNIMPLIMENTED;
}

static RTNode* createFFEncoder() {
    return new FFNodeEncoder();
}

struct RTNodeStub ff_node_video_encoder {
    .mCreateNode     = createFFEncoder,
    .mNodeType       = RT_NODE_TYPE_ENCODER,
    .mUsePool        = RT_TRUE,
    .mNodeName       = "ff_node_video_encoder",
    .mNodeVersion    = "v1.0",
};


