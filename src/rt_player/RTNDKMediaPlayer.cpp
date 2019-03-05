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
#include "RTNodeBus.h"         // NOLINT
#include "RTNDKMediaPlayer.h"  // NOLINT
#include "rt_string_utils.h"   // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RTNDKMediaPlayer"

typedef struct RTNDKPlayerContext {
    uid_t       mUid;
    int32_t     mState;
    int32_t     mLastError;      // defined in RTMediaError.h
    int64_t     mUsCurPosition;  // unit: microseconds
    int64_t     mUsDuration;     // unit: microseconds
    int64_t     mUsSeek;         // unit: microseconds
    int32_t     mLooping;
    const char* mUri;
    const char* mHeaders;
    RTNodeBus*  mNodeBus;
} _RTNDKPlayerContext;

rt_status setPlayerState(_RTNDKPlayerContext* playerCtx, int32_t new_state) {
    if (playerCtx->mState != new_state) {
        RT_LOGD("Old State is (%d); New State is (%d)", playerCtx->mState, new_state);
    }
    return RTE_NO_ERROR;
}
/* 
 * construction and destructor
 */
RTNDKMediaPlayer::RTNDKMediaPlayer() {
    mPlayerCtx           = rt_malloc(_RTNDKPlayerContext);
    rt_memset(mPlayerCtx, 0, sizeof(_RTNDKPlayerContext));
    mPlayerCtx->mNodeBus = new RTNodeBus();
}

RTNDKMediaPlayer::~RTNDKMediaPlayer() {
    rt_safe_delete(mPlayerCtx->mNodeBus);
    rt_safe_free(mPlayerCtx);
}

rt_status RTNDKMediaPlayer::setUID(uid_t uid) {
    if (RT_NULL != mPlayerCtx) {
        mPlayerCtx->mUid = uid;
        return RTE_NO_ERROR;
    }
    return RTE_UNKNOWN;
}

rt_status RTNDKMediaPlayer::setDataSource(const char *url, const char *headers) {
    if ((RT_NULL == mPlayerCtx) || (RT_NULL == mPlayerCtx->mNodeBus)) {
        return RTE_NO_MEMORY;
    }

    mPlayerCtx->mUri     = url;
    mPlayerCtx->mHeaders = headers;

    // configure node bus and summary
    NodeBusSetting setting = {0};
    rt_str_snprintf(setting.mUri, sizeof(setting.mUri), "%s", url);
    if (NULL != headers) {
        rt_str_snprintf(setting.mUserAgent, sizeof(setting.mUserAgent), "%s", headers);
    }

    // auto build node bus and initialization
    mPlayerCtx->mNodeBus->autoBuild(&setting);
    mPlayerCtx->mNodeBus->summary(0);

    return RTE_NO_ERROR;
}

rt_status RTNDKMediaPlayer::setDataSource(int fd, int64_t offset, int64_t length) {
    char uri[1024] = {0};
    // @TODO
    // getUri(fd, uri);
    return setDataSource(uri, RT_NULL);
}

rt_status RTNDKMediaPlayer::setLooping(int loop) {
    if (RT_NULL != mPlayerCtx) {
        mPlayerCtx->mLooping  = loop;
        return RTE_NO_ERROR;
    }
    return RTE_UNKNOWN;
}

rt_status RTNDKMediaPlayer::setVideoSurfaceTexture(void* bufferProducer) {
    return RTE_UNSUPPORTED;
}

rt_status RTNDKMediaPlayer::setVideoSurface(void* surface) {
    return RTE_UNSUPPORTED;
}

rt_status RTNDKMediaPlayer::initCheck() {
    if ((RT_NULL == mPlayerCtx) || (RT_NULL == mPlayerCtx->mNodeBus)) {
        return RTE_NO_MEMORY;
    }
    mPlayerCtx->mState = mPlayerCtx->mNodeBus->getCurState();

    if ((RT_NULL != mPlayerCtx) && (mPlayerCtx->mState >= RTM_PLAYER_INITIALIZED)) {
        return RTE_NO_ERROR;
    }
    return RTE_UNKNOWN;
}

rt_status RTNDKMediaPlayer::prepare() {
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    // driver core data-flow
    RT_LOGD("node_bus prepare");
    mPlayerCtx->mNodeBus->prepare();
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
    RT_LOGD("node_bus seekTo(%lld us)", usec);
    mPlayerCtx->mUsSeek = usec;
    mPlayerCtx->mNodeBus->seekTo(usec);
    return err;
}

rt_status RTNDKMediaPlayer::start() {
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    // driver core data-flow
    RT_LOGD("node_bus start");
    mPlayerCtx->mNodeBus->start();
    return err;
}

rt_status RTNDKMediaPlayer::stop() {
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    RT_LOGD("node_bus stop");
    // driver core data-flow
    mPlayerCtx->mNodeBus->stop();
    return err;
}

rt_status RTNDKMediaPlayer::pause() {
    RT_LOGD("play_ndk pause");
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    RT_LOGD("node_bus pause");
    mPlayerCtx->mNodeBus->pause();
    return err;
}

rt_status RTNDKMediaPlayer::reset() {
    RT_LOGD("play_ndk reset");
    int32_t err = initCheck();
    if (RTE_NO_ERROR != err) {
        return err;
    }
    RT_LOGD("node_bus reset");
    mPlayerCtx->mNodeBus->reset();
    return RTE_NO_ERROR;
}

rt_status RTNDKMediaPlayer::getState() {
    if (RT_NULL != mPlayerCtx) {
        return mPlayerCtx->mState;
    }
    return RTM_PLAYER_STATE_ERROR;
}

rt_status RTNDKMediaPlayer::getCurrentPosition(int64_t *usec) {
    *usec = 0;
    if (RT_NULL != mPlayerCtx) {
        *usec = mPlayerCtx->mUsCurPosition;
    }
    return RTE_NO_ERROR;
}

rt_status RTNDKMediaPlayer::getDuration(int64_t *usec) {
    *usec = 0;
    if (RT_NULL != mPlayerCtx) {
        *usec = mPlayerCtx->mUsDuration;
    }
    return RTE_NO_ERROR;
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