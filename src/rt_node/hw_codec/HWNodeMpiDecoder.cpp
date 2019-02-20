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
 *   Date: 2019/01/18
 *   Task: hardware mpi decoder
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "HWMpiDecoder"

#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x0


#include "HWNodeMpiDecoder.h"       // NOLINT
#include "RTAllocatorBase.h"        // NOLINT
#include "RTAllocatorStore.h"       // NOLINT
#include "MpiAdapterCodec.h"        // NOLINT
#include "rt_message.h"             // NOLINT

#define MAX_INPUT_BUFFER_COUNT      30
#define MAX_OUTPUT_BUFFER_COUNT     30

void* hw_decode_loop(void* ptr_node) {
    HWNodeMpiDecoder* node = reinterpret_cast<HWNodeMpiDecoder*>(ptr_node);
    node->runTask();
    return RT_NULL;
}

RTObject *allocHWInputBuffer(void *) {
    return new RTMediaBuffer(NULL, 0);
}

RTObject *allocHWOutputBuffer(void *ctx) {
    HWNodeMpiDecoder *hw_ctx = reinterpret_cast<HWNodeMpiDecoder*>(ctx);
    RTAllocator *allocator = hw_ctx->getLinearAllocator();
    RTMediaBuffer *buffer = RT_NULL;
    if (allocator) {
        allocator->newBuffer(1920 * 1088 * 3 / 2, &buffer);
    }

    return buffer;
}

HWNodeMpiDecoder::HWNodeMpiDecoder() {
    const char* name = "HWMpiDecoder";
    mProcThread = new RtThread(hw_decode_loop, reinterpret_cast<void*>(this));
    mProcThread->setName(name);

    mAllocatorStore = new RTAllocatorStore();
}

HWNodeMpiDecoder::~HWNodeMpiDecoder() {
    release();
}

RT_RET HWNodeMpiDecoder::runTask() {
    RTMediaBuffer *input = NULL;
    RTMediaBuffer *output = NULL;
    while (mRunning) {
        RT_RET err = RT_OK;
        if (!input) {
            input = reinterpret_cast<RTMediaBuffer *>(mUsedInputPort->borrowObj());
        }

        if (!input) {
            RtTime::sleepMs(5);
            continue;
        }

        RT_LOGD("input and output ready, go to decode!");
        err = ma_decode_send_packet(mMpiAdapterCtx, input);
        if (err) {
            if (err == RT_ERR_TIMEOUT) {
                input = NULL;
            }
        } else {
            mUnusedInputPort->returnObj(input);
            input = NULL;
        }
        err = ma_decode_get_frame(mMpiAdapterCtx, &output);
        if (err) {
        } else {
            if (output->getStatus() == RT_MEDIA_BUFFER_STATUS_READY
                    || output->getStatus() == RT_MEDIA_BUFFER_STATUS_INFO_CHANGE) {
                mAvailOutputPort->returnObj(output);
                output = NULL;
            }
        }
    }
    return RT_OK;
}

RT_RET HWNodeMpiDecoder::init(RtMetaData *metadata) {
    RT_RET ret = RT_OK;
    // init memory allocator
    mAllocatorStore = new RTAllocatorStore();
    ret = mAllocatorStore->priorAvailLinearAllocator(metadata, &mLinearAllocator);
    if (ret) {
        RT_LOGE("init failed!");
        return ret;
    }

    mUnusedInputPort = new RTObjectPool(allocHWInputBuffer, MAX_INPUT_BUFFER_COUNT, this);
    mUsedInputPort = new RTObjectPool(RT_NULL, MAX_INPUT_BUFFER_COUNT);
    mAvailOutputPort = new RTObjectPool(allocHWOutputBuffer, MAX_OUTPUT_BUFFER_COUNT, this);

    mMpiAdapterCtx = ma_decode_create(metadata);
    if (mMpiAdapterCtx == RT_NULL) {
        RT_LOGE("mpi adapter context create failed!");
        signalError(CB_ERROR_UNSUPPORT_VIDEO_CODEC);
        return RT_ERR_UNKNOWN;
    }

    allocateBuffersOnPort(RT_PORT_INPUT);
    allocateBuffersOnPort(RT_PORT_OUTPUT);

    return ret;
}

RTAllocator *HWNodeMpiDecoder::getLinearAllocator() {
    return mLinearAllocator;
}
RTAllocator *HWNodeMpiDecoder::getGraphicAllocator() {
    return mGraphicAllocator;
}

