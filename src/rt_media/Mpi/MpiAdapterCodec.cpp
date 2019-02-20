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

#include <string.h>             // NOLINT
#include "MpiAdapterCodec.h"    // NOLINT
#include "RTMediaMetaKeys.h"    // NOLINT
#include "rt_metadata.h"        // NOLINT
#include "RTMediaBuffer.h"      // NOLINT
#include "MpiAdapterUtils.h"    // NOLINT
#include "mpp_err.h"            // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "MpiAdapterCodec"
#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x1

typedef struct {
    RTMediaBuffer       *rt_media_buffer;
    MppBufferInfo        mpp_buffer_info;
    MppBuffer            mpp_buffer;
} MABufferMaping;

MADecodeContext* ma_decode_create(RtMetaData *meta) {
    MPP_RET             err = MPP_OK;
    MppCtx              mpp_ctx = RT_NULL;
    MppApi             *mpp_mpi = RT_NULL;
    MADecodeContext    *ctx = RT_NULL;
    MppBufferGroup      frm_grp = RT_NULL;
    RtArrayList        *rt_list = RT_NULL;
    MppCodingType       mpp_coding_type;
    RTCodecID           type;

    CHECK_IS_NULL(meta);

    ctx = rt_malloc(MADecodeContext);
    rt_memset(ctx, 0, sizeof(*ctx));

    err = mpp_create(&mpp_ctx, &mpp_mpi);
    CHECK_EQ(err, MPP_OK);

    // get parameters from metadata
    CHECK_EQ(meta->findInt32(kKeyCodecID, reinterpret_cast<INT32 *>(&type)), RT_TRUE);
    mpp_coding_type = (MppCodingType)ma_rt_to_mpp_codec_id(type);
    if (mpp_coding_type == MPP_VIDEO_CodingUnused) {
        RT_LOGE("unsupport rockit codec id: 0x%x", type);
        goto __FAILED;
    }
    INT32 width, height;
    CHECK_EQ(meta->findInt32(kKeyVCodecWidth,    &width), RT_TRUE);
    CHECK_EQ(meta->findInt32(kKeyVCodecHeight,    &height), RT_TRUE);

    err = mpp_init(mpp_ctx, MPP_CTX_DEC, mpp_coding_type);
    CHECK_EQ(err, MPP_OK);

    {
        MppFrame frame = NULL;
        mpp_frame_init(&frame);
        mpp_frame_set_width(frame, width);
        mpp_frame_set_height(frame, height);
        mpp_frame_set_fmt(frame, MPP_FMT_YUV420SP);
        mpp_frame_set_hor_stride(frame, width);
        mpp_frame_set_ver_stride(frame, height);
        mpp_mpi->control(mpp_ctx, MPP_DEC_SET_FRAME_INFO, (MppParam)frame);
        mpp_frame_deinit(&frame);
    }
    // init frame grp
    err = mpp_buffer_group_get_external(&frm_grp, MPP_BUFFER_TYPE_ION);
    CHECK_EQ(err, MPP_OK);

    // TODO(control cmds)
    mpp_mpi->control(mpp_ctx, MPP_DEC_SET_EXT_BUF_GROUP, frm_grp);

    rt_list = array_list_create();
    CHECK_IS_NULL(rt_list);

    ctx->codec_ctx.mpp_ctx = mpp_ctx;
    ctx->codec_ctx.mpp_mpi = mpp_mpi;
    ctx->codec_ctx.frm_grp = frm_grp;
    ctx->codec_ctx.frm_list = rt_list;
    return ctx;

__FAILED:
    if (rt_list) {
        array_list_destroy(rt_list);
        rt_list = RT_NULL;
    }
    if (mpp_ctx) {
        mpp_destroy(mpp_ctx);
        mpp_ctx = RT_NULL;
    }
    if (ctx) {
        rt_free(ctx);
        ctx = RT_NULL;
    }
    return RT_NULL;
}

