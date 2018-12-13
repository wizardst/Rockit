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
 * Author: martin.cheng@rock-chips.com
 *   Date: 2018/12/18
 *   Task: @TODO ffmpeg wrapper
 */
#ifndef SRC_RT_NODE_FF_NODE_INCLUDE_FFMPEG_WRAPPER_H_
#define SRC_RT_NODE_FF_NODE_INCLUDE_FFMPEG_WRAPPER_H_

extern "C" {
#include "libavformat/avformat.h" // NOLINT
#include "libavformat/version.h" // NOLINT
#include "libavdevice/avdevice.h"
#include "libavutil/opt.h"
}

#endif  // SRC_RT_NODE_FF_NODE_INCLUDE_FFMPEG_WRAPPER_H_
