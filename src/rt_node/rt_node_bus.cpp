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
 *   Task: construct and manage pipeline of media node
 */

#include "rt_node_bus.h"
#include "rt_node_parser.h"
#include "rt_node_codec.h"

UINT32  node_hash_func(UINT32 bucktes, const void *key)
{
    return *((UINT32 *)(key)) % (bucktes);
}

// ! life cycles of node_bus
INT32 rt_node_bus_init(NodeBusContext* ctx)
{
   ctx->node_bus = array_list_create();
   ctx->node_all = rt_hash_table_init((NODE_TYPE_MAX - NODE_TYPE_BASE), node_hash_func, RT_NULL);
   return RT_OK;
}

INT32 rt_node_bus_build(NodeBusContext* ctx)
{
    return RT_OK;
}

INT32 rt_node_bus_release(NodeBusContext* ctx)
{
    array_list_destroy(&ctx->node_bus);
    rt_hash_table_destory(ctx->node_all);
    ctx->node_all = RT_NULL;

    return RT_OK;
}

INT32 rt_node_bus_summary(NodeBusContext* ctx, RT_BOOL full)
{
   rt_hash_table_dump(ctx->node_all);
   return RT_OK;
}

INT32 rt_node_register_all(NodeBusContext *bus)
{
    rt_node_register(bus, RT_NULL);
    rt_node_register(bus, RT_NULL);
    return RT_OK;
}

INT32 rt_node_register(NodeBusContext *bus, RT_Node *node)
{
    INT32 node_type = node->type;
    rt_hash_table_insert(bus->node_all, (void*)(node_type), node);
    return RT_OK;
}

RT_Node* rt_node_find(NodeBusContext *bus, UINT8 node_type, UINT8 node_id)
{
    void* data = rt_hash_table_find(bus->node_all, (void*)node_type);
    if(RT_NULL != data) {
        // node = node->next;
    }
    return RT_NULL;
}
