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

#ifndef SRC_RT_MEDIA_FFMPEG_FFADAPTERFORMAT_H_
#define SRC_RT_MEDIA_FFMPEG_FFADAPTERFORMAT_H_

#include "FFAdapterUtils.h"  // NOLINT

typedef enum _FC_FLAG {
    FLAG_DEMUXER,
    FLAG_MUXER,
} FC_FLAG;

struct FAFormatContext;
class RtMetaData;
// specified user agent is needed by some video server
FAFormatContext* fa_format_open(const char* uri, FC_FLAG flag = FLAG_DEMUXER);

void   fa_format_track_query(FAFormatContext* fc, UINT32 track_id, RtMetaData* meta);
UINT32 fa_format_count_tracks(FAFormatContext* fc);
void   fa_format_close(FAFormatContext* fc);
void   fa_format_seek_to(FAFormatContext* fc, INT32 track_id, UINT64 ts, UINT32 flags);
void   fa_format_read_packet(FAFormatContext* fc, char* buffer, UINT32* size);

// specified user agent is needed by some video server
void fa_format_set_user_agent(FAFormatContext* fc, const char* ua);

#endif  // SRC_RT_MEDIA_FFMPEG_FFADAPTERFORMAT_H_
