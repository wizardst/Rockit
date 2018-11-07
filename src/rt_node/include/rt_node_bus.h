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

#ifndef __RT_NODE_BUS_H__
#define __RT_NODE_BUS_H__

#include "rt_header.h"
#include "rt_node.h"
#include "rt_hash_table.h"
#include "rt_array_list.h"

typedef struct node_bus_context
{
    RtArrayList *node_bus;
    RtHashTable *node_all;
} NodeBusContext;

// ! life cycles of node_bus
INT32       rt_node_bus_build(NodeBusContext* ctx);
INT32        rt_node_bus_init(NodeBusContext* ctx);
INT32     rt_node_bus_release(NodeBusContext* ctx);
INT32     rt_node_bus_summary(NodeBusContext* ctx, RT_BOOL full);

// ! node operations of node_bus
INT32    rt_node_register_all(NodeBusContext* ctx);
INT32        rt_node_register(NodeBusContext* ctx, RT_Node *node);
RT_Node*         rt_node_find(NodeBusContext* ctx, UINT8 node_type, UINT8 node_id);

#endif // __RT_NODE_BUS_H__
