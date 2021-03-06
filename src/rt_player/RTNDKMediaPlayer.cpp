/*
 * Copyright 2019 Rockchip Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License") { return NO_ERROR; }
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
 *   date: 2019/02/27
 * module: RTNDKMediaPlayer
 */

#include "rt_header.h"         // NOLINT
#include "RTNDKNodePlayer.h"   // NOLINT
#include "RTNDKMediaPlayer.h"  // NOLINT
#include "rt_string_utils.h"   // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTNDKMediaPlayer"
#ifdef DEBUG_FLAG
#undef DEBUG_FLAG
#endif
#define DEBUG_FLAG 0x1

typedef struct RtNDKPlayerContext {
    uid_t       mUid;
    int32_t     mState;
    int32_t     mLastError;      // defined in RTMediaError.h
    int64_t     mUsCurPosition;  // unit: microseconds
    int64_t     mUsDuration;     // unit: microseconds
    int64_t     mUsSeek;         // unit: microseconds
    int32_t     mLooping;
    const char* mUri;
    const char* mHeaders;
    RTNDKNodePlayer*  mNodePlayer;
} RTNDKPlayerContext;

rt_status checkRuntime(RTNDKPlayerContext* ctx, const char* caller) {
    if ((RT_NULL == ctx) || (RT_NULL == ctx->mNodePlayer)) {
        RT_LOGE("fail to %s, context in null", caller);
        return RTE_NO_MEMORY;
    }
    return RTE_NO_ERROR;
}

/*
 * construction and destructor
 */
RTNDKMediaPlayer::RTNDKMediaPlayer() {
    mPlayerCtx              = rt_malloc(RTNDKPlayerContext);
    rt_memset(mPlayerCtx, 0, sizeof(RTNDKPlayerContext));
    mPlayerCtx->mNodePlayer = new RTNDKNodePlayer();
}

RTNDKMediaPlayer::~RTNDKMediaPlayer() {
    rt_safe_delete(mPlayerCtx->mNodePlayer);
    rt_safe_free(mPlayerCtx);
    RT_LOGD("done, ~RTNDKMediaPlayer()");
}

rt_status RTNDKMediaPlayer::setUID(uid_t uid) {
    rt_status err = checkRuntime(mPlayerCtx, "setUID");
    if (RTE_NO_ERROR != err) {
        return err;
    }

    mPlayerCtx->mUid = uid;
    return err;
}

rt_status RTNDKMediaPlayer::setDataSource(const char *url, const char *headers) {
    rt_status err = checkRuntime(mPlayerCtx, "setDataSource");
    if (RTE_NO_ERROR != err) {
        return err;
    }

    mPlayerCtx->mUri     = url;
    mPlayerCtx->mHeaders = headers;

    // configure node bus and summary
    RTMediaUri setting = {0};
    if (RT_NULL != url) {
        rt_str_snprintf(setting.mUri, sizeof(setting.mUri), "%s", url);
    }
    if (NULL != headers) {
        rt_str_snprintf(setting.mUserAgent, sizeof(setting.mUserAgent), "%s", headers);
    }

    // auto build node bus and initialization
    err = mPlayerCtx->mNodePlayer->setDataSource(&setting);
    if (RTE_NO_ERROR != err) {
        RT_LOGE("setDataSource fail\n");
        return RT_ERR_UNKNOWN;
    }

    return err;
}

rt_status RTNDKMediaPlayer::setDataSource(int fd, int64_t offset, int64_t length) {
    char uri[1024] = {0};
    // @TODO
    // getUri(fd, uri);
    return setDataSource(uri, RT_NULL);
}

rt_status RTNDKMediaPlayer::setLooping(int loop) {
    rt_status err = checkRuntime(mPlayerCtx, "setLooping");
    if (RTE_NO_ERROR != err) {
        return err;
    }

    mPlayerCtx->mLooping  = loop;
    mPlayerCtx->mNodePlayer->setLooping(loop);
    return err;
}

rt_status RTNDKMediaPlayer::setVideoSurfaceTexture(void* bufferProducer) {
    return RTE_UNSUPPORTED;
}

rt_status RTNDKMediaPlayer::setVideoSurface(void* surface) {
    return RTE_UNSUPPORTED;
}

rt_status RTNDKMediaPlayer::initCheck() {
    rt_status err = checkRuntime(mPlayerCtx, "initCheck");
    if (RTE_NO_ERROR != err) {
        return err;
    }

    mPlayerCtx->mState = mPlayerCtx->mNodePlayer->getCurState();
    if ((RT_NULL != mPlayerCtx) && (mPlayerCtx->mState >= RT_STATE_INITIALIZED)) {
        err = RTE_NO_ERROR;
    }
    return err;
}

