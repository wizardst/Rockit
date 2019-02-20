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
 *   date: 2019/02/19
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
#include "rt_message.h"      // NOLINT
#include "rt_msg_handler.h"  // NOLINT
#include "rt_msg_looper.h"   // NOLINT
#include "RTNodeSink.h"      // NOLINT

#ifdef OS_WINDOWS
#define TEST_URI "E:\\CloudSync\\low-used\\videos\\h264-1080p.mp4"
#else
#define TEST_URI "test_720.mp4"
#endif

#define DUMP_VIDEO_OUTPUT       1
#define DUMP_AUDIO_OUTPUT       1

#define VIDEO_TEST_OUTPUT_FILE      "/data/video_output.bin"
#define AUDIO_TEST_OUTPUT_FILE      "/data/audio_output.bin"

RT_VOID simpleAudioCodecBufferCallback(RTNode* pNode, RTMediaBuffer* data) {
    RTNodeAdapter::queueCodecBuffer(pNode, data, RT_PORT_OUTPUT);
}

class SimplePlayer: public RTMsgHandler {
 public:
    SimplePlayer();
    ~SimplePlayer();
    void onMessageReceived(struct RTMessage* msg);
    RT_RET init(const char *uri);
    RT_RET start();
    RT_RET stop();
    RT_RET release();

    RT_BOOL isRunning() { return mProcRunning; }

    RT_RET process();

 private:
    RTNode *createRTNode(RT_NODE_TYPE node_type, BUS_LINE_TYPE ltype);

 private:
    struct RTMsgLooper* mLooper;
    RtMetaData *mDemuxerMeta;
    RtMetaData *mAudioDecoderMeta;
    RtMetaData *mVideoDecoderMeta;
    RTNodeDemuxer *mDemuxer;
    RTNode *mVideoDecoder;
    RTNode *mAudioDecoder;
    RTNodeSink *mAudioSink;

    INT32 mVideoStreamIndex;
    INT32 mAudioStreamIndex;

    RT_BOOL mHasVideo;
    RT_BOOL mHasAudio;

    RtThread *mProcThread;
    RT_BOOL mProcRunning;
#ifdef DUMP_VIDEO_OUTPUT
    FILE *mVideoDumpFd;
#endif
#ifdef DUMP_AUDIO_OUTPUT
    FILE *mAudioDumpFd;
#endif
};

void* simple_player_process(void* ctx) {
    SimplePlayer *player = reinterpret_cast<SimplePlayer *>(ctx);
    player->process();
    return RT_NULL;
}

SimplePlayer::SimplePlayer()
        : mHasVideo(RT_FALSE),
          mHasAudio(RT_FALSE),
          mVideoStreamIndex(-1),
          mAudioStreamIndex(-1) {
    mLooper = new RTMsgLooper();
    mLooper->setName("SimplePlayer");
    mLooper->start();
#ifdef DUMP_VIDEO_OUTPUT
    mVideoDumpFd = RT_NULL;
#endif
#ifdef DUMP_AUDIO_OUTPUT
    mAudioDumpFd = RT_NULL;
#endif
}

SimplePlayer::~SimplePlayer() {
    if (RT_NULL != mLooper) {
        mLooper->stop();
    }
    rt_safe_delete(mLooper);
}

