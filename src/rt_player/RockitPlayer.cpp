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
 * Author: hery.xu@rock-chips.com
 *   Date: 2019/03/13
 *   Task: RockitPlayer
 */
#include "rt_header.h"          // NOLINT
#include "RockitPlayer.h"       // NOLINT
#include "RTNDKMediaPlayer.h"   // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RockitPlayer"


void * Rockit_CreatePlayer() {
    return reinterpret_cast<void *>(new RTNDKMediaPlayer());
}

int Rockit_EventAttach(void * player, int type, Rockit_callback_t callback, void *opaque) {
    RTNDKMediaPlayer * rtplayer = reinterpret_cast<RTNDKMediaPlayer *>(player);
    if (NULL != rtplayer) {
        RT_LOGD("RockitPlayer EventAttach");
        rtplayer->setCallBack(callback, type, opaque);
        return RT_OK;
    }
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayAudio_PCM(void *player) {
    return RT_ERR_UNKNOWN;
}

int Rockit_ReleasePlayer(void *player) {
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayAudio(void *player, const char* url, const int pcm) {
    RTNDKMediaPlayer * rtplayer = reinterpret_cast<RTNDKMediaPlayer *>(player);
    if (NULL != rtplayer) {
        RT_LOGD("RockitPlayer url(%s)", url);
        rtplayer->setDataSource(url, RT_NULL);
        rtplayer->prepare();
        RT_LOGD("RockitPlayer url(%s) done", url);
        return RT_OK;
    }
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayerPlay(void *player) {
    RTNDKMediaPlayer * rtplayer = reinterpret_cast<RTNDKMediaPlayer *>(player);
    if (NULL != rtplayer) {
        RT_LOGD("RockitPlayer PlayerPlay");
        rtplayer->start();
        RT_LOGD("RockitPlayer PlayerPlay done");
        return RT_OK;
    }
    return RT_ERR_UNKNOWN;
}
int Rockit_WriteData(void *player, const char * data, const unsigned int length) {
    RTNDKMediaPlayer * rtplayer = reinterpret_cast<RTNDKMediaPlayer *>(player);
    if (NULL != rtplayer) {
       RT_LOGD("RockitPlayer WriteData");
       rtplayer->writeData(data, length, 0/*PCM stream*/, 0/*only es use*/);
       RT_LOGD("RockitPlayer WriteData done");
       return RT_OK;
    }
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayerStop(void *player) {
    RTNDKMediaPlayer * rtplayer = reinterpret_cast<RTNDKMediaPlayer *>(player);
    if (NULL != rtplayer) {
        RT_LOGD("RockitPlayer PlayerStop");
        rtplayer->stop();
        RT_LOGD("RockitPlayer PlayerStop done");
        return RT_OK;
    }
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayerPause(void *player) {
    RTNDKMediaPlayer * rtplayer = reinterpret_cast<RTNDKMediaPlayer *>(player);
    if (NULL != rtplayer) {
        RT_LOGD("RockitPlayer PlayerPause");
        rtplayer->pause();
        RT_LOGD("RockitPlayer PlayerPause done");
        return RT_OK;
    }
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayerResume(void *player) {
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayerSeek(void *player, const double time) {
    RTNDKMediaPlayer * rtplayer = reinterpret_cast<RTNDKMediaPlayer *>(player);
    if (NULL != rtplayer) {
        RT_LOGD("RockitPlayer PlayerSeek");
        rtplayer->seekTo(time);
        RT_LOGD("RockitPlayer PlayerSeek done");
        return RT_OK;
    }
    return RT_ERR_UNKNOWN;
}

Rockit_PlayerState Rockit_GetPlayerState(void *player) {
    RTNDKMediaPlayer * rtplayer = reinterpret_cast<RTNDKMediaPlayer *>(player);
    if (NULL != rtplayer) {
        RT_LOGD("RockitPlayer PlayerState");
        rt_status ret = rtplayer->getState();
        RT_LOGD("RockitPlayer PlayerState done");
        return Rockit_CLOSED;
    }
    return Rockit_CLOSED;
}

int Rockit_GetPlayerDuration(void *player) {
    int64_t usec;
    RTNDKMediaPlayer * rtplayer = reinterpret_cast<RTNDKMediaPlayer *>(player);
    if (NULL != rtplayer) {
        RT_LOGD("RockitPlayer PlayerDuration");
        rtplayer->getDuration(&usec);
        RT_LOGD("RockitPlayer PlayerDuration done");
        return RT_OK;
    }
    return RT_ERR_UNKNOWN;
}

int Rockit_GetPlayerPosition(void *player) {
    int64_t usec;
    RTNDKMediaPlayer * rtplayer = reinterpret_cast<RTNDKMediaPlayer *>(player);
    if (NULL != rtplayer) {
        RT_LOGD("RockitPlayer PlayerPosition");
        rtplayer->getCurrentPosition(&usec);
        RT_LOGD("RockitPlayer PlayerPosition done");
        return RT_OK;
    }
    return RT_ERR_UNKNOWN;
}
