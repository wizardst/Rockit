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

#include "rt_node.h" // NOLINT
#include "rt_node_bus.h" // NOLINT
#include "rt_node_tests.h" // NOLINT

void unit_test_node_operations(struct NodeBusContext* bus) {
    rt_node_bus_register_all(bus);
    RT_Node* demuxer = rt_node_bus_find(bus, RT_NODE_TYPE_DEMUXER, 0);
    if (RT_NULL != demuxer) {
        demuxer->init();

        void *data   = RT_NULL;
        UINT32 *size = 0;
        demuxer->pull(data, size);
        demuxer->push(data, size);
        demuxer->run_cmd(RT_NODE_CMD_INIT,    0, RT_NULL);
        demuxer->run_cmd(RT_NODE_CMD_PREPARE, 0, RT_NULL);
        demuxer->run_cmd(RT_NODE_CMD_START,   0, RT_NULL);
        demuxer->run_cmd(RT_NODE_CMD_STOP,    0, RT_NULL);
        demuxer->run_cmd(RT_NODE_CMD_CAPS_CHANGE,  0, RT_NULL);
        demuxer->run_cmd(RT_NODE_CMD_SEEK,         0, RT_NULL);
        demuxer->run_cmd(RT_NODE_CMD_RESET,        0, RT_NULL);
        demuxer->run_cmd(RT_NODE_CMD_REINIT,       0, RT_NULL);
        demuxer->run_cmd(RT_NODE_CMD_NAVIGATION,   0, RT_NULL);
        demuxer->run_cmd(RT_NODE_CMD_DRAIN,        0, RT_NULL);
        demuxer->release();
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

