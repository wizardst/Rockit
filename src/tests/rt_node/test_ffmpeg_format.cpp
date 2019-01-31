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
 *   date: 2018/12/11
 */

#include "rt_ffmpeg_tests.h"  // NOLINT
#include "FFAdapterFormat.h"  // NOLINT

#ifdef OS_WINDOWS
static const char* TEST_URI = "E:\\CloudSync\\low-used\\videos\\h264-1080p.mp4";
#else
static const char* TEST_URI = "h264-1080p.mp4";
#endif

void updateBestTrack(FAFormatContext* fa_ctx, RTTrackType tType) {
    INT32 bestIndex = fa_format_find_best_track(fa_ctx, tType);
    fa_format_select_track(fa_ctx, bestIndex, tType);
}

void dumpBestTrack(FAFormatContext* fa_ctx, RTTrackType tType) {
    INT32 usedIndex = 0;
    RTTrackParms tParms;
    RtMetaData*  tMeta;

    rt_memset(&tParms, 0, sizeof(RTTrackParms));
    if (usedIndex == fa_format_query_track(fa_ctx, usedIndex, tType, &tParms)) {
        rt_utils_dump_track(&tParms);

        tMeta = new RtMetaData();
        rt_medatdata_from_trackpar(tMeta, &tParms);
        rt_utils_dump_track(tMeta);

        rt_safe_delete(tMeta);
    }
}

RT_RET unit_test_ffmpeg_format(INT32 index, INT32 total) {
    FAFormatContext* fa_ctx = fa_format_open(TEST_URI, FLAG_DEMUXER);
    // find best track
    updateBestTrack(fa_ctx, RTTRACK_TYPE_VIDEO);
    updateBestTrack(fa_ctx, RTTRACK_TYPE_AUDIO);
    updateBestTrack(fa_ctx, RTTRACK_TYPE_SUBTITLE);

    // dump track info
    dumpBestTrack(fa_ctx, RTTRACK_TYPE_VIDEO);
    dumpBestTrack(fa_ctx, RTTRACK_TYPE_AUDIO);
    dumpBestTrack(fa_ctx, RTTRACK_TYPE_SUBTITLE);

    fa_format_close(fa_ctx);

    return RT_OK;
}
