/*
 * Copyright 2018 Rockchip Electronics Co. LTD
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
 *   date: 2018/07/05
 */

#ifndef SRC_RT_NODE_INCLUDE_RT_NODE_CODEC_H_
#define SRC_RT_NODE_INCLUDE_RT_NODE_CODEC_H_

#include "rt_node.h" // NOLINT

#ifdef  __cplusplus
extern "C" {
#endif

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
    RT_VIDEO_CodingUnused,             /**< Value when coding is N/A */
    RT_VIDEO_CodingAutoDetect,         /**< Autodetection of coding type */
    RT_VIDEO_CodingMPEG2,              /**< AKA: H.262 */
    RT_VIDEO_CodingH263,               /**< H.263 */
    RT_VIDEO_CodingMPEG4,              /**< MPEG-4 */
    RT_VIDEO_CodingWMV,                /**< Windows Media Video (WMV1,WMV2,WMV3)*/
    RT_VIDEO_CodingRV,                 /**< all versions of Real Video */
    RT_VIDEO_CodingAVC,                /**< H.264/AVC */
    RT_VIDEO_CodingMJPEG,              /**< Motion JPEG */
    RT_VIDEO_CodingVP8,                /**< VP8 */
    RT_VIDEO_CodingVP9,                /**< VP9 */
    RT_VIDEO_CodingHEVC,               /**< ITU H.265/HEVC */
    RT_VIDEO_CodingDolbyVision,        /**< Dolby Vision */
    RT_VIDEO_CodingImageHEIC,          /**< HEIF image encoded with HEVC */
    RT_VIDEO_CodingVC1 = 0x01000000,   /**< Windows Media Video (WMV1,WMV2,WMV3)*/
    RT_VIDEO_CodingFLV1,               /**< Sorenson H.263 */
    RT_VIDEO_CodingDIVX3,              /**< DIVX3 */
    RT_VIDEO_CodingVP6,
    RT_VIDEO_CodingAVSPLUS,            /**< AVS+ */
    RT_VIDEO_CodingAVS,                /**< AVS profile=0x20 */
    RT_VIDEO_CodingKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */
    RT_VIDEO_CodingVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    RT_VIDEO_CodingMax = 0x7FFFFFFF
}RtCodingType;


#ifdef  __cplusplus
}
#endif

#endif  // SRC_RT_NODE_INCLUDE_RT_NODE_CODEC_H_

