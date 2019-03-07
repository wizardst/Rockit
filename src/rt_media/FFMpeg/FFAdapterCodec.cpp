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
 * Author: martin.cheng@rock-chips.com
 *   Date: 2019/01/03
 */

#include <string.h>          // NOLINT
#include "FFAdapterCodec.h"  // NOLINT
#include "FFAdapterUtils.h"  // NOLINT
#include "RTMediaMetaKeys.h" // NOLINT
#include "RTMediaDef.h"      // NOLINT
#include "RTMediaBuffer.h"   // NOLINT
#include "rt_metadata.h"     // NOLINT
#include "rt_mem.h"          // NOLINT
#include "rt_log.h"          // NOLINT
#include "rt_common.h"       // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FFAdapterCodec"
#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x1

FACodecContext* fa_video_decode_create(RtMetaData *meta) {
    INT32 err = 0;
    AVCodecContext *codec_ctx = NULL;
    FACodecContext *ctx = rt_malloc(FACodecContext);
    ctx->mTrackType = RTTRACK_TYPE_VIDEO;
    ctx->mAvCodecCtx = avcodec_alloc_context3(NULL);
    CHECK_IS_NULL(ctx->mAvCodecCtx);
    codec_ctx = ctx->mAvCodecCtx;

    // necessary parameters
    RTCodecID codecID;
    CHECK_EQ(meta->findInt32(kKeyCodecID, reinterpret_cast<INT32 *>(&codecID)), RT_TRUE);

    INT32 width, height;
    CHECK_EQ(meta->findInt32(kKeyVCodecWidth,  &width), RT_TRUE);
    CHECK_EQ(meta->findInt32(kKeyVCodecHeight, &height), RT_TRUE);

    // non necessary parameters
    RT_PTR extra_data;
    INT32  extra_size;
    if (!meta->findPointer(kKeyCodecExtraData, &extra_data)) {
        extra_data = NULL;
    }
    if (!meta->findInt32(kKeyCodecExtraSize, &extra_size)) {
        extra_size = 0;
    }
    RT_LOGE("Codec Extra(ptr=0x%p, size=%d)", extra_data, extra_size);

    // codec context parameters configure
    codec_ctx->codec_type = AVMEDIA_TYPE_AUDIO;
    codec_ctx->codec_id   = (AVCodecID)fa_utils_to_av_codec_id(codecID);
    codec_ctx->extradata_size = extra_size;
    codec_ctx->extradata = reinterpret_cast<UINT8 *>(extra_data);
    codec_ctx->width  = width;
    codec_ctx->height = height;

    // find decoder again as codec_id may have changed
    codec_ctx->codec = avcodec_find_decoder(codec_ctx->codec_id);
    if (NULL == codec_ctx->codec) {
        RT_LOGE("Fail to find decoder(%s)", avcodec_get_name(codec_ctx->codec_id));
        goto __FAILED;
    }

    RT_LOGD("Try to create decoder(%s)", avcodec_get_name(codec_ctx->codec_id));
    err = avcodec_open2(codec_ctx, codec_ctx->codec, NULL);
    if (err < 0) {
        RT_LOGE("Fail to create decoder(%s) err=%d", \
                 avcodec_get_name(codec_ctx->codec_id), err);
        goto __FAILED;
    }
    RT_LOGD("Success to open ffmpeg decoder(%s)!", avcodec_get_name(codec_ctx->codec_id));

    return ctx;

__FAILED:
    if (ctx && ctx->mAvCodecCtx) {
        avcodec_free_context(&ctx->mAvCodecCtx);
        ctx->mAvCodecCtx = NULL;
    }
    if (ctx) {
        rt_free(ctx);
        ctx = NULL;
    }
    return NULL;
}

