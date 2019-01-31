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
 *   date: 2018/11/05
 */

#include "RTNode.h"           // NOLINT
#include "RTNodeBus.h"        // NOLINT
#include "rt_node_tests.h"    // NOLINT
#include "rt_string_utils.h"  // NOLINT

#ifdef OS_WINDOWS
#define TEST_URI "E:\\CloudSync\\low-used\\videos\\h264-1080p.mp4"
#else
#define TEST_URI "h264-1080p.mp4"
#endif

void unit_test_node_operations(RTNodeBus *pNodeBus) {
    RTNodeStub* stub = pNodeBus->findStub(RT_NODE_TYPE_DEMUXER, BUS_LINE_SOURCE);
    if (0 && RT_NULL != stub) {
        RTNode* demuxer = stub->mCreateNode();
        demuxer->init(RT_NULL);
        demuxer->pullBuffer(RT_NULL);
        demuxer->pushBuffer(RT_NULL);
        demuxer->runCmd(RT_NODE_CMD_PREPARE, RT_NULL);
        demuxer->runCmd(RT_NODE_CMD_START,   RT_NULL);
        demuxer->runCmd(RT_NODE_CMD_STOP,    RT_NULL);
        demuxer->runCmd(RT_NODE_CMD_CAPS_CHANGE,  RT_NULL);
        demuxer->runCmd(RT_NODE_CMD_SEEK,         RT_NULL);
        demuxer->runCmd(RT_NODE_CMD_RESET,        RT_NULL);
        demuxer->runCmd(RT_NODE_CMD_REINIT,       RT_NULL);
        demuxer->runCmd(RT_NODE_CMD_NAVIGATION,   RT_NULL);
        demuxer->runCmd(RT_NODE_CMD_DRAIN,        RT_NULL);
        demuxer->release();
        delete demuxer;
        demuxer = RT_NULL;
    }
}

RT_RET unit_test_node_bus(INT32 index, INT32 total) {
    // configure node bus and summary
    NodeBusSetting setting = {0};
    rt_str_snprintf(setting.mUri, sizeof(setting.mUri), "%s", TEST_URI);
    RTNodeBus *pNodeBus = new RTNodeBus();

    // auto build node bus and initialization
    pNodeBus->autoBuild(&setting);
    pNodeBus->summary(0);

    // driver core data-flow
    pNodeBus->coreLoopDriver();

    // unit_test_node_operations(pNodeBus);

    // release and reconfigure node bus
    #if TO_DO_FLAG
    pNodeBus->release();
    pNodeBus->autoBuild(&setting);
    pNodeBus->summary(0);
    #endif

    rt_safe_delete(pNodeBus);

    return RT_OK;
}

