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
    UINT32         count    = 0;
    AVPacket*      avPacket = RT_NULL;
    RtMetaData*    metaData = RT_NULL;
    RTMediaBuffer* rtPacket = RT_NULL;

    RT_LOGD("nodestub(ptr=%p)", &ff_node_demuxer);

    RtMetaData * metadata = RT_NULL;
    metadata = new RtMetaData();
    metadata->setCString(kKeyUrl, TEST_URI);

    RTNodeStub* nodestub =  &ff_node_demuxer;
    RTNode* demuxer = nodestub->mCreateNode();

    RTNodeAdapter::init(demuxer, metadata);
    // RTNodeAdapter::runCmd(demuxer, RT_NODE_CMD_INIT, metadata);
    RTNodeAdapter::runCmd(demuxer, RT_NODE_CMD_START, metadata);

    rtPacket = new RTMediaBuffer(RT_NULL, 0);
    while (++count < 100) {
        rtPacket->reset();
        RTNodeAdapter::pullBuffer(demuxer, &rtPacket);
        if (rtPacket->getSize() > 0) {
            metaData = rtPacket->getMetaData();
            metaData->findPointer(kKeyAVPacket, reinterpret_cast<void **>(&avPacket));
            av_packet_unref(avPacket);
            // RtTime::sleepUs(21000);
        }
    }
    RTNodeAdapter::runCmd(demuxer, RT_NODE_CMD_STOP, reinterpret_cast<RtMetaData *>(NULL));
    RTNodeAdapter::release(demuxer);

    delete demuxer;
    demuxer  =  RT_NULL;

    delete rtPacket;
    rtPacket =  RT_NULL;

    return RT_OK;
}

