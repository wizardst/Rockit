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
 *   date: 2018/07/04
 */

#ifndef __RT_NODE_BASIC_H__
#define __RT_NODE_BASIC_H__

#include "rt_node.h"
#include "../include/rt_base_header.h"
#include "../include/rt_node_header.h"

#ifdef  __cplusplus
extern "C" {
#endif

INT8 unit_test_demuxer() {
    RT_LOGE("%s ptr = %p", __FUNCTION__, &ff_node_parser);
    RT_Node* demuxer =  &ff_node_parser;
    RT_NODE_CONTEXT node_ctx =  demuxer->init();

    void *data   = RT_NULL;
    UINT32 *size = 0;
    demuxer->read(node_ctx, data, size);
    demuxer->write(node_ctx,data, size);
    demuxer->dump(node_ctx);
    demuxer->release(node_ctx);
    return RT_OK;
}

INT8 unit_test_muxer() {
    RT_LOGE("%s ptr = %p", __FUNCTION__, &ff_node_parser);
    RT_Node* muxer =  &ff_node_parser;
    RT_NODE_CONTEXT node_ctx = muxer->init();

    void *data   = RT_NULL;
    UINT32 *size = 0;
    muxer->read(node_ctx, data, size);
    muxer->write(node_ctx, data, size);
    muxer->dump(node_ctx);
    muxer->release(&node_ctx);
    return RT_OK;
}

#ifdef  __cplusplus
}
#endif

#endif /*__RT_NODE_BASIC_H__*/

