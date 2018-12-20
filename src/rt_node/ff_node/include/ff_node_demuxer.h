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
 * Author: Hery.xu@rock-chips.com
 *   Date: 2018/12/10
 *   Task: use ffmpeg as demuxer and muxer
 */

#ifndef SRC_RT_NODE_FF_NODE_INCLUDE_FF_NODE_DEMUXER_H_
#define SRC_RT_NODE_FF_NODE_INCLUDE_FF_NODE_DEMUXER_H_

#include "rt_node.h" // NOLINT
#include "rt_media_frame.h" // NOLINT

#ifdef  __cplusplus
extern "C" {
#endif

RT_RET    node_ff_demuxer_init(void **ctx);
RT_RET    node_ff_demuxer_release(void **ctx);
RT_RET    node_ff_demuxer_pull(void *ctx, void* packet, UINT32 *size);
RT_RET    node_ff_demuxer_push(void *ctx, void* packet, UINT32 *size);
RT_RET    node_ff_demuxer_run_cmd(void* ctx, RT_NODE_CMD cmd, RtMetaData *metadata);

extern RT_Node ff_node_demuxer;

#ifdef  __cplusplus
}
#endif

#endif  // SRC_RT_NODE_FF_NODE_INCLUDE_FF_NODE_DEMUXER_H_