RT_RET HWNodeMpiDecoder::release() {
    ma_decode_destroy(&mMpiAdapterCtx);

    if (mUnusedInputPort) {
        delete mUnusedInputPort;
        mUnusedInputPort = NULL;
    }
    if (mUsedInputPort) {
        delete mUsedInputPort;
        mUsedInputPort = NULL;
    }

    if (mAvailOutputPort) {
        delete mAvailOutputPort;
        mAvailOutputPort = NULL;
    }

    // thread release
    delete(mProcThread);
    mProcThread = RT_NULL;

    return RT_OK;
__FAILED:
    return RT_ERR_UNKNOWN;
}

RT_RET HWNodeMpiDecoder::pullBuffer(RTMediaBuffer **data) {
    mCountPull++;
    dequeBuffer(data, RT_PORT_OUTPUT);
}

RT_RET HWNodeMpiDecoder::pushBuffer(RTMediaBuffer* data) {
    mCountPush++;
    queueBuffer(data, RT_PORT_INPUT);
}

RT_RET HWNodeMpiDecoder::runCmd(RT_NODE_CMD cmd, RtMetaData *metadata) {
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

void HWNodeMpiDecoder::signalError(UINT32 what) {
    RTMessage *msg = new RTMessage();
    msg->setWhat(what);
    mEventLooper->post(msg, 0ll);
}

RT_RET HWNodeMpiDecoder::setEventLooper(RTMsgLooper* eventLooper) {
    mEventLooper = eventLooper;
}

RtMetaData* HWNodeMpiDecoder::queryFormat(RTPortType port) {
    return RT_NULL;
}

RTNodeStub* HWNodeMpiDecoder::queryStub() {
    return &hw_node_mpi_decoder;
}

RT_RET HWNodeMpiDecoder::onStart() {
    RT_RET          err = RT_OK;
    mRunning = RT_TRUE;
    mProcThread->start();
    return err;
}

RT_RET HWNodeMpiDecoder::onPause() {
}

RT_RET HWNodeMpiDecoder::onStop() {
    RT_RET          err = RT_OK;
    mRunning = RT_FALSE;
    mProcThread->join();
    return err;
}

RT_RET HWNodeMpiDecoder::onReset() {
}

RT_RET HWNodeMpiDecoder::onFlush() {
}

RT_RET HWNodeMpiDecoder::allocateBuffersOnPort(RTPortType port) {
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
                buffer[i] = reinterpret_cast<RTMediaBuffer *>(mAvailOutputPort->borrowObj());
            }
            for (i = 0; i < MAX_OUTPUT_BUFFER_COUNT; i++) {
                ma_commit_buffer_to_group(mMpiAdapterCtx->codec_ctx, buffer[i]);
            }
        }
            break;
        default:
            RT_LOGE("unknown port! port: %d", port);
            return RT_ERR_UNKNOWN;
    }

    return RT_OK;
}

RT_RET HWNodeMpiDecoder::dequeBuffer(RTMediaBuffer** data, RTPortType port) {
    switch (port) {
        case RT_PORT_INPUT:
            *data = reinterpret_cast<RTMediaBuffer *>(mUnusedInputPort->borrowObj());
            break;
        case RT_PORT_OUTPUT:
            *data = reinterpret_cast<RTMediaBuffer *>(mAvailOutputPort->borrowObj());
            break;
        default:
            RT_LOGE("unknown port! port: %d", port);
            return RT_ERR_UNKNOWN;
    }
    if (!*data) {
        return RT_ERR_UNKNOWN;
    }
    (*data)->getMetaData()->setInt32(kKeyCodecType, RTTRACK_TYPE_VIDEO);
    return RT_OK;
}

RT_RET HWNodeMpiDecoder::queueBuffer(RTMediaBuffer* data, RTPortType port) {
    RT_RET ret = RT_OK;
    switch (port) {
        case RT_PORT_INPUT:
            ret = mUsedInputPort->returnObj(data);
            break;
        case RT_PORT_OUTPUT:
            // TODO(fill buffer to mpp frame group)
            ret = ma_fill_buffer_to_group(mMpiAdapterCtx->codec_ctx, data);
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

static RTNode* createMpiDecoder() {
    return new HWNodeMpiDecoder();
}

struct RTNodeStub hw_node_mpi_decoder {
    .mCreateNode     = createMpiDecoder,
    .mNodeType       = RT_NODE_TYPE_DECODER,
    .mUsePool        = RT_TRUE,
    .mNodeName       = "hw_node_mpi_decoder",
    .mNodeVersion    = "v1.0",
};

