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
 *   date: 2019/02/27
 */

#include "rt_header.h"          // NOLINT
#include "RTPairAudioPlayer.h"  // NOLINT

RTPairAudioPlayer::RTPairAudioPlayer() {
    mAudioCur = mAudioA = mAudioB = RT_NULL;
}

RTPairAudioPlayer::~RTPairAudioPlayer() {
    deinit();
    mAudioA = mAudioB = RT_NULL;
}

rt_status RTPairAudioPlayer::init() {
    mAudioA = new RTNDKMediaPlayer();
    mAudioB = new RTNDKMediaPlayer();
    return RTE_NO_ERROR;
}

rt_status RTPairAudioPlayer::wait() {
    if (RT_NULL != mAudioCur) {
        mAudioCur->wait();
    }
}

rt_status RTPairAudioPlayer::playUri(const char* uri) {
    swapAudio();
    if (NULL != mAudioCur) {
       RT_LOGD("\r\nAudioPlayer(%p) uri(%s)", mAudioCur, uri);
       mAudioCur->setDataSource(uri, RT_NULL);
       mAudioCur->prepare();
       mAudioCur->start();
       RT_LOGD("AudioPlayer(%p) uri(%s) done", mAudioCur, uri);
    }
    return RTE_NO_ERROR;
}

rt_status RTPairAudioPlayer::playPcm(const char* pcm) {
    swapAudio();
    if (NULL != mAudioCur) {
       RT_LOGD("\r\nAudioPlayer(%p) pcm(%s)", mAudioCur, pcm);
       mAudioCur->setDataSource(pcm, RT_NULL);
       mAudioCur->prepare();
       mAudioCur->start();
       RT_LOGD("AudioPlayer(%p) pcm(%s) done", mAudioCur, pcm);
    }
    return RTE_NO_ERROR;
}

rt_status RTPairAudioPlayer::deinit() {
    mAudioCur = NULL;
    if (NULL != mAudioA) {
        mAudioA->pause();
        mAudioA->reset();
    }
    if (NULL != mAudioB) {
        mAudioB->pause();
        mAudioB->reset();
    }
    rt_safe_delete(mAudioA);
    rt_safe_delete(mAudioB);
    return RTE_NO_ERROR;
}

rt_status RTPairAudioPlayer::swapAudio() {
    if (NULL == mAudioCur) {
        if (NULL != mAudioA) {
            mAudioCur = mAudioA;
        } else if (NULL != mAudioB) {
            mAudioCur = mAudioB;
        }
    } else {
        mAudioCur->pause();
        mAudioCur->reset();
        if (mAudioCur == mAudioA) {
            mAudioCur = mAudioB;
        } else if (mAudioCur == mAudioB) {
            mAudioCur = mAudioA;
        }
    }
    return RTE_NO_ERROR;
}
