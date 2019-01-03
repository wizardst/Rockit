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

struct FACodecContext;

FACodecContext* fa_codec_open(const char* codec_name);
void fa_codec_close(FACodecContext *fc);
void fa_codec_flush(FACodecContext *fc);
void fa_codec_push(FACodecContext *fc, char* buffer, UINT32 size);
void fa_codec_pull(FACodecContext *fc, char* buffer, UINT32* size);

#endif  // SRC_RT_MEDIA_FFMPEG_FFADAPTERCODEC_H_

