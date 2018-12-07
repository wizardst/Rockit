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

#include "rt_media_packet.h" // NOLINT
#include "rt_buffer_tag.h" // NOLINT

struct RtMediaPacketImpl {
    RtBuffer buffer;
};

RtMediaPacket rt_media_packet_create(UINT32 capacity) {
    RtMediaPacketImpl *rt_packet_impl = NULL;
    rt_packet_impl = rt_malloc(RtMediaPacketImpl);
    CHECK_IS_NULL(rt_packet_impl);
    rt_memset(rt_packet_impl, 0, sizeof(*rt_packet_impl));

    rt_packet_impl->buffer = rt_buffer_create(TAG_MEDIA_PACKET, capacity);
    CHECK_IS_NULL(rt_packet_impl->buffer);

    return reinterpret_cast<RtMediaPacket>(rt_packet_impl);

__FAILED:
    if (rt_packet_impl) {
        rt_free(rt_packet_impl);
        rt_packet_impl = NULL;
    }
    return NULL;
}

RtMediaPacket rt_media_packet_create(void *data, UINT32 capacity) {
    RtMediaPacketImpl *rt_packet_impl = NULL;

    // check input param.
    CHECK_IS_NULL(data);

    rt_packet_impl = rt_malloc(RtMediaPacketImpl);
    CHECK_IS_NULL(rt_packet_impl);
    rt_memset(rt_packet_impl, 0, sizeof(*rt_packet_impl));

    rt_packet_impl->buffer = rt_buffer_create(TAG_MEDIA_PACKET, data, capacity);
    CHECK_IS_NULL(rt_packet_impl->buffer);

    return reinterpret_cast<RtMediaPacket>(rt_packet_impl);

__FAILED:
    if (rt_packet_impl) {
        rt_free(rt_packet_impl);
        rt_packet_impl = NULL;
    }
    return NULL;
}

RtMediaPacket rt_media_packet_create_as_copy(const void *data, UINT32 capacity) {
    RtMediaPacketImpl *rt_packet_impl = NULL;

    // check input param.
    CHECK_IS_NULL(data);

    rt_packet_impl = rt_malloc(RtMediaPacketImpl);
    CHECK_IS_NULL(rt_packet_impl);
    rt_memset(rt_packet_impl, 0, sizeof(*rt_packet_impl));

    rt_packet_impl->buffer = rt_buffer_create_as_copy(TAG_MEDIA_PACKET, data, capacity);
    CHECK_IS_NULL(rt_packet_impl->buffer);

    return reinterpret_cast<RtMediaPacket>(rt_packet_impl);

__FAILED:
    if (rt_packet_impl) {
        rt_free(rt_packet_impl);
        rt_packet_impl = NULL;
    }
    return NULL;
}

RT_RET rt_media_packet_destroy(RtMediaPacket *rt_packet) {
    RtMediaPacketImpl *rt_packet_impl = NULL;

    // check input param.
    CHECK_IS_NULL(rt_packet);
    CHECK_IS_NULL(*rt_packet);

    rt_packet_impl = reinterpret_cast<RtMediaPacketImpl *>(*rt_packet);

    CHECK_EQ(rt_buffer_destroy(&(rt_packet_impl->buffer)), RT_OK);
    if (rt_packet_impl) {
        rt_free(rt_packet_impl);
        rt_packet_impl = NULL;
    }

    return RT_OK;

__FAILED:
    if (rt_packet_impl) {
        rt_free(rt_packet_impl);
        rt_packet_impl = NULL;
    }

    return RT_ERR_UNKNOWN;
}

/*
 * object access function macro
 */
#define RT_MEDIA_PACKET_GET_ACCESSORS(type, field) \
    type rt_media_packet_get_##field(const RtMediaPacket s) \
    { \
        RtMediaPacketImpl *packet = reinterpret_cast<RtMediaPacketImpl *>(s); \
        return packet->field; \
    }

#define RT_MEDIA_PACKET_SET_ACCESSORS(type, field) \
    void rt_media_packet_set_##field(RtMediaPacket s, type v) \
    { \
        RtMediaPacketImpl *packet = reinterpret_cast<RtMediaPacketImpl *>(s); \
        packet->field = v; \
        return; \
    }

RT_MEDIA_PACKET_GET_ACCESSORS(RtMediaPacket, buffer);
