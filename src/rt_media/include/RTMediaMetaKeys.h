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
 *   date: 20181226
 * Module: audio/video/subtitle track's keys for metadata.
 */

#ifndef SRC_RT_MEDIA_INCLUDE_RTMEDIAMETAKEYS_H_
#define SRC_RT_MEDIA_INCLUDE_RTMEDIAMETAKEYS_H_

enum {
    /* common param */
    kKeyWidth       = MKTAG('w', 'i', 't', 'h'),
    kKeyHeight      = MKTAG('h', 'i', 'g', 'h'),

    /* decode and encode */
    kKeyCodingType  = MKTAG('c', 'd', 't', 'p'),

    /* media common */
    kKeyTimeStamps  = MKTAG('t', 's', 't', 'p'),
    kKeyFrameWidth  = MKTAG('f', 'w', 'i', 't'),
    kKeyFrameHeight = MKTAG('f', 'h', 'i', 't'),
    kKeyEOS         = MKTAG('k', 'e', 'o', 's'),
    kKeyUrl         = MKTAG('k', 'u', 'r', 'l'),
    kKeyAVPacket    = MKTAG('a', 'v', 'p', 't'),   // AVPacket
    kKeyAVPktPts    = MKTAG('p', 'p', 't', 's'),   // AVPacket dts
    kKeyAVPktDts    = MKTAG('p', 'd', 't', 's'),   // AVPacket pts
    kKeyAVPktSize   = MKTAG('p', 's', 'i', 'z'),   // AVPacket size
    kKeyAVPktFlag   = MKTAG('p', 'f', 'l', 'g'),   // AVPacket flag
    kKeyAVPktIndex  = MKTAG('p', 'i', 'd', 'x'),   // AVPacket index
    kKeyAVPktData   = MKTAG('p', 'd', 'a', 't'),  // AVPacket data
    kKeyExtraData   = MKTAG('e', 'x', 'd', 'a'),  // void *
    kKeyExtraDataSize = MKTAG('e', 'x', 'd', 's'),  // INT32
    kKeyBitrate     = MKTAG('b', 't', 'r', 't'),  // INT32
    kKeyFramerate   = MKTAG('f', 'n', 'r', 't'),  // INT32
    kKeyPixFormat   = MKTAG('p', 'f', 'r', 'm'),  // INT32

    // media track
    kKeyTrackType      = MKTAG('t', 't', 'p', 'e'),  // INT32
    kKeyTrackSubType   = MKTAG('t', 's', 't', 'y'),  // INT32
    kKeyTrackCodecID   = MKTAG('t', 'c', 'i', 'd'),  // INT32
    kKeyTrackDuration  = MKTAG('t', 'd', 't', 'n'),  // UINT64
    kKeyTrackLanguage  = MKTAG('t', 'l', 'a', 'n'),  // char*

    // video track features
    kKeyTVideoFrameWidth    = MKTAG('v', 'f', 'w', 'h'),  // INT32
    kKeyTVideoFrameHeight   = MKTAG('v', 'f', 'h', 't'),  // INT32
    kKeyTVideoDisplayWidth  = MKTAG('v', 'd', 'w', 'h'),  // INT32
    kKeyTVideoDisplayHeight = MKTAG('v', 'd', 'h', 't'),  // INT32
    kKeyTVideoAlign         = MKTAG('v', 'a', 'l', 'n'),  // INT32
    kKeyTVideoBitrate       = MKTAG('v', 'b', 'i', 'e'),  // INT32
    kKeyTVideoFrameRate     = MKTAG('v', 'f', 'r', 'a'),  // INT32 23.976 x 1000
    kKeyTVideoColorSpace    = MKTAG('v', 'c', 's', 'e'),  // INT32
    kKeyTVideoBitDepth      = MKTAG('v', 'b', 'e', 'h'),  // INT32
    kKeyTVideoScanType      = MKTAG('v', 's', 't', 'e'),  // INT32

    // audio track features
    kKeyTAudioBitrate       = MKTAG('a', 'c', 's', 'e'),  // INT32
    kKeyTAudioSample        = MKTAG('a', 'b', 'e', 'h'),  // INT32
    kKeyTAudioChannelNB     = MKTAG('a', 'c', 'n', 'b'),  // INT32
    kKeyTAudioBitDepth      = MKTAG('a', 'c', 'n', 'b'),  // INT32

    // subtitle track features
    kKeyTSubtitleEncoding   = MKTAG('s', 'e', 'n', 'g'),  // INT32

    // encode features
    kKeyGopSize             = MKTAG('g', 'p', 's', 'z'),  // INT32
    kKeyMaxBFrames          = MKTAG('m', 'b', 'f', 'm'),  // INT32
};

#endif  // SRC_RT_MEDIA_INCLUDE_RTMEDIAMETAKEYS_H_


