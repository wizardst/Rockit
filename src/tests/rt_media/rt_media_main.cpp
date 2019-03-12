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
 *   date: 2018/12/15
 */

#include "rt_media_tests.h" // NOLINT
#include "rt_check.h" // NOLINT

int main(int argc, char **argv) {
    RT_LOGE("rockit media test begin..\n");

    TestFunc *func = NULL;
    RtTestCtx *test_ctx =
        rt_tests_init(const_cast<char *>("Rockit-Media-Test"));
    CHECK_IS_NULL(test_ctx);

    /*
     * testcases: unit tests for node&nodebus 
     *    author: martin.cheng@rock-chips.com
     */
    #ifdef HAVE_MPI
    rt_tests_add(test_ctx,
                 unit_test_display_gles,
                 const_cast<char *>("UnitTest-Display-GLES"));

    rt_tests_add(test_ctx,
                 unit_test_allocator,
                 const_cast<char *>("UnitTest-Allocator"));
    #endif

    rt_tests_add(test_ctx,
                unit_test_mediabuffer_pool,
                const_cast<char *>("UnitTest-MediaBufferPool"));

    // ! run all testcases
    rt_tests_run(test_ctx, /*mem_dump=*/RT_TRUE);

    rt_tests_deinit(&test_ctx);

__RET:
    RT_LOGE("rockit media test end..\n");
    return RT_OK;
__FAILED:
    RT_LOGE("rockit media test end..\n");
    return RT_ERR_UNKNOWN;
}
