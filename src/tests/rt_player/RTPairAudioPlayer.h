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

#ifndef SRC_TESTS_RT_PLAYER_RTPAIRAUDIOPLAYER_H_
#define SRC_TESTS_RT_PLAYER_RTPAIRAUDIOPLAYER_H_

#ifdef  __cplusplus
extern "C" {
#endif

#include "RTNDKMediaPlayer.h"

class RTPairAudioPlayer {
 public:
    RTPairAudioPlayer();
    ~RTPairAudioPlayer();
    rt_status init();
    rt_status playUri(const char* uri);
    rt_status playPcm(const char* pcm);
    rt_status deinit();
 private:
    rt_status swapAudio();
    RTNDKMediaPlayer* mAudioCur;
    RTNDKMediaPlayer* mAudioA;
    RTNDKMediaPlayer* mAudioB;
};

#ifdef  __cplusplus
}
#endif

#endif  // SRC_TESTS_RT_PLAYER_RTPAIRAUDIOPLAYER_H_