void ma_decode_destroy(MADecodeContext **mc) {
    ma_decode_reset(*mc);
    if ((*mc)->codec_ctx.frm_grp) {
        mpp_buffer_group_put((*mc)->codec_ctx.frm_grp);
        (*mc)->codec_ctx.frm_grp = NULL;
    }
    if ((*mc)->codec_ctx.mpp_ctx) {
        mpp_destroy((*mc)->codec_ctx.mpp_ctx);
        (*mc)->codec_ctx.mpp_ctx = RT_NULL;
    }
    if ((*mc)->codec_ctx.frm_list) {
        array_list_destroy((*mc)->codec_ctx.frm_list);
    }
    if (*mc) {
        rt_free(*mc);
        *mc = RT_NULL;
    }
}

void ma_decode_reset(MADecodeContext *ctx) {
    if (ctx->codec_ctx.frm_list) {
        INT32 i = 0;
        INT32 list_size = array_list_get_size(ctx->codec_ctx.frm_list);
        for (i = 0; i < list_size; i++) {
            void *data = array_list_get_data(ctx->codec_ctx.frm_list, i);
            rt_free(data);
        }
        array_list_remove_all(ctx->codec_ctx.frm_list);
    }
}

RT_RET ma_commit_buffer_to_group(MACodecContext codec_ctx, RTMediaBuffer *buffer) {
    MPP_RET err = MPP_OK;
    RT_RET ret = RT_OK;
    MABufferMaping *ma_buffer = NULL;

    CHECK_IS_NULL(buffer);
    ma_buffer = rt_malloc(MABufferMaping);
    rt_memset(&(ma_buffer->mpp_buffer_info), 0, sizeof(MppBufferInfo));

    ma_buffer->mpp_buffer_info.type = MPP_BUFFER_TYPE_ION;
    ma_buffer->mpp_buffer_info.fd = buffer->getFd();
    ma_buffer->mpp_buffer_info.ptr = buffer->getData();
    ma_buffer->mpp_buffer_info.hnd = reinterpret_cast<void *>(buffer->getHandle());
    ma_buffer->mpp_buffer_info.size = buffer->getLength();
    ma_buffer->mpp_buffer_info.index = (buffer->getLength() & 0xf8000000) >> 27;

    mpp_buffer_commit(codec_ctx.frm_grp, &(ma_buffer->mpp_buffer_info));

    ma_buffer->rt_media_buffer = buffer;
    array_list_add(codec_ctx.frm_list, reinterpret_cast<void *>(ma_buffer));

 __FAILED:
    return ret;
}

RT_RET ma_fill_buffer_to_group(MACodecContext codec_ctx, RTMediaBuffer *buffer) {
    RT_RET ret = RT_OK;
    MPP_RET err = MPP_OK;
    INT32 i = 0;
    RT_BOOL found = RT_FALSE;
    MABufferMaping *ma_buffer = RT_NULL;

    CHECK_IS_NULL(buffer);

    // find mpp buffer from rockit media buffer
    for (i = 0; i < array_list_get_size(codec_ctx.frm_list); i++) {
        ma_buffer = reinterpret_cast<MABufferMaping *>
                        (array_list_get_data(codec_ctx.frm_list, i));
        if (ma_buffer->rt_media_buffer == buffer) {
            found = RT_TRUE;
            break;
        }
    }
    if (found) {
        mpp_buffer_put(ma_buffer->mpp_buffer);
    } else {
        RT_LOGE("not found matching mpp buffer from rockit media buffer");
        ret = RT_ERR_UNKNOWN;
        goto __FAILED;
    }

__FAILED:
    return ret;
}

