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

#ifndef __RT_NODE_H__
#define __RT_NODE_H__

#include "rt_header.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _RT_NODE_TYPE
{
    // ! basic nodes
    NODE_TYPE_BASE      = 2000,
    NODE_TYPE_EXTRACTOR,
    NODE_TYPE_DEMUXER,
    NODE_TYPE_MUXER,
    NODE_TYPE_DECODER,
    NODE_TYPE_ENCODER,
    NODE_TYPE_FILTER,
    NODE_TYPE_RENDER,
    NODE_TYPE_DEVICE,

    // !
    NODE_TYPE_MAX,
} RT_NODE_TYPE;

typedef struct rt_node_type_entry_t {
    UINT32      type;
    const char *name;
} rt_node_type_entry;

static const rt_node_type_entry node_type_entries[] = {
    { NODE_TYPE_EXTRACTOR, "Media Extractor" },
    { NODE_TYPE_DEMUXER,   "  Media Demuxer" },
    { NODE_TYPE_MUXER,     "    Media Muxer" },
    { NODE_TYPE_DECODER,   "  Media Decoder" },
    { NODE_TYPE_ENCODER,   "  Media Encoder" },
    { NODE_TYPE_FILTER,    "   Media Filter" },
    { NODE_TYPE_RENDER,    "   Media Render" },
    { NODE_TYPE_DEVICE,    "   Media Device" },
};

typedef enum _RT_NODE_CMD {
    RT_NODE_CMD_BASE  = 400,
    RT_NODE_CMD_INIT,
    RT_NODE_CMD_PREPARE,
    RT_NODE_CMD_START,
    RT_NODE_CMD_STOP,
    RT_NODE_CMD_CAPS_CHANGE,
    RT_NODE_CMD_SEEK,
    RT_NODE_CMD_RESET,
    RT_NODE_CMD_REINIT,
    RT_NODE_CMD_NAVIGATION,
    RT_NODE_CMD_DRAIN,

    // QOS and debug cmd
    RT_NODE_CMD_LATENCY,
    RT_NODE_CMD_STAT,
    RT_NODE_CMD_QOS,
    RT_NODE_CMD_DUMP,
} RT_NODE_CMD;

typedef enum _RT_NODE_MSG{
    RT_NODE_MSG_BASE = 500,
    RT_NODE_MSG_CAPS,
    RT_NODE_MSG_SEGMENT,
    RT_NODE_MSG_BUFFERSIZE,
    RT_NODE_MSG_EOS,
    RT_NODE_MSG_SINK,
} RT_NODE_MSG;

#define RT_NODE_CONTEXT void*

typedef struct _RT_Node {
    // public api
    INT8 init();
    INT8 release();
    INT8 pull(void *data, UINT32 *size);
    INT8 push(void *data, UINT32 *size);
    INT8 run_cmd(RT_NODE_CMD cmd, UINT32 data_int, void* data_void);

    // private
    const INT32 type;
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

#endif // __RT_NODE_H__
