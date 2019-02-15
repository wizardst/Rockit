/*
 * Copyright 2019 Rockchip Electronics Co. LTD
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
 * Author: rimon.xu@rock-chips.com
 *   Date: 2019/01/18
 */

#ifndef SRC_RT_MEDIA_MPI_INCLUDE_MPIADAPTERCODEC_H_
#define SRC_RT_MEDIA_MPI_INCLUDE_MPIADAPTERCODEC_H_

#include "FFAdapterUtils.h"     // NOLINT
#include "rt_header.h"          // NOLINT
#include "rk_mpi.h"             // NOLINT
#include "rt_array_list.h"      // NOLINT

struct MACodecContext {
    MppCtx              mpp_ctx;
    MppApi             *mpp_mpi;
    MppBufferGroup      frm_grp;
    RtArrayList        *frm_list;
};

class RtMetaData;
class RTMediaBuffer;

MACodecContext* ma_decode_create(RtMetaData *meta);
void ma_decode_destroy(MACodecContext **mc);
void ma_decode_reset(MACodecContext *ctx);

RT_RET ma_decode_send_packet(MACodecContext *ctx, RTMediaBuffer *packet);
RT_RET ma_decode_get_frame(MACodecContext *ctx, RTMediaBuffer **frame);

RT_RET ma_commit_buffer_to_group(MACodecContext *ctx, RTMediaBuffer *buffer);
RT_RET ma_fill_buffer_to_group(MACodecContext *ctx, RTMediaBuffer *buffer);

#endif  // SRC_RT_MEDIA_MPI_INCLUDE_MPIADAPTERCODEC_H_
