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
 * Author: martin.cheng@rock-chips.com
 *   Date: 2018/12/26
 */

#ifndef SRC_RT_MEDIA_FFMPEG_FFADAPTERFILTER_H_
#define SRC_RT_MEDIA_FFMPEG_FFADAPTERFILTER_H_

#include "FFAdapterUtils.h"  // NOLINT

struct FAFilterContext;

FAFilterContext* fa_filter_open(const char* filter_name);
void fa_filter_close(FAFilterContext *fc);
void fa_filter_flush(FAFilterContext *fc);
void fa_filter_push(FAFilterContext *fc, char* buffer, UINT32 size);
void fa_filter_pull(FAFilterContext *fc, char* buffer, UINT32* size);

#endif  // SRC_RT_MEDIA_FFMPEG_FFADAPTERFILTER_H_

