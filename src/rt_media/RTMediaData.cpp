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
 * author: martin.cheng@rock-chips.com
 *   date: 20190122
 *    ref: ffmpeg/libavcodec/avcodec.h
 */

#include "RTMediaData.h"
#include "RTMediaMetaKeys.h"
#include "FFAdapterUtils.h"
#include "RTMediaBuffer.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTCodecData"

#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x0

RT_RET rt_utils_packet_free(RTPacket* rt_pkt) {
    RT_RET err = RT_ERR_NULL_PTR;
    if ((RT_NULL != rt_pkt) && (RT_NULL != rt_pkt->mFuncFree)) {
        if (RT_NULL != rt_pkt->mRawPtr) {
            rt_pkt->mFuncFree(rt_pkt->mRawPtr);
            err = RT_OK;
        }
    }
    return err;
}

RT_RET rt_mediabuf_from_packet(RTMediaBuffer* media_buf, RTPacket* rt_pkt) {
    RT_RET err = RT_ERR_NULL_PTR;
    if ((RT_NULL != rt_pkt) && (RT_NULL != media_buf)) {
        RtMetaData* meta  = media_buf->getMetaData();
        meta->setPointer(kKeyPacketPtr,  rt_pkt->mRawPtr);
        meta->setInt64(kKeyPacketPts,    rt_pkt->mPts);
        meta->setInt64(kKeyPacketDts,    rt_pkt->mDts);
        meta->setInt64(kKeyPacketPos,    rt_pkt->mPos);
        meta->setInt32(kKeyPacketSize,   rt_pkt->mSize);
        meta->setInt32(kKeyPacketFlag,   rt_pkt->mFlags);
        meta->setInt32(kKeyPacketIndex,  rt_pkt->mTrackIndex);
        meta->setPointer(kKeyPacketData, rt_pkt->mData);
    }
    return err;
}

RT_RET rt_mediabuf_goto_packet(RTMediaBuffer* media_buf, RTPacket* rt_pkt) {
    RT_RET err = RT_ERR_NULL_PTR;
    if ((RT_NULL != rt_pkt) && (RT_NULL != media_buf)) {
        RtMetaData* meta  = media_buf->getMetaData();
        meta->findPointer(kKeyPacketPtr,  &(rt_pkt->mRawPtr));
        meta->findInt64(kKeyPacketPts,    &(rt_pkt->mPts));
        meta->findInt64(kKeyPacketDts,    &(rt_pkt->mDts));
        meta->findInt64(kKeyPacketPos,    &(rt_pkt->mPos));
        meta->findInt32(kKeyPacketSize,   &(rt_pkt->mSize));
        meta->findInt32(kKeyPacketFlag,   &(rt_pkt->mFlags));
        meta->findInt32(kKeyPacketIndex,  &(rt_pkt->mTrackIndex));
        meta->findPointer(kKeyPacketData, reinterpret_cast<void**>(&(rt_pkt->mData)));
    }
    return err;
}

RT_RET rt_utils_frame_free(RTFrame* rt_frame) {
    RT_RET err = RT_ERR_NULL_PTR;
    if ((RT_NULL != rt_frame) && (RT_NULL != rt_frame->mFuncFree)) {
        if (RT_NULL != rt_frame->mRawPtr) {
            rt_frame->mFuncFree(rt_frame->mRawPtr);
            err = RT_OK;
        }
    }
    return err;
}

RT_RET rt_mediabuf_from_frame(RTMediaBuffer* media_buf, RTFrame* rt_frame) {
    RT_RET err = RT_ERR_NULL_PTR;
    if ((RT_NULL != rt_frame) && (RT_NULL != media_buf)) {
        RtMetaData* meta  = media_buf->getMetaData();
        media_buf->setData(rt_frame->mData, rt_frame->mSize);
        meta->setInt32(kKeyFrameW,      rt_frame->mFrameW);
        meta->setInt32(kKeyFrameH,     rt_frame->mFrameH);
        meta->setInt64(kKeyFramePts,        rt_frame->mPts);
        meta->setInt32(kKeyDisplayW,    rt_frame->mDisplayW);
        meta->setInt32(kKeyDisplayH,   rt_frame->mDisplayH);
        meta->setInt32(kKeyFrameType,       rt_frame->mFrameType);
        meta->setInt32(kKeyFieldOrder,      rt_frame->mFieldOrder);
    }
    return err;
}

