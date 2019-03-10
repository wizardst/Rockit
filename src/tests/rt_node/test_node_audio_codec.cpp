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

#include "rt_node_tests.h"   // NOLINT
#include "rt_metadata.h"     // NOLINT

#include "RTNodeCodec.h"     // NOLINT
#include "RTNodeDemuxer.h"   // NOLINT
#include "RTNodeBus.h"       // NOLINT
#include "RTMediaBuffer.h"   // NOLINT
#include "RTMediaMetaKeys.h" // NOLINT
#include "RTMediaDef.h"      // NOLINT
#include "FFAdapterUtils.h"  // NOLINT
#include "RTNodeAudioSink.h"

#ifdef OS_WINDOWS
// #define TEST_URI "E:\\CloudSync\\low-used\\videos\\h264-1080p.mp4"
#else
//  #define TEST_URI "monkey.ape"
//  #define TEST_URI "llama_aac_audio.mp4"
//  #define TEST_URI "bear.mp3"
#endif

#define AUDIO_VOL_TEST

#define TEST_URI "test.mp3"

#define DEC_TEST_OUTPUT_FILE    "output.pcm"

RTNode* createRTNode(RT_NODE_TYPE node_type, BUS_LINE_TYPE lType) {
    RTNodeStub* stub     = findStub(node_type, lType);
    RTNode*     node     = stub->mCreateNode();
    return node;
}

RT_VOID audioCodecBufferCallback(RTNode* pNode, RTMediaBuffer* data) {
    RTNodeAdapter::queueCodecBuffer(pNode, data, RT_PORT_OUTPUT);
}

