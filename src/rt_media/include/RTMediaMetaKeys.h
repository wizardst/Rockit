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
    /* media format features */
    kKeyFormatDuration   = MKTAG('f', 'd', 'u', 'r'),  // UINT64
    kKeyFormatEOS        = MKTAG('f', 'e', 'o', 's'),
    kKeyFormatUri        = MKTAG('f', 'u', 'r', 'i'),
    kKeyUserAgent        = MKTAG('u', 's', 'a', 't'),

    /* common track features*/
    kKeyCodecType        = MKTAG('c', 't', 'y', 'p'),
    kKeyCodecID          = MKTAG('c', 'c', 'i', 'd'),
    kKeyCodecProfile     = MKTAG('c', 'p', 'r', 'o'),
    kKeyCodecLevel       = MKTAG('c', 'l', 'e', 'v'),
    kKeyCodecFormat      = MKTAG('c', 'f', 'o', 'r'),
    kKeyCodecBitrate     = MKTAG('c', 'b', 'i', 't'),     // INT64
    kKeyCodecExtraData      = MKTAG('v', 'd', 'a', 't'),  // void *
    kKeyCodecExtraSize      = MKTAG('v', 's', 'i', 'z'),  // INT32
    kKeyCodecByePass     = MKTAG('c', 'b', 'p', 's'),  // INT32

    /* video track features*/
    kKeyVCodecWidth          = MKTAG('v', 'w', 'i', 'd'),
    kKeyVCodecHeight         = MKTAG('v', 'h', 'e', 'i'),
    kKeyVCodecVideoDelay     = MKTAG('v', 'v', 'd', 'e'),  // INT32
    kKeyVCodecFrameRate      = MKTAG('v', 'f', 'r', 'a'),  // INT32 23.976 x 1000
    kKeyVCodecFieldOrder     = MKTAG('v', 'f', 'i', 'e'),  // INT32
    kKeyVCodecColorRange     = MKTAG('v', 'c', 'r', 'a'),  // INT32
    kKeyVCodecColorPrimary   = MKTAG('v', 'c', 'p', 'r'),  // INT32
    kKeyVCodecColorTransfer  = MKTAG('v', 'c', 't', 'r'),  // INT32
    kKeyVCodecColorSpace     = MKTAG('v', 'c', 's', 'p'),  // INT32
    kKeyVCodecChromaLocation = MKTAG('v', 'c', 'l', 'o'),  // INT32

    /* video extra features*/
    kKeyVCodecGopSize        = MKTAG('g', 'p', 's', 'z'),  // INT32 encoder feature
    kKeyVCodecMaxBFrames     = MKTAG('m', 'b', 'f', 'm'),  // INT32 encoder feature
    kKeyVCodecRCMode         = MKTAG('v', 'r', 'c', 'm'),  // INT32 encoder feature
    kKeyVCodecQP             = MKTAG('v', 'c', 'q', 'p'),  // INT32 encoder feature

    /* audio track features*/
    kKeyACodecChanneLayout      = MKTAG('a', 'c', 'l', 't'),
    kKeyACodecChannels          = MKTAG('a', 'd', 'h', 's'),
    kKeyACodecSampleRate        = MKTAG('a', 's', 'r', 'e'),
    kKeyACodecBlockAlign        = MKTAG('a', 'e', 'b', 'n'),
    kKeyACodecFrameSize         = MKTAG('a', 'd', 'f', 'e'),
    kKeyACodecInitialPadding    = MKTAG('a', 's', 'r', 'g'),
    kKeyACodecTrailinglPadding  = MKTAG('a', 'e', 't', 'p'),
    kKeyACodecBitPerCodedSample = MKTAG('a', 'b', 'p', 'c'),

    /* subtitle track features */
    kKeySCodecLanguage      = MKTAG('s', 'l', 'a', 'n'),  // char*
    kKeySCodecEncoding      = MKTAG('s', 'e', 'n', 'g'),  // INT32

    /* RTFrame */
    kKeyFrameType        = MKTAG('f', 't', 'y', 'e'),   // INT32
    kKeyFieldOrder       = MKTAG('f', 'f', 'o', 'r'),   // INT32
    kKeyFrameW           = MKTAG('f', 'w', 'i', 'd'),   // INT32
    kKeyFrameH           = MKTAG('f', 'h', 'e', 'i'),   // INT32
    kKeyFramePts         = MKTAG('f', 'p', 't', 's'),   // INT64 AVPacket pts
    kKeyFrameEOS         = MKTAG('p', 'e', 'o', 's'),   // INT32 EOS
    kKeyDisplayW         = MKTAG('d', 'w', 'i', 'd'),   // INT32
    kKeyDisplayH         = MKTAG('d', 'h', 'e', 'i'),   // INT32

    /* RTPacket */
    kKeyPacketPtr        = MKTAG('a', 'v', 'p', 't'),   // AVPacket
    kKeyPacketPts        = MKTAG('p', 'p', 't', 's'),   // INT64 AVPacket dts
    kKeyPacketDts        = MKTAG('p', 'd', 't', 's'),   // INT64 AVPacket pts
    kKeyPacketPos        = MKTAG('p', 'p', 'o', 's'),   // INT64 AVPacket pos
    kKeyPacketSize       = MKTAG('p', 's', 'i', 'z'),   // INT32 AVPacket size
    kKeyPacketFlag       = MKTAG('p', 'f', 'l', 'g'),   // INT32 AVPacket flag
    kKeyPacketIndex      = MKTAG('p', 'i', 'd', 'x'),   // INT32 AVPacket index
    kKeyPacketData       = MKTAG('p', 'd', 'a', 't'),   // AVPacket data
    kKeyPacketEOS        = MKTAG('p', 'e', 'o', 's'),   // INT32 EOS
    kKeyPacketIsExtra    = MKTAG('p', 'i', 'e', 'e'),   // INT32 is extradata

    // memory features
    kKeyMemAlign            = MKTAG('m', 'e', 'a', 'l'),  // INT32
    kKeyMemFlags            = MKTAG('m', 'e', 'f', 'l'),  // INT32
    kKeyMemHeapMask         = MKTAG('m', 'h', 'm', 's'),  // INT32
    kKeyMemUsage            = MKTAG('m', 'e', 'u', 's'),  // INT32

    /* command options */
    kKeySeekTimeUs          = MKTAG('s', 't', 'u', 's'),  // INT64
    kKeySeekMode            = MKTAG('s', 'm', 'o', 'd'),  // INT32
};

#endif  // SRC_RT_MEDIA_INCLUDE_RTMEDIAMETAKEYS_H_


