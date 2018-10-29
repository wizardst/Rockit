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
 *   date: 2018/07/05
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTNodeParser"

#include "rt_node.h"
#include "rt_node_parser.h"
#include "rt_mem.h"

INT8 node_parser_init(void **ctx) {
    RT_NODE_PARSER_CONTEXT* parser_ctx = rt_malloc(RT_NODE_PARSER_CONTEXT);
    *ctx = parser_ctx;
    return RT_ERR_UNKNOWN;
}

INT8 node_parser_release(void **ctx) {
    RT_NODE_PARSER_CONTEXT* parser_ctx = (RT_NODE_PARSER_CONTEXT*)(*ctx);
    if (RT_NULL != parser_ctx) {
        if(RT_NULL != parser_ctx->sub_ctx) {
            rt_free(parser_ctx->sub_ctx);
            parser_ctx->sub_ctx = RT_NULL;
        }
        rt_free(parser_ctx);
        parser_ctx = RT_NULL;
        *ctx       = RT_NULL;
    }
    return RT_ERR_UNKNOWN;
}

INT8 node_parser_pull(void *ctx, void *data, UINT32 *size) {
    RT_NODE_PARSER_CONTEXT* parser_ctx = (RT_NODE_PARSER_CONTEXT*)(ctx);
    if ( RT_NULL != parser_ctx ) {
        if(RT_NULL != parser_ctx->fp_parser_pull) {
            parser_ctx->fp_parser_pull(parser_ctx->sub_ctx, data, size);
        }
    }
    return RT_ERR_UNKNOWN;
}


INT8 node_parser_push(void *ctx, void *data, UINT32 *size) {
    RT_NODE_PARSER_CONTEXT* parser_ctx = (RT_NODE_PARSER_CONTEXT*)(ctx);
    if ( RT_NULL != parser_ctx ) {
        if(RT_NULL != parser_ctx->fp_parser_push) {
            parser_ctx->fp_parser_push(parser_ctx->sub_ctx, data, size);
        }
    }
    return RT_ERR_UNKNOWN;
}

INT8 node_parser_dump(void *ctx) {
    RT_NODE_PARSER_CONTEXT* parser_ctx = (RT_NODE_PARSER_CONTEXT*)(ctx);
    if ( RT_NULL != parser_ctx ) {
        if(RT_NULL != parser_ctx->fp_parser_dump) {
            parser_ctx->fp_parser_dump(parser_ctx->sub_ctx);
        }
    }
    return RT_ERR_UNKNOWN;
}