RT_RET ma_decode_send_packet(MADecodeContext *ctx, RTMediaBuffer *packet) {
    MppPacket               pkt = RT_NULL;
    INT32                   eos = 0;
    RtMetaData             *meta = RT_NULL;
    RT_RET                  ret = RT_OK;
    MPP_RET                 err = MPP_OK;
    INT64                   pts = 0ll;
    MppCtx                  mpp_ctx = RT_NULL;
    MppApi                 *mpp_mpi = RT_NULL;

    CHECK_IS_NULL(ctx);
    CHECK_IS_NULL(packet);
    mpp_ctx = ctx->codec_ctx.mpp_ctx;
    mpp_mpi = ctx->codec_ctx.mpp_mpi;

    // init mpp packet
    mpp_packet_init(&pkt, packet->getData(), packet->getLength());
    meta = packet->getMetaData();
    if (meta->findInt32(kKeyPacketEOS, &eos)) {
        mpp_packet_set_eos(pkt);
    }
    if (meta->findInt64(kKeyPacketPts, &pts)) {
        mpp_packet_set_pts(pkt, pts);
    }
    INT32 is_extradata;
    if (meta->findInt32(kKeyPacketIsExtra, &is_extradata)) {
        mpp_packet_set_extra_data(pkt);
    }
    mpp_packet_set_pos(pkt, packet->getData());
    mpp_packet_set_length(pkt, packet->getLength());

    err = mpp_mpi->decode_put_packet(mpp_ctx, pkt);
    if (MPP_OK != err) {
        ret = RT_ERR_LIST_FULL;
    }

__FAILED:
    return ret;
}

RT_RET ma_decode_get_frame(MADecodeContext *ctx, RTMediaBuffer **frame) {
    MppFrame                mpp_frame = NULL;
    RT_RET                  ret = RT_OK;
    MPP_RET                 err = MPP_OK;
    RtMetaData             *meta = RT_NULL;
    MppCtx                  mpp_ctx = RT_NULL;
    MppApi                 *mpp_mpi = RT_NULL;
    MppBufferGroup          frm_grp;
    RtArrayList            *frm_list;

    CHECK_IS_NULL(ctx);
    CHECK_IS_NULL(frame);
    mpp_ctx = ctx->codec_ctx.mpp_ctx;
    mpp_mpi = ctx->codec_ctx.mpp_mpi;
    frm_grp = ctx->codec_ctx.frm_grp;
    frm_list = ctx->codec_ctx.frm_list;

    err = mpp_mpi->decode_get_frame(mpp_ctx, &mpp_frame);
    if (MPP_OK != err) {
        RT_LOGD("mpp decode get frame failed! ret: %d", err);
        ret = RT_ERR_UNKNOWN;
        goto __FAILED;
    }

    if (mpp_frame) {
        INT32 frm_eos = 0;
        MppBuffer buffer = RT_NULL;
        MppBufferInfo info;
        if (mpp_frame_get_info_change(mpp_frame)) {
            RK_U32 width = mpp_frame_get_width(mpp_frame);
            RK_U32 height = mpp_frame_get_height(mpp_frame);
            RK_U32 hor_stride = mpp_frame_get_hor_stride(mpp_frame);
            RK_U32 ver_stride = mpp_frame_get_ver_stride(mpp_frame);

            RT_LOGD("decode_get_frame get info changed found\n");
            RT_LOGD("decoder require buffer w:h [%d:%d] stride [%d:%d]",
                        width, height, hor_stride, ver_stride);

            mpp_buffer_get(frm_grp, &buffer, 1);
            mpp_mpi->control(mpp_ctx, MPP_DEC_SET_EXT_BUF_GROUP, frm_grp);
            mpp_mpi->control(mpp_ctx, MPP_DEC_SET_INFO_CHANGE_READY, NULL);
        } else if (mpp_frame_get_eos(mpp_frame)) {
            mpp_buffer_get(frm_grp, &buffer, 1);
        } else {
            buffer = mpp_frame_get_buffer(mpp_frame);
        }

        mpp_buffer_info_get(buffer, &info);
        // get media buffer from mpp buffer
        if (buffer) {
            INT32 i = 0;
            RT_BOOL found = RT_FALSE;
            MABufferMaping *ma_buffer = RT_NULL;
            for (i = 0; i < array_list_get_size(frm_list); i++) {
                ma_buffer = reinterpret_cast<MABufferMaping *>
                                (array_list_get_data(frm_list, i));
                if (ma_buffer->mpp_buffer_info.fd == info.fd
                        && ma_buffer->mpp_buffer_info.hnd == info.hnd
                        && ma_buffer->mpp_buffer_info.ptr == info.ptr) {
                    found = RT_TRUE;
                    if (ma_buffer->mpp_buffer == NULL) {
                        ma_buffer->mpp_buffer = buffer;
                    }
                    break;
                }
            }
            if (found) {
                *frame = ma_buffer->rt_media_buffer;
            } else {
                RT_LOGE("not found matching rockit media buffer from mpp buffer");
                ret = RT_ERR_UNKNOWN;
                goto __FAILED;
            }
        } else {
            ret = RT_ERR_UNKNOWN;
            goto __FAILED;
        }
        meta = (*frame)->getMetaData();
        meta->clear();
        frm_eos = mpp_frame_get_eos(mpp_frame);
        if (frm_eos) {
            meta->setInt32(kKeyFrameEOS, frm_eos);
        }
        meta->setInt32(kKeyVCodecWidth,  mpp_frame_get_hor_stride(mpp_frame));
        meta->setInt32(kKeyVCodecHeight, mpp_frame_get_ver_stride(mpp_frame));
        meta->setInt32(kKeyFrameW,       mpp_frame_get_width(mpp_frame));
        meta->setInt32(kKeyFrameH,       mpp_frame_get_height(mpp_frame));
        meta->setInt64(kKeyFramePts,     mpp_frame_get_pts(mpp_frame));
        (*frame)->setRange(0, mpp_frame_get_hor_stride(mpp_frame) * mpp_frame_get_ver_stride(mpp_frame) * 3 / 2);

        RT_LOGD("get frame frame width: %d, frame height: %d, width: %d, height: %d timestamps: %lld us",
                     mpp_frame_get_hor_stride(mpp_frame),
                     mpp_frame_get_ver_stride(mpp_frame),
                     mpp_frame_get_width(mpp_frame),
                     mpp_frame_get_height(mpp_frame),
                     mpp_frame_get_pts(mpp_frame));
        if (mpp_frame_get_info_change(mpp_frame)) {
            (*frame)->setStatus(RT_MEDIA_BUFFER_STATUS_INFO_CHANGE);
        } else {
            (*frame)->setStatus(RT_MEDIA_BUFFER_STATUS_READY);
        }
    } else {
        ret = RT_ERR_UNKNOWN;
    }

__FAILED:
    return ret;
}

