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

#ifndef SRC_RT_MEDIA_FFMPEG_FFADAPTERCODEC_H_
#define SRC_RT_MEDIA_FFMPEG_FFADAPTERCODEC_H_

#include "FFAdapterUtils.h"  // NOLINT
#include "RTMediaDef.h"      // NOLINT

typedef struct AudioParams {
    int freq;
    int channels;
    int64_t channel_layout;
    enum AVSampleFormat fmt;
    int frame_size;
    int bytes_per_sec;
} AudioParams;

struct FACodecContext {
    AVCodecContext  *mAvCodecCtx;
    RTTrackType      mTrackType;
    AudioParams      mAudioSrc;
    AudioParams      mAudioTgt;
    SwrContext      *mSwrCtx;
};

class RtMetaData;
class RTMediaBuffer;

FACodecContext* fa_decode_create(RtMetaData *meta);
void fa_video_decode_destroy(FACodecContext** fc);

FACodecContext* fa_video_encode_create(RtMetaData *meta);
void fa_video_encode_destroy(FACodecContext** fc);


RT_RET fa_decode_send_packet(FACodecContext* fc, RTMediaBuffer *buffer);
RT_RET fa_decode_get_frame(FACodecContext* fc, RTMediaBuffer *buffer);

RT_RET fa_encode_send_frame(FACodecContext* fc, RTMediaBuffer *buffer);
RT_RET fa_encode_get_packet(FACodecContext* fc, RTMediaBuffer *buffer);


void fa_codec_flush(FACodecContext* fc);
void fa_codec_push(FACodecContext* fc, char* buffer, UINT32 size);
void fa_codec_pull(FACodecContext* fc, char* buffer, UINT32* size);

INT32 fa_init_audio_params_from_metadata(FACodecContext *ctx, RtMetaData *meta);

#endif  // SRC_RT_MEDIA_FFMPEG_FFADAPTERCODEC_H_

