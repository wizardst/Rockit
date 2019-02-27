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
 *   date: 20180714
 */

#ifndef SRC_RT_NODE_INCLUDE_RTNODEBUS_H_
#define SRC_RT_NODE_INCLUDE_RTNODEBUS_H_

#include "RTNode.h"        // NOLINT
#include "rt_header.h"     // NOLINT
#include "rt_hash_table.h" // NOLINT
#include "rt_array_list.h" // NOLINT
#include "rt_message.h"      // NOLINT
#include "rt_msg_handler.h"  // NOLINT
#include "rt_msg_looper.h"   // NOLINT

struct NodeBusContext;

typedef struct _NodeBusSetting {
    char   mUri[1024];
    char   mUserAgent[256];
    char   mVersion[32];
} NodeBusSetting;

typedef enum _BUS_LINE_TYPE {
    BUS_LINE_SOURCE = 0,
    BUS_LINE_VIDEO,
    BUS_LINE_AUDIO,
    BUS_LINE_SUBTE,
    BUS_LINE_MAX,
} BUS_LINE_TYPE;

typedef struct _BUS_LINE_NAME {
    UINT32      cmd;
    const char *name;
} BUS_LINE_NAME;

static const BUS_LINE_NAME mBusLineNames[] = {
    { BUS_LINE_SOURCE, "BUS_LINE_SOURCE" },
    { BUS_LINE_VIDEO,  "BUS_LINE_VIDEO" },
    { BUS_LINE_AUDIO,  "BUS_LINE_AUDIO" },
    { BUS_LINE_SUBTE,  "BUS_LINE_SUBTE" },
    { BUS_LINE_MAX,    "BUS_LINE_MAX" },
};

RTNodeStub* findStub(RT_NODE_TYPE nType, BUS_LINE_TYPE lType);

class RTNodeBus : public RTMsgHandler {
 public:
    RTNodeBus();
    ~RTNodeBus();

    /* use metadata to init node_bus */
    RT_RET      autoBuild(NodeBusSetting *setting);

    /* commit control operations */
    RT_RET      reset();
    RT_RET      prepare();
    RT_RET      start();
    RT_RET      pause();
    RT_RET      stop();
    RT_RET      seekToAsync(INT64 usec);
    RT_RET      seekTo(INT64 usec);
    RT_RET      setCurState(UINT32 newState);
    UINT32      getCurState();

    /* decode proc */
    RT_RET      startDataLooper();
    RT_RET      startAudioPlayerProc();

    /* node manager */
    RT_RET      summary(INT32 fd, RT_BOOL full = RT_FALSE);
    RT_RET      registerStub(RTNodeStub *nStub);
    RT_RET      registerNode(RTNode     *pNode);
    RTNodeStub* findStub(RT_NODE_TYPE nType, BUS_LINE_TYPE lType);
    RTNode*     findNode(RT_NODE_TYPE nType, BUS_LINE_TYPE lType);

    void        onMessageReceived(struct RTMessage* msg);

 private:
    RT_RET      registerCoreStubs();
    RT_RET      nodeChainAppend(RTNode *pNode, BUS_LINE_TYPE lType);
    RT_RET      nodeChainDriver(RTNode *pNode, BUS_LINE_TYPE lType);
    RT_RET      nodeChainDumper(BUS_LINE_TYPE lType);
    RT_RET      excuteCommand(RT_NODE_CMD cmd);

 private:
    struct NodeBusContext* mBusCtx;
};
#endif  // SRC_RT_NODE_INCLUDE_RTNODEBUS_H_
