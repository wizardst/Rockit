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
 * author: Rimon.Xu@rock-chips.com
 *   date: 20181210
 */

#ifndef SRC_RT_BASE_INCLUDE_RT_MEDIA_FRAME_H_
#define SRC_RT_BASE_INCLUDE_RT_MEDIA_FRAME_H_

#include "rt_header.h" // NOLINT

typedef void* RtMediaFrame;

enum RtMediaFrameStatus {
    RT_MEDIA_FRAME_STATUS_UNKONN = 0,
    RT_MEDIA_FRAME_STATUS_READY,
};

RtMediaFrame rt_media_frame_create(UINT32 capacity);
RtMediaFrame rt_media_frame_create(void *data, UINT32 capacity);
RtMediaFrame rt_media_frame_create_as_copy(const void *data, UINT32 capacity);
RT_RET       rt_media_frame_destroy(RtMediaFrame *rt_frame);

RtBuffer     rt_media_frame_get_buffer(const RtMediaFrame rt_frame);
void     rt_media_frame_set_status(const RtMediaFrame rt_frame, RtMediaFrameStatus status);
RtMediaFrameStatus rt_media_frame_get_status(const RtMediaFrame rt_frame);

#endif  // SRC_RT_BASE_INCLUDE_RT_MEDIA_FRAME_H_