FACodecContext* fa_audio_decode_create(RtMetaData *meta) {
    INT32 err = 0;
    // AVCodecContext *codec_ctx = NULL;
    FACodecContext *ctx = rt_malloc(FACodecContext);
    rt_memset(ctx, 0, sizeof(FACodecContext));
    ctx->mTrackType     = RTTRACK_TYPE_AUDIO;
    ctx->mSwrCtx        = RT_NULL;
    ctx->mAvCodecCtx    = RT_NULL;
    // codec_ctx = ctx->mAvCodecCtx;

    // necessary parameters
    INT32 rt_codec_id;
    meta->findInt32(kKeyCodecID, reinterpret_cast<INT32 *>(&rt_codec_id));
    AVCodecID av_codec_id = (AVCodecID)fa_utils_to_av_codec_id(rt_codec_id);

    // find decoder again as codec_id may have changed
    AVCodec* audio_codec = avcodec_find_decoder(av_codec_id);
    if (NULL == audio_codec) {
        RT_LOGE("Fail to avcodec_find_decoder for %s", avcodec_get_name(av_codec_id));
        goto __FAILED;
    }

    ctx->mAvCodecCtx = avcodec_alloc_context3(audio_codec);
    if (RT_NULL == ctx->mAvCodecCtx) {
        RT_LOGE("Fail to avcodec_alloc_context3 for %s", avcodec_get_name(av_codec_id));
        goto __FAILED;
    }
    ctx->mAvCodecCtx->codec_id = av_codec_id;

    err = fa_init_audio_params_from_metadata(ctx, meta);
    if (err < 0) {
        RT_LOGE("failed to init audio params from metadata");
        goto __FAILED;
    }

    err = avcodec_open2(ctx->mAvCodecCtx, audio_codec, NULL);
    if (fa_utils_check_error(err, "avcodec_open2") < 0) {
        goto __FAILED;
    }
    RT_LOGD("Success to open ffmpeg decoder(%s)!", avcodec_get_name(av_codec_id));

    ctx->mAudioSrc.fmt = AV_SAMPLE_FMT_S16;

    return ctx;

__FAILED:
    if (ctx && ctx->mAvCodecCtx) {
        avcodec_free_context(&ctx->mAvCodecCtx);
        ctx->mAvCodecCtx = NULL;
    }
    if (ctx) {
        rt_free(ctx);
        ctx = NULL;
    }
    return NULL;
}

