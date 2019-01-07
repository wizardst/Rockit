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

#include "RTNode.h" // NOLINT
#include "RTNodeBus.h" // NOLINT
#include "rt_node_tests.h" // NOLINT

void unit_test_node_operations(struct NodeBusContext* bus) {
    rt_node_bus_register_all(bus);

    RTNodeStub* stub = rt_node_bus_find(bus, RT_NODE_TYPE_DEMUXER, 0);
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
    struct NodeBusContext* bus = rt_node_bus_create();
    rt_node_bus_summary(bus, RT_FALSE);
    rt_node_bus_build(bus);

    unit_test_node_operations(bus);
    rt_node_bus_summary(bus, RT_FALSE);
    rt_node_bus_build(bus);

    rt_node_bus_destory(bus);
    return RT_OK;
}

