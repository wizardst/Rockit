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
 * author: rimon.xu@rock-chips.com
 *   date: 20181031
 *
 * author: martin.cheng@rock-chips.com
 *   date: 20181107
 * update: replaced by universal hash table api.
 */

#include "rt_base_tests.h"
#include "rt_hash_table.h"

typedef struct _fake_hash_node {
    UINT32 key;
    UINT32 value;
} fake_hash_node;

RT_RET unit_test_hash_table(INT32 index, INT32 total_index)
{
    // ! Fake 1000 hash nodes
    fake_hash_node test_nodes[1000];
    rt_memset(test_nodes, 0, sizeof(fake_hash_node) * 1000);
    for (INT32 i = 0; i < 1000; i++) {
        test_nodes[i].key = i;
        test_nodes[i].value = 1000 - i;
    }

    RtHashTable *hash = rt_hash_table_init(100, hash_ptr_func, hash_ptr_compare);
    INT32 num_nodes = sizeof(test_nodes) / sizeof(test_nodes[0]);
    RT_LOGE("num_nodes = %d", num_nodes);
    for (INT32 i = 0; i < num_nodes; i++) {
        rt_hash_table_insert(hash, (void*)test_nodes[i].key, (void*)(&test_nodes[i]));
    }

    // rt_hash_table_dump(hash);

    fake_hash_node *hn = NULL;
    for (INT32 i = 0; i < num_nodes; i++) {
        hn = (fake_hash_node*)rt_hash_table_find(hash, (void*)test_nodes[i].key);
        #if 0
        RT_ASSERT(RT_NULL != st);
        RT_ASSERT(st->value == (1000-i));
        #else
        CHECK_UE(hn, RT_NULL);
        CHECK_EQ(hn->value, (1000-i));
        #endif
    }
    for (INT32 i = 0; i < num_nodes; i++) {
        rt_hash_table_remove (hash, (void*)test_nodes[i].key);
    }

    for (INT32 i = 0; i < num_nodes; i++) {
        fake_hash_node* hn = (fake_hash_node*)rt_hash_table_find(hash, (void*)test_nodes[i].key);
        #if 0
        RT_ASSERT(RT_NULL != hn);
        #else
        CHECK_EQ(hn, RT_NULL);
        #endif
    }

    rt_hash_table_dump(hash);
    rt_hash_table_destory(hash);
    hash = RT_NULL;

__RET:
    return RT_OK;
__FAILED:
    return RT_ERR_UNKNOWN;
}

