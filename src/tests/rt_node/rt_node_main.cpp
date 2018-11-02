/*
 * Copyright 2018 The Rockit Open Source Project
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
 *   date: 2018/11/05
 */

#include "rt_node_tests.h"
#include "rt_check.h"

int main(int argc, char **argv)
{
    RT_LOGE("rockit base test begin..\n");

    TestFunc *func = NULL;
    RtTestCtx *test_ctx = rt_tests_init((char *)"Rockit-Node-Test");
    CHECK_IS_NULL(test_ctx);

    /*
     * testcases: unit tests for node&nodebus 
     *    author: martin.cheng@rock-chips.com
     */
    rt_tests_add(test_ctx, unit_test_node_basic,     (char *)"UnitTest-NodeBasic");
    rt_tests_add(test_ctx, unit_test_node_bus,       (char *)"UnitTest-NodeBus");
    rt_tests_add(test_ctx, unit_test_node_data_flow, (char *)"UnitTest-Dataflow");

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
