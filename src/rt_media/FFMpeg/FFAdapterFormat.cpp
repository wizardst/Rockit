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
    AVFormatContext *mAvfc;
    FC_FLAG          mFcFlag;
};

FAFormatContext* fa_format_open(const char* uri, FC_FLAG flag /*FLAG_DEMUXER*/) {
    INT32 err = 0;
    FAFormatContext* fafc = rt_malloc(FAFormatContext);
    fafc->mAvfc           = RT_NULL;
    fafc->mFcFlag         = flag;

    RT_LOGE_IF(DEBUG_FLAG, " uri=%s", uri);

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

void fa_format_track_query(FAFormatContext* fc, UINT32 track_id, RtMetaData *meta) {
    AVFormatContext* avfc = fc->mAvfc;

    const AVOption* option = RT_NULL;
    const AVStream* stream = RT_NULL;
    if ((track_id >= 0) && (track_id < avfc->nb_streams)) {
        stream = avfc->streams[track_id];
    }
    if ((RT_NULL == stream)/*||(RT_NULL == meta)*/) {
        return;
    }

    AVCodecParameters* codec_par = stream->codecpar;
    AVCodec*           codec_cur = avcodec_find_decoder(codec_par->codec_id);
    AVCodecContext*    codec_ctx = avcodec_alloc_context3(codec_cur);

    while (option = av_opt_next(codec_ctx, option)) {
        uint8_t* str;

        if (option->type == AV_OPT_TYPE_CONST)
            continue;

        if (!strcmp(option->name, "frame_number"))
            continue;

        if (av_opt_get(codec_ctx, option->name, 0, &str) >= 0) {
            RT_LOGE_IF(DEBUG_FLAG, "option: %s=%s", option->name, str);
            av_free(str);
        }
    }
}

UINT32 fa_format_count_tracks(FAFormatContext* fc) {
    if ((RT_NULL != fc) && (RT_NULL != fc->mAvfc)) {
        AVFormatContext* avfc = fc->mAvfc;
        return avfc->nb_streams;
    }
    return 0;
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
}

void fa_format_seek_to(FAFormatContext* fc, INT32 track_id, UINT64 ts, UINT32 flags) {
    INT32 err = 0;
    err = avformat_seek_file(fc->mAvfc, track_id, RT_INT64_MIN, ts, RT_INT64_MAX, flags);
    fa_utils_check_error(err, "avformat_seek_file");
}
