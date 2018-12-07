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

#include <string.h>

#include "rt_buffer.h" // NOLINT
#include "rt_metadata.h" // NOLINT
#include "rt_buffer_tag.h" // NOLINT
#include "rt_string_utils.h" // NOLINT

struct RtBufferImpl {
    char        tag[TAG_MAX_LENGHT];
    RtMetaData *meta;
    void       *data;
    UINT32      capacity;
    UINT32      range_offset;
    UINT32      range_length;
    RT_BOOL     owns_data;
};

RtBuffer rt_buffer_create(const char *tag, UINT32 capacity) {
    RtBufferImpl *rt_buffer_impl = rt_malloc(RtBufferImpl);

    CHECK_IS_NULL(rt_buffer_impl);
    rt_memset(rt_buffer_impl, 0, sizeof(*rt_buffer_impl));

    if (NULL != tag && strlen(tag) > 0 && strlen(tag) < TAG_MAX_LENGHT) {
        rt_str_snprintf(rt_buffer_impl->tag, strlen(tag) + 1, "%s", tag);
    }
    rt_buffer_impl->range_offset = 0;
    rt_buffer_impl->owns_data    = true;
    rt_buffer_impl->data         = rt_malloc_size(UINT8, capacity);
    CHECK_IS_NULL(rt_buffer_impl->data);
    rt_buffer_impl->range_length = capacity;
    rt_buffer_impl->capacity     = capacity;
    rt_buffer_impl->meta         = new RtMetaData();
    CHECK_IS_NULL(rt_buffer_impl->meta);

    return reinterpret_cast<RtBuffer>(rt_buffer_impl);

__FAILED:
    if (rt_buffer_impl->meta) {
        delete rt_buffer_impl->meta;
        rt_buffer_impl->meta = NULL;
    }

    if (rt_buffer_impl->data) {
        rt_free(rt_buffer_impl->data);
        rt_buffer_impl->data = NULL;
    }

    if (rt_buffer_impl) {
        rt_free(rt_buffer_impl);
        rt_buffer_impl = NULL;
    }
    return NULL;
}

RtBuffer rt_buffer_create(const char *tag, void *data, UINT32 capacity) {
    RtBufferImpl *rt_buffer_impl = rt_malloc(RtBufferImpl);
    if (NULL == rt_buffer_impl) {
        RT_LOGE("malloc rt buffer failed!");
        return NULL;
    }
    CHECK_IS_NULL(rt_buffer_impl);
    rt_memset(rt_buffer_impl, 0, sizeof(*rt_buffer_impl));

    if (NULL != tag && strlen(tag) > 0 && strlen(tag) < TAG_MAX_LENGHT) {
        rt_str_snprintf(rt_buffer_impl->tag, strlen(tag) + 1, "%s", tag);
    }
    rt_buffer_impl->range_offset = 0;
    rt_buffer_impl->owns_data    = false;
    rt_buffer_impl->data         = data;
    rt_buffer_impl->range_length = capacity;
    rt_buffer_impl->capacity     = capacity;
    rt_buffer_impl->meta         = new RtMetaData();
    CHECK_IS_NULL(rt_buffer_impl->meta);

    return reinterpret_cast<RtBuffer>(rt_buffer_impl);

__FAILED:
    if (rt_buffer_impl->meta) {
        delete rt_buffer_impl->meta;
        rt_buffer_impl->meta = NULL;
    }

    if (rt_buffer_impl) {
        rt_free(rt_buffer_impl);
        rt_buffer_impl = NULL;
    }
    return NULL;
}

RtBuffer rt_buffer_create_as_copy(const char *tag, const void *data, UINT32 capacity) {
    RtBuffer rt_buffer = NULL;
    RtBufferImpl *rt_buffer_impl = NULL;
    rt_buffer = rt_buffer_create(tag, capacity);

    if (rt_buffer) {
        rt_buffer_impl = reinterpret_cast<RtBufferImpl *>(rt_buffer);
        rt_memcpy(rt_buffer_impl->data, data, capacity);
    } else {
        RT_LOGD("create rt buffer failed, size: %d", capacity);
        goto __FAILED;
    }

    return rt_buffer;

__FAILED:
    return NULL;
}