RT_RET ma_encode_config(MAEncodeContext *ctx, RtMetaData *meta) {
    RT_RET ret = RT_OK;
    MPP_RET err = MPP_OK;
    MppApi *mpp_mpi;
    MppCtx mpp_ctx;
    MppEncCodecCfg *codec_cfg;
    MppEncPrepCfg *prep_cfg;
    MppEncRcCfg *rc_cfg;

    CHECK_IS_NULL(meta);

    mpp_mpi = ctx->codec_ctx.mpp_mpi;
    mpp_ctx = ctx->codec_ctx.mpp_ctx;
    codec_cfg = &ctx->codec_cfg;
    prep_cfg = &ctx->prep_cfg;
    rc_cfg = &ctx->rc_cfg;

    /* setup default parameter */
    {
        INT32 frame_width;
        if (!meta->findInt32(kKeyFrameW, &frame_width)) {
            RT_LOGE("frame width not be set!");
            ret = RT_ERR_VALUE;
            return ret;
        }
        INT32 frame_height;
        if (!meta->findInt32(kKeyFrameH, &frame_height)) {
            RT_LOGE("frame height not be set!");
            ret = RT_ERR_VALUE;
            return ret;
        }
        INT32 codec_width;
        if (!meta->findInt32(kKeyVCodecWidth, &codec_width)) {
            RT_LOGE("codec width not be set!");
            ret = RT_ERR_VALUE;
            return ret;
        }
        INT32 codec_height;
        if (!meta->findInt32(kKeyVCodecHeight, &codec_height)) {
            RT_LOGE("codec height not be set!");
            ret = RT_ERR_VALUE;
            return ret;
        }
        RtVideoFormat format;
        if (!meta->findInt32(kKeyCodecFormat, reinterpret_cast<INT32 *>(&format))) {
            format = RT_FMT_YUV420SP;
        }

        prep_cfg->change        = MPP_ENC_PREP_CFG_CHANGE_INPUT |
                                  MPP_ENC_PREP_CFG_CHANGE_ROTATION |
                                  MPP_ENC_PREP_CFG_CHANGE_FORMAT;
        prep_cfg->width         = frame_width;
        prep_cfg->height        = frame_height;
        prep_cfg->hor_stride    = codec_width;
        prep_cfg->ver_stride    = codec_height;
        prep_cfg->format        = (MppFrameFormat)ma_rt_to_mpp_color_format(format);
        prep_cfg->rotation      = MPP_ENC_ROT_0;
        err = mpp_mpi->control(mpp_ctx, MPP_ENC_SET_PREP_CFG, prep_cfg);
        CHECK_EQ(err, MPP_OK);
    }

    /* rate control config */
    {
        RtVideoRCMode rc_mode;
        if (!meta->findInt32(kKeyVCodecRCMode, reinterpret_cast<INT32 *>(&rc_mode))) {
            rc_mode = RT_RC_MODE_CBR;
        }
        INT32 frame_rate;
        if (!meta->findInt32(kKeyVCodecFrameRate, &frame_rate)) {
            frame_rate = 30;
        }
        INT32 bit_rate;
        if (!meta->findInt32(kKeyCodecBitrate, &bit_rate)) {
            bit_rate = prep_cfg->width * prep_cfg->height / 8 * frame_rate;
        }
        INT32 gop;
        if (!meta->findInt32(kKeyVCodecGopSize, &gop)) {
            gop = 60;
        }

        if (rc_mode == RT_RC_MODE_CBR) {
            /* constant bitrate has very small bps range of 1/16 bps */
            rc_cfg->bps_target   = bit_rate;
            rc_cfg->bps_max      = bit_rate * 17 / 16;
            rc_cfg->bps_min      = bit_rate * 15 / 16;
        } else if (rc_mode ==  RT_RC_MODE_CQP) {
            /* constant QP does not have bps */
            rc_cfg->bps_target   = -1;
            rc_cfg->bps_max      = -1;
            rc_cfg->bps_min      = -1;
        } else {
            /* variable bitrate has large bps range */
            rc_cfg->bps_target   = bit_rate;
            rc_cfg->bps_max      = bit_rate * 17 / 16;
            rc_cfg->bps_min      = bit_rate * 1 / 16;
        }
        rc_cfg->change  = MPP_ENC_RC_CFG_CHANGE_ALL;
        rc_cfg->rc_mode = (MppEncRcMode)ma_rt_to_mpp_rc_mode(rc_mode);
        rc_cfg->quality = (rc_mode == RT_RC_MODE_CQP) ?
                              MPP_ENC_RC_QUALITY_CQP :
                              MPP_ENC_RC_QUALITY_MEDIUM;
        rc_cfg->fps_in_flex      = 0;
        rc_cfg->fps_in_num       = frame_rate;
        rc_cfg->fps_in_denorm    = 1;
        rc_cfg->fps_out_flex     = 0;
        rc_cfg->fps_out_num      = frame_rate;
        rc_cfg->fps_out_denorm   = 1;
        rc_cfg->gop              = gop;
        rc_cfg->skip_cnt         = 0;
        err = mpp_mpi->control(mpp_ctx, MPP_ENC_SET_RC_CFG, rc_cfg);
        CHECK_EQ(err, MPP_OK);
    }

    /* codec config */
    {
        RTCodecID codec_id;
        if (!meta->findInt32(kKeyCodecID, reinterpret_cast<INT32 *>(&codec_id))) {
            codec_id = RT_VIDEO_ID_AVC;
        }
        INT32 qp;
        if (!meta->findInt32(kKeyVCodecQP, &qp)) {
            qp = (codec_id == RT_VIDEO_ID_AVC) ? (26) :
                 (codec_id == RT_VIDEO_ID_MJPEG) ? (10) :
                 (codec_id == RT_VIDEO_ID_VP8) ? (56) :
                 (codec_id == RT_VIDEO_ID_HEVC) ? (26) : (0);
        }
        codec_cfg->coding = (MppCodingType)ma_rt_to_mpp_codec_id(codec_id);
        switch (codec_cfg->coding) {
        case MPP_VIDEO_CodingAVC : {
            codec_cfg->h264.change = MPP_ENC_H264_CFG_CHANGE_PROFILE |
                                     MPP_ENC_H264_CFG_CHANGE_ENTROPY |
                                     MPP_ENC_H264_CFG_CHANGE_TRANS_8x8 |
                                     MPP_ENC_H264_CFG_CHANGE_QP_LIMIT;
            /*
             * H.264 profile_idc parameter
             * 66  - Baseline profile
             * 77  - Main profile
             * 100 - High profile
             */
            codec_cfg->h264.profile  = 100;
            /*
             * H.264 level_idc parameter
             * 10 / 11 / 12 / 13    - qcif@15fps / cif@7.5fps / cif@15fps / cif@30fps
             * 20 / 21 / 22         - cif@30fps / half-D1@@25fps / D1@12.5fps
             * 30 / 31 / 32         - D1@25fps / 720p@30fps / 720p@60fps
             * 40 / 41 / 42         - 1080p@30fps / 1080p@30fps / 1080p@60fps
             * 50 / 51 / 52         - 4K@30fps
             */
            codec_cfg->h264.level    = 40;
            codec_cfg->h264.entropy_coding_mode  = 1;
            codec_cfg->h264.cabac_init_idc  = 0;
            codec_cfg->h264.transform8x8_mode = 1;

            if (rc_cfg->rc_mode == MPP_ENC_RC_MODE_CBR) {
                /* constant bitrate do not limit qp range */
                codec_cfg->h264.qp_max              = 48;
                codec_cfg->h264.qp_min              = 4;
                codec_cfg->h264.qp_max_step         = 16;
                codec_cfg->h264.qp_init             = qp;
            } else if (rc_cfg->rc_mode == MPP_ENC_RC_MODE_VBR) {
                if (rc_cfg->quality == MPP_ENC_RC_QUALITY_CQP) {
                    /* constant QP mode qp is fixed */
                    codec_cfg->h264.qp_max          = 26;
                    codec_cfg->h264.qp_min          = 26;
                    codec_cfg->h264.qp_max_step     = 0;
                } else {
                    /* variable bitrate has qp min limit */
                    codec_cfg->h264.qp_max          = 40;
                    codec_cfg->h264.qp_min          = 12;
                    codec_cfg->h264.qp_max_step     = 8;
                    codec_cfg->h264.qp_init         = 0;
                }
            }
        } break;
        case MPP_VIDEO_CodingMJPEG : {
            codec_cfg->jpeg.change  = MPP_ENC_JPEG_CFG_CHANGE_QP;
            codec_cfg->jpeg.quant   = qp;
        } break;
        case MPP_VIDEO_CodingVP8 : {
        } break;
        case MPP_VIDEO_CodingHEVC : {
            codec_cfg->h265.change = MPP_ENC_H265_CFG_INTRA_QP_CHANGE;
            codec_cfg->h265.intra_qp = qp;
        } break;
        default : {
            RT_LOGE("support encoder coding type %d\n", codec_cfg->coding);
        } break;
        }
        err = mpp_mpi->control(mpp_ctx, MPP_ENC_SET_CODEC_CFG, codec_cfg);
        CHECK_EQ(err, MPP_OK);
    }

    /* sei config */
    {
        MppEncSeiMode sei_mode = MPP_ENC_SEI_MODE_ONE_FRAME;
        err = mpp_mpi->control(mpp_ctx, MPP_ENC_SET_SEI_CFG, &sei_mode);
        CHECK_EQ(err, MPP_OK);
    }

__FAILED:
    return ret;
}