FACodecContext* fa_video_encode_create(RtMetaData *meta) {
    INT32 err = 0;
    AVCodecContext *codec_ctx = RT_NULL;
    const AVCodec  *codec     = RT_NULL;
    FACodecContext *ctx       = rt_malloc(FACodecContext);
    ctx->mAvCodecCtx          = RT_NULL;

    // necessary parameters
    RTCodecID codecID   = RT_VIDEO_ID_Unused;
    INT32     avCodecID = 0;
    CHECK_EQ(meta->findInt32(kKeyCodecID, reinterpret_cast<INT32 *>(&codecID)), RT_TRUE);

    INT32 width, height;
    CHECK_EQ(meta->findInt32(kKeyVCodecWidth,  &width), RT_TRUE);
    CHECK_EQ(meta->findInt32(kKeyVCodecHeight, &height), RT_TRUE);

    // non necessary parameters
    INT32 bitrate;
    if (!meta->findInt32(kKeyCodecBitrate, &bitrate)) {
        bitrate = 2000000;
    }

    INT32 framerate;
    if (!meta->findInt32(kKeyVCodecFrameRate, &framerate)) {
        framerate = 30;
    }

    INT32 gop_size;
    if (!meta->findInt32(kKeyVCodecGopSize, &gop_size)) {
        gop_size = 10;
    }

    INT32 max_b_frames;
    if (!meta->findInt32(kKeyVCodecMaxBFrames, &max_b_frames)) {
        max_b_frames = 1;
    }

    // find encoder again as codec_id may have changed
    avCodecID = fa_utils_to_av_codec_id(codecID);
    codec = avcodec_find_encoder((AVCodecID)avCodecID);
    if (NULL == codec) {
        RT_LOGE("Fail to create encoder(%s)", fa_utils_codec_name(avCodecID));
        goto __FAILED;
    }

    ctx->mAvCodecCtx = avcodec_alloc_context3(codec);
    CHECK_IS_NULL(ctx->mAvCodecCtx);
    codec_ctx = ctx->mAvCodecCtx;

    // codec context parameters configure
    codec_ctx->width = width;
    codec_ctx->height = height;
    codec_ctx->bit_rate = bitrate;
    /* frames per second */
    codec_ctx->time_base = (AVRational){1, framerate};
    codec_ctx->framerate = (AVRational){framerate, 1};

    /* emit one intra frame every ten frames
     * check frame pict_type before passing frame
     * to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    codec_ctx->gop_size = gop_size;
    codec_ctx->max_b_frames = max_b_frames;
    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

    RT_LOGD("begin to open ffmpeg encoder(%s) now", avcodec_get_name(codec_ctx->codec_id));
    err = avcodec_open2(codec_ctx, codec_ctx->codec, NULL);
    if (err < 0) {
        RT_LOGE("ffmpeg video encoder failed to initialize. (%d)", (err));
        goto __FAILED;
    }
    RT_LOGD("open ffmpeg video encoder(%s) success", avcodec_get_name(codec_ctx->codec_id));

    return ctx;

__FAILED:
    if (ctx && ctx->mAvCodecCtx) {
        avcodec_free_context(&ctx->mAvCodecCtx);
        ctx->mAvCodecCtx = NULL;
    }
    if (ctx) {
        rt_free(ctx);
        ctx = NULL;
    }
    return NULL;
}

FACodecContext *fa_decode_create(RtMetaData *meta, RTTrackType type) {
    FACodecContext *ctx = RT_NULL;
    CHECK_IS_NULL(meta);

    switch (type) {
    case RTTRACK_TYPE_VIDEO:
        ctx = fa_video_decode_create(meta);
        break;
    case RTTRACK_TYPE_AUDIO:
        ctx = fa_audio_decode_create(meta);
        break;
    default:
        RT_LOGE("not support track type: %d\n", type);
        break;
    }

    return ctx;
__FAILED:
    return RT_NULL;
}

void fa_video_decode_destroy(FACodecContext **fc) {
    if (*fc && (*fc)->mAvCodecCtx) {
        avcodec_free_context(&((*fc)->mAvCodecCtx));
    }

    if (*fc) {
        rt_free(*fc);
        *fc = NULL;
    }
}

void fa_video_encode_destroy(FACodecContext **fc) {
    if (*fc && (*fc)->mAvCodecCtx) {
        avcodec_free_context(&((*fc)->mAvCodecCtx));
    }

    if (*fc) {
        rt_free(*fc);
        *fc = NULL;
    }
}


static RT_RET fa_init_av_packet(AVPacket *pkt, RTMediaBuffer *buffer) {
    RtMetaData *meta = buffer->getMetaData();
    rt_memset(pkt, 0, sizeof(AVPacket));
    av_init_packet(pkt);

    if (buffer) {
        pkt->data = reinterpret_cast<UINT8 *>(buffer->getData());
        pkt->size = buffer->getSize();
        meta->findInt64(kKeyPacketPts, &pkt->pts);
        meta->findInt64(kKeyPacketDts, &pkt->dts);
    } else {
        // empty packet. like eos.
        pkt->data = NULL;
        pkt->size = 0;
        pkt->pts = AV_NOPTS_VALUE;
        pkt->dts = AV_NOPTS_VALUE;
    }

    RT_LOGD("pkt->data: %p, size: %d pts: %lld", pkt->data, pkt->size, pkt->pts);

    return RT_OK;
}

static RT_RET fa_init_av_frame(FACodecContext* fc, AVFrame **frame, RTMediaBuffer *buffer) {
    // RtMetaData *meta = buffer->getMetaData();
    UINT8 *data = reinterpret_cast<UINT8 *>(buffer->getData());
    AVFrame *tmp_frame = RT_NULL;
    tmp_frame = av_frame_alloc();
    tmp_frame->format = fc->mAvCodecCtx->pix_fmt;
    tmp_frame->width  = fc->mAvCodecCtx->width;
    tmp_frame->height = fc->mAvCodecCtx->height;

    /* Y */
    tmp_frame->data[0] = data;
    tmp_frame->linesize[0] = fc->mAvCodecCtx->width;

    /* U */
    tmp_frame->data[1] = data + fc->mAvCodecCtx->width * fc->mAvCodecCtx->height;
    tmp_frame->linesize[1] = fc->mAvCodecCtx->width / 2;

    /* V */
    tmp_frame->data[2] = data + fc->mAvCodecCtx->width * fc->mAvCodecCtx->height * 5 / 4;
    tmp_frame->linesize[2] = fc->mAvCodecCtx->width / 2;

    *frame = tmp_frame;

    return RT_OK;
}

