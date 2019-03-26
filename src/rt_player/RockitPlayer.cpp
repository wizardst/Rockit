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
#include "rt_message.h"         // NOLINT
#include "RTNDKMediaDef.h"      // NOLINT
#include  "rt_type.h"           // NOLINT
#include <cstring>              // NOLINT

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "RockitPlayer"


enum PlayCase {
    PCM_PLAY,
    LOCAL_PLAY,
};

enum PlayProtocolType {
    PRO_LOCAL,
    PRO_HTTP,
};

struct RockitContext {
    RTNDKMediaPlayer *local_player;
    RTNDKMediaPlayer *pcm_player;
    RT_BOOL           player_select;
    RtMutex*          mDataLock;
    RT_CALLBACK_T     mRT_Callback;
    void*             mRT_Callback_Data;
    int               mProtocolType;
};

void Rockit_Callback(int p_event, void *player) {
    RT_LOGD("RockitPlayer1 Rockit_Callback!!!");
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    if (NULL != rtplayer) {
        if (p_event == Rockit_MediaPlayerEndReached_pcm) {
            rtplayer->mRT_Callback(Rockit_MediaPlayerEndReached_pcm, rtplayer->mRT_Callback_Data);
        } else {
           if (rtplayer->mProtocolType == PRO_LOCAL) {
               rtplayer->mRT_Callback(Rockit_MediaPlayerEndReached_kit, rtplayer->mRT_Callback_Data);
           } else {
               rtplayer->mRT_Callback(Rockit_MediaPlayerEndReached_vlc, rtplayer->mRT_Callback_Data);
           }
       }
    }
}

void * Rockit_CreatePlayer() {
    RT_LOGD("RockitPlayer1 CreatePlayer");
    struct RockitContext* mPlayerCtx = rt_malloc(RockitContext);
    mPlayerCtx->mDataLock      = new RtMutex();
    RtMutex::RtAutolock autoLock(mPlayerCtx->mDataLock);
    mPlayerCtx->local_player =  new RTNDKMediaPlayer();
    mPlayerCtx->pcm_player =    new RTNDKMediaPlayer();
    mPlayerCtx->player_select  = LOCAL_PLAY;
    mPlayerCtx->mProtocolType  = PRO_LOCAL;
    mPlayerCtx->mRT_Callback   = NULL;
    mPlayerCtx->mRT_Callback_Data = NULL;
    RT_LOGD("RockitPlayer1 CreatePlayer Done");
    return reinterpret_cast<void *>(mPlayerCtx);
}

