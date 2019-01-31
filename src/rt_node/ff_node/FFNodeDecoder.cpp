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
 * Module: use ffmpeg as decoder
 *
 * author: martin.cheng@rock-chips.com
 *   Date: 2019/01/04
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FFNodeDecoder"

#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x0

#include "FFNodeDecoder.h" // NOLINT
#include "RTObjectPool.h" // NOLINT
#include "FFMPEGAdapter.h" // NOLINT
#include "rt_metadata.h" // NOLINT
#include "RTMediaMetaKeys.h" // NOLINT
#include "rt_thread.h" // NOLINT
#include "rt_dequeue.h" // NOLINT
#include "RTMediaBuffer.h" // NOLINT
#include "FFAdapterCodec.h" // NOLINT

#define MAX_INPUT_BUFFER_COUNT      30
#define MAX_OUTPUT_BUFFER_COUNT     8

void* ff_codec_loop(void* ptr_node) {
    FFNodeDecoder* node = reinterpret_cast<FFNodeDecoder*>(ptr_node);
    node->runTask();
    return RT_NULL;
}

RTObject *allocInputBuffer(void *) {
    return new RTMediaBuffer(NULL, 0);
}

RTObject *allocOutputBuffer(void *) {
    return new RTMediaBuffer(1920 * 1088 * 3 / 2);
}

FFNodeDecoder::FFNodeDecoder() {
    const char* name = "FFDecoder";
    mProcThread = new RtThread(ff_codec_loop, reinterpret_cast<void*>(this));
    mProcThread->setName(name);

    mUnusedInputPort  = RT_NULL;
    mUsedInputPort    = RT_NULL;
    mUnusedOutputPort = RT_NULL;
    mUsedOutputPort   = RT_NULL;
    mUnusedInputPort  = new RTObjectPool(allocInputBuffer, MAX_INPUT_BUFFER_COUNT);
    mUsedInputPort    = new RTObjectPool(NULL, MAX_INPUT_BUFFER_COUNT);
    mUnusedOutputPort = new RTObjectPool(allocOutputBuffer, MAX_OUTPUT_BUFFER_COUNT);
    mUsedOutputPort   = new RTObjectPool(RT_NULL, MAX_OUTPUT_BUFFER_COUNT);

    mTrackParms       = rt_malloc(RTTrackParms);

    mMetaInput  = RT_NULL;
    mMetaOutput = new RtMetaData;
}

FFNodeDecoder::~FFNodeDecoder() {
    release();
    rt_safe_free(mTrackParms);
    mNodeContext = RT_NULL;
}

RT_RET FFNodeDecoder::init(RtMetaData *metadata) {
    mFFCodec = fa_video_decode_create(metadata);
    if (!mFFCodec) {
        RT_LOGE("fa_video_decode_open failed");
        return RT_ERR_UNKNOWN;
    }

    mMetaInput = metadata;
    rt_medatdata_goto_trackpar(metadata, mTrackParms);
    mMetaOutput->clear();
    mMetaOutput->setInt32(kKeyFrameW,   mTrackParms->mVideoWidth);
    mMetaOutput->setInt32(kKeyFrameH,   mTrackParms->mVideoHeight);

    allocateBuffersOnPort(RT_PORT_INPUT);
    allocateBuffersOnPort(RT_PORT_OUTPUT);

    return RT_OK;
}

RT_RET FFNodeDecoder::allocateBuffersOnPort(RTPortType port) {
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

RT_RET FFNodeDecoder::release() {
    fa_video_decode_destroy(&mFFCodec);

    rt_safe_delete(mUnusedInputPort);
    rt_safe_delete(mUsedInputPort);
    rt_safe_delete(mMetaInput);

    rt_safe_delete(mUnusedOutputPort);
    rt_safe_delete(mUsedOutputPort);

    // thread release
    rt_safe_delete(mProcThread);

    return RT_OK;
}

RT_RET FFNodeDecoder::dequeBuffer(RTMediaBuffer **buffer, RTPortType type) {
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
    if (!*buffer) {
        return RT_ERR_UNKNOWN;
    }

    return RT_OK;
}

RT_RET FFNodeDecoder::queueBuffer(RTMediaBuffer *buffer, RTPortType type) {
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


RT_RET FFNodeDecoder::pullBuffer(RTMediaBuffer** data) {
    mCountPull++;
    return dequeBuffer(data, RT_PORT_OUTPUT);
}

RT_RET FFNodeDecoder::pushBuffer(RTMediaBuffer*  data) {
    mCountPush++;
    return queueBuffer(data, RT_PORT_INPUT);
}

RT_RET FFNodeDecoder::runCmd(RT_NODE_CMD cmd, RtMetaData *metadata) {
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

RT_RET FFNodeDecoder::setEventLooper(RTMsgLooper* eventLooper) {
    mEventLooper = eventLooper;
    return RT_OK;
}

RtMetaData* FFNodeDecoder::queryFormat(RTPortType port) {
    RtMetaData *nMeta = RT_NULL;
    switch (port) {
    case RT_PORT_INPUT:
        nMeta = mMetaInput;
        break;
    case RT_PORT_OUTPUT:
        nMeta = mMetaOutput;
        break;
    default:
        break;
    }
    return nMeta;
}

RTNodeStub* FFNodeDecoder::queryStub() {
    return &ff_node_video_decoder;
}

RT_RET FFNodeDecoder::runTask() {
    RTMediaBuffer *input = NULL;
    RTMediaBuffer *output = NULL;
    while (mRunning) {
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
        err = fa_decode_send_packet(mFFCodec, input);
        if (err) {
            if (err == RT_ERR_TIMEOUT) {
                input = NULL;
            }
            continue;
        } else {
            mUnusedInputPort->returnObj(input);
            input = NULL;
        }
        err = fa_decode_get_frame(mFFCodec, output);
        if (err) {
            continue;
        } else {
            if (output->getStatus() == RT_MEDIA_BUFFER_STATUS_READY) {
                mUsedOutputPort->returnObj(output);
                output = NULL;
            }
        }
    }
    return RT_OK;
}

RT_RET FFNodeDecoder::onStart() {
    RT_RET          err = RT_OK;
    mRunning = RT_TRUE;
    mProcThread->start();
    return err;
}

RT_RET FFNodeDecoder::onPause() {
    return RT_OK;
}

RT_RET FFNodeDecoder::onStop() {
    RT_RET          err = RT_OK;
    mRunning = RT_FALSE;
    mProcThread->join();
    return err;
}

RT_RET FFNodeDecoder::onReset() {
    return RT_ERR_UNIMPLIMENTED;
}

RT_RET FFNodeDecoder::onFlush() {
    return RT_ERR_UNIMPLIMENTED;
}

static RTNode* createFFDecoder() {
    return new FFNodeDecoder();
}

struct RTNodeStub ff_node_video_decoder {
    .mCreateNode     = createFFDecoder,
    .mNodeType       = RT_NODE_TYPE_DECODER,
    .mUsePool        = RT_TRUE,
    .mNodeName       = "ff_node_video_decoder",
    .mNodeVersion    = "v1.0",
};