RT_RET fa_decode_send_packet(FACodecContext* fc, RTMediaBuffer *buffer) {
    if (!fc && buffer) {
        RT_LOGE("fc or pkt is NULL: fc: %p pkt: %p", fc, buffer);
        return RT_ERR_VALUE;
    }

    AVPacket pkt;
    fa_init_av_packet(&pkt, buffer);

    if (avcodec_send_packet(fc->mAvCodecCtx, &pkt) == AVERROR(EAGAIN)) {
        RT_LOGE("send_packet returned EAGAIN, which is an API violation.\n");
        return RT_ERR_TIMEOUT;
    }

    av_packet_unref(&pkt);
    return RT_OK;
}

RT_RET fa_video_decode_get_frame(FACodecContext* fc, RTMediaBuffer *buffer) {
    int      i;
    UINT8 *data[4];
    int      linesize[4];
    // INT64  pts = AV_NOPTS_VALUE;
    UINT8 *dst = NULL;
    INT32 ret = 0;
    RtMetaData *meta = NULL;

    AVFrame *frame = av_frame_alloc();
    if (frame) {
        ret = avcodec_receive_frame(fc->mAvCodecCtx, frame);
        if (ret == AVERROR(EAGAIN)) {
            av_frame_unref(frame);
            return RT_ERR_TIMEOUT;
        }
    }

    meta = buffer->getMetaData();
    dst = reinterpret_cast<UINT8 *>(buffer->getData());
    data[0] = dst;
    data[1] = dst + frame->width * frame->height;
    data[2] = data[1] + (frame->width / 2  * frame->height / 2);
    linesize[0] = frame->width;
    linesize[1] = frame->width / 2;
    linesize[2] = frame->width / 2;

    RT_LOGD("frame_width=%d frame_height=%d timstamps: %lld",
            frame->width, frame->height, frame->pts);

    // TODO(format) just used yuv420
    for (i = 0; i < frame->height; i++) {
       rt_memcpy(dst, frame->data[0] + frame->linesize[0] * i, frame->width);
       dst += frame->width;
    }

    for (i = 0; i < frame->height / 2; i++) {
        rt_memcpy(dst, frame->data[1] + frame->linesize[1] * i, frame->width / 2);
        dst += frame->width / 2;
    }
    for (i = 0; i < frame->height / 2; i++) {
        rt_memcpy(dst, frame->data[2] + frame->linesize[2] * i, frame->width / 2);
        dst += frame->width / 2;
    }

    buffer->setRange(0, frame->width * frame->height * 3 / 2);
    meta->setInt64(kKeyFramePts, frame->pts);
    meta->setInt32(kKeyFrameW,   frame->width);
    meta->setInt32(kKeyFrameH,   frame->height);
    av_frame_unref(frame);
    if (ret == AVERROR_EOF) {
        RT_LOGE("reach EOS!");
        avcodec_flush_buffers(fc->mAvCodecCtx);
        meta->setInt32(kKeyFrameEOS, 1);
    }
    buffer->setStatus(RT_MEDIA_BUFFER_STATUS_READY);
    return RT_OK;
}

