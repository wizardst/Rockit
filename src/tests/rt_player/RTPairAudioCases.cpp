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
#include "RTPairAudioCases.h"   // NOLINT

#define TEST_AUDIO "man_sea.mp3"

RT_RET unit_test_pair_audio_player_case_easy(INT32 index, INT32 total) {
    RTPairAudioPlayer* aPlayer = new RTPairAudioPlayer();
    aPlayer->init();
    for (int idx = 0; idx < 2; idx++) {
        aPlayer->playUri(TEST_AUDIO);
        aPlayer->playPcm(TEST_AUDIO);
    }
    aPlayer->deinit();
    delete aPlayer;
    aPlayer = NULL;
    return RT_OK;
}

RT_RET unit_test_pair_audio_player_case_hard(INT32 index, INT32 total) {
    RTPairAudioPlayer* aPlayer = new RTPairAudioPlayer();
    aPlayer->init();
    for (int idx = 0; idx < 2; idx++) {
        aPlayer->playUri(TEST_AUDIO);
        aPlayer->playPcm(TEST_AUDIO);
    }
    aPlayer->deinit();
    delete aPlayer;
    aPlayer = NULL;
    return RT_OK;
}
