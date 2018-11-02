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
 * author: rimon.xu
 *   date: 20181102
 * e-mail: rimon.xu@rock-chips.com
 */

#include "rt_test_header.h"
#include "rt_check.h"
#include "rt_mem.h"

static RT_RET rt_test_impl(UINT32 index,
                           UINT32 total_index,
                           TestFunc func,
                           char *name)
{
    CHECK_IS_NULL(name);
    PRINT_TEST_BEGIN(index, total_index, name);

    CHECK_IS_NULL(func);
    FUNC_CHECK(func(index, total_index));

__RET:
    PRINT_TEST_END(index, total_index, name, RT_OK);
    return RT_OK;
__FAILED:
    PRINT_TEST_END(index, total_index, name, RT_ERR_UNKNOWN);
    return RT_ERR_UNKNOWN;
}

RtTestCtx *rt_tests_init(char *name)
{
    RtTestCtx *ctx = rt_malloc(RtTestCtx);
    CHECK_IS_NULL(ctx);
    memset(ctx, 0, sizeof(RtTestCtx));

    /* init params */
    CHECK_IS_NULL(name);
    strcpy(ctx->name, name);
    ctx->list = array_list_create();
    CHECK_IS_NULL(ctx->list);

__RET:
    return ctx;
__FAILED:
    return RT_NULL;
}

RT_RET rt_tests_run(RtTestCtx *ctx, RT_BOOL mem_dump)
{
    RT_RET ret = RT_OK;
    int total_count = 0;

    CHECK_IS_NULL(ctx);

    total_count = array_list_get_size(ctx->list);

    for (int i = 0; i < total_count; i++) {
        RtTestNode *node = NULL;
        char full_name[256];
        memset(full_name, 0, 256);
        node = (RtTestNode *)array_list_get_data(ctx->list, 0);
        CHECK_IS_NULL(node);

        sprintf(full_name, "%s#%s", ctx->name, node->name);
        UINT64 start = RtTime::getNowTimeUs();
        if(RT_TRUE == mem_dump) {
            rt_mem_record_reset();
            rt_test_impl(i, total_count, node->func, full_name);
            rt_mem_record_dump();
        } else {
            rt_test_impl(i, total_count, node->func, full_name);
        }
        UINT64 end = RtTime::getNowTimeUs();
        UINT64 space_time = end - start;
        RT_LOGE("test space time: %lld.%lld ms\n\n", space_time / 1000, space_time % 1000);
        CHECK_EQ(array_list_remove_at(ctx->list, 0),  RT_OK);
        rt_free(node);
        node = NULL;
    }

__RET:
    return RT_OK;
__FAILED:
    return RT_ERR_UNKNOWN;
}

RT_RET rt_tests_add(RtTestCtx *ctx, TestFunc func, char *name)
{
    RT_RET ret = RT_OK;
    RtTestNode *node = NULL;

    CHECK_IS_NULL(ctx);
    CHECK_IS_NULL(func);
    CHECK_IS_NULL(name);

    node = rt_malloc(RtTestNode);
    CHECK_IS_NULL(node);
    node->func = func;
    strcpy(node->name, name);

    ret = (RT_RET)array_list_add(ctx->list, (void *)node);
    ctx->count = array_list_get_size(ctx->list);

__RET:
    return ret;
__FAILED:
    return RT_ERR_UNKNOWN;
}

RT_RET rt_tests_deinit(RtTestCtx **ctx)

{
    RT_RET ret = RT_OK;

    CHECK_IS_NULL(ctx);
    CHECK_IS_NULL(*ctx);

    array_list_destroy(&((*ctx)->list));

__RET:
    rt_free(*ctx);
    *ctx = NULL;
    return ret;
__FAILED:
    rt_free(*ctx);
    *ctx = NULL;
    return RT_ERR_UNKNOWN;
}
