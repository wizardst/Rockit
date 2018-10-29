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

#ifndef __RT_NODE_PARSER__
#define __RT_NODE_PARSER__

#include "rt_node.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct _RT_NODE_PARSER_CONTEXT {
   const char* name;
   void* sub_ctx;

    // parser operations
    INT8   (*fp_parser_pull)(void *sub_ctx, void *data, UINT32 *size);
    INT8   (*fp_parser_push)(void *sub_ctx, void *data, UINT32 *size);
    INT8   (*fp_parser_dump)(void *sub_ctx);
} RT_NODE_PARSER_CONTEXT;

INT8 node_parser_init(void **ctx);
INT8 node_parser_release(void **ctx);
INT8 node_parser_pull(void *ctx, void *data, UINT32 *size);
INT8 node_parser_push(void *ctx, void *data, UINT32 *size);
INT8 node_parser_dump(void *ctx);

extern RT_Node ff_node_parser;

#ifdef  __cplusplus
}
#endif

#endif // __RT_NODE_PARSER__

