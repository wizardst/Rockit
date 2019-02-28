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

#include "RTMediaPixel.h"   // NOLINT
#include "rt_type.h"      // NOLINT
#include "rt_metadata.h"  // NOLINT

/**
* @ingroup rt_node
* @brief Enumeration used to define the possible video compression codings.
*        sync with the omx_video.h & omx_audio.h
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
     RT_VIDEO_ID_KhronosExtensions = 0x2F000000,
     /* *< Reserved region for introducing Vendor Extensions */
     RT_VIDEO_ID_VendorStartUnused = 0x3F000000,
     RT_VIDEO_ID_Max = 0x3FFFFFFF,

     RT_AUDIO_ID_Unused = 0x40000000,  /**< Placeholder value when coding is N/A  */
     RT_AUDIO_ID_AutoDetect,  /**< auto detection of audio format */
     RT_AUDIO_ID_PCM,         /**< Any variant of PCM coding */
     RT_AUDIO_ID_ADPCM,       /**< Any variant of ADPCM encoded data */
     RT_AUDIO_ID_AMR,         /**< Any variant of AMR encoded data */
     RT_AUDIO_ID_GSMFR,       /**< Any variant of GSM fullrate (i.e. GSM610) */
     RT_AUDIO_ID_GSMEFR,      /**< Any variant of GSM Enhanced Fullrate encoded data*/
     RT_AUDIO_ID_GSMHR,       /**< Any variant of GSM Halfrate encoded data */
     RT_AUDIO_ID_PDCFR,       /**< Any variant of PDC Fullrate encoded data */
     RT_AUDIO_ID_PDCEFR,      /**< Any variant of PDC Enhanced Fullrate encoded data */
     RT_AUDIO_ID_PDCHR,       /**< Any variant of PDC Halfrate encoded data */
     RT_AUDIO_ID_TDMAFR,      /**< Any variant of TDMA Fullrate encoded data (TIA/EIA-136-420) */
     RT_AUDIO_ID_TDMAEFR,     /**< Any variant of TDMA Enhanced Fullrate encoded data (TIA/EIA-136-410) */
     RT_AUDIO_ID_QCELP8,      /**< Any variant of QCELP 8kbps encoded data */
     RT_AUDIO_ID_QCELP13,     /**< Any variant of QCELP 13kbps encoded data */
     RT_AUDIO_ID_EVRC,        /**< Any variant of EVRC encoded data */
     RT_AUDIO_ID_SMV,         /**< Any variant of SMV encoded data */
     RT_AUDIO_ID_G711,        /**< Any variant of G.711 encoded data */
     RT_AUDIO_ID_G723,        /**< Any variant of G.723 dot 1 encoded data */
     RT_AUDIO_ID_G726,        /**< Any variant of G.726 encoded data */
     RT_AUDIO_ID_G729,        /**< Any variant of G.729 encoded data */
     RT_AUDIO_ID_AAC,         /**< Any variant of AAC encoded data */
     RT_AUDIO_ID_MP3,         /**< Any variant of MP3 encoded data */
     RT_AUDIO_ID_SBC,         /**< Any variant of SBC encoded data */
     RT_AUDIO_ID_VORBIS,      /**< Any variant of VORBIS encoded data */
     RT_AUDIO_ID_WMA,         /**< Any variant of WMA encoded data */
     RT_AUDIO_ID_RA,          /**< Any variant of RA encoded data */
     RT_AUDIO_ID_MIDI,        /**< Any variant of MIDI encoded data */
     RT_AUDIO_ID_APE = 0x50000000,
     /**< Reserved region for introducing Khronos Standard Extensions */
     RT_AUDIO_CodingKhronosExtensions = 0x6F000000,
     /**< Reserved region for introducing Vendor Extensions */
     RT_AUDIO_CodingVendorStartUnused = 0x7F000000,
     RT_AUDIO_ID_WMAV1,
     RT_AUDIO_ID_WMAV2,
     RT_AUDIO_ID_WMAPRO,
     RT_AUDIO_ID_WMALOSSLESS,
     RT_AUDIO_CodingMax = 0x7FFFFFFF
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

#define RT_VIDEO_FMT_MASK                   0x000f0000
#define RT_VIDEO_FMT_YUV                    0x00000000
#define RT_VIDEO_FMT_RGB                    0x00010000

typedef enum {
    RT_FMT_YUV420SP        = RT_VIDEO_FMT_YUV,         /* YYYY... UV...            */
    RT_FMT_YUV420SP_10BIT,
    RT_FMT_YUV422SP,                                   /* YYYY... UVUV...          */
    RT_FMT_YUV422SP_10BIT,                             ///< Not part of ABI
    RT_FMT_YUV420P,                                    /* YYYY... UUUU... VVVV     */
    RT_FMT_YUV420SP_VU,                                /* YYYY... VUVUVU...        */
    RT_FMT_YUV422P,                                    /* YYYY... UUUU... VVVV     */
    RT_FMT_YUV422SP_VU,                                /* YYYY... VUVUVU...        */
    RT_FMT_YUV422_YUYV,                                /* YUYVYUYV...              */
    RT_FMT_YUV422_UYVY,                                /* UYVYUYVY...              */
    RT_FMT_YUV400SP,                                   /* YYYY...                  */
    RT_FMT_YUV440SP,                                   /* YYYY... UVUV...          */
    RT_FMT_YUV411SP,                                   /* YYYY... UV...            */
    RT_FMT_YUV444SP,                                   /* YYYY... UVUVUVUV...      */
    RT_FMT_YUV_BUTT,
    RT_FMT_RGB565          = RT_VIDEO_FMT_RGB,         /* 16-bit RGB               */
    RT_FMT_BGR565,                                     /* 16-bit RGB               */
    RT_FMT_RGB555,                                     /* 15-bit RGB               */
    RT_FMT_BGR555,                                     /* 15-bit RGB               */
    RT_FMT_RGB444,                                     /* 12-bit RGB               */
    RT_FMT_BGR444,                                     /* 12-bit RGB               */
    RT_FMT_RGB888,                                     /* 24-bit RGB               */
    RT_FMT_BGR888,                                     /* 24-bit RGB               */
    RT_FMT_RGB101010,                                  /* 30-bit RGB               */
    RT_FMT_BGR101010,                                  /* 30-bit RGB               */
    RT_FMT_ARGB8888,                                   /* 32-bit RGB               */
    RT_FMT_ABGR8888,                                   /* 32-bit RGB               */
    RT_FMT_RGB_BUTT,
    RT_FMT_BUTT            = RT_FMT_RGB_BUTT,
} RtVideoFormat;

typedef enum {
    RT_RC_MODE_DISABLE = 0,
    RT_RC_MODE_VBR,
    RT_RC_MODE_CBR,
    RT_RC_MODE_CQP,
    RT_RC_MODE_BUTT,
} RtVideoRCMode;


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
