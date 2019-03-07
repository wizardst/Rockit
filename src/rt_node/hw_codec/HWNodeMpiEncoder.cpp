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
 *   Date: 2019/01/28
 *   Task: hardware mpi encoder
 */

#include "HWNodeMpiEncoder.h"       // NOLINT
#include "RTAllocatorBase.h"        // NOLINT
#include "RTAllocatorStore.h"       // NOLINT
#include "MpiAdapterCodec.h"        // NOLINT

#define MAX_INPUT_BUFFER_COUNT      30
#define MAX_OUTPUT_BUFFER_COUNT     30

void* hw_encode_loop(void* ptr_node) {
    HWNodeMpiEncoder* node = reinterpret_cast<HWNodeMpiEncoder*>(ptr_node);
    node->runTask();
    return RT_NULL;
}

RTObject *hwAllocInputBuffer(void *ctx) {
    RT_LOGD("in");
    HWNodeMpiEncoder *hw_ctx = reinterpret_cast<HWNodeMpiEncoder*>(ctx);
    RTAllocator *allocator = hw_ctx->getLinearAllocator();
    RTMediaBuffer *buffer = RT_NULL;
    if (allocator) {
        allocator->newBuffer(1920 * 1088 * 3 / 2, &buffer);
    }

    return buffer;
}

RTObject *hwAllocOutputBuffer(void *ctx) {
    RT_LOGD("in");
    HWNodeMpiEncoder *hw_ctx = reinterpret_cast<HWNodeMpiEncoder*>(ctx);
    RTAllocator *allocator = hw_ctx->getLinearAllocator();
    RTMediaBuffer *buffer = RT_NULL;
    if (allocator) {
        allocator->newBuffer(1920 * 1088 * 3 / 2, &buffer);
    }

    return buffer;
}

HWNodeMpiEncoder::HWNodeMpiEncoder() {
    mProcThread = new RtThread(hw_encode_loop, reinterpret_cast<void*>(this));
    mProcThread->setName("MPIEncoder");

    mAllocatorStore = new RTAllocatorStore();
}

HWNodeMpiEncoder::~HWNodeMpiEncoder() {
    release();
}

RT_RET HWNodeMpiEncoder::runTask() {
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
            RtTime::sleepMs(5);
            continue;
        }

        RT_LOGD("input and output ready, go to decode!");
        err = ma_encode_send_frame(mMpiAdapterCtx, input);
        if (err) {
            if (err == RT_ERR_TIMEOUT) {
                input = NULL;
            }
        } else {
            mUnusedInputPort->returnObj(input);
            input = NULL;
        }
        err = ma_encode_get_packet(mMpiAdapterCtx, output);
        if (err) {
        } else {
            if (output->getStatus() == RT_MEDIA_BUFFER_STATUS_READY) {
                mUsedOutputPort->returnObj(output);
                output = NULL;
            }
        }
    }
    return RT_OK;
}

RT_RET HWNodeMpiEncoder::init(RtMetaData *metadata) {
    RT_RET ret = RT_OK;
    // init memory allocator
    mAllocatorStore = new RTAllocatorStore();
    ret = mAllocatorStore->priorAvailLinearAllocator(metadata, &mLinearAllocator);
    if (ret) {
        RT_LOGE("init failed!");
        return ret;
    }

    mUnusedInputPort = new RTObjectPool(hwAllocInputBuffer, MAX_INPUT_BUFFER_COUNT, this);
    mUsedInputPort = new RTObjectPool(RT_NULL, MAX_INPUT_BUFFER_COUNT);
    mUnusedOutputPort = new RTObjectPool(hwAllocOutputBuffer, MAX_OUTPUT_BUFFER_COUNT, this);
    mUsedOutputPort = new RTObjectPool(RT_NULL, MAX_OUTPUT_BUFFER_COUNT);

    mMpiAdapterCtx = ma_encode_create(metadata);
    if (mMpiAdapterCtx == RT_NULL) {
        RT_LOGE("mpi adapter context create failed!");
        return RT_ERR_UNKNOWN;
    }

    ret = ma_encode_prepare(mMpiAdapterCtx, metadata);
    if (ret) {
        RT_LOGE("mpi encode prepare failed!");
        return ret;
    }

    allocateBuffersOnPort(RT_PORT_INPUT);
    allocateBuffersOnPort(RT_PORT_OUTPUT);

    return ret;
}

