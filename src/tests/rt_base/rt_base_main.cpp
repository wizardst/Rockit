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
 * author: rimon.xu
 *   date: 20181102
 * e-mail: rimon.xu@rock-chips.com
 */

#include "rt_base_tests.h"
#include "rt_check.h"

int main(int argc, char **argv)
{
    RT_LOGE("rockit base test begin..\n");

    TestFunc *func = NULL;
    RtTestCtx *test_ctx = rt_tests_init((char *)"RockitBaseTest");
    CHECK_IS_NULL(test_ctx);

    /*
     * testcases: standard containers which store a collection of other objects 
     *    author: rimon.xu@rock-chips.com
     */
    rt_tests_add(test_ctx, unit_test_array_list,   (char *)"UnitTest-ArrayList");
    rt_tests_add(test_ctx, unit_test_deque_normal, (char *)"UnitTest-DequeNormal");
    rt_tests_add(test_ctx, unit_test_deque_limit,  (char *)"UnitTest-DequeLimit");
    rt_tests_add(test_ctx, unit_test_linked_list,  (char *)"UnitTest-LinkedList");
    rt_tests_add(test_ctx, unit_test_hash_table,   (char *)"UnitTest-HashTable");

    /*
     * testcases: unit tests for OS Adaptive Layer(OSAL) 
     *    author: rimon.xu@rock-chips.com
     */
    rt_tests_add(test_ctx, unit_test_memory, (char *)"UnitTest-Memory");
    rt_tests_add(test_ctx, unit_test_mutex,  (char *)"UnitTest-Mutex");
    rt_tests_add(test_ctx, unit_test_thread, (char *)"UnitTest-Thread");

    // ! run all testcases
    rt_tests_run(test_ctx, /*mem_dump=*/RT_TRUE);

    rt_tests_deinit(&test_ctx);

__RET:
    RT_LOGE("rockit base test end..\n");
    return RT_OK;
__FAILED:
    RT_LOGE("rockit base test end..\n");
    return RT_ERR_UNKNOWN;
}
