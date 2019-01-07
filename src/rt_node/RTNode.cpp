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
 * Author: martin.cheng@rock-chips.com
 *   Date: 2018/11/03
 *   Task: stable api for media node
 */

#include "RTNode.h" // NOLINT
#include "rt_header.h" // NOLINT
#include "rt_metadata.h" // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTNode"

#define DEBUG_NODE_CMD   0x1

RT_RET check_err(RTNode* node, RT_RET err, const char* func_name) {
    if (RT_OK != err) {
        RTNodeStub* stub = node->queryInfo();
        RT_LOGE("RTNode(%15.15s):  errno=%d, Fail to %s",
                stub->mNodeName, err, func_name);
    }
    return err;
}

#define CHECK_ERR(node, err) check_err(node, err, __FUNCTION__)

RT_RET RTNodeAdapter::init(RTNode* node, RtMetaData* metadata) {
    RT_RET err = node->init(metadata);
    return CHECK_ERR(node, err);
}

RT_RET RTNodeAdapter::release(RTNode* node) {
    RT_RET err = node->release();
    return CHECK_ERR(node, err);
}

RT_RET RTNodeAdapter::pullBuffer(RTNode* node, RTMediaBuffer* data) {
    RT_RET err = node->pullBuffer(data);
    return CHECK_ERR(node, err);
}

RT_RET RTNodeAdapter::dequePoolBuffer(RTNode* node, RTMediaBuffer** data) {
    RT_RET err = node->dequePoolBuffer(data);
    return CHECK_ERR(node, err);
}

RT_RET RTNodeAdapter::pushBuffer(RTNode* node, RTMediaBuffer* data) {
    RT_RET err = node->pushBuffer(data);
    return CHECK_ERR(node, err);
}

RT_RET RTNodeAdapter::runCmd(RTNode* node, RT_NODE_CMD cmd, RtMetaData* metadata) {
    RT_RET err = node->runCmd(cmd, metadata);
    RTNodeStub* info = node->queryInfo();
    RT_LOGD_IF(DEBUG_NODE_CMD, "RTNode(%15.15s):  exec %-10s",
                  info->mNodeName, rt_node_cmd_name(cmd));
    return CHECK_ERR(node, err);
}

RtMetaData* RTNodeAdapter::queryCap(RTNode* node) {
    return node->queryCap();
}

RTNodeStub* RTNodeAdapter::queryInfo(RTNode* node) {
    return node->queryInfo();
}
