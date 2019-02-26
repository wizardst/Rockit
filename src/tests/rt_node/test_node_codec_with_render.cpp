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
 * author: martin.cheng@rock-chips.com
 *   Date: 2019/02/25
 */

#include "rt_node_tests.h"   // NOLINT
#include "rt_metadata.h"     // NOLINT

#include "RTNodeCodec.h"     // NOLINT
#include "RTNodeDemuxer.h"   // NOLINT
#include "RTSinkDisplay.h"        // NOLINT
#include "RTNodeSinkGLES.h"       // NOLINT
#include "RTNodeSinkAWindow.h"    // NOLINT

#include "RTNodeBus.h"       // NOLINT
#include "RTMediaBuffer.h"   // NOLINT
#include "RTMediaMetaKeys.h" // NOLINT
#include "RTMediaDef.h"      // NOLINT
#include "FFAdapterUtils.h"  // NOLINT

#ifdef OS_WINDOWS
#define TEST_URI "E:\\CloudSync\\low-used\\videos\\h264-1080p.mp4"
#else
#define TEST_URI "airplay.mp4"
#endif

#define DEFAULT_WIDTH           608
#define DEFAULT_HEIGHT          1088

#define DEC_TEST_OUTPUT_FILE    "dec_output.bin"
#define ENC_TEST_OUTPUT_FILE    "enc_output.bin"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "TestCodecWithRender"


typedef enum _RT_RENDER_TYPE {
    RENDER_TYPE_AWIN,
    RENDER_TYPE_GLES,
    RENDER_TYPE_MAX,
} RT_RENDER_TYPE;

RT_RET init_decoder_meta(RtMetaData *meta) {
    CHECK_IS_NULL(meta);

    meta->setInt32(kKeyVCodecWidth,  DEFAULT_WIDTH);
    meta->setInt32(kKeyVCodecHeight, DEFAULT_HEIGHT);
    meta->setInt32(kKeyCodecID,      RT_VIDEO_ID_AVC);

__FAILED:
    return RT_ERR_UNKNOWN;
}

UINT8* allocVideoBuf2(INT32 width, INT32 height) {
    UINT8* pixels = reinterpret_cast<UINT8*>(rt_malloc_size(UINT8, width*height*3));
    RT_LOGE("pixels(%p) - %dx%d", pixels, width, height);
    return pixels;
}

RT_RET init_encoder_meta(RtMetaData *meta) {
    CHECK_IS_NULL(meta);

    meta->setInt32(kKeyVCodecWidth,     DEFAULT_WIDTH);
    meta->setInt32(kKeyVCodecHeight,    DEFAULT_HEIGHT);
    meta->setInt32(kKeyFrameW,          DEFAULT_WIDTH);
    meta->setInt32(kKeyFrameH,          DEFAULT_HEIGHT);
    meta->setInt32(kKeyCodecID,         RT_VIDEO_ID_AVC);
    meta->setInt32(kKeyCodecBitrate,    2000000);

__FAILED:
    return RT_ERR_UNKNOWN;
}

RTNode* createRenderNode(RT_RENDER_TYPE rType) {
    RTNodeStub* stub = RT_NULL;
    RTNode*     node = RT_NULL;
    switch (rType) {
      case RENDER_TYPE_GLES:
        // stub = &rt_sink_display_gles;
        break;
      case RENDER_TYPE_AWIN:
        stub = &rt_sink_display_awindow;
        break;
      default:
        break;
    }
    if (RT_NULL != stub) {
        node = stub->mCreateNode();
    }
    return node;
}

RTNode* createRTNode(RT_NODE_TYPE node_type) {
    RTNode*     node = RT_NULL;
    RTNodeStub* stub = findStub(node_type);
    if (RT_NULL != stub) {
        node = stub->mCreateNode();
    }
    return node;
}

