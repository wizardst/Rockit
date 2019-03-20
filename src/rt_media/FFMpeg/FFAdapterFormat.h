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
#include "RTMediaData.h"

typedef enum _FC_FLAG {
    FLAG_DEMUXER,
    FLAG_MUXER,
} FC_FLAG;

struct FAFormatContext;
class RtMetaData;

// some operations for read and seek
FAFormatContext* fa_format_open(const char* uri, FC_FLAG flag = FLAG_DEMUXER);
INT32  fa_format_close(FAFormatContext* fc);
INT32  fa_format_seek_to(FAFormatContext* fc, INT32 track_id, UINT64 ts, UINT32 flags);
INT32  fa_format_packet_read(FAFormatContext* fc, void** raw_pkt);
INT32  fa_format_packet_parse(FAFormatContext* fc, void* raw_pkt, RTPacket* rt_pkt);
INT32  fa_format_packet_type(void*  raw_pkt);
INT32  fa_format_packet_free(void*  raw_pkt);
INT64  fa_format_get_duraton(FAFormatContext* fc);


// some operations for media tracks
INT32 fa_format_query_track(FAFormatContext* fc, UINT32 idx, RTTrackType tType, RTTrackParms* track);
INT32 fa_format_query_used_track(FAFormatContext* fc, RTTrackType tType);
INT32 fa_format_select_track(FAFormatContext* fc, UINT32 idx, RTTrackType tType);
INT32 fa_format_find_track(FAFormatContext* fc, RTTrackType tType);
INT32 fa_format_find_best_track(FAFormatContext* fc, RTTrackType tType);
INT32 fa_format_count_tracks(FAFormatContext* fc, RTTrackType tType);

// specified user agent is needed by some video server
void fa_format_set_user_agent(FAFormatContext* fc, const char* ua);

#endif  // SRC_RT_MEDIA_FFMPEG_FFADAPTERFORMAT_H_
