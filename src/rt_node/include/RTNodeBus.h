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

#include "RTNode.h" // NOLINT
#include "rt_header.h" // NOLINT
#include "rt_hash_table.h" // NOLINT
#include "rt_array_list.h" // NOLINT

struct NodeBusContext;

// ! life cycles of node_bus
struct NodeBusContext* rt_node_bus_create();
INT32     rt_node_bus_destory(struct NodeBusContext* bus);
INT32       rt_node_bus_build(struct NodeBusContext* bus);
INT32     rt_node_bus_summary(struct NodeBusContext* bus, RT_BOOL full);

// ! node operations of node_bus
INT32  rt_node_bus_register_all(struct NodeBusContext* bus);
INT32      rt_node_bus_register(struct NodeBusContext* bus, RTNodeStub* node_info);
RTNodeStub*    rt_node_bus_find(struct NodeBusContext* bus, RT_NODE_TYPE node_type, UINT8 node_id);

#endif  // SRC_RT_NODE_INCLUDE_RTNODEBUS_H_