RT_RET fa_audio_decode_get_frame(FACodecContext* fc, RTMediaBuffer *buffer) {
    UINT8 *dst = NULL;
    RtMetaData *meta = NULL;
    INT32 data_size = 0;
    INT32 ret = 0;
    AVFrame *frame = av_frame_alloc();
    if (frame) {
        ret = avcodec_receive_frame(fc->mAvCodecCtx, frame);
        if (ret == AVERROR(EAGAIN)) {
            av_frame_unref(frame);
            return RT_ERR_TIMEOUT;
        }
    }

    meta = buffer->getMetaData();
    if (ret >= 0) {
        INT64 dec_channel_layout;

        dst = reinterpret_cast<UINT8 *>(buffer->getData());

        data_size = av_samples_get_buffer_size(NULL, frame->channels,
                frame->nb_samples, (enum AVSampleFormat)frame->format, 1);

        RT_LOGD("sample data size:%d, channels: %d, nb_samples:%d, timstamps: %lld ret: %d, frame->format: %d"
                "channel_layout: %lld, sample_rate:%d",
                data_size, frame->channels, frame->nb_samples, frame->pts, ret, frame->format,
                frame->channel_layout, frame->sample_rate);

        /* TODO(): dst audio params should can be set */
        {
            dec_channel_layout =
                (frame->channel_layout && frame->channels == av_get_channel_layout_nb_channels(frame->channel_layout)) ?
                frame->channel_layout : av_get_default_channel_layout(frame->channels);

            if (frame->format != fc->mAudioSrc.fmt) {
                swr_free(&fc->mSwrCtx);
                fc->mSwrCtx = swr_alloc_set_opts(NULL,
                                                 dec_channel_layout, AV_SAMPLE_FMT_S16, 44100,
                                                 dec_channel_layout, (AVSampleFormat)frame->format, frame->sample_rate,
                                                 0, NULL);
                if (!fc->mSwrCtx || swr_init(fc->mSwrCtx) < 0) {
                    RT_LOGE(
                           "Cannot create sample rate converter for conversion of "
                           "%d Hz %s %d channels to %d Hz %s %d channels!\n",
                            frame->sample_rate, av_get_sample_fmt_name(AVSampleFormat(frame->format)), frame->channels,
                            44100, av_get_sample_fmt_name(AV_SAMPLE_FMT_S16), 2);
                    swr_free(&fc->mSwrCtx);
                    return RT_ERR_UNKNOWN;
                }
                fc->mAudioSrc.fmt = (AVSampleFormat)frame->format;
            }

            if (fc->mSwrCtx) {
                const UINT8 **in = (const UINT8 **)frame->extended_data;
                UINT8 **out = &dst;
                int out_count = (INT64)192000 / fc->mAvCodecCtx->channels / av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
                int len2;
                len2 = swr_convert(fc->mSwrCtx, out, out_count, in, frame->nb_samples);
                if (len2 < 0) {
                    RT_LOGE("swr_convert() failed\n");
                    return RT_ERR_UNKNOWN;
                }
                if (len2 == out_count) {
                    RT_LOGD("audio buffer is probably too small\n");
                    if (swr_init(fc->mSwrCtx) < 0)
                        swr_free(&fc->mSwrCtx);
                }
                data_size = len2 * frame->channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
            } else {
                rt_memcpy(dst, frame->data[0], data_size);
            }
        }

        buffer->setRange(0, data_size);
        meta->setInt64(kKeyFramePts, frame->pts);
    }
    if (ret == AVERROR_EOF) {
        RT_LOGE("reach EOS!");
        avcodec_flush_buffers(fc->mAvCodecCtx);
        meta->setInt32(kKeyFrameEOS, 1);
    }
    av_frame_unref(frame);
    buffer->setStatus(RT_MEDIA_BUFFER_STATUS_READY);
    return RT_OK;
}


RT_RET fa_decode_get_frame(FACodecContext* fc, RTMediaBuffer *buffer) {
    RT_RET ret = RT_OK;
    CHECK_IS_NULL(fc);
    CHECK_IS_NULL(buffer);

    switch (fc->mTrackType) {
    case RTTRACK_TYPE_VIDEO:
        ret = fa_video_decode_get_frame(fc, buffer);
        break;
    case RTTRACK_TYPE_AUDIO:
        ret = fa_audio_decode_get_frame(fc, buffer);
        break;
    default:
        RT_LOGE("not support track type: %d\n", fc->mTrackType);
        break;
    }

    return ret;
__FAILED:
    return RT_ERR_UNKNOWN;
}

