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

#ifndef __TEST_BASE_MEMORY_H__
#define __TEST_BASE_MEMORY_H__

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "test_base_memory"

#include "rt_header.h"
#include "rt_base_tests.h"
#include "rt_mem_service.h"
#define add_node_test 1
#define remove_node_test 0

typedef struct _person{
    int   age;
    char *name;
} Person;

RT_RET unit_test_memory(INT32 index, INT32 total_index){
    UINT32 idx = 0, case_idx=0, count=0;
    for(idx = 0; idx < 100; idx++) {
        Person* martin = RT_NULL;
        case_idx = idx%3;
        switch(case_idx) {
        case 0:
            martin = (Person*)rt_mem_malloc(__FUNCTION__, sizeof(Person)); //test pass
            break;
        case 1:
            martin = (Person*)rt_mem_calloc(__FUNCTION__, sizeof(Person));//test pass
            break;
        case 2:
            martin = (Person*)rt_mem_realloc(__FUNCTION__, martin, sizeof(Person));//test pass
            break;
        default:
            break;
        }
        if(RT_NULL != martin){
            martin->age  = idx;
            martin->name = (char*)"martin";
            rt_mem_free(__FUNCTION__, martin);
            martin = RT_NULL;
            count++;
        }else{
            break;
        }
    }
    RT_LOGE("Success:%02d, Testcase:%02d", count, idx);
    if(idx == 100) {
        return RT_OK;
    }
    return RT_ERR_MALLOC;
}

RT_RET unit_test_mem_service(INT32 index, INT32 total_index) {
    RT_LOGE("Enter ...");
    UINT32 idx ;
    rt_mem_service * mem_record = new rt_mem_service();
    mem_record->reset();

    RT_LOGE("Case: memory leakage ...");
    for(idx = 0; idx < 10; idx++) {
        Person* prince = RT_NULL;
        prince = (Person*)rt_mem_malloc(__FUNCTION__, sizeof(Person));
        prince->age  = idx;
        prince->name = (char*)"martin";
        mem_record->add_node(__FUNCTION__,prince,sizeof(prince));
    }
    mem_record->dump();

    RT_LOGE("Case: find mem node, then remove ...");
    MemNode *node = mem_record->mem_nodes;
    UINT32  *node_size;
    while(RT_NULL != node) {
        if(RT_NULL != node->ptr) {
            UINT32 score = mem_record->find_node(__FUNCTION__, node->ptr, node_size);
            mem_record->remove_node(node->ptr, node_size);
            rt_mem_free(__FUNCTION__, node->ptr);
            node++;
        }else{
            break;
        }
    }
    mem_record->dump();
    RT_LOGE("Done ...");
}

#endif //__TEST_BASE_REBUST_H__
