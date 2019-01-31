/*
 * Copyright 2019 Rockchip Electronics Co. LTD
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

#ifndef SRC_RT_NODE_HW_CODEC_INCLUDE_HWNODEMPIDECODER_H_
#define SRC_RT_NODE_HW_CODEC_INCLUDE_HWNODEMPIDECODER_H_

#ifdef RK_HW_CODEC

#include "rt_header.h"       // NOLINT
#include "rt_thread.h"       // NOLINT
#include "MpiAdapterCodec.h" // NOLINT

#include "RTNodeCodec.h"     // NOLINT
#include "RTMediaBuffer.h"   // NOLINT
#include "RTObject.h"        // NOLINT
#include "RTObjectPool.h"    // NOLINT

class RTAllocator;
class RTAllocatorStore;

class HWNodeMpiDecoder : public RTNodeCodec {
 public:
    HWNodeMpiDecoder();
    virtual ~HWNodeMpiDecoder();
    RT_RET runTask();

 public:
    // override RTNode public methods
    virtual RT_RET init(RtMetaData *metadata);
    virtual RT_RET release();

    virtual RT_RET pullBuffer(RTMediaBuffer **data);
    virtual RT_RET pushBuffer(RTMediaBuffer*  data);

    virtual RT_RET runCmd(RT_NODE_CMD cmd, RtMetaData *metadata);
    virtual RT_RET setEventLooper(RTMsgLooper* eventLooper);

    virtual RtMetaData* queryFormat(RTPortType port);
    virtual RTNodeStub* queryStub();

 protected:
    // override RTNode protected method
    virtual RT_RET onStart();
    virtual RT_RET onPause();
    virtual RT_RET onStop();
    virtual RT_RET onReset();
    virtual RT_RET onFlush();

    RT_RET allocateBuffersOnPort(RTPortType port);

 public:
    // borrow and return buffer for pool buffer
    virtual RT_RET dequeBuffer(RTMediaBuffer** data, RTPortType port);
    virtual RT_RET queueBuffer(RTMediaBuffer*  data, RTPortType port);

 public:
    RTAllocator *getLinearAllocator();
    RTAllocator *getGraphicAllocator();

 private:
    MACodecContext      *mMpiAdapterCtx;
    RtThread            *mProcThread;
    RTObjectPool        *mUnusedInputPort;
    RTObjectPool        *mUsedInputPort;
    RTObjectPool        *mAvailOutputPort;
    RTMsgLooper         *mEventLooper;

    RT_BOOL              mRunning;
    UINT32               mCountPull;
    UINT32               mCountPush;
    RT_BOOL              mUsePool;

    RTAllocatorStore    *mAllocatorStore;
    RTAllocator         *mLinearAllocator;
    RTAllocator         *mGraphicAllocator;
};

extern struct RTNodeStub hw_node_mpi_decoder;

#endif

#endif  // SRC_RT_NODE_HW_CODEC_INCLUDE_HWNODEMPIDECODER_H_

