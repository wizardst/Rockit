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
#include "RTMediaBuffer.h" // NOLINT
#include "rt_metadata.h" // NOLINT
#include "RTMediaMetaKeys.h" // NOLINT
#include "RTVideoUtils.h"   // NOLINT

extern "C" {
#include "libavformat/avformat.h" // NOLINT
#include "libavformat/version.h" // NOLINT
#include "libavdevice/avdevice.h" // NOLINT
#include "libavutil/opt.h" // NOLINT
}

#ifdef OS_WINDOWS
#define TEST_URI "E:\\CloudSync\\low-used\\videos\\h264-1080p.mp4"
#else
#define TEST_URI "airplay.mp4"
#endif

#define DEFAULT_WIDTH           608
#define DEFAULT_HEIGHT          1080

#define DEC_TEST_OUTPUT_FILE    "dec_output.bin"
#define ENC_TEST_OUTPUT_FILE    "enc_output.bin"

RT_RET init_decoder_meta(RtMetaData *meta) {
    CHECK_IS_NULL(meta);

    meta->setInt32(kKeyVCodecWidth,  DEFAULT_WIDTH);
    meta->setInt32(kKeyVCodecHeight, DEFAULT_HEIGHT);
    meta->setInt32(kKeyCodecID,      RT_VIDEO_CodingAVC);

__FAILED:
    return RT_ERR_UNKNOWN;
}

RT_RET init_encoder_meta(RtMetaData *meta) {
    CHECK_IS_NULL(meta);

    meta->setInt32(kKeyVCodecWidth,  DEFAULT_WIDTH);
    meta->setInt32(kKeyVCodecHeight, DEFAULT_HEIGHT);
    meta->setInt32(kKeyCodecID,      RT_VIDEO_CodingMPEG4);
    meta->setInt32(kKeyCodecBitrate, 5000000);

__FAILED:
    return RT_ERR_UNKNOWN;
}

RTNode* initRTNode(struct NodeBusContext* bus, RT_NODE_TYPE node_type) {
    RTNodeStub* stub     = rt_node_bus_find(bus, node_type, 0);
    RTNode*     node     = stub->mCreateNode();
    return node;
}