RT_RET rt_mediabuf_goto_frame(RTMediaBuffer* media_buf, RTFrame* rt_frame) {
    RT_RET err = RT_ERR_NULL_PTR;
    if ((RT_NULL != rt_frame) && (RT_NULL != media_buf)) {
        RtMetaData* meta  = media_buf->getMetaData();
        rt_memset(rt_frame, 0, sizeof(RTFrame));
        rt_frame->mData = media_buf->getData();
        rt_frame->mSize = media_buf->getSize();
        meta->findInt32(kKeyFrameW,    &(rt_frame->mFrameW));
        meta->findInt32(kKeyFrameH,    &(rt_frame->mFrameH));
        meta->findInt64(kKeyFramePts,  &(rt_frame->mPts));
        meta->findInt32(kKeyDisplayW,  &(rt_frame->mDisplayW));
        meta->findInt32(kKeyDisplayH,  &(rt_frame->mDisplayH));
    }
    return err;
}

RT_RET rt_medatdata_from_trackpar(RtMetaData* meta, RTTrackParms* tpar) {
    if ((RT_NULL == tpar) || (RT_NULL == meta)) {
        return RT_ERR_NULL_PTR;
    }
    meta->setInt32(kKeyCodecType,    tpar->mCodecType);
    meta->setInt32(kKeyCodecID,      tpar->mCodecID);
    meta->setInt32(kKeyCodecFormat,  tpar->mCodecFormat);
    meta->setInt32(kKeyCodecProfile, tpar->mCodecProfile);
    meta->setInt32(kKeyCodecLevel,   tpar->mCodecLevel);
    meta->setInt64(kKeyCodecBitrate, tpar->mBitrate);   // INT64
    meta->setInt32(kKeyCodecExtraSize,      tpar->mExtraDataSize);
    meta->setPointer(kKeyCodecExtraData,    tpar->mExtraData);

    switch (tpar->mCodecType) {
    case RTTRACK_TYPE_VIDEO:
        // frame_rate = stream->avg_frame_rate.num/stream->avg_frame_rate.den;
        meta->setInt32(kKeyVCodecWidth,          tpar->mVideoWidth);
        meta->setInt32(kKeyVCodecHeight,         tpar->mVideoHeight);
        meta->setInt32(kKeyVCodecVideoDelay,     tpar->mVideoDelay);
        meta->setInt32(kKeyVCodecFrameRate,      tpar->mVideoFrameRate);
        meta->setInt32(kKeyVCodecFieldOrder,     tpar->mFieldOrder);
        meta->setInt32(kKeyVCodecColorRange,     tpar->mColorRange);
        meta->setInt32(kKeyVCodecColorPrimary,   tpar->mColorPrimaries);
        meta->setInt32(kKeyVCodecColorTransfer,  tpar->mColorTrc);
        meta->setInt32(kKeyVCodecColorSpace,     tpar->mColorSpace);
        meta->setInt32(kKeyVCodecChromaLocation, tpar->mChromaLocation);
        break;
    case RTTRACK_TYPE_AUDIO:
        meta->setInt64(kKeyACodecChanneLayout,      tpar->mAudioChannelLayout);
        meta->setInt32(kKeyACodecChannels,          tpar->mAudioChannels);
        meta->setInt32(kKeyACodecSampleRate,        tpar->mAudioSampleRate);
        meta->setInt32(kKeyACodecBlockAlign,        tpar->mAudioBlockAlign);
        meta->setInt32(kKeyACodecFrameSize,         tpar->mAudioFrameSize);
        meta->setInt32(kKeyACodecInitialPadding,    tpar->mAudioInitialPadding);
        meta->setInt32(kKeyACodecTrailinglPadding,  tpar->mAudioTrailingPadding);
        meta->setInt32(kKeyACodecBitPerCodedSample, tpar->mAudiobitsPerCodedSample);
        break;
    case RTTRACK_TYPE_DATA:
        break;
    case RTTRACK_TYPE_SUBTITLE:
        // @TODO save subtitle parameters
        break;
    default:
        break;
    }
    return RT_OK;
}

