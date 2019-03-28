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
 * author: hery.xu@rock-chips.com
 *   date: 2018/12/10
 *
 * author: martin.cheng@rock-chips.com
 *   Date: 2019/01/04
 */

#include "FFNodeDemuxer.h" // NOLINT
#include "FFMPEGAdapter.h" // NOLINT

#include "rt_node_tests.h"   // NOLINT
#include "rt_metadata.h"     // NOLINT
#include "RTMediaMetaKeys.h" // NOLINT

#ifdef OS_WINDOWS
#define TEST_URI "E:\\CloudSync\\low-used\\videos\\h264-1080p.mp4"
#else
#define TEST_URI "h264-1080p.mp4"
#endif

RT_RET unit_test_ff_node_demuxer(INT32 index, INT32 total) {
    UINT32         count     = 0;
    RT_RET         rt_err    = RT_OK;
    RTPacket       rt_pkt    = {0};
    RTMediaBuffer* rt_buf    = RT_NULL;
    RtMetaData*    node_meta = RT_NULL;

    RT_LOGD("nodestub(ptr=%p)", &ff_node_demuxer);

    node_meta = new RtMetaData();
    node_meta->setCString(kKeyFormatUri, TEST_URI);

    RTNodeStub* nodestub =  &ff_node_demuxer;
    RTNode* demuxer = nodestub->mCreateNode();

    RTNodeAdapter::init(demuxer, node_meta);
    RTNodeAdapter::runCmd(demuxer, RT_NODE_CMD_PREPARE, node_meta);
    RTNodeAdapter::runCmd(demuxer, RT_NODE_CMD_START, node_meta);

    rt_buf = new RTMediaBuffer(RT_NULL, 0);
    while (count < 100) {
        rt_buf->reset();
        rt_buf->getMetaData()->setInt32(kKeyCodecType,
                (count%2 == 0) ? RTTRACK_TYPE_VIDEO : RTTRACK_TYPE_AUDIO);
        rt_err = demuxer->pullBuffer(&rt_buf);
        if (rt_err == RT_OK) {
            rt_mediabuf_goto_packet(rt_buf, &rt_pkt);
            RT_LOGD(" --> RTPacket(ptr=0x%p, size=%d) MediaBuffer=0x%p", \
                      rt_pkt.mRawPtr, rt_pkt.mSize, rt_buf);
            rt_utils_packet_free(&rt_pkt);
            rt_buf->getMetaData()->clear();
        }
        count++;
        RtTime::sleepUs(5*1000llu);
    }
    RTNodeAdapter::runCmd(demuxer, RT_NODE_CMD_STOP, reinterpret_cast<RtMetaData *>(NULL));
    RTNodeAdapter::release(demuxer);

    delete demuxer;
    demuxer  =  RT_NULL;

    delete rt_buf;
    rt_buf =  RT_NULL;

    return RT_OK;
}

