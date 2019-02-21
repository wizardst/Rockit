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

#include "FFAdapterFormat.h" // NOLINT
#include "FFAdapterUtils.h"  // NOLINT
#include "RTMediaMetaKeys.h" // NOLINT
#include "RTMediaDef.h"      // NOLINT
#include "rt_metadata.h"     // NOLINT
#include "rt_mem.h"          // NOLINT
#include "rt_log.h"          // NOLINT
#include "rt_common.h"       // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FFAdapterFormat"
#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x1

struct FAFormatContext {
    AVFormatContext  *mAvfc;
    FC_FLAG          mFcFlag;
};

FAFormatContext* fa_format_open(const char* uri, FC_FLAG flag /*FLAG_DEMUXER*/) {
    INT32 err = 0;
    FAFormatContext* fafc = rt_malloc(FAFormatContext);
    fafc->mAvfc           = RT_NULL;
    fafc->mFcFlag         = flag;

    RT_LOGE_IF(DEBUG_FLAG, "uri = %s", uri);

    switch (flag) {
    case FLAG_DEMUXER:
        // MUST initialize or it will cause crash in avformat_open_input
        err = avformat_open_input(&(fafc->mAvfc), uri, NULL, NULL);
        if (fa_utils_check_error(err, "avformat_open_input") < 0) {
            goto error_func;
        }
        break;
    case FLAG_MUXER:
        fafc->mAvfc = avformat_alloc_context();
        fafc->mAvfc->oformat = av_guess_format(NULL, uri, NULL);
        err = avio_open(&fafc->mAvfc->pb, uri, AVIO_FLAG_WRITE);
        if (fa_utils_check_error(err, "avio_open") < 0) {
            goto error_func;
        }
        break;
    default:
        break;
    }
    return fafc;

error_func:
    return RT_NULL;
}

void fa_format_close(FAFormatContext* fc) {
    switch (fc->mFcFlag) {
    case FLAG_DEMUXER:
        avformat_close_input(&(fc->mAvfc));
        break;
    case FLAG_MUXER:
        break;
    default:
        break;
    }
    rt_safe_free(fc);
}

void fa_format_seek_to(FAFormatContext* fc, INT32 track_id, UINT64 ts, UINT32 flags) {
    INT32 err = 0;
    err = avformat_seek_file(fc->mAvfc, track_id, RT_INT64_MIN, ts, RT_INT64_MAX, flags);
    fa_utils_check_error(err, "avformat_seek_file");
}

INT32 fa_format_packet_read(FAFormatContext* fc, void** raw_pkt) {
    INT32 err = 0;
    AVPacket *avPacket = av_packet_alloc();
    av_init_packet(avPacket);
    err = av_read_frame(fc->mAvfc, avPacket);
    *raw_pkt = avPacket;
    return err;
}

INT32 fa_format_packet_free(void* raw_pkt) {
    AVPacket* ff_pkt = reinterpret_cast<AVPacket*>(raw_pkt);
    if (RT_NULL != ff_pkt) {
        av_packet_unref(ff_pkt);
        return 0;
    }
    return -1;
}

INT32  fa_format_packet_type(void* raw_pkt) {
    AVPacket* ff_pkt = reinterpret_cast<AVPacket*>(raw_pkt);
    if (RT_NULL != ff_pkt) {
        return ff_pkt->stream_index;
    }
    return -1;
}

INT32 fa_format_packet_parse(void* raw_pkt, RTPacket* rt_pkt) {
    rt_memset(rt_pkt, 0, sizeof(RTPacket));
    AVPacket* ff_pkt = reinterpret_cast<AVPacket*>(raw_pkt);
    if (RT_NULL != ff_pkt) {
        rt_pkt->mPts      = ff_pkt->pts;
        rt_pkt->mDts      = ff_pkt->dts;
        rt_pkt->mPos      = ff_pkt->pos;
        rt_pkt->mData     = ff_pkt->data;
        rt_pkt->mSize     = ff_pkt->size;
        rt_pkt->mFlags    = ff_pkt->flags;
        rt_pkt->mRawPtr   = raw_pkt;
        rt_pkt->mFuncFree   = fa_format_packet_free;
        rt_pkt->mTrackIndex = ff_pkt->stream_index;
        return 0;
    }
    return -1;
}