RT_RET fa_encode_send_frame(FACodecContext* fc, RTMediaBuffer *buffer) {
    if (!fc && buffer) {
        RT_LOGE("fc or pkt is NULL: fc: %p pkt: %p", fc, buffer);
        return RT_ERR_VALUE;
    }

    AVFrame *frame;
    fa_init_av_frame(fc, &frame, buffer);

    if (avcodec_send_frame(fc->mAvCodecCtx, frame) < 0) {
        RT_LOGE("send_frame returned EAGAIN, which is an API violation.\n");
        return RT_ERR_TIMEOUT;
    }

    return RT_OK;
}

RT_RET fa_encode_get_packet(FACodecContext* fc, RTMediaBuffer *buffer) {
    RtMetaData *meta = NULL;
    int ret = 0;

    AVPacket *pkt = av_packet_alloc();

    meta = buffer->getMetaData();

    ret = avcodec_receive_packet(fc->mAvCodecCtx, pkt);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        RT_LOGE("receive_packet returned EAGAIN, which is an API violation.\n");
        av_packet_free(&pkt);
        return RT_ERR_TIMEOUT;
    } else if (ret < 0) {
        av_packet_free(&pkt);
        return RT_ERR_TIMEOUT;
    }

    meta->setInt64(kKeyPacketPts, pkt->pts);
    meta->setInt64(kKeyPacketDts, pkt->dts);
    buffer->setRange(0, pkt->size);

    rt_memcpy(buffer->getData(), pkt->data, pkt->size);
    buffer->setStatus(RT_MEDIA_BUFFER_STATUS_READY);
    return RT_OK;
}


void fa_codec_close(FACodecContext *fc);
void fa_codec_flush(FACodecContext *fc);
void fa_codec_push(FACodecContext *fc, char* buffer, UINT32 size);
void fa_codec_pull(FACodecContext *fc, char* buffer, UINT32* size);

INT32 fa_init_audio_params_from_metadata(FACodecContext *ctx, RtMetaData *meta) {
    AVCodecContext  *codec_ctx = RT_NULL;
    CHECK_IS_NULL(ctx);
    CHECK_IS_NULL(meta);

    codec_ctx = ctx->mAvCodecCtx;

    // non necessary parameters
    RT_PTR extra_data;
    INT32 extra_size;
    if (!meta->findPointer(kKeyCodecExtraData, &extra_data)) {
        extra_data = NULL;
    }
    if (!meta->findInt32(kKeyCodecExtraSize, &extra_size)) {
        extra_size = 0;
    }
    INT32 channels;
    if (!meta->findInt32(kKeyACodecChannels, &channels)) {
        channels = 2;
    }
    INT32 sample_rate;
    if (!meta->findInt32(kKeyACodecSampleRate, &sample_rate)) {
        sample_rate = 44100;
    }
    INT32 bit_rate;
    if (!meta->findInt32(kKeyCodecBitrate, &bit_rate)) {
        bit_rate = 0;
    }
    INT32 bit_per_coded_sample;
    if (!meta->findInt32(kKeyACodecBitPerCodedSample, &bit_per_coded_sample)) {
        bit_per_coded_sample = 0;
    }

    RT_LOGE("Codec Extra(ptr=0x%p, size=%d)", extra_data, extra_size);

    // codec context parameters configure
    codec_ctx->codec_type = AVMEDIA_TYPE_AUDIO;
    codec_ctx->extradata_size  = extra_size;
    codec_ctx->extradata   = reinterpret_cast<UINT8 *>(extra_data);
    codec_ctx->channels    = channels;
    codec_ctx->sample_rate = sample_rate;
    codec_ctx->bit_rate    = bit_rate;
    codec_ctx->sample_fmt  = AV_SAMPLE_FMT_NONE;
    codec_ctx->bits_per_coded_sample = bit_per_coded_sample;

    return 0;
__FAILED:
    return -1;
}