MAEncodeContext* ma_encode_create(RtMetaData *meta) {
    MAEncodeContext    *ctx = RT_NULL;
    MPP_RET             err = MPP_OK;
    MppCtx              mpp_ctx = RT_NULL;
    MppApi             *mpp_mpi = RT_NULL;
    MppBufferGroup      frm_grp = RT_NULL;
    RtArrayList        *frm_list = RT_NULL;
    MppCodingType       mpp_coding_type;
    RTCodecID           type;

    CHECK_IS_NULL(meta);

    ctx = rt_malloc(MAEncodeContext);
    rt_memset(ctx, 0, sizeof(*ctx));

    err = mpp_create(&mpp_ctx, &mpp_mpi);
    CHECK_EQ(err, MPP_OK);

    CHECK_EQ(meta->findInt32(kKeyCodecID, reinterpret_cast<INT32 *>(&type)), RT_TRUE);
    mpp_coding_type = (MppCodingType)ma_rt_to_mpp_codec_id(type);
    err = mpp_init(mpp_ctx, MPP_CTX_ENC, mpp_coding_type);
    CHECK_EQ(err, MPP_OK);

    // init frame grp
    err = mpp_buffer_group_get_external(&frm_grp, MPP_BUFFER_TYPE_ION);
    CHECK_EQ(err, MPP_OK);

    ctx->codec_ctx.mpp_ctx = mpp_ctx;
    ctx->codec_ctx.mpp_mpi = mpp_mpi;
    ctx->codec_ctx.frm_grp = frm_grp;
    ctx->codec_ctx.frm_list = frm_list;
    ctx->is_first_frame = RT_TRUE;
    return ctx;
__FAILED:
    if (ctx) {
        rt_free(ctx);
    }
    return RT_NULL;
}

