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
 *   date: 20181102
 */

#include "rt_base_tests.h" // NOLINT
#include "rt_check.h" // NOLINT

int main(int argc, char **argv) {
    RT_LOGE("rockit base test begin..\n");

    TestFunc *func = NULL;
    RtTestCtx *test_ctx = rt_tests_init(const_cast<char *>("RockitBaseTest"));
    CHECK_IS_NULL(test_ctx);

    /*
     * testcases: standard containers which store a collection of other objects 
     *    author: rimon.xu@rock-chips.com
     */
    rt_tests_add(test_ctx, unit_test_array_list, const_cast<char *>("UnitTest-ArrayList"));
    rt_tests_add(test_ctx, unit_test_deque_normal, const_cast<char *>("UnitTest-DequeNormal"));
    rt_tests_add(test_ctx, unit_test_deque_limit, const_cast<char *>("UnitTest-DequeLimit"));
    rt_tests_add(test_ctx, unit_test_linked_list, const_cast<char *>("UnitTest-LinkedList"));
    rt_tests_add(test_ctx, unit_test_hash_table, const_cast<char *>("UnitTest-HashTable"));
    rt_tests_add(test_ctx, unit_test_metadata, const_cast<char *>("UnitTest-MetaData"));

    /*
     * testcases: unit tests for OS Adaptive Layer(OSAL) 
     *    author: rimon.xu@rock-chips.com
     */
    rt_tests_add(test_ctx, unit_test_memory, const_cast<char *>("UnitTest-Memory"));
    rt_tests_add(test_ctx, unit_test_mem_service, const_cast<char *>("UnitTest-Mem-Service"));

    rt_tests_add(test_ctx, unit_test_mutex, const_cast<char *>("UnitTest-Mutex"));
    rt_tests_add(test_ctx, unit_test_thread, const_cast<char *>("UnitTest-Thread"));

    rt_tests_add(test_ctx, unit_test_lock_unlock, const_cast<char *>("UnitTest-Lock-Unlock"));
    rt_tests_add(test_ctx, unit_test_cond_lock, const_cast<char *>("UnitTest-Cond-Lock"));

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
