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
 * author: rimon.xu@rock-chips.com
 *   date: 2018/12/12
 *
 * author: martin.cheng@rock-chips.com
 *   Date: 2019/01/04
 */

#include "RTNodeCodec.h" // NOLINT
#include "RTNodeBus.h" // NOLINT
#include "rt_node_tests.h" // NOLINT
#include "rt_media_frame.h" // NOLINT
#include "rt_media_packet.h" // NOLINT
#include "rt_metadata.h" // NOLINT
#include "rt_metadata_key.h" // NOLINT

extern "C" {
#include "libavformat/avformat.h" // NOLINT
#include "libavformat/version.h" // NOLINT
#include "libavdevice/avdevice.h" // NOLINT
#include "libavutil/opt.h" // NOLINT
}

#define MAX_BUFFER_SIZE (1024 * 1024)
#define BULK_SIZE 32768

class VideoBuffer {
 public:
    VideoBuffer() {
    }

    void SetBuffer(unsigned char* buffer) {
        mBuffer = buffer;
        mTotalLength = 0;
    }

    void AppendBuffer(unsigned char* buffer, int len) {
        memcpy(mBuffer + mTotalLength, buffer, len);
        mTotalLength += len;
    }

    void DisposeOneFrame(int len) {
        memmove(mBuffer, mBuffer + len, mTotalLength - len);
        mTotalLength -= len;
    }

    int SearchStartCode() {
        int count = 0;
        for (int i = 4; i < mTotalLength; i++) {
            switch (count) {
            case 0:
            case 1:
            case 2:
                if (mBuffer[i] == 0) {
                    count++;
                } else {
                    count = 0;
                }
                break;
            case 3:
                if (mBuffer[i] == 1) {
                    return i - 3;
                } else {
                    count = 0;
                }
                break;
            default:
                break;
            }
        }

        return 0;
    }

    unsigned char* GetBuffer() {
        return mBuffer;
    }

 private:
    unsigned char* mBuffer;
    int mTotalLength;
};

RT_RET node_init_meta(RtMetaData *meta) {
    CHECK_IS_NULL(meta);

    meta->setInt32(kKeyWidth, 352);
    meta->setInt32(kKeyHeight, 288);
    meta->setInt32(kKeyCodingType, RT_VIDEO_CodingAVC);

__FAILED:
    return RT_ERR_UNKNOWN;
}

RTNode* initRTNode(struct NodeBusContext* bus, RT_NODE_TYPE node_type) {
    RTNodeStub* stub     = rt_node_bus_find(bus, node_type, 0);
    RTNode*     node     = stub->mCreateNode();
    RtMetaData *nodeMeta = new RtMetaData();
    node_init_meta(nodeMeta);
    RTNodeAdapter::init(node, nodeMeta);
    // nodeMeta->clear();
    delete nodeMeta;
    nodeMeta = RT_NULL;
    return node;
}

RT_RET unit_test_ff_node_decoder_proc(struct NodeBusContext* bus) {
    RTNode* demuxer = initRTNode(bus, RT_NODE_TYPE_DEMUXER);
    RTNode* decoder = initRTNode(bus, RT_NODE_TYPE_DECODER);
    if ((RT_NULL != demuxer)&&(RT_NULL != decoder)) {
        RTNodeAdapter::runCmd(demuxer, RT_NODE_CMD_START, NULL);
        RTNodeAdapter::runCmd(decoder, RT_NODE_CMD_START, NULL);

        RTMediaBuffer* esPacket;
        do {
            // deqeue buffer from object pool
            RTNodeAdapter::dequePoolBuffer(decoder, &esPacket);
            if (RT_NULL != esPacket) {
                // save es-packet to buffer
                RTNodeAdapter::pullBuffer(demuxer, esPacket);

                // push es-packet to decoder
                RTNodeAdapter::pushBuffer(decoder, esPacket);

                // pull av-frame from decoder
                RTNodeAdapter::pullBuffer(decoder, NULL);

                // dump AVFrame
                RtTime::sleepMs(50);
            }
        } while (true);

        RTNodeAdapter::runCmd(decoder, RT_NODE_CMD_STOP, NULL);
        RTNodeAdapter::runCmd(demuxer, RT_NODE_CMD_STOP, NULL);
        decoder->release();
        demuxer->release();
    }
    return RT_OK;
}

RT_RET unit_test_ff_node_decoder(INT32 index, INT32 total) {
    RT_RET ret = RT_OK;

    struct NodeBusContext* bus = rt_node_bus_create();
    rt_node_bus_register_all(bus);

    ret = unit_test_ff_node_decoder_proc(bus);
    rt_node_bus_destory(bus);

    return ret;
}