RT_RET SimplePlayer::init(const char *uri) {
    RT_RET ret = RT_OK;
    mDemuxer = reinterpret_cast<RTNodeDemuxer*>(createRTNode(RT_NODE_TYPE_DEMUXER, BUS_LINE_SOURCE));
    if (!mDemuxer) {
        RT_LOGE("demuxer create failed!");
        return RT_ERR_UNKNOWN;
    }
    mAudioSink = reinterpret_cast<RTNodeSink*>(createRTNode(RT_NODE_TYPE_AUDIO_SINK, BUS_LINE_AUDIO));
    if (!mAudioSink) {
        RT_LOGE("audio sink create failed");
        return RT_ERR_UNKNOWN;
    }
    mVideoDecoder = createRTNode(RT_NODE_TYPE_DECODER, BUS_LINE_VIDEO);
    if (!mVideoDecoder) {
        RT_LOGE("video decoder create failed!");
        return RT_ERR_UNKNOWN;
    }
    mAudioDecoder = createRTNode(RT_NODE_TYPE_DECODER, BUS_LINE_AUDIO);
    if (!mAudioDecoder) {
        RT_LOGE("audio decoder create failed!");
        return RT_ERR_UNKNOWN;
    }

    mDemuxerMeta = new RtMetaData();
    mDemuxerMeta->setCString(kKeyFormatUri, uri);

    RTNodeAdapter::init(mDemuxer, mDemuxerMeta);

    mVideoStreamIndex = mDemuxer->queryTrackUsed(RTTRACK_TYPE_VIDEO);
    if (mVideoStreamIndex >= 0) {
        mVideoDecoderMeta = mDemuxer->queryTrackMeta(mVideoStreamIndex, RTTRACK_TYPE_VIDEO);
        RT_ASSERT(mVideoDecoderMeta);
        RTNodeAdapter::init(mVideoDecoder, mVideoDecoderMeta);
        mLooper->setHandler(this);
        RTNodeAdapter::setEventLooper(mAudioDecoder, mLooper);
        mHasVideo = RT_TRUE;
    }
    mAudioStreamIndex = mDemuxer->queryTrackUsed(RTTRACK_TYPE_AUDIO);
    if (mAudioStreamIndex >= 0) {
        mAudioDecoderMeta = mDemuxer->queryTrackMeta(mAudioStreamIndex, RTTRACK_TYPE_AUDIO);
        RT_ASSERT(mAudioDecoderMeta);

        /* init audio sink */
        mAudioSink->queueCodecBuffer = simpleAudioCodecBufferCallback;
        mAudioSink->callback_ptr = mAudioDecoder;
        RTNodeAdapter::init(mAudioSink, NULL);
        RTNodeAdapter::init(mAudioDecoder, mAudioDecoderMeta);
        mLooper->setHandler(this);
        RTNodeAdapter::setEventLooper(mAudioDecoder, mLooper);
        RTNodeAdapter::setEventLooper(mAudioSink, mLooper);
        mHasAudio = RT_TRUE;
    }

#ifdef DUMP_VIDEO_OUTPUT
    mVideoDumpFd = fopen(VIDEO_TEST_OUTPUT_FILE, "wb");
    if (!mVideoDumpFd) {
        RT_LOGE("%s open failed!!", VIDEO_TEST_OUTPUT_FILE);
        return RT_ERR_UNKNOWN;
    }
#endif
#ifdef DUMP_AUDIO_OUTPUT
    mAudioDumpFd = fopen(AUDIO_TEST_OUTPUT_FILE, "wb");
    if (!mAudioDumpFd) {
        RT_LOGE("%s open failed!!", AUDIO_TEST_OUTPUT_FILE);
        return RT_ERR_UNKNOWN;
    }
#endif
}

RT_RET SimplePlayer::start() {
    RTNodeAdapter::runCmd(mDemuxer, RT_NODE_CMD_START, NULL);
    if (mHasVideo) {
        RTNodeAdapter::runCmd(mVideoDecoder, RT_NODE_CMD_START, NULL);
    }
    if (mHasAudio) {
        RTNodeAdapter::runCmd(mAudioDecoder, RT_NODE_CMD_START, NULL);
        RTNodeAdapter::runCmd(mAudioSink, RT_NODE_CMD_START, NULL);
    }
    const char* name = "SimplePlayerTaskProcess";
    mProcThread = new RtThread(simple_player_process, reinterpret_cast<void*>(this));
    mProcThread->setName(name);

    mProcRunning = RT_TRUE;
    mProcThread->start();
}

RT_RET SimplePlayer::stop() {
    RTNodeAdapter::runCmd(mDemuxer, RT_NODE_CMD_STOP, NULL);
    RTNodeAdapter::runCmd(mVideoDecoder, RT_NODE_CMD_STOP, NULL);
    RTNodeAdapter::runCmd(mAudioDecoder, RT_NODE_CMD_STOP, NULL);
    RTNodeAdapter::runCmd(mAudioSink, RT_NODE_CMD_STOP, NULL);
}

RT_RET SimplePlayer::release() {
    mDemuxer->release();
    mVideoDecoder->release();
    mAudioDecoder->release();
    mAudioSink->release();

    rt_safe_delete(mDemuxerMeta);
    rt_safe_delete(mVideoDecoderMeta);
    rt_safe_delete(mAudioDecoderMeta);
#ifdef DUMP_VIDEO_OUTPUT
    if (mVideoDumpFd) {
        fclose(mVideoDumpFd);
        mVideoDumpFd = RT_NULL;
    }
#endif
#ifdef DUMP_AUDIO_OUTPUT
    if (mAudioDumpFd) {
        fclose(mAudioDumpFd);
        mAudioDumpFd = RT_NULL;
    }
#endif
}

