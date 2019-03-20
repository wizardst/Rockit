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

#ifndef API_HAVE_AV_REGISTER_ALL
#define API_HAVE_AV_REGISTER_ALL (LIBAVFORMAT_VERSION_MAJOR < 58)
#endif

struct FAFormatContext {
    AVFormatContext  *mAvfc;
    FC_FLAG          mFcFlag;
    INT64            mDuration;
};

static void ffmpeg_log_callback(void *ptr, int level, const char *fmt, va_list vl) {
    if (level != AV_LOG_ERROR) {
        return;
    }

    char mesg[1024];
    vsprintf(mesg, fmt, vl);
    size_t len = strlen(mesg);
    if (len > 0 && len < 1024&&mesg[len - 1] == '\n') {
        mesg[len - 1] = '\0';
    }

    RT_LOGD("ffmpeg-%d: %s", LIBAVFORMAT_VERSION_MAJOR, mesg);
}

INT32 check_av_format_ctx(FAFormatContext* fc) {
    if ((RT_NULL == fc) || (RT_NULL == fc->mAvfc)) {
        return -1;
    }
    return 0;
}

void fa_ffmpeg_runtime_init() {
    av_log_set_level(AV_LOG_ERROR);
    av_log_set_callback(ffmpeg_log_callback);
}

FAFormatContext* fa_format_open(const char* uri, FC_FLAG flag /*FLAG_DEMUXER*/) {
    INT32 err = 0;
    FAFormatContext* fafc = rt_malloc(FAFormatContext);
    fafc->mAvfc           = RT_NULL;
    fafc->mFcFlag         = flag;
    fafc->mDuration       = 0;
    AVDictionary*    opts = NULL;

    RT_LOGE_IF(DEBUG_FLAG, "uri = %s", uri);
    fa_ffmpeg_runtime_init();
    switch (flag) {
      case FLAG_DEMUXER:
        /* register all formats and codecs */
        #if API_HAVE_AV_REGISTER_ALL
        av_register_all();
        #endif
        avformat_network_init();

        /* open input file, and allocate format context */
        err = avformat_open_input(&(fafc->mAvfc), uri, NULL, &opts);
        if (fa_utils_check_error(err, "avformat_open_input") < 0) {
            goto error_func;
        }

        av_dump_format(fafc->mAvfc, 0, uri, 0);

        /* retrieve stream information */
        err = avformat_find_stream_info(fafc->mAvfc, NULL);
        if (fa_utils_check_error(err, "avformat_find_stream_info") < 0) {
            goto error_func;
        }
        fafc->mDuration = fafc->mAvfc->duration;
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
    rt_safe_free(fafc);
    return RT_NULL;
}

INT32 fa_format_close(FAFormatContext* fc) {
    INT32 err = check_av_format_ctx(fc);
    if (-1 == err) {
        return err;
    }

    switch (fc->mFcFlag) {
    case FLAG_DEMUXER:
        avformat_close_input(&(fc->mAvfc));
        break;
    case FLAG_MUXER:
        break;
    default:
        break;
    }
    avformat_network_deinit();
    rt_safe_free(fc);
    return err;
}

INT32 fa_format_seek_to(FAFormatContext* fc, INT32 track_id, UINT64 ts, UINT32 flags) {
    INT32 err = check_av_format_ctx(fc);
    if (-1 == err) {
        return err;
    }


    err = avformat_seek_file(fc->mAvfc, track_id, RT_INT64_MIN, ts, RT_INT64_MAX, flags);
    fa_utils_check_error(err, "avformat_seek_file");
    return err;
}

INT32 fa_format_packet_read(FAFormatContext* fc, void** raw_pkt) {
    INT32 err = check_av_format_ctx(fc);
    if (-1 == err) {
        return err;
    }

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

INT32 fa_format_packet_parse(FAFormatContext* fc, void* raw_pkt, RTPacket* rt_pkt) {
    if ((RT_NULL == rt_pkt) || (RT_NULL == raw_pkt)) {
        return -1;
    }

    rt_memset(rt_pkt, 0, sizeof(RTPacket));
    INT32 err = check_av_format_ctx(fc);
    if (-1 == err) {
        return err;
    }

    AVPacket* ff_pkt = reinterpret_cast<AVPacket*>(raw_pkt);
    AVStream *stream = fc->mAvfc->streams[ff_pkt->stream_index];
    INT64 startTimeUs = stream->start_time == AV_NOPTS_VALUE ? 0 :
        av_rescale_q(stream->start_time, stream->time_base, AV_TIME_BASE_Q);

    if (RT_NULL != ff_pkt) {
        rt_pkt->mPts      = av_rescale_q(ff_pkt->pts, stream->time_base, AV_TIME_BASE_Q) - startTimeUs;
        rt_pkt->mDts      = av_rescale_q(ff_pkt->dts, stream->time_base, AV_TIME_BASE_Q) - startTimeUs;
        rt_pkt->mPos      = ff_pkt->pos;
        rt_pkt->mData     = ff_pkt->data;
        rt_pkt->mSize     = ff_pkt->size;
        rt_pkt->mFlags    = ff_pkt->flags;
        rt_pkt->mRawPtr   = raw_pkt;
        rt_pkt->mFuncFree   = fa_format_packet_free;
        rt_pkt->mTrackIndex = ff_pkt->stream_index;
        return 0;
    }
    return err;
}

INT64 fa_format_get_duraton(FAFormatContext* fc) {
    INT64 duration = 0;
    if (0 == check_av_format_ctx(fc)) {
        duration = fc->mDuration;
    }
    return duration;
}

void fa_format_build_track_meta(const AVStream* stream, RTTrackParms* track) {
    UINT32 frame_rate = 0;
    if (stream->avg_frame_rate.den > 0) {
        frame_rate = stream->avg_frame_rate.num/stream->avg_frame_rate.den;
    }
    AVCodecParameters* cpar = stream->codecpar;
    track->mCodecType       = (RTTrackType)cpar->codec_type;
    track->mCodecID         = (RTCodecID)fa_utils_to_rt_codec_id(cpar->codec_id);
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
    RT_LOGD("audio params: channels %d, sample_rate %d, block_align %d," \
             "bits_per_coded_sample %d, bits_per_raw_sample %d",
             cpar->channels, cpar->sample_rate, cpar->block_align, \
             cpar->bits_per_coded_sample, cpar->bits_per_raw_sample);
}

INT32 fa_format_query_track(FAFormatContext* fc, UINT32 idx, \
                              RTTrackType tType, RTTrackParms* track) {
    UINT32 count = 0;

    const AVStream* stream = RT_NULL;
    if ((RT_NULL != fc) && (RT_NULL != fc->mAvfc)) {
        AVFormatContext* avfc = fc->mAvfc;
        for (UINT32 ii = 0; ii < avfc->nb_streams; ii++) {
            count++;
            if (avfc->streams[ii]->codecpar->codec_type == (AVMediaType)tType) {
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
            break;
        }
        bestIdx = av_find_best_stream(fc->mAvfc, avType, -1, -1, NULL, 0);
    }
    if (bestIdx < 0) {
        fa_utils_check_error(bestIdx, "av_find_best_stream");
        bestIdx = fa_format_find_track(fc, tType);
    }
    RT_LOGD("av_find_best_stream(best_id=%2d) for %s", bestIdx, av_get_media_type_string(avType));
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
