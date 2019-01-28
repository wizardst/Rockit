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

#include "RTSinkDisplayNative.h"  // NOLINT
#include "RTMediaData.h"          // NOLINT

RTSinkDisplayNative::RTSinkDisplayNative() {
    // @TODO
}

RTSinkDisplayNative::~RTSinkDisplayNative() {
    // @TODO
}

// override RTSinkDisplay public methods
RT_RET RTSinkDisplayNative::onFireFrame() {
    return RT_OK;
}

// override RTNode public methods
RT_RET RTSinkDisplayNative::init(RtMetaData *metadata) {
    return RT_OK;
}

RT_RET RTSinkDisplayNative::release() {
    return RT_OK;
}

RT_RET RTSinkDisplayNative::pullBuffer(RTMediaBuffer **frame) {
    return RT_OK;
}

RT_RET RTSinkDisplayNative::pushBuffer(RTMediaBuffer*  packet) {
    return RT_OK;
}

RT_RET RTSinkDisplayNative::runCmd(RT_NODE_CMD cmd, RtMetaData *metadata) {
    RT_RET err = RT_OK;
    switch (cmd) {
    case RT_NODE_CMD_INIT:
        err = this->init(metadata);
        break;
    case RT_NODE_CMD_START:
        err = this->onStart();
        break;
    case RT_NODE_CMD_STOP:
        err = this->onPause();
        break;
    case RT_NODE_CMD_RESET:
        err = this->onReset();
        break;
    case RT_NODE_CMD_FLUSH:
        err = this->onFlush();
        break;
    case RT_NODE_CMD_PAUSE:
        err = this->onPause();
        break;
    default:
        break;
    }
}

RT_RET RTSinkDisplayNative::setEventLooper(RTMsgLooper* eventLooper) {
    return RT_OK;
}

RtMetaData* RTSinkDisplayNative::queryFormat(RTPortType port) {
    return RT_NULL;
}

RTNodeStub* RTSinkDisplayNative::queryStub() {
    return &rt_sink_display_native;
}

// override RTNode protected method
RT_RET RTSinkDisplayNative::onStart() {
    return RT_OK;
}

RT_RET RTSinkDisplayNative::onPause() {
    return RT_OK;
}

RT_RET RTSinkDisplayNative::onStop() {
    return RT_OK;
}

RT_RET RTSinkDisplayNative::onReset() {
    return RT_OK;
}

RT_RET RTSinkDisplayNative::onFlush() {
    return RT_OK;
}

static RTNode* createSinkDisplay() {
    return new RTSinkDisplayNative();
}

struct RTNodeStub rt_sink_display_native {
    .mCreateNode     = createSinkDisplay,
    .mNodeType       = RT_NODE_TYPE_RENDER,
    .mUsePool        = RT_FALSE,
    .mNodeName       = "rt_sink_display_native",
    .mNodeVersion    = "v1.0",
};
