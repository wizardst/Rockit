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

#include "rt_node_bus.h" // NOLINT
#include "rt_node_parser.h" // NOLINT
#include "rt_node_header.h" // NOLINT

struct NodeBusContext {
    RtArrayList *node_bus;
    RtHashTable *node_all;
} NodeBusContext;

UINT32  node_hash_func(UINT32 bucktes, const void *key) {
    void *tmp_key = const_cast<void *>(key);
    return *(reinterpret_cast<UINT32 *>(tmp_key)) % (bucktes);
}

// ! life cycles of node_bus
struct NodeBusContext* rt_node_bus_create() {
    struct NodeBusContext* bus = rt_malloc(struct NodeBusContext);
    bus->node_bus = array_list_create();
    bus->node_all = rt_hash_table_create((RT_NODE_TYPE_MAX - RT_NODE_TYPE_BASE),
                                       hash_ptr_func, hash_ptr_compare);
    return bus;
}

INT32 rt_node_bus_destory(struct NodeBusContext* bus) {
    array_list_destroy(bus->node_bus);
    rt_hash_table_destory(bus->node_all);

    RT_ASSERT(RT_NULL == bus->node_bus);
    RT_ASSERT(RT_NULL == bus->node_all);

    rt_safe_free(bus);

    return RT_OK;
}

INT32 rt_node_bus_build(struct NodeBusContext* bus) {
    return RT_OK;
}

INT32 rt_node_bus_summary(struct NodeBusContext* bus, RT_BOOL full) {
    RT_LOGE("bus = %p full =%d", bus, full);
    // rt_hash_table_dump(bus->node_all);

    UINT32 num_buckets = rt_hash_table_get_num_buckets(bus->node_all);
    for (UINT32 idx = 0; idx < num_buckets; idx++) {
        struct rt_hash_node* node = RT_NULL;
        UINT32 num_plugin = 0;
        struct rt_hash_node* root = rt_hash_table_get_bucket(bus->node_all, idx);
        for (node = root->next; node != root; node = node->next, num_plugin++) {
            RT_Node* plugin = reinterpret_cast<RT_Node*>(node->data);
            RT_LOGE("buckets[%02d:%02d]: type:%-10s, ptr:%p",
                     idx, num_plugin, rt_node_type_name((RT_NODE_TYPE)plugin->type), node->data);
        }
    }
    RT_LOGE("done\r\n");
    return RT_OK;
}

INT32 rt_node_bus_register_all(struct NodeBusContext *bus) {
    rt_node_bus_register(bus, &ff_node_parser);
    rt_node_bus_register(bus, &ff_node_parser);
    return RT_OK;
}

INT32 rt_node_bus_register(struct NodeBusContext *bus, RT_Node *node) {
    INT32 node_type = node->type;
    rt_hash_table_insert(bus->node_all, reinterpret_cast<void*>(node_type), node);
    return RT_OK;
}

RT_Node* rt_node_bus_find(struct NodeBusContext *bus, RT_NODE_TYPE node_type, UINT8 node_id) {
    void* data = rt_hash_table_find(bus->node_all,
                     reinterpret_cast<void *>(node_type));
    if (RT_NULL != data) {
        return reinterpret_cast<RT_Node*>(data);
    }
    return RT_NULL;
}
