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
    RtCodingType        rt_coding_type;
    MppCodingType       mpp_coding_type;
} MACodingTypeInfo;

static MACodingTypeInfo kMACodecMappingList[] = {
    { RT_VIDEO_CodingMPEG2,         MPP_VIDEO_CodingMPEG2 },
    { RT_VIDEO_CodingH263,          MPP_VIDEO_CodingH263 },
    { RT_VIDEO_CodingMPEG4,         MPP_VIDEO_CodingMPEG4 },
    { RT_VIDEO_CodingWMV,           MPP_VIDEO_CodingWMV },
    { RT_VIDEO_CodingAVC,           MPP_VIDEO_CodingAVC },
    { RT_VIDEO_CodingMJPEG,         MPP_VIDEO_CodingMJPEG },
    { RT_VIDEO_CodingVP8,           MPP_VIDEO_CodingVP8 },
    { RT_VIDEO_CodingVP9,           MPP_VIDEO_CodingVP9 },
    { RT_VIDEO_CodingHEVC,          MPP_VIDEO_CodingHEVC },
    { RT_VIDEO_CodingVC1,           MPP_VIDEO_CodingVC1 },
    { RT_VIDEO_CodingAVS,           MPP_VIDEO_CodingAVS },
};

RtCodingType fa_utils_find_codectype_from_mpp(MppCodingType mpp_coding_type) {
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
        return RT_VIDEO_CodingUnused;
    }
}

MppCodingType fa_utils_find_mpp_codectype_from_rt(RtCodingType rt_coding_type) {
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

