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
 * Author: TOD - UNKOWN
 *   Date: 2018/11/03
 *   Task: use ffmpeg as encoder and decoder
 */

#ifndef SRC_RT_NODE_FF_NODE_INCLUDE_FFNODECODEC_H_
#define SRC_RT_NODE_FF_NODE_INCLUDE_FFNODECODEC_H_

#include "RTNodeCodec.h"
#include "rt_header.h" // NOLINT

typedef enum _CodecType {
    FFCodecDecoder = 0,
    FFCodecEncoder,
    FFCodecMax,
} FFCodecType;

class FFNodeCodec : public RTNodeCodec {
 public:
    explicit FFNodeCodec(FFCodecType type);
    virtual ~FFNodeCodec();
    RT_RET runTask();

 public:
    // override RTNode methods
    virtual RT_RET init(RtMetaData *metadata);
    virtual RT_RET release();
    virtual RT_RET pullBuffer(RTMediaBuffer* frame);
    virtual RT_RET dequePoolBuffer(RTMediaBuffer** data);
    virtual RT_RET pushBuffer(RTMediaBuffer* packet);
    virtual RT_RET runCmd(RT_NODE_CMD cmd, RtMetaData *metadata);
    virtual RtMetaData* queryCap();
    virtual RTNodeStub* queryInfo();

 protected:
    // override RTNodeCodec method
    virtual RT_RET onStart();
    virtual RT_RET onStop();
    virtual RT_RET onPause();
    virtual RT_RET onFlush();

 private:
    FFCodecType mCodecType;   // Encoder or Decoder
};

extern struct RTNodeStub ff_node_decoder;
extern struct RTNodeStub ff_node_encoder;

#endif  // SRC_RT_NODE_FF_NODE_INCLUDE_FFNODECODEC_H_
