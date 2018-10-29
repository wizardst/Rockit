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
    NODE_TYPE_UNKOWN    = -1,
    NODE_TYPE_EXTRACTOR = 0,
    NODE_TYPE_DEMUXER,
    NODE_TYPE_MUXER,
    NODE_TYPE_DECODER,
    NODE_TYPE_ENCODER,
    NODE_TYPE_FILTER,
    NODE_TYPE_RENDER,
    NODE_TYPE_DEVICE,
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

#define RT_NODE_CONTEXT void*

typedef struct _RT_Node {
    // public api
    RT_NODE_CONTEXT init() const;
    INT8 release(RT_NODE_CONTEXT ctx) const;
    INT8 read(RT_NODE_CONTEXT ctx, void *data,  UINT32 *size) const;
    INT8 write(RT_NODE_CONTEXT ctx, void *data, UINT32 *size) const;
    INT8 dump(RT_NODE_CONTEXT ctx) const;
    INT8 log_error(INT8 err, const char* func_name) const;

    // node context
    UINT8    type;
    char*    name;

    // private api
    INT8   (*impl_init   )(void **ctx);
    INT8   (*impl_release)(void **ctx);
    INT8   (*impl_read   )(void *ctx, void *data, UINT32 *size);
    INT8   (*impl_write  )(void *ctx, void *data, UINT32 *size);
    INT8   (*impl_dump   )(void *ctx);
    INT8   (*impl_subinit)(void *ctx);
} RT_Node;

#ifdef __cplusplus
}
#endif

#endif // __RT_NODE_H__
