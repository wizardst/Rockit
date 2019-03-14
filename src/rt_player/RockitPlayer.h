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
 * author: hery.xu@rock-chips.com
 *   date: 2019/03/13
 * module: RockitPlayer
 */

#ifndef SRC_RT_PLAYER_ROCKITPLAYER_H_
#define SRC_RT_PLAYER_ROCKITPLAYER_H_

#ifdef  __cplusplus
extern "C" {
#endif

typedef enum Rockit_PlayerState {
    Rockit_STOPPED = 0,  ///  < Playback stopped or has not started yet.
    Rockit_START,        ///  < Playbakc add for start.
    Rockit_PLAYING,      ///  < Playback started & player is actively decoding.
    Rockit_PAUSED,       ///  < Playback paused; player is actively decoding but no new data is given out.
    Rockit_CLOSED,       ///  < Playback is stopped and player is closing.
} Rockit_PlayerState;

enum Rockit_event_e {
    Rockit_MediaPlayerPlaying = 0,
    Rockit_MediaPlayerEndReached_pcm,
    Rockit_MediaPlayerEndReached_kit,
    Rockit_MediaPlayerEndReached_vlc
};

typedef void ( *Rockit_callback_t )( int p_event, void *p_data);


void * Rockit_CreatePlayer();
int Rockit_EventAttach(void * player, int type, Rockit_callback_t callback, void *opaque);
int Rockit_PlayAudio_PCM(void *player);
int Rockit_ReleasePlayer(void *player);
int Rockit_PlayAudio(void *player, const char* url, const int pcm);
int Rockit_PlayerPlay(void *player);
int Rockit_WriteData(void *player, const char * data, const unsigned int length);
int Rockit_PlayerStop(void *player);
int Rockit_PlayerPause(void *player);
int Rockit_PlayerResume(void *player);
int Rockit_PlayerSeek(void *player, const double time);
Rockit_PlayerState Rockit_GetPlayerState(void *player);
int Rockit_GetPlayerDuration(void *player);
int Rockit_GetPlayerPosition(void *player);

#ifdef  __cplusplus
}
#endif

#endif  // SRC_RT_PLAYER_ROCKITPLAYER_H_