RT_RET ma_encode_prepare(MAEncodeContext *ctx, RtMetaData *meta) {
    RT_RET              ret = RT_OK;
    MPP_RET             err = MPP_OK;
    MppCtx              mpp_ctx;
    MppApi             *mpp_mpi;
    MppPacket           packet = NULL;

    CHECK_IS_NULL(ctx);
    CHECK_IS_NULL(meta);

    ret = ma_encode_config(ctx, meta);
    CHECK_EQ(ret, RT_OK);

    mpp_ctx = ctx->codec_ctx.mpp_ctx;
    mpp_mpi = ctx->codec_ctx.mpp_mpi;
    err = mpp_mpi->control(mpp_ctx, MPP_ENC_GET_EXTRA_INFO, &packet);
    CHECK_EQ(err, MPP_OK);

    /* get and write sps/pps for H.264 */
    if (packet) {
        ctx->extradata_size = mpp_packet_get_length(packet);
        if (ctx->extradata_size > 0) {
            void *src = mpp_packet_get_data(packet);
            ctx->extra_data = rt_malloc_size(UINT8, ctx->extradata_size + 1);
            rt_memcpy(ctx->extra_data, src, ctx->extradata_size);
            RT_LOGD("get extra data: %p size: %d", ctx->extra_data, ctx->extradata_size);
        }
        packet = RT_NULL;
    }

__FAILED:
    return ret;
}

