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
 * author: hery.xu@rock-chips.com
 *   date: 2018/12/10
 */

#include "rt_node.h"         // NOLINT
#include "rt_node_header.h"  // NOLINT
#include "rt_node_tests.h"   // NOLINT
#include "ff_node_demuxer.h" // NOLINT
#include "rt_metadata.h"     // NOLINT
#include "rt_metadata_key.h" // NOLINT
#include "rt_media_frame.h"  // NOLINT
#include "rt_media_packet.h"  // NOLINT
#include "ffmpeg_wrapper.h"  // NOLINT

#ifdef OS_WINDOWS
#define TEST_URI "C:\\1080_60p.TS"
#else
#define TEST_URI "h264-1080p.mp4"
#endif

RT_RET unit_test_demuxer() {
    RT_LOGD("\n");
    RT_LOGD("%s ptr = %p", __FUNCTION__, &ff_node_demuxer);

    RtMetaData * metadata = NULL;
    metadata = new RtMetaData();
    metadata->setCString(kKeyUrl, TEST_URI);

    RT_Node* demuxer =  &ff_node_demuxer;
    demuxer->init();
    demuxer->run_cmd(RT_NODE_CMD_INIT, metadata);
    demuxer->run_cmd(RT_NODE_CMD_SET_DATASOURCE, metadata);
    demuxer->run_cmd(RT_NODE_CMD_START, metadata);

    int index = 0;
    while (++index < 100) {
        RtMediaPacket rt_media_packet = rt_media_packet_create(1);
        AVPacket *packet;
        UINT32 size = 0;
        demuxer->pull(rt_media_packet, &size);
        if (size != 0) {
            RtBuffer     prt_buffer     = rt_media_packet_get_buffer(rt_media_packet);
            RtMetaData   *prt_metadata  = rt_buffer_get_metadata(prt_buffer);
            prt_metadata->findPointer(kKeyAVPacket, reinterpret_cast<void **>(&packet));
            av_packet_unref(packet);
            rt_media_packet_destroy(&rt_media_packet);
        } else {
            RtTime::sleepUs(20000);
            rt_media_packet_destroy(&rt_media_packet);
        }
    }
    demuxer->run_cmd(RT_NODE_CMD_STOP, reinterpret_cast<RtMetaData *>(NULL));
    demuxer->release();
    delete demuxer;
    demuxer = NULL;
    RT_LOGE("%s out!", __FUNCTION__);
    return RT_OK;
}

RT_RET unit_test_node_basic(INT32 index, INT32 total) {
    return unit_test_demuxer();
}


