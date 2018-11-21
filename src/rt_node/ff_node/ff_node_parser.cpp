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
 *   Task: use ffmpeg as demuxer and muxer
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FFNodeParser"

#include "rt_node.h" // NOLINT
#include "ff_node_parser.h" // NOLINT
#include "rt_mem.h" // NOLINT

typedef struct _FF_PARSER_CONTEXT {
    UINT32 pull_cnt;
    UINT32 push_cnt;
} FF_PARSER_CONTEXT;

INT8 node_ff_parser_init(void **ctx) {
    *ctx = rt_malloc(FF_PARSER_CONTEXT);
    rt_memset(*ctx, 0, sizeof(FF_PARSER_CONTEXT));
    return RT_OK;
}

INT8 node_ff_parser_release(void **ctx) {
    rt_safe_free(*ctx);
    return RT_OK;
}

INT8 node_ff_parser_pull(void *ctx, void *data, UINT32 *size) {
    FF_PARSER_CONTEXT* parser_ctx = reinterpret_cast<FF_PARSER_CONTEXT*>(ctx);
    RT_ASSERT(RT_NULL != parser_ctx);
    parser_ctx->pull_cnt++;
    RT_LOGE("pull_cnt = %03d", parser_ctx->pull_cnt);
    return RT_OK;
}

INT8 node_ff_parser_push(void *ctx, void *data, UINT32 *size) {
    FF_PARSER_CONTEXT* parser_ctx = reinterpret_cast<FF_PARSER_CONTEXT*>(ctx);
    RT_ASSERT(RT_NULL != parser_ctx);
    parser_ctx->push_cnt++;
    RT_LOGE("push_cnt = %03d", parser_ctx->push_cnt);
    return RT_OK;
}

INT8 node_ff_parser_run_cmd(void* ctx, RT_NODE_CMD cmd,
                            UINT32 data_int, void* data_void) {
    FF_PARSER_CONTEXT* parser_ctx = reinterpret_cast<FF_PARSER_CONTEXT*>(ctx);
    RT_ASSERT(RT_NULL != parser_ctx);
    return RT_OK;
}

RT_Node ff_node_parser = {
    .type         = RT_NODE_TYPE_DEMUXER,
    .name         = "ff_demuxer",
    .version      = "v1.0",
    .node_ctx     = RT_NULL,
    .impl_init    = node_ff_parser_init,
    .impl_release = node_ff_parser_release,
    .impl_pull    = node_ff_parser_pull,
    .impl_push    = node_ff_parser_push,
    .impl_run_cmd = node_ff_parser_run_cmd,
};