RT_RET ma_encode_send_frame(MAEncodeContext *ctx, RTMediaBuffer *frame) {
    RT_RET              ret = RT_OK;
    MPP_RET             err = MPP_OK;
    MppCtx              mpp_ctx;
    MppApi             *mpp_mpi;

    CHECK_IS_NULL(ctx);
    CHECK_IS_NULL(frame);
    /* init mpp frame */
    err = mpp_frame_init(&ctx->mpp_frame);
    CHECK_EQ(err, MPP_OK);
    mpp_frame_set_width(ctx->mpp_frame, ctx->prep_cfg.width);
    mpp_frame_set_height(ctx->mpp_frame, ctx->prep_cfg.height);
    mpp_frame_set_hor_stride(ctx->mpp_frame, ctx->prep_cfg.hor_stride);
    mpp_frame_set_ver_stride(ctx->mpp_frame, ctx->prep_cfg.ver_stride);
    mpp_frame_set_fmt(ctx->mpp_frame, ctx->prep_cfg.format);

    mpp_ctx = ctx->codec_ctx.mpp_ctx;
    mpp_mpi = ctx->codec_ctx.mpp_mpi;
    CHECK_IS_NULL(ctx->mpp_frame);

    if (frame->getFd() >= 0) {
        MppBufferInfo buffer_info;
        MppBuffer buffer;
        rt_memset(&buffer_info, 0, sizeof(buffer_info));
        buffer_info.type = MPP_BUFFER_TYPE_ION;
        buffer_info.size = frame->getLength();
        buffer_info.fd = frame->getFd();

        err = mpp_buffer_import(&buffer, &buffer_info);
        CHECK_EQ(err, MPP_OK);

        mpp_frame_set_buffer(ctx->mpp_frame, buffer);
        mpp_buffer_put(buffer);
        buffer = NULL;
    } else {
        RT_LOGE("unsupported none-dma buffer!");
        goto __FAILED;
    }

    err = mpp_mpi->encode_put_frame(mpp_ctx, ctx->mpp_frame);
    if (err) {
        RT_LOGE("encode_put_frame ret %d\n", err);
    }

__FAILED:
    return ret;
}