RT_RET unit_test_node_audio_decoder_proc() {
    FILE          *write_fd = 0;
    RTMediaBuffer *media_buf = RT_NULL;
    RtMetaData    *demuxer_meta = RT_NULL;
    RtMetaData    *decoder_meta = RT_NULL;
    RtMetaData    *vrender_meta = RT_NULL;

    write_fd = fopen(DEC_TEST_OUTPUT_FILE, "wb");
    if (!write_fd) {
        RT_LOGE("%s open failed!!", DEC_TEST_OUTPUT_FILE);
        return RT_ERR_UNKNOWN;
    }

    RTNodeDemuxer *demuxer = reinterpret_cast<RTNodeDemuxer*>(createRTNode(RT_NODE_TYPE_DEMUXER, BUS_LINE_ROOT));
    RTNode        *decoder = createRTNode(RT_NODE_TYPE_DECODER, BUS_LINE_AUDIO);
    RTNodeAudioSink    *audiosink = reinterpret_cast<RTNodeAudioSink*>(createRTNode(RT_NODE_TYPE_SINK, BUS_LINE_AUDIO));

    if ((RT_NULL != demuxer)&&(RT_NULL != decoder)) {
        demuxer_meta = new RtMetaData();
        demuxer_meta->setCString(kKeyFormatUri, TEST_URI);

        RTNodeAdapter::init(demuxer, demuxer_meta);

        INT32 audio_idx  = demuxer->queryTrackUsed(RTTRACK_TYPE_AUDIO);
        INT32 video_idx  = demuxer->queryTrackUsed(RTTRACK_TYPE_VIDEO);
        RT_LOGE("RTTRACK_TYPE_AUDIO audio_idx:%d", audio_idx);
        RT_LOGE("RTTRACK_TYPE_VIDEO video_idx:%d", video_idx);
        decoder_meta = demuxer->queryTrackMeta(0, RTTRACK_TYPE_AUDIO);

        audiosink->queueCodecBuffer = audioCodecBufferCallback;
        audiosink->callback_ptr = decoder;

        RTNodeAdapter::init(decoder, decoder_meta);
        RTNodeAdapter::init(audiosink, NULL);

        RTNodeAdapter::runCmd(demuxer, RT_NODE_CMD_START, NULL);
        RTNodeAdapter::runCmd(decoder, RT_NODE_CMD_START, NULL);
        RTNodeAdapter::runCmd(audiosink, RT_NODE_CMD_START, NULL);

        RTMediaBuffer *frame = RT_NULL;
        RTMediaBuffer* esPacket;
        INT32 counter = 0;
        bool is_muted = false;
        INT32 vol = 0;

        do {
            // deqeue buffer from object pool
            RTNodeAdapter::dequeCodecBuffer(decoder, &esPacket, RT_PORT_INPUT);
            if (RT_NULL != esPacket) {
                RT_BOOL got_pkt = RT_FALSE;
                while (!got_pkt) {
                    // save es-packet to buffer
                    while (RTNodeAdapter::pullBuffer(demuxer, &esPacket) != RT_OK) {
                        RtTime::sleepMs(10);
                    }
                    INT32 track_index = 0;
                    esPacket->getMetaData()->findInt32(kKeyPacketIndex, &track_index);

                 //   RT_LOGD("track_index: %d, audio_idx: %d", track_index, audio_idx);
                    if (track_index == audio_idx) {
                        UINT8 *data = reinterpret_cast<UINT8 *>(esPacket->getData());
                        UINT32 size = esPacket->getSize();
                     //   RT_LOGD("NEW audio MediaBuffer(ptr=0x%p, size=%d)", esPacket, size);
                        got_pkt = RT_TRUE;
                    } else {
                        /* pass other packet */
                        INT32 eos = 0;
                        esPacket->getMetaData()->findInt32(kKeyFrameEOS, &eos);
                        if (eos) {
                            RT_LOGD("receive eos , break");
                            break;
                        }
                        continue;
                    }
                }
             } else {
                 continue;
             }
            // push es-packet to decoder
            RTNodeAdapter::pushBuffer(decoder, esPacket);

            // pull av-frame from decoder
            RTNodeAdapter::pullBuffer(decoder, &frame);

            if (frame) {
                if (frame->getStatus() == RT_MEDIA_BUFFER_STATUS_READY) {
                  //  RT_LOGD("NEW Frame(ptr=0x%p, size=%d)", frame->getData(), frame->getLength());
                    char* data = reinterpret_cast<char *>(frame->getData());

                    if (NULL != write_fd) {
                    //    fwrite(frame->getData(), frame->getLength(), 1, write_fd);
                    //    fflush(write_fd);
                    }
                    RTNodeAdapter::pushBuffer(audiosink, frame);
#ifdef AUDIO_VOL_TEST
                    RT_LOGD("unit_test_node_audio_decoder_proc test counter=%d", counter);

                    if (counter == 100) {
                        is_muted = audiosink->GetMute();
                        vol = audiosink->GetVolume();
                        RT_LOGD("get vol = %d,counter = %d", vol, counter);
                        RT_LOGD("get is_muted = %d,counter = %d", is_muted, counter);
                        audiosink->SetVolume(40);
                    }

                    if (counter == 400) {
                        vol = audiosink->GetVolume();
                        RT_LOGD("get vol = %d,counter = %d", vol, counter);
                        audiosink->Mute(true);
                        is_muted = audiosink->GetMute();
                        vol = audiosink->GetVolume();
                        RT_LOGD("get vol = %d,counter = %d", vol, counter);
                        RT_LOGD("get is_muted = %d,counter = %d", is_muted, counter);
                    }

                    if (counter == 600) {
                        audiosink->Mute(false);
                        is_muted = audiosink->GetMute();
                        vol = audiosink->GetVolume();
                        RT_LOGD("get vol = %d,counter = %d", vol, counter);
                        RT_LOGD("get is_muted = %d,counter = %d", is_muted, counter);
                    }

                    if (counter == 800) {
                        audiosink->Mute(true);
                        RT_LOGD("get is_muted = %d,counter = %d", is_muted, counter);
                    }

                    if (counter == 1000) {
                        audiosink->Mute(false);
                        audiosink->SetVolume(10);
                        is_muted = audiosink->GetMute();
                        vol = audiosink->GetVolume();
                        RT_LOGD("get vol = %d,counter = %d", vol, counter);
                        RT_LOGD("get is_muted = %d,counter = %d", is_muted, counter);
                    }

                    counter++;
#endif
                }
              //  RTNodeAdapter::queueCodecBuffer(decoder, frame, RT_PORT_OUTPUT);
                INT32 eos = 0;
                frame->getMetaData()->findInt32(kKeyFrameEOS, &eos);
                if (eos) {
                    RT_LOGD("receive eos , break");
                    break;
                }
                frame = NULL;
            }

            // dump AVFrame
            RtTime::sleepMs(5);
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
        RTNodeAdapter::runCmd(audiosink, RT_NODE_CMD_STOP, NULL);

        decoder->release();
        demuxer->release();
        audiosink->release();

        if (write_fd) {
            fclose(write_fd);
        }
    }
    return RT_OK;
}

RT_RET unit_test_node_audio_decoder(INT32 index, INT32 total) {
    return unit_test_node_audio_decoder_proc();
}

