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
 * author: martin.cheng
 *   date: 20180803
 *   Task: node_parsers for <muxer and demuxer>
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FFNodeParser"

#include "rt_node.h"
#include "rt_node_parser.h"
#include "rt_mem.h"

typedef struct _FF_PARSER_SUB_CONTEXT {
    UINT32 pull_cnt;
    UINT32 push_cnt;
} FF_PARSER_SUB_CONTEXT;

INT8   ff_parser_pull(void *sub_ctx, void *data, UINT32 *size) {
    FF_PARSER_SUB_CONTEXT* ff_sub_ctx = (FF_PARSER_SUB_CONTEXT*)sub_ctx;
    RT_ASSERT(RT_NULL != ff_sub_ctx);
    ff_sub_ctx->pull_cnt++;
    RT_LOGT("pull_cnt = %03d", ff_sub_ctx->pull_cnt);
    return RT_ERR_UNKNOWN;
}

INT8   ff_parser_push(void *sub_ctx, void *data, UINT32 *size) {
    FF_PARSER_SUB_CONTEXT* ff_sub_ctx = (FF_PARSER_SUB_CONTEXT*)sub_ctx;
    RT_ASSERT(RT_NULL != ff_sub_ctx);
    ff_sub_ctx->push_cnt++;
    RT_LOGT("push_cnt = %03d", ff_sub_ctx->push_cnt);
    return RT_ERR_UNKNOWN;
}

INT8   ff_parser_dump(void *sub_ctx) {
    return RT_ERR_UNKNOWN;
}

/*
      Task: impliment callback
 * Prototype: INT8 (*impl_subinit)(void *ctx);
 */
INT8 ff_node_parser_subinit(void *ctx) {
    RT_NODE_PARSER_CONTEXT* parser_ctx = (RT_NODE_PARSER_CONTEXT*)(ctx);
    RT_ASSERT(parser_ctx);
    parser_ctx->name           = "<muxer/demuxer>";
    parser_ctx->sub_ctx        = rt_malloc(FF_PARSER_SUB_CONTEXT);
    rt_memset(parser_ctx->sub_ctx, 0, sizeof(FF_PARSER_SUB_CONTEXT));
    // install callbacks
    parser_ctx->fp_parser_pull = ff_parser_pull;
    parser_ctx->fp_parser_push = ff_parser_push;
    parser_ctx->fp_parser_dump = ff_parser_dump;

    return RT_ERR_UNKNOWN;
}

 RT_Node ff_node_parser = {
     .type         = NODE_TYPE_DEMUXER,
     .name         = (char*)"node_parser",
     .impl_init    = node_parser_init,
     .impl_release = node_parser_release,
     .impl_read    = node_parser_pull,
     .impl_write   = node_parser_push,
     .impl_dump    = node_parser_dump,
     .impl_subinit = ff_node_parser_subinit
 };