RT_RET unit_test_node_decoder_proc() {
    UINT32         v_width   = 1280;
    UINT32         v_height  = 720;
    RTFrame        rt_frame  = {0};
    RTMediaBuffer *media_buf = RT_NULL;
    RtMetaData    *demuxer_meta = RT_NULL;
    RtMetaData    *decoder_meta = RT_NULL;
    RtMetaData    *vrender_meta = RT_NULL;

    RTNodeDemuxer *demuxer = reinterpret_cast<RTNodeDemuxer*>(createRTNode(RT_NODE_TYPE_DEMUXER));
    RTNode        *decoder = createRTNode(RT_NODE_TYPE_DECODER);
    RTNode        *vrender = createRenderNode(RENDER_TYPE_AWIN);
    if ((RT_NULL != demuxer)&&(RT_NULL != decoder)) {
        demuxer_meta = new RtMetaData();
        // decoder_meta = new RtMetaData();
        vrender_meta = new RtMetaData();

        init_decoder_meta(decoder_meta);
        demuxer_meta->setCString(kKeyFormatUri, TEST_URI);

        vrender_meta->setInt32(kKeyFrameW, v_width);
        vrender_meta->setInt32(kKeyFrameH, v_height);

        RTNodeAdapter::init(demuxer, demuxer_meta);

        INT32 v_idx  = demuxer->queryTrackUsed(RTTRACK_TYPE_VIDEO);
        RT_LOGE("RTTRACK_TYPE_VIDEO v_idx=%d", v_idx);
        decoder_meta = demuxer->queryTrackMeta(v_idx, RTTRACK_TYPE_VIDEO);

        RTNodeAdapter::init(decoder, decoder_meta);
        RTNodeAdapter::init(vrender, vrender_meta);

        RTNodeAdapter::runCmd(demuxer, RT_NODE_CMD_START, NULL);
        RTNodeAdapter::runCmd(decoder, RT_NODE_CMD_START, NULL);
        RTNodeAdapter::runCmd(vrender, RT_NODE_CMD_START, NULL);

        RTMediaBuffer *frame = RT_NULL;
        RTMediaBuffer* esPacket;

        /* faked random frame */
        rt_frame.mData = allocVideoBuf2(v_width, v_height);
        rt_frame.mSize = v_width*v_height*3;
        rt_frame.mFrameW = rt_frame.mDisplayW = v_width;
        rt_frame.mFrameH = rt_frame.mDisplayH = v_height;
        media_buf = new RTMediaBuffer(rt_frame.mData, rt_frame.mSize);
        rt_mediabuf_from_frame(media_buf, &rt_frame);

        do {
            // deqeue buffer from object pool
            RTNodeAdapter::dequeCodecBuffer(decoder, &esPacket, RT_PORT_INPUT);
            if (RT_NULL != esPacket) {
                // save es-packet to buffer
                while (RTNodeAdapter::pullBuffer(demuxer, &esPacket) != RT_OK) {
                    RtTime::sleepMs(10);
                }

                UINT8 *data = reinterpret_cast<UINT8 *>(esPacket->getData());
                UINT32 size = esPacket->getSize();
                RT_LOGD("NEW RTPacket(ptr=0x%p, size=%d)", esPacket, size);

                // TODO(nal2annaB): must convert by demux!!!! this is test codec.
                #if 1
                while (size > 0) {
                    UINT32 nal_length = (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
                    data[0] = 0;
                    data[1] = 0;
                    data[2] = 0;
                    data[3] = 1;
                    data += nal_length + 4;
                    size -= nal_length + 4;
                }
                #endif
            }
            // push es-packet to decoder
            RTNodeAdapter::pushBuffer(decoder, esPacket);

            // pull av-frame from decoder
            RTNodeAdapter::pullBuffer(decoder, &frame);

            if (frame) {
                if (frame->getStatus() == RT_MEDIA_BUFFER_STATUS_READY) {
                    RT_LOGD("NEW RTFrame(ptr=0x%p, size=%d)", frame->getData(), frame->getLength());
                    // fa_utils_yuv420_to_rgb(frame->getData(), (unsigned char*)rt_frame.mData,
                    //                        v_width, v_height);
                    vrender->pushBuffer(frame);
                }
                RTNodeAdapter::queueCodecBuffer(decoder, frame, RT_PORT_OUTPUT);
                frame = NULL;
            }

            // dump AVFrame
            RtTime::sleepMs(10);
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
        RTNodeAdapter::runCmd(vrender, RT_NODE_CMD_STOP, NULL);

        vrender->release();
        decoder->release();
        demuxer->release();
    }
    return RT_OK;
}

RT_RET unit_test_node_encoder_proc() {
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

    RTNode* encoder = createRTNode(RT_NODE_TYPE_ENCODER);
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
            RtTime::sleepMs(5);
        } while (true);

        rt_safe_delete(encoder_meta);

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


RT_RET unit_test_node_decoder_with_render(INT32 index, INT32 total) {
    return unit_test_node_decoder_proc();
}

RT_RET unit_test_node_encoder_with_render(INT32 index, INT32 total) {
    return unit_test_node_encoder_proc();
}

