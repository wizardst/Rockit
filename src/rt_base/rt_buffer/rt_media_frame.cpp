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

#include "rt_media_frame.h" // NOLINT
#include "rt_buffer_tag.h" // NOLINT

struct RtMediaFrameImpl {
    RtMediaFrameStatus status;
    RtBuffer buffer;
};

RtMediaFrame rt_media_frame_create(UINT32 capacity) {
    RtMediaFrameImpl *rt_frame_impl = NULL;
    rt_frame_impl = rt_malloc(RtMediaFrameImpl);
    CHECK_IS_NULL(rt_frame_impl);
    rt_memset(rt_frame_impl, 0, sizeof(*rt_frame_impl));

    rt_frame_impl->buffer = rt_buffer_create(const_cast<char *>(TAG_MEDIA_FRAME), capacity);
    CHECK_IS_NULL(rt_frame_impl->buffer);

    return reinterpret_cast<RtMediaFrame>(rt_frame_impl);

__FAILED:
    if (rt_frame_impl) {
        rt_free(rt_frame_impl);
        rt_frame_impl = NULL;
    }
    return NULL;
}

RtMediaFrame rt_media_frame_create(void *data, UINT32 capacity) {
    RtMediaFrameImpl *rt_frame_impl = NULL;

    // check input param.
    CHECK_IS_NULL(data);

    rt_frame_impl = rt_malloc(RtMediaFrameImpl);
    CHECK_IS_NULL(rt_frame_impl);
    rt_memset(rt_frame_impl, 0, sizeof(*rt_frame_impl));

    rt_frame_impl->buffer = rt_buffer_create(TAG_MEDIA_FRAME, data, capacity);
    CHECK_IS_NULL(rt_frame_impl->buffer);

    return reinterpret_cast<RtMediaFrame>(rt_frame_impl);

__FAILED:
    if (rt_frame_impl) {
        rt_free(rt_frame_impl);
        rt_frame_impl = NULL;
    }
    return NULL;
}

RtMediaFrame rt_media_frame_create_as_copy(const void *data, UINT32 capacity) {
    RtMediaFrameImpl *rt_frame_impl = NULL;

    // check input param.
    CHECK_IS_NULL(data);

    rt_frame_impl = rt_malloc(RtMediaFrameImpl);
    CHECK_IS_NULL(rt_frame_impl);
    rt_memset(rt_frame_impl, 0, sizeof(*rt_frame_impl));

    rt_frame_impl->buffer = rt_buffer_create_as_copy(TAG_MEDIA_FRAME, data, capacity);
    CHECK_IS_NULL(rt_frame_impl->buffer);

    return reinterpret_cast<RtMediaFrame>(rt_frame_impl);

__FAILED:
    if (rt_frame_impl) {
        rt_free(rt_frame_impl);
        rt_frame_impl = NULL;
    }
    return NULL;
}

RT_RET rt_media_frame_destroy(RtMediaFrame *rt_frame) {
    RtMediaFrameImpl *rt_frame_impl = NULL;

    // check input param.
    CHECK_IS_NULL(rt_frame);
    CHECK_IS_NULL(*rt_frame);

    rt_frame_impl = reinterpret_cast<RtMediaFrameImpl *>(*rt_frame);

    CHECK_EQ(rt_buffer_destroy(&(rt_frame_impl->buffer)), RT_OK);
    if (rt_frame_impl) {
        rt_free(rt_frame_impl);
        rt_frame_impl = NULL;
    }

    return RT_OK;

__FAILED:
    if (rt_frame_impl) {
        rt_free(rt_frame_impl);
        rt_frame_impl = NULL;
    }

    return RT_ERR_UNKNOWN;
}

/*
 * object access function macro
 */
#define RT_MEDIA_FRAME_GET_ACCESSORS(type, field) \
    type rt_media_frame_get_##field(const RtMediaFrame s) \
    { \
        RtMediaFrameImpl *frame = reinterpret_cast<RtMediaFrameImpl *>(s); \
        return frame->field; \
    }

#define RT_MEDIA_FRAME_SET_ACCESSORS(type, field) \
    void rt_media_frame_set_##field(RtMediaFrame s, type v) \
    { \
        RtMediaFrameImpl *frame = reinterpret_cast<RtMediaFrameImpl *>(s); \
        frame->field = v; \
    }

RT_MEDIA_FRAME_GET_ACCESSORS(RtBuffer, buffer);
RT_MEDIA_FRAME_SET_ACCESSORS(RtMediaFrameStatus, status);
RT_MEDIA_FRAME_GET_ACCESSORS(RtMediaFrameStatus, status);
