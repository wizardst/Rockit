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

MACodecContext* ma_decode_create(RtMetaData *meta) {
    MPP_RET err = MPP_OK;
    MppCtx mpp_ctx = RT_NULL;
    MppApi *mpp_mpi = RT_NULL;
    MACodecContext *ctx = RT_NULL;
    MppBufferGroup frm_grp = RT_NULL;
    RtArrayList *rt_list = RT_NULL;
    MppCodingType mpp_coding_type;
    RTCodecID     type;

    CHECK_IS_NULL(meta);

    ctx = rt_malloc(MACodecContext);
    rt_memset(ctx, 0, sizeof(*ctx));

    err = mpp_create(&mpp_ctx, &mpp_mpi);
    CHECK_EQ(err, MPP_OK);

    // get parameters from metadata
    CHECK_EQ(meta->findInt32(kKeyCodecID, reinterpret_cast<INT32 *>(&type)), RT_TRUE);
    mpp_coding_type = fa_utils_find_mpp_codectype_from_rt(type);

    err = mpp_init(mpp_ctx, MPP_CTX_DEC, mpp_coding_type);
    CHECK_EQ(err, MPP_OK);

    {
        MppFrame frame = NULL;
        mpp_frame_init(&frame);
        mpp_frame_set_width(frame, 608);
        mpp_frame_set_height(frame, 1080);
        mpp_frame_set_fmt(frame, MPP_FMT_YUV420SP);
        mpp_frame_set_hor_stride(frame, 608);
        mpp_frame_set_ver_stride(frame, 1088);
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

    ctx->mpp_ctx = mpp_ctx;
    ctx->mpp_mpi = mpp_mpi;
    ctx->frm_grp = frm_grp;
    ctx->frm_list = rt_list;
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

void ma_decode_destroy(MACodecContext **mc) {
    ma_decode_reset(*mc);
    if ((*mc)->frm_grp) {
        mpp_buffer_group_put((*mc)->frm_grp);
        (*mc)->frm_grp = NULL;
    }
    if ((*mc)->mpp_ctx) {
        mpp_destroy((*mc)->mpp_ctx);
        (*mc)->mpp_ctx = RT_NULL;
    }
    if ((*mc)->frm_list) {
        array_list_destroy((*mc)->frm_list);
    }
    if (*mc) {
        rt_free(*mc);
        *mc = RT_NULL;
    }
}

void ma_decode_reset(MACodecContext *ctx) {
    if (ctx->frm_list) {
        INT32 i = 0;
        INT32 list_size = array_list_get_size(ctx->frm_list);
        for (i = 0; i < list_size; i++) {
            void *data = array_list_get_data(ctx->frm_list, i);
            rt_free(data);
        }
        array_list_remove_all(ctx->frm_list);
    }
}

RT_RET ma_commit_buffer_to_group(MACodecContext *ctx, RTMediaBuffer *buffer) {
    MPP_RET err = MPP_OK;
    RT_RET ret = RT_OK;
    MABufferMaping *ma_buffer = NULL;

    CHECK_IS_NULL(ctx);
    CHECK_IS_NULL(buffer);
    ma_buffer = rt_malloc(MABufferMaping);
    rt_memset(&(ma_buffer->mpp_buffer_info), 0, sizeof(MppBufferInfo));

    ma_buffer->mpp_buffer_info.type = MPP_BUFFER_TYPE_ION;
    ma_buffer->mpp_buffer_info.fd = buffer->getFd();
    ma_buffer->mpp_buffer_info.ptr = buffer->getData();
    ma_buffer->mpp_buffer_info.hnd = reinterpret_cast<void *>(buffer->getHandle());
    ma_buffer->mpp_buffer_info.size = buffer->getLength();
    ma_buffer->mpp_buffer_info.index = (buffer->getLength() & 0xf8000000) >> 27;

    mpp_buffer_commit(ctx->frm_grp, &(ma_buffer->mpp_buffer_info));

    ma_buffer->rt_media_buffer = buffer;
    array_list_add(ctx->frm_list, reinterpret_cast<void *>(ma_buffer));

 __FAILED:
    return ret;
}

RT_RET ma_fill_buffer_to_group(MACodecContext *ctx, RTMediaBuffer *buffer) {
    RT_RET ret = RT_OK;
    MPP_RET err = MPP_OK;
    INT32 i = 0;
    RT_BOOL found = RT_FALSE;
    MABufferMaping *ma_buffer = RT_NULL;

    CHECK_IS_NULL(ctx);
    CHECK_IS_NULL(buffer);

    // find mpp buffer from rockit media buffer
    for (i = 0; i < array_list_get_size(ctx->frm_list); i++) {
        ma_buffer = reinterpret_cast<MABufferMaping *>
                        (array_list_get_data(ctx->frm_list, i));
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

RT_RET ma_decode_send_packet(MACodecContext *ctx, RTMediaBuffer *packet) {
    MppPacket pkt = RT_NULL;
    INT32 eos = 0;
    RtMetaData *meta = RT_NULL;
    RT_RET ret = RT_OK;
    MPP_RET err = MPP_OK;
    INT64 pts = 0ll;

    CHECK_IS_NULL(ctx);
    CHECK_IS_NULL(packet);

    // init mpp packet
    mpp_packet_init(&pkt, packet->getData(), packet->getLength());
    meta = packet->getMetaData();
    if (meta->findInt32(kKeyPacketEOS, &eos)) {
        mpp_packet_set_eos(pkt);
    }
    if (meta->findInt64(kKeyPacketPts, &pts)) {
        mpp_packet_set_pts(pkt, pts);
    }
    mpp_packet_set_pos(pkt, packet->getData());
    mpp_packet_set_length(pkt, packet->getLength());

    err = ctx->mpp_mpi->decode_put_packet(ctx->mpp_ctx, pkt);
    if (MPP_OK != err) {
        ret = RT_ERR_LIST_FULL;
    }

__FAILED:
    return ret;
}

RT_RET ma_decode_get_frame(MACodecContext *ctx, RTMediaBuffer **frame) {
    MppFrame mpp_frame = NULL;
    RT_RET ret = RT_OK;
    MPP_RET err = MPP_OK;
    RtMetaData *meta = RT_NULL;

    CHECK_IS_NULL(ctx);
    CHECK_IS_NULL(frame);

    err = ctx->mpp_mpi->decode_get_frame(ctx->mpp_ctx, &mpp_frame);
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

            mpp_buffer_get(ctx->frm_grp, &buffer, 1);
            ctx->mpp_mpi->control(ctx->mpp_ctx, MPP_DEC_SET_EXT_BUF_GROUP, ctx->frm_grp);
            ctx->mpp_mpi->control(ctx->mpp_ctx, MPP_DEC_SET_INFO_CHANGE_READY, NULL);
        } else {
            buffer = mpp_frame_get_buffer(mpp_frame);
        }

        mpp_buffer_info_get(buffer, &info);
        // get media buffer from mpp buffer
        if (buffer) {
            INT32 i = 0;
            RT_BOOL found = RT_FALSE;
            MABufferMaping *ma_buffer = RT_NULL;
            for (i = 0; i < array_list_get_size(ctx->frm_list); i++) {
                ma_buffer = reinterpret_cast<MABufferMaping *>
                                (array_list_get_data(ctx->frm_list, i));
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
        frm_eos = mpp_frame_get_eos(mpp_frame);
        if (frm_eos) {
            meta->setInt32(kKeyFrameEOS, frm_eos);
        }

        meta->clear();
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


