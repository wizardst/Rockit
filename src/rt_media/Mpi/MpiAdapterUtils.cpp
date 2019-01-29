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
 *   Date: 2019/01/21
 */

#include "MpiAdapterUtils.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "MpiAdapterCodec"
#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x1

typedef struct {
    union {
        UINT32              rt_parm;
        RTCodecID           rt_coding_type;
    };
    union {
        UINT32              mpp_parm;
        MppCodingType       mpp_coding_type;
    };
} MACodingTypeInfo;

static MACodingTypeInfo kMACodecIDMappingList[] = {
    { RT_VIDEO_ID_MPEG2,         MPP_VIDEO_CodingMPEG2 },
    { RT_VIDEO_ID_H263,          MPP_VIDEO_CodingH263 },
    { RT_VIDEO_ID_MPEG4,         MPP_VIDEO_CodingMPEG4 },
    { RT_VIDEO_ID_WMV,           MPP_VIDEO_CodingWMV },
    { RT_VIDEO_ID_AVC,           MPP_VIDEO_CodingAVC },
    { RT_VIDEO_ID_MJPEG,         MPP_VIDEO_CodingMJPEG },
    { RT_VIDEO_ID_VP8,           MPP_VIDEO_CodingVP8 },
    { RT_VIDEO_ID_VP9,           MPP_VIDEO_CodingVP9 },
    { RT_VIDEO_ID_HEVC,          MPP_VIDEO_CodingHEVC },
    { RT_VIDEO_ID_VC1,           MPP_VIDEO_CodingVC1 },
    { RT_VIDEO_ID_AVS,           MPP_VIDEO_CodingAVS },
};

typedef struct {
    union {
        UINT32               rt_parm;
        RtVideoFormat        rt_video_format;
    };
    union {
        UINT32               mpp_parm;
        MppFrameFormat       mpp_video_format;
    };
} MAVideoFormatInfo;

static MAVideoFormatInfo kMAFormatMappingList[] = {
    { RT_FMT_YUV420SP,         MPP_FMT_YUV420SP },
    { RT_FMT_YUV420P,          MPP_FMT_YUV420P },
};

typedef struct {
    union {
        UINT32               rt_parm;
        RtVideoRCMode        rt_rc_mode;
    };
    union {
        UINT32               mpp_parm;
        MppEncRcMode         mpp_rc_mode;
    };
} MAVideoRCModeInfo;

static MAVideoRCModeInfo kMARCModeMappingList[] = {
    { RT_RC_MODE_CBR,          MPP_ENC_RC_MODE_CBR },
    { RT_RC_MODE_VBR,          MPP_ENC_RC_MODE_VBR },
    { RT_RC_MODE_CQP,          MPP_ENC_RC_MODE_VBR },
};


/*
 * object access function macro
 */
#define MA_INFO_TRANS_FUNC(field, map_list, rt_default, mpp_default) \
    UINT32 ma_mpp_to_rt_##field(const UINT32 type) {        \
        RT_BOOL found = RT_FALSE;                           \
        UINT32 i = 0;                                       \
        for (i = 0; i < RT_ARRAY_ELEMS(map_list); i++) {    \
            if (type == map_list[i].mpp_parm) {             \
                found = RT_TRUE;                            \
                break;                                      \
            }                                               \
        }                                                   \
        if (found) {                                        \
            return map_list[i].rt_parm;                     \
        } else {                                            \
            RT_LOGE("unknown mpp type: %d", type);          \
            return rt_default;                              \
        }                                                   \
    }                                                       \
    UINT32 ma_rt_to_mpp_##field(const UINT32 type) {        \
        RT_BOOL found = RT_FALSE;                           \
        UINT32 i = 0;                                       \
        for (i = 0; i < RT_ARRAY_ELEMS(map_list); i++) {    \
            if (type == map_list[i].rt_parm) {              \
                found = RT_TRUE;                            \
                break;                                      \
            }                                               \
        }                                                   \
        if (found) {                                        \
            return map_list[i].mpp_parm;                    \
        } else {                                            \
            RT_LOGE("unknown rt type: %d", type);           \
            return mpp_default;                             \
        }                                                   \
    }

MA_INFO_TRANS_FUNC(color_format, kMAFormatMappingList,
                   RT_FMT_YUV420SP, MPP_FMT_YUV420SP)
MA_INFO_TRANS_FUNC(codec_id, kMACodecIDMappingList,
                   RT_VIDEO_ID_Unused, MPP_VIDEO_CodingUnused)
MA_INFO_TRANS_FUNC(rc_mode, kMARCModeMappingList,
                   RT_RC_MODE_VBR, MPP_ENC_RC_MODE_VBR)


