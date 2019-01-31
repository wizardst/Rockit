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
 * author: martin.cheng@rock-chips.com
 *   date: 20190115
 *    ref: ffmpeg/libavcodec/avcodec.h
 */

#ifndef SRC_RT_MEDIA_INCLUDE_RTMEDIADEF_H_
#define SRC_RT_MEDIA_INCLUDE_RTMEDIADEF_H_

#include "RTPixelDef.h"   // NOLINT
#include "rt_type.h"      // NOLINT
#include "rt_metadata.h"  // NOLINT

/**
* @ingroup rt_node
* @brief Enumeration used to define the possible video compression codings.
*        sync with the omx_video.h
*
* @note  This essentially refers to file extensions. If the coding is
*        being used to specify the ENCODE type, then additional work
*        must be done to configure the exact flavor of the compression
*        to be used.  For decode cases where the user application can
*        not differentiate between MPEG-4 and H.264 bit streams, it is
*        up to the codec to handle this.
*/

typedef enum {
     RT_VIDEO_ID_Unused,             /**< Value when coding is N/A */
     RT_VIDEO_ID_AutoDetect,         /**< Autodetection of coding type */
     RT_VIDEO_ID_MPEG2,              /**< AKA: H.262 */
     RT_VIDEO_ID_H263,               /**< H.263 */
     RT_VIDEO_ID_MPEG4,              /**< MPEG-4 */
     RT_VIDEO_ID_WMV,                /**< Windows Media Video (WMV1,WMV2,WMV3)*/
     RT_VIDEO_ID_RV,                 /**< all versions of Real Video */
     RT_VIDEO_ID_AVC,                /**< H.264/AVC */
     RT_VIDEO_ID_MJPEG,              /**< Motion JPEG */
     RT_VIDEO_ID_VP8,                /**< VP8 */
     RT_VIDEO_ID_VP9,                /**< VP9 */
     RT_VIDEO_ID_HEVC,               /**< ITU H.265/HEVC */
     RT_VIDEO_ID_DolbyVision,        /**< Dolby Vision */
     RT_VIDEO_ID_ImageHEIC,          /**< HEIF image encoded with HEVC */
     RT_VIDEO_ID_VC1 = 0x01000000,   /**< Windows Media Video (WMV1,WMV2,WMV3)*/
     RT_VIDEO_ID_FLV1,               /**< Sorenson H.263 */
     RT_VIDEO_ID_DIVX3,              /**< DIVX3 */
     RT_VIDEO_ID_VP6,
     RT_VIDEO_ID_AVSPLUS,            /**< AVS+ */
     RT_VIDEO_ID_AVS,                /**< AVS profile=0x20 */

     /* *< Reserved region for introducing Khronos Standard Extensions */
     RT_VIDEO_ID_KhronosExtensions = 0x6F000000,

     /* *< Reserved region for introducing Vendor Extensions */
     RT_VIDEO_ID_VendorStartUnused = 0x7F000000,
     RT_VIDEO_ID_Max = 0x7FFFFFFF
} RTCodecID;

enum RTFieldOrder {
    RT_FIELD_UNKNOWN,
    RT_FIELD_PROGRESSIVE,
    RT_FIELD_TT,          // < Top coded_first, top displayed first
    RT_FIELD_BB,          // < Bottom coded first, bottom displayed first
    RT_FIELD_TB,          // < Top coded first, bottom displayed first
    RT_FIELD_BT,          // < Bottom coded first, top displayed first
};

enum RTImageType {
    RT_IMAGE_TYPE_NONE = 0,  ///< Undefined
    RT_IMAGE_TYPE_I,         ///< Intra
    RT_IMAGE_TYPE_P,         ///< Predicted
    RT_IMAGE_TYPE_B,         ///< Bi-dir predicted
    RT_IMAGE_TYPE_S,         ///< S(GMC)-VOP MPEG-4
    RT_IMAGE_TYPE_SI,        ///< Switching Intra
    RT_IMAGE_TYPE_SP,        ///< Switching Predicted
    RT_IMAGE_TYPE_BI,        ///< BI type
};

typedef enum _RTTrackType {
    RTTRACK_TYPE_UNKNOWN = -1,  // < Usually treated as AVMEDIA_TYPE_DATA
    RTTRACK_TYPE_VIDEO,
    RTTRACK_TYPE_AUDIO,
    RTTRACK_TYPE_DATA,          // < Opaque data information usually continuous
    RTTRACK_TYPE_SUBTITLE,
    RTTRACK_TYPE_ATTACHMENT,    // < Opaque data information usually sparse
    RTTRACK_TYPE_MAX
} RTTrackType;

enum RTSampleFormat {
    RT_SAMPLE_FMT_NONE = -1,
    RT_SAMPLE_FMT_U8,          ///< unsigned 8 bits
    RT_SAMPLE_FMT_S16,         ///< signed 16 bits
    RT_SAMPLE_FMT_S32,         ///< signed 32 bits
    RT_SAMPLE_FMT_FLT,         ///< float
    RT_SAMPLE_FMT_DBL,         ///< double

    RT_SAMPLE_FMT_U8P,         ///< unsigned 8 bits, planar
    RT_SAMPLE_FMT_S16P,        ///< signed 16 bits, planar
    RT_SAMPLE_FMT_S32P,        ///< signed 32 bits, planar
    RT_SAMPLE_FMT_FLTP,        ///< float, planar
    RT_SAMPLE_FMT_DBLP,        ///< double, planar
    RT_SAMPLE_FMT_S64,         ///< signed 64 bits
    RT_SAMPLE_FMT_S64P,        ///< signed 64 bits, planar

    RT_SAMPLE_FMT_MAX          ///< Number of sample formats. DO NOT USE if linking dynamically
};

struct RTTrackParms {
    RTTrackType mCodecType;
    RTCodecID   mCodecID;
    INT32  mCodecFormat;  // video: the pixel format; audio: the sample format
    INT32  mCodecProfile;
    INT32  mCodecLevel;

    UINT64   mBitrate;
    uint8_t *mExtraData;
    INT32    mExtraDataSize;

    /* video track features */
    INT32  mVideoWidth;
    INT32  mVideoHeight;
    INT32  mVideoDelay;
    INT32  mVideoFrameRate;      // 23.976 x 10000
    enum RTFieldOrder        mFieldOrder;
    enum RTColorRange        mColorRange;
    enum RTColorPrimaries    mColorPrimaries;
    enum RTColorTransfer     mColorTrc;
    enum RTColorSpace        mColorSpace;
    enum RTChromaLocation    mChromaLocation;

    /* audio track features*/
    UINT64 mAudioChannelLayout;
    INT32  mAudioChannels;
    INT32  mAudioSampleRate;
    INT32  mAudioBlockAlign;
    INT32  mAudioFrameSize;
    INT32  mAudioInitialPadding;
    INT32  mAudioTrailingPadding;
    INT32  mAudiobitsPerCodedSample;
    INT32  mAudiobitsPerRawSample;
};

#endif  // SRC_RT_MEDIA_INCLUDE_RTMEDIADEF_H_