rt_status RTNDKMediaPlayer::prepare() {
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    // driver core data-flow
    RT_LOGD("NodePlayer prepare");
    mPlayerCtx->mNodePlayer->prepare();
    mPlayerCtx->mNodePlayer->summary(0);
    return err;
}

rt_status RTNDKMediaPlayer::prepareAsync() {
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    return err;
}

rt_status RTNDKMediaPlayer::seekTo(int64_t usec) {
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    RT_LOGD("NodePlayer seekTo(%lld us)", usec);
    mPlayerCtx->mUsSeek = usec;
    mPlayerCtx->mNodePlayer->seekTo(usec);
    return err;
}

rt_status RTNDKMediaPlayer::start() {
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    // driver core data-flow
    RT_LOGD("NodePlayer start");
    mPlayerCtx->mNodePlayer->start();
    return err;
}

rt_status RTNDKMediaPlayer::stop() {
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    RT_LOGD("call, stop");
    // driver core data-flow
    mPlayerCtx->mNodePlayer->stop();
    return err;
}

rt_status RTNDKMediaPlayer::pause() {
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    RT_LOGD("NodePlayer pause");
    mPlayerCtx->mNodePlayer->pause();
    return err;
}

rt_status RTNDKMediaPlayer::reset() {
    RT_LOGD("play_ndk reset");
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    RT_LOGD("NodePlayer reset");
    mPlayerCtx->mNodePlayer->reset();
    return RTE_NO_ERROR;
}

rt_status RTNDKMediaPlayer::wait(int64_t timeUs) {
    RT_LOGD("call, wait until playback done, max timetout = %lldms", timeUs/1000);
    mPlayerCtx->mNodePlayer->wait(timeUs);
    RT_LOGD("done, playback has done...");
    return RTE_NO_ERROR;
}

rt_status RTNDKMediaPlayer::getState() {
    int32_t err = checkRuntime(mPlayerCtx, "getState");
    if (RTE_NO_ERROR != err) {
        return err;
    }

    return mPlayerCtx->mNodePlayer->getCurState();
}

rt_status RTNDKMediaPlayer::getCurrentPosition(int64_t *usec) {
    *usec = 0;
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    return mPlayerCtx->mNodePlayer->getCurrentPosition(usec);
}

rt_status RTNDKMediaPlayer::getDuration(int64_t *usec) {
    *usec = 0;
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    return mPlayerCtx->mNodePlayer->getDuration(usec);
}

rt_status RTNDKMediaPlayer::dump(int fd, const char* args) {
    if (0 != fd) {
        return RTE_NO_ERROR;
    }
    return RTE_BAD_VALUE;
}

/*
 * advanced query/cotrol operations
 *   Parcel is defined in /android/frameworks/native/libs/binder/Parcel.cpp
 * RTParcel is defined in /src/rt_media/sink-osal/RTParcel.cpp
 */
rt_status RTNDKMediaPlayer::invoke(const RTParcel &request, RTParcel *reply) {
    return RTE_UNSUPPORTED;
}

rt_status RTNDKMediaPlayer::setParameter(int key, const RTParcel &request) {
    return RTE_UNSUPPORTED;
}

rt_status RTNDKMediaPlayer::getParameter(int key, RTParcel *reply) {
    return RTE_UNSUPPORTED;
}

/*
 * basic operations for audiotrack
 * attachAuxEffect: attaches an auxiliary effect to the audio track
 */
rt_status RTNDKMediaPlayer::setAudioSink(const void* audioSink) {
    return RTE_UNSUPPORTED;
}

rt_status RTNDKMediaPlayer::setVolume(float leftVolume, float rightVolume) {
    return RTE_UNSUPPORTED;
}

rt_status RTNDKMediaPlayer::setAuxEffectSendLevel(float level) {
    return RTE_UNSUPPORTED;
}

rt_status RTNDKMediaPlayer::attachAuxEffect(int effectId)  {
    return RTE_UNSUPPORTED;
}

rt_status RTNDKMediaPlayer::writeData(const char * data, const UINT32 length, int flag, int type) {
    if (RT_NULL != mPlayerCtx) {
        mPlayerCtx->mNodePlayer->writeData(data, length, flag, type);
        return RTE_NO_ERROR;
    }
    return RTE_BAD_VALUE;
}

rt_status RTNDKMediaPlayer::setCallBack(RT_CALLBACK_T callback, int p_event, void *p_data) {
    if (RT_NULL != mPlayerCtx) {
        mPlayerCtx->mNodePlayer->setCallBack(callback, p_event, p_data);
        return RTE_NO_ERROR;
    }
    return RTE_BAD_VALUE;
}
