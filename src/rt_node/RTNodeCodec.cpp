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
 *   date: 2018/07/05
 */

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "NodeDemuxer"

#include "RTNode.h" // NOLINT

INT8 node_codec_init(void *ctx) {
    return RT_ERR_UNKNOWN;
}

INT8 node_codec_release(void *ctx) {
    return RT_ERR_UNKNOWN;
}

INT8 node_codec_read(void *ctx, void *data, UINT32 *size) {
    return RT_ERR_UNKNOWN;
}

INT8 node_codec_write(void *ctx, void *data, UINT32 *size) {
    return RT_ERR_UNKNOWN;
}

INT8 node_codec_dump(void *ctx) {
    return RT_ERR_UNKNOWN;
}