RT_RET rt_medatdata_goto_trackpar(RtMetaData* meta, RTTrackParms* tpar) {
    if ((RT_NULL == tpar) || (RT_NULL ==  meta)) {
        return RT_ERR_NULL_PTR;
    }
    meta->findInt32(kKeyCodecType,    reinterpret_cast<INT32*>(&(tpar->mCodecType)));
    meta->findInt32(kKeyCodecID,      reinterpret_cast<INT32*>(&(tpar->mCodecID)));
    meta->findInt32(kKeyCodecFormat,  &(tpar->mCodecFormat));
    meta->findInt32(kKeyCodecProfile, &(tpar->mCodecProfile));
    meta->findInt32(kKeyCodecLevel,   &(tpar->mCodecLevel));
    meta->findInt64(kKeyCodecBitrate, reinterpret_cast<INT64*>(&(tpar->mBitrate)));
    meta->findPointer(kKeyCodecExtraData, reinterpret_cast<void **>(&(tpar->mExtraData)));
    meta->findInt32(kKeyCodecExtraSize,      &(tpar->mExtraDataSize));

    switch (tpar->mCodecType) {
    case RTTRACK_TYPE_VIDEO:
        meta->findInt32(kKeyVCodecWidth,          &(tpar->mVideoWidth));
        meta->findInt32(kKeyVCodecHeight,         &(tpar->mVideoHeight));
        meta->findInt32(kKeyVCodecVideoDelay,     &(tpar->mVideoDelay));
        meta->findInt32(kKeyVCodecFrameRate,      &(tpar->mVideoFrameRate));
        meta->findInt32(kKeyVCodecFieldOrder,     reinterpret_cast<INT32*>(&(tpar->mFieldOrder)));
        meta->findInt32(kKeyVCodecColorRange,     reinterpret_cast<INT32*>(&(tpar->mColorRange)));
        meta->findInt32(kKeyVCodecColorPrimary,   reinterpret_cast<INT32*>(&(tpar->mColorPrimaries)));
        meta->findInt32(kKeyVCodecColorTransfer,  reinterpret_cast<INT32*>(&(tpar->mColorTrc)));
        meta->findInt32(kKeyVCodecColorSpace,     reinterpret_cast<INT32*>(&(tpar->mColorSpace)));
        meta->findInt32(kKeyVCodecChromaLocation, reinterpret_cast<INT32*>(&(tpar->mChromaLocation)));
        break;
    case RTTRACK_TYPE_AUDIO:
        meta->findInt64(kKeyACodecChanneLayout,         reinterpret_cast<INT64*>(&(tpar->mAudioChannelLayout)));
        meta->findInt32(kKeyACodecChannels,             &(tpar->mAudioChannels));
        meta->findInt32(kKeyACodecSampleRate,           &(tpar->mAudioSampleRate));
        meta->findInt32(kKeyACodecBlockAlign,           &(tpar->mAudioBlockAlign));
        meta->findInt32(kKeyACodecFrameSize,            &(tpar->mAudioFrameSize));
        meta->findInt32(kKeyACodecInitialPadding,       &(tpar->mAudioInitialPadding));
        meta->findInt32(kKeyACodecTrailinglPadding,     &(tpar->mAudioTrailingPadding));
        meta->findInt32(kKeyACodecBitPerCodedSample,    &(tpar->mAudiobitsPerCodedSample));
        break;
    case RTTRACK_TYPE_DATA:
        break;
    case RTTRACK_TYPE_SUBTITLE:
        // @TODO save subtitle parameters
        break;
    default:
        break;
    }
    return RT_OK;
}

RT_RET rt_utils_dump_track(RTTrackParms* tpar, RT_BOOL full/*=false*/) {
    INT32 avCodeID = tpar->mCodecID;
    switch (tpar->mCodecType) {
      case RTTRACK_TYPE_VIDEO:
      case RTTRACK_TYPE_AUDIO:
        avCodeID = fa_utils_to_av_codec_id(tpar->mCodecID);
        break;
      default:
        break;
    }

    INT32 profile = (tpar->mCodecProfile > 0)? tpar->mCodecProfile:-1;
    INT64 bitrate = (tpar->mBitrate > 0)? tpar->mBitrate/1024:-1;
    RT_LOGD("%12s: %4s; %12s: %4d; %12s: %5lldk", \
            "CodecID:", fa_utils_codec_name(avCodeID), \
            "Profile:", profile, \
            "Bitrate:", bitrate);
    switch (tpar->mCodecType) {
    case RTTRACK_TYPE_VIDEO:
        RT_LOGD("%12s: %04d; %12s: %04d; %12s: %04d", \
                "VideoWidth",  tpar->mVideoWidth, \
                "VideoHeight", tpar->mVideoHeight, \
                "VFrameRate",  tpar->mVideoFrameRate);
        break;
    case RTTRACK_TYPE_AUDIO:
        RT_LOGD("%12s: %04d; %12s: %04d; %12s: %06d", \
                "Channels",   tpar->mAudioChannels, \
                "FrameSize",  tpar->mAudioFrameSize, \
                "SampleRate", tpar->mAudioSampleRate);
        break;
    case RTTRACK_TYPE_SUBTITLE:
        break;
    default:
        break;
    }
    return RT_OK;
}

RT_RET rt_utils_dump_track(RtMetaData* meta, RT_BOOL full/*=false*/) {
    RTTrackParms track_par;
    rt_memset(&track_par, 0, sizeof(RTTrackParms));
    rt_medatdata_goto_trackpar(meta, &track_par);
    rt_utils_dump_track(&track_par, full);
    return RT_OK;
}