RT_RET unit_test_ff_node_decoder_proc(struct NodeBusContext* bus) {
    FILE *write_fd = fopen(DEC_TEST_OUTPUT_FILE, "wb");
    if (!write_fd) {
        RT_LOGE("dec_output.bin open failed!!");
    }
    RTNode* demuxer = initRTNode(bus, RT_NODE_TYPE_DEMUXER);
    RTNode* decoder = initRTNode(bus, RT_NODE_TYPE_DECODER);
    if ((RT_NULL != demuxer)&&(RT_NULL != decoder)) {
        RtMetaData *demuxer_meta = RT_NULL;
        RtMetaData *decoder_meta = RT_NULL;
        demuxer_meta = new RtMetaData();
        decoder_meta = new RtMetaData();

        init_decoder_meta(decoder_meta);
        demuxer_meta->setCString(kKeyFormatUri, TEST_URI);

        RTNodeAdapter::init(demuxer, demuxer_meta);
        RTNodeAdapter::init(decoder, decoder_meta);

        RTNodeAdapter::runCmd(demuxer, RT_NODE_CMD_START, NULL);
        RTNodeAdapter::runCmd(decoder, RT_NODE_CMD_START, NULL);

        RTMediaBuffer *frame = RT_NULL;
        RTMediaBuffer* esPacket;
        do {
            // deqeue buffer from object pool
            RTNodeAdapter::dequeCodecBuffer(decoder, &esPacket, RT_PORT_INPUT);
            if (RT_NULL != esPacket) {
                // save es-packet to buffer
                RTNodeAdapter::pullBuffer(demuxer, &esPacket);

                UINT8 *data = reinterpret_cast<UINT8 *>(esPacket->getData());
                UINT32 size = esPacket->getSize();
                RT_LOGD("data: %p size: %d", data, size);
                if (!data) {
                    RT_LOGD("eos!!!!! break");
                    break;
                }

                // TODO(nal2annaB): must convert by demux!!!! this is test codec.
                while (size > 0) {
                    UINT32 nal_length = (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
                    data[0] = 0;
                    data[1] = 0;
                    data[2] = 0;
                    data[3] = 1;
                    data += nal_length + 4;
                    size -= nal_length + 4;
                }
            }
            // push es-packet to decoder
            RTNodeAdapter::pushBuffer(decoder, esPacket);

            // pull av-frame from decoder
            RTNodeAdapter::pullBuffer(decoder, &frame);

            if (frame) {
                RT_LOGD("dec complete frame data: %p length: %d", frame->getData(), frame->getLength());
                fwrite(frame->getData(), frame->getLength(), 1, write_fd);
                fflush(write_fd);
                RTNodeAdapter::queueCodecBuffer(decoder, frame, RT_PORT_OUTPUT);
                frame = NULL;
            }

            // dump AVFrame
            RtTime::sleepMs(50);
        } while (true);

        if (demuxer_meta) {
            delete demuxer_meta;
            demuxer_meta = RT_NULL;
        }

        if (decoder_meta) {
            delete decoder_meta;
            decoder_meta = RT_NULL;
        }

        RTNodeAdapter::runCmd(decoder, RT_NODE_CMD_STOP, NULL);
        RTNodeAdapter::runCmd(demuxer, RT_NODE_CMD_STOP, NULL);
        decoder->release();
        demuxer->release();
        if (write_fd) {
            fclose(write_fd);
        }
    }
    return RT_OK;
}

RT_RET unit_test_ff_node_encoder_proc(struct NodeBusContext* bus) {
    FILE *write_fd = fopen(ENC_TEST_OUTPUT_FILE, "wb");
    if (!write_fd) {
        RT_LOGE("%s open failed!!", ENC_TEST_OUTPUT_FILE);
        return RT_ERR_UNKNOWN;
    }

    FILE *read_fd = fopen(DEC_TEST_OUTPUT_FILE, "rb");
    if (!read_fd) {
        RT_LOGE("%s open failed!!", DEC_TEST_OUTPUT_FILE);
        return RT_ERR_UNKNOWN;
    }

    RTNode* encoder = initRTNode(bus, RT_NODE_TYPE_ENCODER);
    if (RT_NULL != encoder) {
        RtMetaData *encoder_meta = RT_NULL;
        encoder_meta = new RtMetaData();

        init_encoder_meta(encoder_meta);

        RTNodeAdapter::init(encoder, encoder_meta);
        RTNodeAdapter::runCmd(encoder, RT_NODE_CMD_START, NULL);

        RTMediaBuffer *pkt = RT_NULL;
        RTMediaBuffer *frame = RT_NULL;
        RT_RET err = RT_OK;
        do {
            // deqeue buffer from object pool
            if (!frame) {
                err = RTNodeAdapter::dequeCodecBuffer(encoder, &frame, RT_PORT_INPUT);
            }
            if (err == RT_OK && frame) {
                if (fread(frame->getData(),
                          DEFAULT_HEIGHT * DEFAULT_WIDTH * 3 / 2,
                          1,
                          read_fd) < 1) {
                    RT_LOGD("read eof, break");
                    break;
                }
                fflush(read_fd);

                if (RTNodeAdapter::pushBuffer(encoder, frame) == RT_OK) {
                    frame = NULL;
                }
                err = RTNodeAdapter::pullBuffer(encoder, &pkt);
                if (err == RT_OK && pkt) {
                    RT_LOGD("encode complete pkt data: %p length: %d",
                                 pkt->getData(), pkt->getLength());
                    fwrite(pkt->getData(), pkt->getLength(), 1, write_fd);
                    fflush(write_fd);
                    RTNodeAdapter::queueCodecBuffer(encoder, pkt, RT_PORT_OUTPUT);
                    pkt = NULL;
                }
            }

            // dump AVFrame
            RtTime::sleepMs(50);
        } while (true);

        if (encoder_meta) {
            delete encoder_meta;
            encoder_meta;
        }

        RTNodeAdapter::runCmd(encoder, RT_NODE_CMD_STOP, NULL);
        encoder->release();
        if (write_fd) {
            fclose(write_fd);
        }
        if (read_fd) {
            fclose(write_fd);
        }
    }
    return RT_OK;
}


RT_RET unit_test_ff_node_decoder(INT32 index, INT32 total) {
    RT_RET err = RT_OK;

    struct NodeBusContext* bus = rt_node_bus_create();
    rt_node_bus_register_all(bus);

    err = unit_test_ff_node_decoder_proc(bus);
    rt_node_bus_destory(bus);

    return err;
}

RT_RET unit_test_ff_node_encoder(INT32 index, INT32 total) {
    RT_RET err = RT_OK;

    struct NodeBusContext* bus = rt_node_bus_create();
    rt_node_bus_register_all(bus);

    err = unit_test_ff_node_encoder_proc(bus);
    rt_node_bus_destory(bus);

    return err;
}

