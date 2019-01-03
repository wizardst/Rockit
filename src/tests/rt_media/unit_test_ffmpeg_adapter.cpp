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
    for (UINT32 idx = 0; idx < fa_format_count_tracks(fafc); idx++) {
        fa_format_track_query(fafc, idx, RT_NULL);
    }
    return RT_OK;
}