RT_RET ma_encode_get_packet(MAEncodeContext *ctx, RTMediaBuffer *packet) {
    RT_RET              ret = RT_OK;
    MPP_RET             err = MPP_OK;
    MppCtx              mpp_ctx;
    MppApi             *mpp_mpi;
    MppBuffer           mpp_packet;

    CHECK_IS_NULL(ctx);
    CHECK_IS_NULL(packet);

    mpp_ctx = ctx->codec_ctx.mpp_ctx;
    mpp_mpi = ctx->codec_ctx.mpp_mpi;

    err = mpp_mpi->encode_get_packet(mpp_ctx, &mpp_packet);
    if (err) {
        RT_LOGE("encode_get_packet failed ret %d\n", err);
        goto __FAILED;
    }

    if (mpp_packet) {
        UINT8 *src = reinterpret_cast<UINT8 *>(mpp_packet_get_data(mpp_packet));
        UINT8 *dst = reinterpret_cast<UINT8 *>(packet->getData());
        UINT32 eos = mpp_packet_get_eos(mpp_packet);
        INT64  pts = mpp_packet_get_pts(mpp_packet);
        UINT32 flag = mpp_packet_get_flag(mpp_packet);
        UINT32 length = mpp_packet_get_length(mpp_packet);
        if (packet->getSize() > length + ctx->extradata_size) {
            if (ctx->is_first_frame) {
                rt_memcpy(dst, ctx->extra_data, ctx->extradata_size);
                rt_memcpy(dst + ctx->extradata_size, src, length);
                packet->setRange(0, length + ctx->extradata_size);
                ctx->is_first_frame = RT_FALSE;
            } else {
                rt_memcpy(dst, src, length);
                packet->setRange(0, length);
            }
            packet->setStatus(RT_MEDIA_BUFFER_STATUS_READY);
        } else {
            RT_LOGE("packet size is not enough");
        }
    }

__FAILED:
    return ret;
}

void ma_encode_destroy(MAEncodeContext **ctx) {
    if ((*ctx)->extra_data) {
        rt_free((*ctx)->extra_data);
        (*ctx)->extra_data = RT_NULL;
    }
    if ((*ctx)->codec_ctx.mpp_ctx) {
        mpp_destroy((*ctx)->codec_ctx.mpp_ctx);
        (*ctx)->codec_ctx.mpp_ctx = RT_NULL;
    }
    if (*ctx) {
        rt_free(*ctx);
        *ctx = RT_NULL;
    }
}


