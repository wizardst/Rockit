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

struct MADecodeContext {
    MACodecContext      codec_ctx;
};

struct MAEncodeContext {
    MACodecContext      codec_ctx;
    MppEncCodecCfg      codec_cfg;
    MppEncPrepCfg       prep_cfg;
    MppEncRcCfg         rc_cfg;

    void               *extra_data;
    UINT32              extradata_size;
    MppFrame            mpp_frame;
    RT_BOOL             is_first_frame;
};

class RtMetaData;
class RTMediaBuffer;

MADecodeContext *ma_decode_create(RtMetaData *meta);
void ma_decode_destroy(MADecodeContext **mc);
void ma_decode_reset(MADecodeContext *ctx);
RT_RET ma_decode_send_packet(MADecodeContext *ctx, RTMediaBuffer *packet);
RT_RET ma_decode_get_frame(MADecodeContext *ctx, RTMediaBuffer **frame);

RT_RET ma_commit_buffer_to_group(MACodecContext codec_ctx, RTMediaBuffer *buffer);
RT_RET ma_fill_buffer_to_group(MACodecContext codec_ctx, RTMediaBuffer *buffer);

MAEncodeContext *ma_encode_create(RtMetaData *meta);
void ma_encode_destroy(MAEncodeContext **ctx);
void ma_encode_reset(MAEncodeContext *ctx);
RT_RET ma_encode_send_frame(MAEncodeContext *ctx, RTMediaBuffer *frame);
RT_RET ma_encode_get_packet(MAEncodeContext *ctx, RTMediaBuffer *packet);
RT_RET ma_encode_config(MAEncodeContext *ctx, RtMetaData *meta);
RT_RET ma_encode_prepare(MAEncodeContext * ctx, RtMetaData *meta);

#endif  // SRC_RT_MEDIA_MPI_INCLUDE_MPIADAPTERCODEC_H_
