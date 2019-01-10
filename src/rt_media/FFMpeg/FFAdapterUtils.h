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

#ifndef SRC_RT_MEDIA_FFMPEG_FFADAPTERUTILS_H_
#define SRC_RT_MEDIA_FFMPEG_FFADAPTERUTILS_H_

#include "rt_header.h"        // NOLINT
#include "RTVideoUtils.h"     // NOLINT

extern "C" {
    #include "libavformat/avformat.h" // NOLINT
    #include "libavformat/version.h"  // NOLINT
    #include "libavutil/avutil.h"     // NOLINT
    #include "libavutil/opt.h"     // NOLINT
}

INT32       fa_utils_error_string(INT32 errnum, char *errbuf, UINT32 errbuf_size);
INT32       fa_utils_check_error(const INT32 errnum, const char* func_name);
void        fa_utils_set_log_callback();
const char* fa_utils_codec_name(UINT32 codec_id);
const char* fa_utils_ffmpeg_version();

RtCodingType fa_utils_find_codectype_from_codecid(UINT32 codec_id);
AVCodecID    fa_utils_find_codecid_from_codectype(RtCodingType coding);


#endif  // SRC_RT_MEDIA_FFMPEG_FFADAPTERUTILS_H_

