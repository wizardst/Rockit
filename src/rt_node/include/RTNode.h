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
  * node_demuxer    node_codec        node_render
  *      |               |                 |
  *  init <meta>    init <meta>        init <meta>
  *      |               |                 |
  *  pullBuffer          |                 |
  *      |           pushBuffer            |
  *      |           pullBuffer            |
  *      |               |             pushBuffer
  *   release          release           release
  *      |               |                 |
  */

#ifndef SRC_RT_NODE_INCLUDE_RTNODE_H_
#define SRC_RT_NODE_INCLUDE_RTNODE_H_

#include "rt_header.h"      // NOLINT
#include "rt_node_define.h" // NOLINT
#include "rt_msg_looper.h"  // NOLINT
#include "RTMediaBuffer.h"  // NOLINT
#include "RTMediaData.h"    // NOLINT
#include "RTMediaDef.h"     // NOLINT

#ifdef __cplusplus
extern "C" {
#endif

struct RtMetaData;

#define RT_NODE_CONTEXT void*

typedef enum {
    RT_PORT_INPUT,
    RT_PORT_OUTPUT,
    RT_PORT_MAX,
}RTPortType;

struct RTNodeStub;
class RTNode {
 public:
    virtual ~RTNode() {}
    // core api for media plugins
    virtual RT_RET init(RtMetaData *metaData) = 0;
    virtual RT_RET release() = 0;

    // pull buffer from last node, then push to next node
    virtual RT_RET pullBuffer(RTMediaBuffer** mediaBuf) = 0;
    virtual RT_RET pushBuffer(RTMediaBuffer*  mediaBuf) = 0;

    // use the command to distribute control flow
    virtual RT_RET runCmd(RT_NODE_CMD cmd, RtMetaData *metaData) = 0;

    // notify event for node-bus
    virtual RT_RET setEventLooper(RTMsgLooper* eventLooper) = 0;

    // node capability and stub description
    virtual RtMetaData* queryFormat(RTPortType port) = 0;
    virtual RTNodeStub* queryStub()   = 0;

 protected:
    virtual RT_RET onStart() = 0;
    virtual RT_RET onPause() = 0;
    virtual RT_RET onStop()  = 0;
    virtual RT_RET onReset() = 0;
    virtual RT_RET onFlush() = 0;

 protected:
    void    *mNodeContext;

 public:
    RTNode  *mNext;
};

class RTNodeAdapter {
 public:
    static RT_RET init(RTNode* pNode, RtMetaData* metadata);
    static RT_RET release(RTNode* pNode);

    static RT_RET pullBuffer(RTNode* pNode, RTMediaBuffer** data);
    static RT_RET pushBuffer(RTNode* pNode, RTMediaBuffer* data);

    // borrow and return buffer for pool buffer
    static  RT_RET dequeCodecBuffer(RTNode* pNode, RTMediaBuffer** data, RTPortType port);
    static  RT_RET queueCodecBuffer(RTNode* pNode, RTMediaBuffer*  data, RTPortType port);

    static RT_RET runCmd(RTNode* pNode, RT_NODE_CMD cmd, RtMetaData* metadata);
    static RT_RET setEventLooper(RTNode* pNode, RTMsgLooper* eventLooper);

    static RtMetaData* queryFormat(RTNode* pNode, RTPortType port);
    static RTNodeStub* queryStub(RTNode* pNode);
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
