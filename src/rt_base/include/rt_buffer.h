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
 *   date: 20181207
 */

#ifndef SRC_RT_BASE_INCLUDE_RT_BUFFER_H_
#define SRC_RT_BASE_INCLUDE_RT_BUFFER_H_

#include "rt_header.h" // NOLINT

typedef void* RtBuffer;

class RtMetaData;

RtBuffer     rt_buffer_create(const char *tag, UINT32 capacity);
RtBuffer     rt_buffer_create(const char *tag, void *data, UINT32 capacity);
RtBuffer     rt_buffer_create_as_copy(const char *tag, const void *data, UINT32 capacity);
RT_RET       rt_buffer_destroy(RtBuffer *rt_buffer);
UINT8 *      rt_buffer_get_base(RtBuffer rt_buffer);
UINT8 *      rt_buffer_get_data(RtBuffer rt_buffer);
UINT32       rt_buffer_get_capacity(RtBuffer rt_buffer);
UINT32       rt_buffer_get_size(RtBuffer rt_buffer);
UINT32       rt_buffer_get_offset(RtBuffer rt_buffer);
RtMetaData * rt_buffer_get_metadata(RtBuffer rt_buffer);
RT_RET       rt_buffer_set_range(RtBuffer rt_buffer,
                                     UINT32 offset,
                                     UINT32 size);

#endif  // SRC_RT_BASE_INCLUDE_RT_BUFFER_H_

