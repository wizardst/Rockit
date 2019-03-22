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

#include "RTNode.h"      // NOLINT
#include "RTNodeCodec.h" // NOLINT
#include "rt_header.h"   // NOLINT
#include "rt_metadata.h" // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTNode"

#define DEBUG_FLAG   0x1

RT_RET check_err(RTNode* pNode, RT_RET err, const char* func_name) {
    if (RT_OK != err) {
        RTNodeStub* nStub = pNode->queryStub();
        RT_LOGE("%-12s { name:%-18s errno=%d, Fail to %s }", \
                                rt_node_type_name(nStub->mNodeType), \
                                nStub->mNodeName, err, func_name);
    }
    return err;
}

#define CHECK_ERR(pNode, err) check_err(pNode, err, __FUNCTION__)

RT_RET RTNodeAdapter::init(RTNode* pNode, RtMetaData* metadata) {
    RT_RET  err  = pNode->init(metadata);
    pNode->mNext = RT_NULL;
    pNode->mPrev = RT_NULL;
    return CHECK_ERR(pNode, err);
}

RT_RET RTNodeAdapter::release(RTNode* pNode) {
    RT_RET err = pNode->release();
    return CHECK_ERR(pNode, err);
}

RT_RET RTNodeAdapter::pullBuffer(RTNode* pNode, RTMediaBuffer** data) {
    RT_RET err = pNode->pullBuffer(data);
    return CHECK_ERR(pNode, err);
}

RT_RET RTNodeAdapter::pushBuffer(RTNode* pNode, RTMediaBuffer* data) {
    RT_RET err = pNode->pushBuffer(data);
    return CHECK_ERR(pNode, err);
}

RT_RET RTNodeAdapter::dequeCodecBuffer(RTNode* pNode, RTMediaBuffer** data, RTPortType port) {
    RT_RET err = RT_ERR_BAD;
    RTNodeStub* stub = pNode->queryStub();
    if ((RT_NODE_TYPE_DECODER == stub->mNodeType) || (RT_NODE_TYPE_ENCODER == stub->mNodeType)) {
        RTNodeCodec* codec = reinterpret_cast<RTNodeCodec*>(pNode);
        err = codec->dequeBuffer(data, port);
    }
    return err;
}

RT_RET RTNodeAdapter::queueCodecBuffer(RTNode* pNode, RTMediaBuffer*  data, RTPortType port) {
    RT_RET err = RT_ERR_BAD;
    RTNodeStub* stub = pNode->queryStub();
    if ((RT_NODE_TYPE_DECODER == stub->mNodeType) || (RT_NODE_TYPE_ENCODER == stub->mNodeType)) {
        RTNodeCodec* codec = reinterpret_cast<RTNodeCodec*>(pNode);
        err = codec->queueBuffer(data, port);
    }
    return err;
}

RT_RET RTNodeAdapter::runCmd(RTNode* pNode, RT_NODE_CMD cmd, RtMetaData* metadata) {
    RT_RET err = RT_OK;

    // run commands in all nodes of active node-chain
    while (RT_NULL != pNode) {
        err = pNode->runCmd(cmd, metadata);
        err = CHECK_ERR(pNode, err);
        pNode = pNode->mNext;
    }
    return err;
}

RT_RET RTNodeAdapter::setEventLooper(RTNode* pNode, RTMsgLooper* eventLooper) {
    RT_RET err = pNode->setEventLooper(eventLooper);
    return CHECK_ERR(pNode, err);
}

RtMetaData* RTNodeAdapter::queryFormat(RTNode* pNode, RTPortType port) {
    return pNode->queryFormat(port);
}

RTNodeStub* RTNodeAdapter::queryStub(RTNode* pNode) {
    return pNode->queryStub();
}