void fa_format_build_track_meta(const AVStream* stream, RTTrackParms* track) {
    UINT32 frame_rate = 0;
    if (stream->avg_frame_rate.den > 0) {
        frame_rate = stream->avg_frame_rate.num/stream->avg_frame_rate.den;
    }
    AVCodecParameters* cpar = stream->codecpar;
    track->mCodecType       = (RTTrackType)cpar->codec_type;
    track->mCodecID     = (RTCodecID)fa_utils_to_rt_codec_id(cpar->codec_id);
    track->mCodecFormat     =  cpar->format;
    track->mCodecProfile    =  cpar->profile;
    track->mCodecLevel      =  cpar->level;

    track->mBitrate         =  cpar->bit_rate;
    track->mExtraData       =  cpar->extradata;
    track->mExtraDataSize   =  cpar->extradata_size;

    /* video track features */
    track->mVideoWidth      =  cpar->width;
    track->mVideoHeight     =  cpar->height;
    track->mVideoDelay      =  cpar->video_delay;
    track->mVideoFrameRate  =  frame_rate;
    track->mFieldOrder      =  (RTFieldOrder)cpar->field_order;
    track->mColorRange      =  (RTColorRange)cpar->color_range;
    track->mColorPrimaries  =  (RTColorPrimaries)cpar->color_primaries;
    track->mColorTrc        =  (RTColorTransfer)cpar->color_trc;
    track->mColorSpace      =  (RTColorSpace)cpar->color_space;
    track->mChromaLocation  =  (RTChromaLocation)cpar->chroma_location;

    /* audio track features*/
    track->mAudioChannelLayout      = cpar->channel_layout;
    track->mAudioChannels           = cpar->channels;
    track->mAudioSampleRate         = cpar->sample_rate;
    track->mAudioBlockAlign         = cpar->block_align;
    track->mAudioFrameSize          = cpar->frame_size;
    track->mAudioInitialPadding     = cpar->initial_padding;
    track->mAudioTrailingPadding    = cpar->trailing_padding;
    track->mAudiobitsPerCodedSample = cpar->bits_per_coded_sample;
    track->mAudiobitsPerRawSample   = cpar->bits_per_raw_sample;
}

INT32 fa_format_query_track(FAFormatContext* fc, UINT32 idx, \
                              RTTrackType tType, RTTrackParms* track) {
    UINT32 count = 0;

    const AVStream* stream = RT_NULL;
    if ((RT_NULL != fc) && (RT_NULL != fc->mAvfc)) {
        AVFormatContext* avfc = fc->mAvfc;
        for (UINT32 ii = 0; ii < avfc->nb_streams; ii++) {
            if (avfc->streams[ii]->codecpar->codec_type == (AVMediaType)tType) {
                count++;
                if ((count-1) == idx) {
                    stream = avfc->streams[ii];
                    if ((RT_NULL != stream) && (RT_NULL != track)) {
                        fa_format_build_track_meta(stream, track);
                        return idx;
                    }
                }
            }
        }
    }
    return -1;
}

INT32 fa_format_select_track(FAFormatContext* fc, UINT32 idx, RTTrackType tType) {
    UINT32 count = 0;
    if ((RT_NULL != fc) && (RT_NULL != fc->mAvfc)) {
        AVFormatContext* avfc = fc->mAvfc;
        for (UINT32 idx = 0; idx < avfc->nb_streams; idx++) {
            if (avfc->streams[idx]->codecpar->codec_type == (AVMediaType)tType) {
                if (count == idx) {
                    return idx;
                } else {
                    count++;
                }
            }
        }
    }
    return -1;
}

INT32 fa_format_find_track(FAFormatContext* fc, RTTrackType tType) {
    UINT32 count = 0;
    if ((RT_NULL != fc) && (RT_NULL != fc->mAvfc)) {
        AVFormatContext* avfc = fc->mAvfc;
        for (UINT32 idx = 0; idx < avfc->nb_streams; idx++) {
            if (avfc->streams[idx]->codecpar->codec_type == (AVMediaType)tType) {
                return idx;
            }
        }
    }
    return -1;
}

INT32 fa_format_find_best_track(FAFormatContext* fc, RTTrackType tType) {
    INT32 bestIdx = -1;
    enum AVMediaType avType;
    if ((RT_NULL != fc) && (RT_NULL != fc->mAvfc)) {
        switch (tType) {
        case RTTRACK_TYPE_VIDEO:
            avType = AVMEDIA_TYPE_VIDEO;
            break;
        case RTTRACK_TYPE_AUDIO:
            avType = AVMEDIA_TYPE_AUDIO;
            break;
        case RTTRACK_TYPE_SUBTITLE:
            avType = AVMEDIA_TYPE_SUBTITLE;
            break;
        default:
            avType = AVMEDIA_TYPE_UNKNOWN;
        }
        bestIdx = av_find_best_stream(fc->mAvfc, avType, -1, -1, NULL, 0);
    }
    if (-1 == bestIdx) {
        bestIdx = fa_format_find_track(fc, tType);
        RT_LOGE("Fail to find_best_track, use find_track instead! bestIdx=%d", bestIdx);
    }
    return bestIdx;
}

INT32 fa_format_count_tracks(FAFormatContext* fc, RTTrackType tType) {
    INT32 count = 0;
    if ((RT_NULL != fc) && (RT_NULL != fc->mAvfc)) {
        AVFormatContext* avfc = fc->mAvfc;
        if (RTTRACK_TYPE_UNKNOWN == tType) {
            return avfc->nb_streams;
        }
        for (UINT32 idx = 0; idx < avfc->nb_streams; idx++) {
            if (avfc->streams[idx]->codecpar->codec_type == (AVMediaType)tType) {
                count++;
            }
        }
    }
    return count;
}
