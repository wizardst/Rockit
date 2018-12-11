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

#include "rt_ffmpeg_tests.h" // NOLINT
extern "C" {
    #include "libavformat/avformat.h" // NOLINT
    #include "libavformat/version.h" // NOLINT
    #include "libavutil/opt.h" // NOLINT
}

#ifdef OS_WINDOWS
static TEST_URI = "E:\\CloudSync\\low-used\\videos\h264-1080p.mp4";
#else
static TEST_URI = "h264-1080p.mp4";
#endif

static void dump_media_tracks(const AVFormatContext *fmt_ctx) {
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        const AVOption *opt = NULL;
        const AVStream *st = fmt_ctx->streams[i];
        AVCodecContext *codec_ctx = st->codec;

        while (opt = av_opt_next(codec_ctx, opt)) {
            uint8_t *str;

            if (opt->type == AV_OPT_TYPE_CONST)
                continue;

            if (!strcmp(opt->name, "frame_number"))
                continue;

            if (av_opt_get(codec_ctx, opt->name, 0, &str) >= 0) {
                printf("    %s=%s\n", opt->name, str);
                av_free(str);
            }
        }
    }
}

INT32 base_test_open_format(AVFormatContext **fmt_ctx, const char *filename) {
    int err = 0;

    err = avformat_open_input(fmt_ctx, filename, NULL, NULL);
    if (err < 0) {
        RT_LOGE("Fail to avformat_open_input(uri=%s)", filename);
        goto _final_end;
    }

    // err = find_video_stream_info(*fmt_ctx, 0);
    if (err < 0) {
        RT_LOGE("Fail to find_video_stream_info(uri=%s)", filename);
        goto _final_end;
    }

_final_end:
    return err;
}

RT_RET unit_test_ffmpeg_format(INT32 index, INT32 total) {
    AVFormatContext *fmt_ctx = NULL;
    INT32 err = base_test_open_format(&fmt_ctx, "");
    if ( err >= 0 ) {
        dump_media_tracks(fmt_ctx);
        avformat_close_input(&fmt_ctx);
    }
    return RT_OK;
}