int Rockit_EventAttach(void * player, int type, Rockit_callback_t callback, void *opaque) {
    RT_LOGD("RockitPlayer1 EventAttach");
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    RtMutex::RtAutolock autoLock(rtplayer->mDataLock);
    if (NULL != rtplayer) {
        rtplayer->mRT_Callback = callback;
        rtplayer->mRT_Callback_Data = opaque;
        rtplayer->local_player->setCallBack(Rockit_Callback, type, rtplayer);
        rtplayer->pcm_player->setCallBack(Rockit_Callback, Rockit_MediaPlayerEndReached_pcm, rtplayer);
        RT_LOGD("RockitPlayer1 EventAttach Done");
        return RT_OK;
    }
    RT_LOGD("RockitPlayer1 EventAttach err");
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayAudio_PCM(void *player) {
    RT_LOGD("RockitPlayer1 PCM");
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    rt_status ret;
    RtMutex::RtAutolock autoLock(rtplayer->mDataLock);

    ret = rtplayer->pcm_player->getState();
    RT_LOGD("RockitPlayer1 ret = %d", ret);
    if (NULL != rtplayer && (ret == RT_STATE_IDLE)) {
        rtplayer->player_select = PCM_PLAY;
        rtplayer->pcm_player->setDataSource(RT_NULL, RT_NULL);
        RT_LOGD("RockitPlayer1 PCM 1");
        rtplayer->pcm_player->prepare();
        RT_LOGD("RockitPlayer1 PCM 2");
        rtplayer->pcm_player->start();
        RT_LOGD("RockitPlayer1 PCM Done");
        return RT_OK;
    }
    RT_LOGD("RockitPlayer1 PCM have done");
    return RT_ERR_UNKNOWN;
}

int Rockit_ReleasePlayer(void *player) {
    RT_LOGD("RockitPlayer1 release");
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    RtMutex::RtAutolock autoLock(rtplayer->mDataLock);
    if (NULL != rtplayer) {
        rtplayer->local_player->stop();
        rtplayer->local_player->reset();
        rtplayer->pcm_player->stop();
        rtplayer->pcm_player->reset();
        delete rtplayer->local_player;
        delete rtplayer->pcm_player;
        delete rtplayer;
    }
    RT_LOGD("RockitPlayer1 release Done");
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayAudio(void *player, const char* url, const int pcm) {
    RT_LOGD("RockitPlayer1 url(%s) pcm = %d", url, pcm);
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    RtMutex::RtAutolock autoLock(rtplayer->mDataLock);
    if (NULL != rtplayer) {
        rtplayer->player_select = LOCAL_PLAY;
        if (!strncasecmp("/oem", url, 4)) {
            rtplayer->mProtocolType = PRO_LOCAL;
        } else {
            rtplayer->mProtocolType = PRO_HTTP;
        }
        rtplayer->local_player->setDataSource(url, RT_NULL);
        rtplayer->local_player->prepare();
        RT_LOGD("RockitPlayer1 url(%s) Done", url);
        return RT_OK;
    }
    RT_LOGD("RockitPlayer1 url(%s) err", url);
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayerPlay(void *player) {
    RT_LOGD("RockitPlayer1 PlayerPlay");
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    RtMutex::RtAutolock autoLock(rtplayer->mDataLock);
    if (NULL != rtplayer) {
        rtplayer->local_player->start();
        RT_LOGD("RockitPlayer1 PlayerPlay Done");
        return RT_OK;
    }
    RT_LOGD("RockitPlayer1 PlayerPlay err");
    return RT_ERR_UNKNOWN;
}
int Rockit_WriteData(void *player, const char * data, const unsigned int length) {
    RT_LOGD("RockitPlayer1 WriteData");
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    //  RtMutex::RtAutolock autoLock(rtplayer->mDataLock);
    if (NULL != rtplayer) {
       Rockit_PlayAudio_PCM(player);
       rtplayer->player_select = PCM_PLAY;
       rtplayer->pcm_player->writeData(data, length, 0/*PCM stream*/, 0/*only es use*/);
       RT_LOGD("RockitPlayer1 WriteData Done");
       return RT_OK;
    }
    RT_LOGD("RockitPlayer1 WriteData err");
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayerStop(void *player) {
    RT_LOGD("RockitPlayer1 PlayerStop");
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    RtMutex::RtAutolock autoLock(rtplayer->mDataLock);
    if (NULL != rtplayer) {
        if (1) {
            RT_LOGD("RockitPlayer1 PlayerStop1");
            rtplayer->pcm_player->stop();
            RT_LOGD("RockitPlayer1 PlayerStop2");
            rtplayer->pcm_player->reset();
            RT_LOGD("RockitPlayer1 PlayerStop3");
            rtplayer->local_player->stop();
            RT_LOGD("RockitPlayer1 PlayerStop4");
            rtplayer->local_player->reset();
        }
        RT_LOGD("RockitPlayer1 PlayerStop done");
        return RT_OK;
    }
    RT_LOGD("RockitPlayer1 PlayerStop err");
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayerPause(void *player) {
    RT_LOGD("RockitPlayer1 PlayerPause");
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    RtMutex::RtAutolock autoLock(rtplayer->mDataLock);
    if (NULL != rtplayer) {
        if (rtplayer->player_select == PCM_PLAY) {
            rtplayer->pcm_player->pause();
        } else {
            rtplayer->local_player->pause();
        }
        RT_LOGD("RockitPlayer1 PlayerPause done");
        return RT_OK;
    }
    RT_LOGD("RockitPlayer1 PlayerPause err");
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayerResume(void *player) {
    RT_LOGD("RockitPlayer1 PlayerResume");
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    RtMutex::RtAutolock autoLock(rtplayer->mDataLock);
    if (NULL != rtplayer) {
        if (rtplayer->player_select == PCM_PLAY) {
            rtplayer->pcm_player->start();
        } else {
            rtplayer->local_player->start();
        }
        RT_LOGD("RockitPlayer1 PlayerResume done");
        return RT_OK;
    }
    RT_LOGD("RockitPlayer1 PlayerResume err");
    return RT_ERR_UNKNOWN;
}

int Rockit_PlayerSeek(void *player, const double time) {
    RT_LOGD("RockitPlayer1 PlayerSeek");
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    RtMutex::RtAutolock autoLock(rtplayer->mDataLock);
    if (NULL != rtplayer) {
        if (rtplayer->player_select == PCM_PLAY) {
            rtplayer->pcm_player->seekTo(time);
        } else {
            rtplayer->local_player->seekTo(time);
        }
        RT_LOGD("RockitPlayer1 PlayerSeek done");
        return RT_OK;
    }
    RT_LOGD("RockitPlayer1 PlayerSeek err");
    return RT_ERR_UNKNOWN;
}

Rockit_PlayerState Rockit_GetPlayerState(void *player) {
    RT_LOGD("RockitPlayer1 PlayerState");
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    RtMutex::RtAutolock autoLock(rtplayer->mDataLock);
    rt_status ret;

    if (NULL != rtplayer) {
        if (rtplayer->player_select == PCM_PLAY) {
            ret = rtplayer->pcm_player->getState();
        } else {
            ret = rtplayer->local_player->getState();
        }
        RT_LOGD("RockitPlayer1 PlayerState done ret = %d", ret);
        if (ret == RT_STATE_STARTED) {
            return Rockit_PLAYING;
        } else if (ret == RT_STATE_PAUSED) {
            return Rockit_PAUSED;
        } else if (ret == RT_STATE_STOPPED) {
            return Rockit_STOPPED;
        } else {
            return Rockit_START;
        }
        return Rockit_CLOSED;
    }
    RT_LOGD("RockitPlayer1 PlayerState err");
    return Rockit_CLOSED;
}

int Rockit_GetPlayerDuration(void *player) {
    RT_LOGD("RockitPlayer1 PlayerDuration");
    int64_t usec;
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    RtMutex::RtAutolock autoLock(rtplayer->mDataLock);
    if (NULL != rtplayer) {
        if (rtplayer->player_select == PCM_PLAY) {
            rtplayer->pcm_player->getDuration(&usec);
        } else {
            rtplayer->local_player->getDuration(&usec);
        }
        RT_LOGD("RockitPlayer1 PlayerDuration done usec = %lld", usec);
        return usec;
    }
    RT_LOGD("RockitPlayer1 PlayerDuration err");
    return RT_ERR_UNKNOWN;
}

int Rockit_GetPlayerPosition(void *player) {
    RT_LOGD("RockitPlayer1 PlayerPosition");
    int64_t usec;
    RockitContext * rtplayer = reinterpret_cast<RockitContext *>(player);
    RtMutex::RtAutolock autoLock(rtplayer->mDataLock);
    if (NULL != rtplayer) {
        if (rtplayer->player_select == PCM_PLAY) {
            rtplayer->pcm_player->getCurrentPosition(&usec);
        } else {
            rtplayer->local_player->getCurrentPosition(&usec);
        }
        RT_LOGD("RockitPlayer1 PlayerPosition done usec = %lld", usec);
        return usec;
    }
    RT_LOGD("RockitPlayer1 PlayerPosition err");
    return RT_ERR_UNKNOWN;
}