RTAllocator *HWNodeMpiEncoder::getLinearAllocator() {
    return mLinearAllocator;
}
RTAllocator *HWNodeMpiEncoder::getGraphicAllocator() {
    return mGraphicAllocator;
}

RT_RET HWNodeMpiEncoder::release() {
    ma_encode_destroy(&mMpiAdapterCtx);

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
__FAILED:
    return RT_ERR_UNKNOWN;
}

RT_RET HWNodeMpiEncoder::pullBuffer(RTMediaBuffer **data) {
    mCountPull++;
    dequeBuffer(data, RT_PORT_OUTPUT);
}

RT_RET HWNodeMpiEncoder::pushBuffer(RTMediaBuffer* data) {
    mCountPush++;
    queueBuffer(data, RT_PORT_INPUT);
}

RT_RET HWNodeMpiEncoder::runCmd(RT_NODE_CMD cmd, RtMetaData *metadata) {
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

RT_RET HWNodeMpiEncoder::setEventLooper(RTMsgLooper* eventLooper) {
    mEventLooper = eventLooper;
}

RtMetaData* HWNodeMpiEncoder::queryFormat(RTPortType port) {
    return RT_NULL;
}

RTNodeStub* HWNodeMpiEncoder::queryStub() {
    return &hw_node_mpi_encoder;
}

RT_RET HWNodeMpiEncoder::onStart() {
    RT_RET          err = RT_OK;
    mProcThread->start();
    return err;
}

RT_RET HWNodeMpiEncoder::onPause() {
}

RT_RET HWNodeMpiEncoder::onStop() {
    RT_RET          err = RT_OK;
    mProcThread->join();
    return err;
}

RT_RET HWNodeMpiEncoder::onReset() {
}

RT_RET HWNodeMpiEncoder::onFlush() {
}

RT_RET HWNodeMpiEncoder::allocateBuffersOnPort(RTPortType port) {
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

RT_RET HWNodeMpiEncoder::dequeBuffer(RTMediaBuffer** data, RTPortType port) {
    switch (port) {
        case RT_PORT_INPUT:
            *data = reinterpret_cast<RTMediaBuffer *>(mUnusedInputPort->borrowObj());
            break;
        case RT_PORT_OUTPUT:
            *data = reinterpret_cast<RTMediaBuffer *>(mUsedOutputPort->borrowObj());
            break;
        default:
            RT_LOGE("unknown port! port: %d", port);
            return RT_ERR_UNKNOWN;
    }
    if (!*data) {
        return RT_ERR_UNKNOWN;
    }

    return RT_OK;
}

RT_RET HWNodeMpiEncoder::queueBuffer(RTMediaBuffer* data, RTPortType port) {
    RT_RET ret = RT_OK;
    switch (port) {
        case RT_PORT_INPUT:
            ret = mUsedInputPort->returnObj(data);
            break;
        case RT_PORT_OUTPUT:
            ret = mUnusedOutputPort->returnObj(data);
            break;
        default:
            RT_LOGE("unknown port! port: %d", port);
            return RT_ERR_UNKNOWN;
    }
    if (!data) {
        return ret;
    }

    return RT_OK;
}

static RTNode* createMpiEncoder() {
    return new HWNodeMpiEncoder();
}

struct RTNodeStub hw_node_mpi_encoder {
    .mCreateNode     = createMpiEncoder,
    .mNodeType       = RT_NODE_TYPE_ENCODER,
    .mUsePool        = RT_TRUE,
    .mNodeName       = "hw_node_mpi_encoder",
    .mNodeVersion    = "v1.0",
};

