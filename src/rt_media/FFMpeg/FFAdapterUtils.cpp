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

#include <stdarg.h>   // NOLINT
#include "FFAdapterUtils.h"  // NOLINT
#include "rt_log.h"  // NOLINT

INT32 fa_utils_error_string(INT32 errnum, char *errbuf, UINT32 errbuf_size) {
    return av_strerror(errnum, errbuf, errbuf_size);
}

INT32 fa_utils_check_error(const INT32 errnum, const char* func_name) {
    if (errnum < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
        fa_utils_error_string(errnum, errbuf, AV_ERROR_MAX_STRING_SIZE);
        RT_LOGE("FFMPEG Fail to %s; Error[%d:%s]", func_name, errnum, errbuf);
    }
    return errnum;
}

void RT_LOG_CALLBACK(void*, int, const char*, va_list) {
    return;
}

void fa_utils_set_log_callback() {
    av_log_set_callback(RT_LOG_CALLBACK);
}

/*
 * const char *avcodec_get_name(enum AVCodecID id);
 * defined in libavcodec/avcodec.h
 */
const char* fa_utils_codec_name(UINT32 codec_id) {
    return avcodec_get_name((AVCodecID)codec_id);
}

const char* fa_utils_ffmpeg_version() {
    return av_version_info();
}
