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
 * author: princejay.dai@rock-chips.com
 *   date: 2018/11/07
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "test_base_memory"

#include "rt_header.h" // NOLINT
#include "rt_base_tests.h" // NOLINT
#include "RTMemService.h" // NOLINT

typedef struct _person {
    int   age;
    char *name;
} Person;

RT_RET unit_test_memory(INT32 index, INT32 total_index) {
    UINT32 idx = 0, case_idx = 0, count = 0;
    for (idx = 0; idx < 100; idx++) {
        Person* martin = RT_NULL;
        case_idx = idx % 3;
        switch (case_idx) {
        case 0:
            martin = rt_malloc(Person);  // test pass
            break;
        case 1:
            martin = rt_calloc(Person, 1);  // test pass
            break;
        case 2:
            martin = rt_realloc(martin, Person, 1);  // test pass
            break;
        default:
            break;
        }
        if (RT_NULL != martin) {
            martin->age  = idx;
            martin->name = const_cast<char *>("martin");
            rt_free(martin);
            martin = RT_NULL;
            count++;
        } else {
            break;
        }
    }
    RT_LOGE("Success:%02d, Testcase:%02d", count, idx);
    if (idx == 100) {
        return RT_OK;
    }
    return RT_ERR_MALLOC;
}

RT_RET unit_test_mem_service(INT32 index, INT32 total_index) {
    RT_LOGE("Enter ...");
    UINT32 idx = 0;
    RTMemService * mem_record = new RTMemService();
    mem_record->reset();

    RT_LOGE("Case: memory leakage ...");
    for (idx = 0; idx < 10; idx++) {
        Person* prince = RT_NULL;
        prince = rt_malloc(Person);
        prince->age  = idx;
        prince->name = const_cast<char*>("prince");
        mem_record->addNode(__FUNCTION__, prince, sizeof(prince));
    }
    mem_record->dump();

    RT_LOGE("Case: find mem node, then remove ...");
    MemNode *node = mem_record->mem_nodes;
    UINT32  node_size;
    for (UINT32 i = 0; i < mem_record->nodes_max; i++, node++) {
        if ((RT_NULL != node->ptr) && (0 != node->size)) {
            mem_record->findNode(__FUNCTION__, node->ptr, &node_size);
            mem_record->removeNode(node->ptr, &node_size);
        } else {
            break;
        }
    }
    mem_record->dump();
    delete mem_record;
    mem_record = RT_NULL;
    RT_LOGE("Done ...");
    return RT_OK;
}