RT_RET rt_buffer_destroy(RtBuffer *rt_buffer) {
    RtBufferImpl *rt_buffer_impl = reinterpret_cast<RtBufferImpl *>(*rt_buffer);

    CHECK_IS_NULL(rt_buffer_impl);

    if (rt_buffer_impl->meta) {
        rt_buffer_impl->meta->clear();
        delete rt_buffer_impl->meta;
        rt_buffer_impl->meta = NULL;
    }

    if (rt_buffer_impl->data && rt_buffer_impl->owns_data) {
        rt_free(rt_buffer_impl->data);
        rt_buffer_impl->data = NULL;
    }

    if (rt_buffer_impl) {
        rt_free(rt_buffer_impl);
        rt_buffer_impl = NULL;
    }

    return RT_OK;

__FAILED:
    return RT_ERR_NULL_PTR;
}

UINT8 * rt_buffer_get_base(RtBuffer rt_buffer) {
    RtBufferImpl *rt_buffer_impl = reinterpret_cast<RtBufferImpl *>(rt_buffer);

    CHECK_IS_NULL(rt_buffer_impl);

    return reinterpret_cast<UINT8 *>(rt_buffer_impl->data);

__FAILED:
    return NULL;
}

UINT8 * rt_buffer_get_data(RtBuffer rt_buffer) {
    RtBufferImpl *rt_buffer_impl = reinterpret_cast<RtBufferImpl *>(rt_buffer);

    CHECK_IS_NULL(rt_buffer_impl);

    return reinterpret_cast<UINT8 *>(rt_buffer_impl->data) + rt_buffer_impl->range_offset;

__FAILED:
    return NULL;
}

UINT32 rt_buffer_get_capacity(RtBuffer rt_buffer) {
    RtBufferImpl *rt_buffer_impl = reinterpret_cast<RtBufferImpl *>(rt_buffer);

    CHECK_IS_NULL(rt_buffer_impl);

    return rt_buffer_impl->capacity;

__FAILED:
    return 0;
}

UINT32 rt_buffer_get_size(RtBuffer rt_buffer) {
    RtBufferImpl *rt_buffer_impl = reinterpret_cast<RtBufferImpl *>(rt_buffer);

    CHECK_IS_NULL(rt_buffer_impl);

    return rt_buffer_impl->range_length;

__FAILED:
    return 0;
}

UINT32 rt_buffer_get_offset(RtBuffer rt_buffer) {
    RtBufferImpl *rt_buffer_impl = reinterpret_cast<RtBufferImpl *>(rt_buffer);

    CHECK_IS_NULL(rt_buffer_impl);

    return rt_buffer_impl->range_offset;

__FAILED:
    return 0;
}

RtMetaData * rt_buffer_get_metadata(RtBuffer rt_buffer) {
    RtBufferImpl *rt_buffer_impl = reinterpret_cast<RtBufferImpl *>(rt_buffer);

    CHECK_IS_NULL(rt_buffer_impl);

    return rt_buffer_impl->meta;

__FAILED:
    return NULL;
}

RT_RET rt_buffer_set_range(
        RtBuffer rt_buffer,
        UINT32 offset,
        UINT32 size) {
    RtBufferImpl *rt_buffer_impl = reinterpret_cast<RtBufferImpl *>(rt_buffer);

    CHECK_IS_NULL(rt_buffer_impl);

    CHECK_LE(offset, rt_buffer_impl->capacity);
    CHECK_LE(offset + size, rt_buffer_impl->capacity);

    rt_buffer_impl->range_offset = offset;
    rt_buffer_impl->range_length = size;

    return RT_OK;

__FAILED:
    return RT_ERR_UNKNOWN;
}

