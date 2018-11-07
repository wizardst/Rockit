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
 *   date: 2018/07/05
 */

#ifndef __TEST_BASE_MEMORY_H__
#define __TEST_BASE_MEMORY_H__

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "test_base_memory"

#include "rt_header.h"
#include "rt_base_tests.h"

typedef struct _person{
    int   age;
    char *name;
} Person;

RT_RET unit_test_memory(INT32 index, INT32 total_index){
    UINT32 idx = 0;
    for(idx = 0; idx < 100; idx++) {
        Person* martin = RT_NULL;
        martin = (Person*)rt_mem_malloc(__FUNCTION__, sizeof(Person));
        if(RT_NULL != martin){
            martin->age  = idx;
            martin->name = (char*)"martin";
            rt_mem_free(__FUNCTION__, martin);
            martin = RT_NULL;
        }else{
            break;
        }
    }
    if(idx == 100) {
        return RT_OK;
    }
    return RT_ERR_MALLOC;
}

#endif //__TEST_BASE_MEMORY_H__
