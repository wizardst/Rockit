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
 * author: martin.cheng@rock-chips.com
 *   date: 20190122
 */

#ifndef SRC_RT_NODE_RT_SINK_INCLUDE_RTNODESINKAWINDOW_H_
#define SRC_RT_NODE_RT_SINK_INCLUDE_RTNODESINKAWINDOW_H_

#include "RTNode.h"    // NOLINT
#include "rt_header.h" // NOLINT

class RTNodeSinkAWindow : public RTNode {
 public:
    RTNodeSinkAWindow();
    virtual ~RTNodeSinkAWindow();
    RT_RET runTask();

 public:
    // override RTNode public methods
    virtual RT_RET init(RtMetaData *metadata);
    virtual RT_RET release();

    virtual RT_RET pullBuffer(RTMediaBuffer **media_buf);
    virtual RT_RET pushBuffer(RTMediaBuffer*  media_buf);

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

 public:
    // override RTSinkDisplay public methods
    virtual RT_RET onFireFrame();
};

extern struct RTNodeStub rt_sink_display_awindow;

#endif  // SRC_RT_NODE_RT_SINK_INCLUDE_RTNODESINKAWINDOW_H_
