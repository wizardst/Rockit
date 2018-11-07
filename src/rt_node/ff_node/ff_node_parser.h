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

#ifndef __FF_NODE_PARSER__
#define __FF_NODE_PARSER__

#include "rt_node.h"

#ifdef  __cplusplus
extern "C" {
#endif

INT8    node_ff_parser_init(void **ctx);
INT8 node_ff_parser_release(void **ctx);
INT8    node_ff_parser_pull(void *ctx, void *data, UINT32 *size);
INT8    node_ff_parser_push(void *ctx, void *data, UINT32 *size);
INT8 node_ff_parser_run_cmd(void* ctx, RT_NODE_CMD cmd,
                            UINT32 data_int, void* data_void);

extern RT_Node ff_node_parser;

#ifdef  __cplusplus
}
#endif

#endif // __FF_NODE_PARSER__