RT_RET SimplePlayer::process() {
    RTMediaBuffer *video_frame = RT_NULL;
    RTMediaBuffer *audio_frame = RT_NULL;
    RTMediaBuffer *video_pkt = RT_NULL;
    RTMediaBuffer *audio_pkt = RT_NULL;
    RT_BOOL first_frame = RT_TRUE;

    while (mProcRunning) {
        // deqeue buffer from object pool
        if (!video_pkt && mHasVideo) {
            RTNodeAdapter::dequeCodecBuffer(mVideoDecoder, &video_pkt, RT_PORT_INPUT);
        }
        if (!audio_pkt && mHasAudio) {
            RTNodeAdapter::dequeCodecBuffer(mAudioDecoder, &audio_pkt, RT_PORT_INPUT);
        }
        if (video_pkt && first_frame) {
            INT32 extradata_size = 0;
            void *extradata = RT_NULL;
            mVideoDecoderMeta->findInt32(kKeyCodecExtraSize, &extradata_size);
            if (extradata_size > 0) {
                mVideoDecoderMeta->findPointer(kKeyCodecExtraData, &extradata);
                video_pkt->setData(extradata, extradata_size);
                UINT8 *data = reinterpret_cast<UINT8 *>(video_pkt->getData());
                UINT32 size = video_pkt->getSize();
                RT_LOGD("NEW Video MediaBuffer(data=0x%p, size=%d)", data, size);
                video_pkt->getMetaData()->setInt32(kKeyPacketIsExtra, 1);
                RTNodeAdapter::pushBuffer(mVideoDecoder, video_pkt);
                video_pkt = RT_NULL;
            }
            first_frame = RT_FALSE;
        } else if (video_pkt && !first_frame) {
            while (RTNodeAdapter::pullBuffer(mDemuxer, &video_pkt) != RT_OK) {
                RtTime::sleepMs(10);
            }
            UINT8 *data = reinterpret_cast<UINT8 *>(video_pkt->getData());
            UINT32 size = video_pkt->getSize();
            RT_LOGD("NEW Video MediaBuffer(ptr=0x%p, size=%d)", video_pkt, size);
            // push es-packet to decoder
            RTNodeAdapter::pushBuffer(mVideoDecoder, video_pkt);
            video_pkt = RT_NULL;
        }
        if (audio_pkt) {
            RT_LOGD("NEW audio MediaBuffer(ptr=0x%p)", audio_pkt);
            while (RTNodeAdapter::pullBuffer(mDemuxer, &audio_pkt) != RT_OK) {
                RtTime::sleepMs(10);
            }
            UINT32 size = audio_pkt->getSize();
            RT_LOGD("NEW audio MediaBuffer(ptr=0x%p, size=%d)", audio_pkt, size);

            // push es-packet to decoder
            RTNodeAdapter::pushBuffer(mAudioDecoder, audio_pkt);
            audio_pkt = RT_NULL;
        }
        /* get video frame */
        if (mHasVideo) {
            // pull av-frame from decoder
            RTNodeAdapter::pullBuffer(mVideoDecoder, &video_frame);
            if (video_frame) {
                if (video_frame->getStatus() == RT_MEDIA_BUFFER_STATUS_READY) {
                    RT_LOGD("NEW video Frame(ptr=0x%p, size=%d)", video_frame->getData(), video_frame->getLength());
#ifdef DUMP_AUDIO_OUTPUT
                  //  if (mVideoDumpFd) {
                   //     fwrite(video_frame->getData(), video_frame->getLength(), 1, mVideoDumpFd);
                    //    fflush(mVideoDumpFd);
                  //  }
#endif
                }
                RTNodeAdapter::queueCodecBuffer(mVideoDecoder, video_frame, RT_PORT_OUTPUT);
                INT32 eos = 0;
                video_frame->getMetaData()->findInt32(kKeyFrameEOS, &eos);
                if (eos) {
                    RT_LOGD("receive eos , break");
                    break;
                }
                video_frame = RT_NULL;
            }
        }
        /* get audio frame */
        if (mHasAudio) {
            // pull av-frame from decoder
            RTNodeAdapter::pullBuffer(mAudioDecoder, &audio_frame);
            if (audio_frame) {
                if (audio_frame->getStatus() == RT_MEDIA_BUFFER_STATUS_READY) {
                    RT_LOGD("NEW audio Frame(ptr=0x%p, size=%d)", audio_frame->getData(), audio_frame->getLength());
#ifdef DUMP_AUDIO_OUTPUT
                    if (mAudioDumpFd) {
                        fwrite(audio_frame->getData(), audio_frame->getLength(), 1, mAudioDumpFd);
                        fflush(mAudioDumpFd);
                    }
#endif
                    RTNodeAdapter::pushBuffer(mAudioSink, audio_frame);
                }
                // RTNodeAdapter::queueCodecBuffer(mAudioDecoder, audio_frame, RT_PORT_OUTPUT);
                audio_frame = RT_NULL;
            }
        }

        RtTime::sleepMs(2);
    }
    mProcRunning = RT_FALSE;
}

void SimplePlayer::onMessageReceived(struct RTMessage* msg) {
    RT_LOGE("message(msg=%p; what=%d, data=%p) received", msg, msg->getWhat(), msg->getData());
}

RTNode* SimplePlayer::createRTNode(RT_NODE_TYPE node_type, BUS_LINE_TYPE ltype) {
    RTNodeStub* stub     = findStub(node_type, ltype);
    RTNode*     node     = stub->mCreateNode();
    return node;
}

RT_RET unit_test_node_simple_player(INT32 index, INT32 total) {
    SimplePlayer *player = new SimplePlayer();

    player->init(TEST_URI);
    player->start();
    while (player->isRunning()) {
        RtTime::sleepMs(10);
    }
    player->stop();
    player->release();
    return RT_OK;
}

