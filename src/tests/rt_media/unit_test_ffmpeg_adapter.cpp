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
 *   date: 2019/01/03
 */

#include "rt_media_tests.h"  // NOLINT

#include "FFAdapterFormat.h"

#ifdef OS_WINDOWS
static const char* TEST_URI = "E:\\CloudSync\\low-used\\videos\\h264-1080p.mp4";
#else
static const char* TEST_URI = "h264-1080p.mp4";
#endif

RT_RET unit_test_ffmpeg_adapter(INT32 index, INT32 total_index) {
    RT_LOGD("ffmpeg version: %s", fa_utils_ffmpeg_version());
    FAFormatContext* fafc = fa_format_open(TEST_URI);

    RTTrackParms track_par;
    for (UINT32 idx = 0; idx < fa_format_count_tracks(fafc, RTTRACK_TYPE_UNKNOWN); idx++) {
        fa_format_query_track(fafc, idx, RTTRACK_TYPE_UNKNOWN, &track_par);
        rt_utils_dump_track(&track_par);
    }
    RT_LOGD("found: %d v_tracks; %d a_tracks; %d s_tracks; ", \
             fa_format_count_tracks(fafc, RTTRACK_TYPE_VIDEO), \
             fa_format_count_tracks(fafc, RTTRACK_TYPE_AUDIO), \
             fa_format_count_tracks(fafc, RTTRACK_TYPE_SUBTITLE));
    return RT_OK;
}
