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
 *   Date: 2019/01/22
 */

#ifndef SRC_RT_MEDIA_INCLUDE_RTCODECDATA_H_
#define SRC_RT_MEDIA_INCLUDE_RTCODECDATA_H_

#include "RTCodecDef.h"
#include "RTPixelDef.h"

typedef INT32 (*RT_RAW_FREE)(void*);

typedef struct _RTPacket {
    INT64    mPts;
    INT64    mDts;
    INT64    mPos;
    INT32    mSize;
    INT32    mTrackIndex;
    INT32    mFlags;
    uint8_t *mData;
    void*    mRawPkt;
    RT_RAW_FREE mFuncFree;
} RTPacket;

typedef struct _RTFrame {
    INT64    mPts;               /* with unit of us*/
    INT64    mDts;               /* with unit of us*/
    uint8_t *mData;
    INT32    mSize;
    UINT32   mWidth;
    UINT32   mHeight;
    UINT32   mFormat;
    /**
     * quality (between 1 (good) and FF_LAMBDA_MAX (bad))
     */
    UINT32   mQuality;
    UINT32   mKeyFrame;
    enum RTImageType  mImgType;
    enum RTFieldOrder mFieldOrder;
    RT_RAW_FREE mFuncFree;
} RTFrame;

/*utils for rt_packet*/
RT_RET rt_utils_packet_free(RTPacket* rt_pkt);
RT_RET rt_utils_pkt_to_meta(RTPacket* rt_pkt, RtMetaData* meta);
RT_RET rt_utils_meta_to_pkt(RtMetaData* meta, RTPacket* rt_pkt);

/*utils for rt_frame*/
RT_RET rt_utils_frame_free(RTFrame* rt_frame);
RT_RET rt_utils_frame_to_meta(RTFrame* rt_frame, RtMetaData* meta);
RT_RET rt_utils_meta_to_frame(RtMetaData* meta, RTFrame* rt_frame);

/*utils for track parameters*/
RT_RET rt_utils_track_par_to_meta(RTTrackParms* tpar, RtMetaData* meta);
RT_RET rt_utils_meta_to_track_par(RtMetaData* meta, RTTrackParms* tpar);
RT_RET rt_utils_dump_track_par(RTTrackParms* tpar, RT_BOOL full = false);

#endif  // SRC_RT_MEDIA_INCLUDE_RTCODECDATA_H_
