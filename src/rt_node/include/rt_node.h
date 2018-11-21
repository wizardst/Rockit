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
 *   date: 20180704
 */

 /*
  *    node         node_codec      ff_node_codec
  *      |  init <ctx>  |                |
  *      |              |    init <ctx>  |
  *      |              |                | init <ctx>
  *      |  send <pkt>  |                |
  *      |              |   send <pkt>   |
  *      | recv <frame> |                |
  *      |              |  recv <frame>  |
  *      |   release    |                |
  *      |              |    release     |
  */

#ifndef SRC_RT_NODE_INCLUDE_RT_NODE_H_
#define SRC_RT_NODE_INCLUDE_RT_NODE_H_

#include "rt_header.h" // NOLINT
#include "rt_node_define.h" // NOLINT

#ifdef __cplusplus
extern "C" {
#endif


#define RT_NODE_CONTEXT void*

typedef struct _RT_Node {
    // public api
    INT8 init();
    INT8 release();
    INT8 pull(void *data, UINT32 *size);
    INT8 push(void *data, UINT32 *size);
    INT8 run_cmd(RT_NODE_CMD cmd, UINT32 data_int, void* data_void);

    // private
    const RT_NODE_TYPE type;
    const char* name;
    const char* version;
    void* node_ctx;

    // private api
    INT8   (*impl_init   )(void** ctx);
    INT8   (*impl_release)(void** ctx);
    INT8   (*impl_pull   )(void* ctx, void *data, UINT32 *size);
    INT8   (*impl_push   )(void* ctx, void *data, UINT32 *size);
    INT8   (*impl_run_cmd)(void* ctx, RT_NODE_CMD cmd, UINT32 data_int, void* data_void);
} RT_Node;

INT8 check_err(const RT_Node node, INT8 err, const char* func_name);

#ifdef __cplusplus
}
#endif

#endif  // SRC_RT_NODE_INCLUDE_RT_NODE_H_
