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
 *   date: 20180704
 */

 /*
  *    node         node_codec      ff_node_codec
  *      |  init <ctx>  |                |
  *      |              |    init <ctx>  |
  *      |              |                | init <ctx>
  *      |  send <pkt>  |                |
  *      |              |   send <pkt>   |
  *      | recv <frame> |                |
  *      |              |  recv <frame>  |
  *      |   release    |                |
  *      |              |    release     |
  */

#ifndef SRC_RT_NODE_INCLUDE_RTNODE_H_
#define SRC_RT_NODE_INCLUDE_RTNODE_H_

#include "rt_header.h" // NOLINT
#include "rt_node_define.h" // NOLINT
#include "RTMediaBuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

struct RtMetaData;

#define RT_NODE_CONTEXT void*

struct RTNodeStub;
class RTNode {
 public:
    virtual ~RTNode() {}
    // core api for media plugins
    virtual RT_RET init(RtMetaData *metadata) = 0;
    virtual RT_RET release() = 0;
    virtual RT_RET pullBuffer(RTMediaBuffer* data) = 0;

    virtual RT_RET dequePoolBuffer(RTMediaBuffer** data) = 0;
    virtual RT_RET pushBuffer(RTMediaBuffer* data) = 0;
    virtual RT_RET runCmd(RT_NODE_CMD cmd, RtMetaData *metadata) = 0;
    virtual RtMetaData* queryCap() = 0;
    virtual RTNodeStub* queryInfo() = 0;
 protected:
    void*   mNodeContext;
};

class RTNodeAdapter {
 public:
    static RT_RET init(RTNode* node, RtMetaData* metadata);
    static RT_RET release(RTNode* node);
    static RT_RET pullBuffer(RTNode* node, RTMediaBuffer* data);
    static RT_RET dequePoolBuffer(RTNode* node, RTMediaBuffer** data);
    static RT_RET pushBuffer(RTNode* node, RTMediaBuffer* data);
    static RT_RET runCmd(RTNode* node, RT_NODE_CMD cmd, RtMetaData* metadata);
    static RtMetaData* queryCap(RTNode* node);
    static RTNodeStub* queryInfo(RTNode* node);
};

typedef RTNode* (*CreateNode)();
struct RTNodeStub {
    CreateNode         mCreateNode;
    const RT_NODE_TYPE mNodeType;
    const RT_BOOL      mUsePool;
    const char*        mNodeName;
    const char*        mNodeVersion;
};

RT_RET check_err(RTNode *node, INT8 err, const char* func_name);

#ifdef __cplusplus
}
#endif

#endif  // SRC_RT_NODE_INCLUDE_RTNODE_H_
