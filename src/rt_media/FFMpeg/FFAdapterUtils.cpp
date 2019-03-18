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

#include <stdarg.h>          // NOLINT
#include "FFAdapterUtils.h"  // NOLINT
#include "RTMediaDef.h"      // NOLINT
#include "rt_log.h"          // NOLINT
#include "rt_common.h"       // NOLINT

typedef struct {
    RTCodecID  rt_codec_id;
    enum AVCodecID  av_codec_id;
} FACodingTypeInfo;

static FACodingTypeInfo kFdCodecMappingList[] = {
    /* video */
    { RT_VIDEO_ID_MPEG2,         AV_CODEC_ID_MPEG2VIDEO },
    { RT_VIDEO_ID_H263,          AV_CODEC_ID_H263 },
    { RT_VIDEO_ID_MPEG4,         AV_CODEC_ID_MPEG4 },
    { RT_VIDEO_ID_WMV,           AV_CODEC_ID_WMV3 },
    { RT_VIDEO_ID_AVC,           AV_CODEC_ID_H264 },
    { RT_VIDEO_ID_MJPEG,         AV_CODEC_ID_MJPEG },
    { RT_VIDEO_ID_VP8,           AV_CODEC_ID_VP8 },
    { RT_VIDEO_ID_VP9,           AV_CODEC_ID_VP9 },
    { RT_VIDEO_ID_HEVC,          AV_CODEC_ID_HEVC },
    { RT_VIDEO_ID_VC1,           AV_CODEC_ID_VC1 },
    { RT_VIDEO_ID_AVS,           AV_CODEC_ID_AVS },
    /* audio */
    { RT_AUDIO_ID_AAC,           AV_CODEC_ID_AAC },
    { RT_AUDIO_ID_APE,           AV_CODEC_ID_APE },
    { RT_AUDIO_ID_MP3,           AV_CODEC_ID_MP3 },
    { RT_AUDIO_ID_WMALOSSLESS,   AV_CODEC_ID_WMALOSSLESS },
    { RT_AUDIO_ID_WMAPRO,        AV_CODEC_ID_WMAPRO },
    { RT_AUDIO_ID_WMAV1,         AV_CODEC_ID_WMAV1 },
    { RT_AUDIO_ID_WMAV2,         AV_CODEC_ID_WMAV2 },
    { RT_AUDIO_ID_ADPCM,         AV_CODEC_ID_ADPCM_IMA_QT },
    { RT_AUDIO_ID_VORBIS,        AV_CODEC_ID_VORBIS },
    { RT_AUDIO_ID_PCM,           AV_CODEC_ID_PCM_S16LE },
    { RT_AUDIO_ID_FLAC,          AV_CODEC_ID_FLAC },
};

// trans AVCodecID to RTCodecID
UINT32 fa_utils_to_rt_codec_id(UINT32 av_codec_id) {
    UINT32 i = 0;
    RT_BOOL found = RT_FALSE;
    for (i = 0; i < RT_ARRAY_ELEMS(kFdCodecMappingList); i++) {
        if (av_codec_id == kFdCodecMappingList[i].av_codec_id) {
            found = RT_TRUE;
            break;
        }
    }

    if (found) {
        return kFdCodecMappingList[i].rt_codec_id;
    } else {
        RT_LOGE("Unknown AVCodecID(%02d,%s)", av_codec_id, fa_utils_codec_name(av_codec_id));
        return RT_VIDEO_ID_Unused;
    }
}

// trans RTCodecID to AVCodecID
UINT32 fa_utils_to_av_codec_id(UINT32 rt_codec_id) {
    UINT32 i = 0;
    RT_BOOL found = RT_FALSE;
    for (i = 0; i < RT_ARRAY_ELEMS(kFdCodecMappingList); i++) {
        if (rt_codec_id == kFdCodecMappingList[i].rt_codec_id) {
            found = RT_TRUE;
            break;
        }
    }

    if (found) {
        return kFdCodecMappingList[i].av_codec_id;
    } else {
        RT_LOGE("Unknown RTCodecID(0x%x)", rt_codec_id);
        return AV_CODEC_ID_NONE;
    }
}

INT32 fa_utils_error_string(INT32 errnum, char *errbuf, UINT32 errbuf_size) {
    return av_strerror(errnum, errbuf, errbuf_size);
}

INT32 fa_utils_check_error(const INT32 errnum, const char* func_name) {
    if (errnum < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
        fa_utils_error_string(errnum, errbuf, AV_ERROR_MAX_STRING_SIZE);
        RT_LOGE("Fail to %s; Error[%d:%s]", func_name, errnum, errbuf);
    }
    return errnum;
}

void RT_LOG_CALLBACK(void*, int, const char*, va_list) {
    return;
}

void fa_utils_set_log_callback() {
    av_log_set_callback(RT_LOG_CALLBACK);
}

/*
 * const char *avcodec_get_name(enum AVCodecID id);
 * defined in libavcodec/avcodec.h
 */
const char* fa_utils_codec_name(UINT32 codec_id) {
    return avcodec_get_name((AVCodecID)codec_id);
}

const char* fa_utils_ffmpeg_version() {
    return av_version_info();
}

#define YUV2R(y, u, v) ({ \
    int r = (y) + ((((v) - 128) * 1436) >> 10); r > 255 ? 255 : r < 0 ? 0 : r; })
#define YUV2G(y, u, v) ({ \
    int g = (y) - ((((u) - 128) * 352 + ((v) - 128) * 731) >> 10); g > 255 ? 255 : g < 0 ? 0 : g; })
#define YUV2B(y, u, v) ({ \
    int b = (y) + ((((u) - 128) * 1814) >> 10); b > 255 ? 255 : b < 0 ? 0 : b; })
UINT32 fa_utils_yuv420_to_rgb(void* src, unsigned char* dest, \
                                   int width, int height) {
    int ix, iy;
    const unsigned char *ysrc = (unsigned char*)src;
    const unsigned char *usrc, *vsrc;

    usrc = (unsigned char*)(ysrc + width * height);
    vsrc = (unsigned char*)(usrc + (width * height) / 4);

    for (iy = 0; iy < height; iy++) {
        for (ix = 0; ix < width; ix += 2) {
            *dest++ = YUV2R(*ysrc, *usrc, *vsrc);
            *dest++ = YUV2G(*ysrc, *usrc, *vsrc);
            *dest++ = YUV2B(*ysrc, *usrc, *vsrc);
            ysrc++;

            *dest++ = YUV2R(*ysrc, *usrc, *vsrc);
            *dest++ = YUV2G(*ysrc, *usrc, *vsrc);
            *dest++ = YUV2B(*ysrc, *usrc, *vsrc);
            ysrc++;
            usrc++;
            vsrc++;
        }
        /* Rewind u and v for next line */
        if (!(iy & 0x1)) {
            usrc -= width / 2;
            vsrc -= width / 2;
        }
    }
    return 0;
}

