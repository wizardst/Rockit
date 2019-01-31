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
    RTCodecID         rt_coding_type;
    MppCodingType     mpp_coding_type;
} MACodingTypeInfo;

static MACodingTypeInfo kMACodecMappingList[] = {
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

RTCodecID fa_utils_find_codectype_from_mpp(MppCodingType mpp_coding_type) {
    UINT32 i = 0;
    RT_BOOL found = RT_FALSE;
    for (i = 0; i < RT_ARRAY_ELEMS(kMACodecMappingList); i++) {
        if (mpp_coding_type == kMACodecMappingList[i].mpp_coding_type) {
            found = RT_TRUE;
            break;
        }
    }

    if (found) {
        return kMACodecMappingList[i].rt_coding_type;
    } else {
        RT_LOGE("unknown codec id: %d", mpp_coding_type);
        return RT_VIDEO_ID_Unused;
    }
}

MppCodingType fa_utils_find_mpp_codectype_from_rt(RTCodecID rt_coding_type) {
    UINT32 i = 0;
    RT_BOOL found = RT_FALSE;
    for (i = 0; i < RT_ARRAY_ELEMS(kMACodecMappingList); i++) {
        if (rt_coding_type == kMACodecMappingList[i].rt_coding_type) {
            found = RT_TRUE;
            break;
        }
    }

    if (found) {
        return kMACodecMappingList[i].mpp_coding_type;
    } else {
        RT_LOGE("unknown codec type: %d", rt_coding_type);
        return MPP_VIDEO_CodingUnused;
    }
}

