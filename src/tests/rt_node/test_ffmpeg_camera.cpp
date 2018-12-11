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
    #include "libavdevice/avdevice.h"
    #include "libavutil/opt.h"
}

const char *pCameraName = "video=HP Wide Vision HD Camera";
const char *pCameraRes  = "640x480";

INT32 rt_utils_list_camera_options(AVFormatContext **ifc) {
    INT32 err = 0;
    AVDictionary* options = NULL;
    av_dict_set(&options, "list_devices", "true", 0);
    // av_dict_set(&options, "list_options", "true", 0);
    AVInputFormat *iformat = av_find_input_format("dshow");
    err = avformat_open_input(ifc, NULL, iformat, &options);

    av_dict_free(&options);
    avformat_close_input(ifc);
    RT_LOGE("avformat_close_input fmt_ctx=%p done.", *ifc);

    return err;
}

INT32 rt_utils_open_format(AVFormatContext **pFmtCtx, RT_BOOL need_decoder = RT_FALSE) {
    INT32 err = 0;
    INT32 video_index = -1;
    AVDictionary  *options   = NULL;
    AVInputFormat *pInputFmt = NULL;
    AVCodecParameters *pCodecPar = NULL;
    AVCodecContext    *pCodecCtx = RT_NULL;

    avdevice_register_all();

    rt_utils_list_camera_options(pFmtCtx);

    av_dict_set(&options, "video_size", pCameraRes, 0);
    // av_dict_set(&options, "list_options", "true", 0);
    pInputFmt = av_find_input_format("dshow");
    err = avformat_open_input(pFmtCtx, pCameraName, pInputFmt, &options);
    if (err < 0) {
        RT_LOGE("Fail to avformat_open_input(uri=%s)", pCameraName);
        goto _final_end;
    }

    av_dump_format(*pFmtCtx, 0, pCameraName, 0);

    err = avformat_find_stream_info(*pFmtCtx, NULL);
    if (err < 0) {
        RT_LOGE("Fail to avformat_find_stream_info(uri=%s)", pCameraName);
        goto _final_end;
    }

    video_index = av_find_best_stream(*pFmtCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (video_index < 0) {
        RT_LOGE("Fail to av_find_best_stream(uri=%d)", pCameraName);
        goto _final_end;
    }

    pCodecPar = (*pFmtCtx)->streams[video_index]->codecpar;
    if (RT_TRUE == need_decoder) {
        AVCodec *pCodec = avcodec_find_decoder(pCodecPar->codec_id);
        if (RT_NULL == pCodec) {
            RT_LOGE("Fail to avcodec_find_decoder(%s)", avcodec_get_name(pCodecPar->codec_id));
            goto _final_end;
        }

        pCodecCtx = avcodec_alloc_context3(pCodec);
        err = avcodec_open2(pCodecCtx, pCodec, NULL);
        if (err < 0) {
            RT_LOGE("Fail to avcodec_open2(codec_id=%d)", pCodecPar->codec_id);
            goto _final_end;
        }
    }

_final_end:
    av_dict_free(&options);
    return err;
}

RT_RET unit_test_ffmpeg_camera(INT32 index, INT32 total) {
    AVFormatContext *fmt_ctx = NULL;
    rt_utils_open_format(&fmt_ctx, RT_FALSE);
    INT32 err = 0, cnt = 0;
    AVPacket pkt;

    av_init_packet(&pkt);
    do {
        err = av_read_frame(fmt_ctx, &pkt);
        RT_LOGE("av_read_frame pkt(ptr=%p, size=%06d, cnt=%03d)",
                 pkt.data, pkt.size, cnt);
        av_packet_unref(&pkt);
        av_init_packet(&pkt);
    }while ((err >= 0) && (cnt++ < 100));

    avformat_close_input(&fmt_ctx);
    RT_LOGE("avformat_close_input fmt_ctx=%p done.", fmt_ctx);
    return RT_OK;
}
