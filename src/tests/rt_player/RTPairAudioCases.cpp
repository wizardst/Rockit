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
 *
 * $ objdump -x rt_player_test | grep NEEDED
 */

#include "rt_header.h"          // NOLINT
#include "rt_time.h"            // NOLINT
#include "rt_thread.h"          // NOLINT
#include "RTPairAudioPlayer.h"  // NOLINT
#include "RTPairAudioCases.h"   // NOLINT

#define MAX_SEEK_COUNT 3
void *seek_test_loop(void* param) {
    RTPairAudioPlayer *player = reinterpret_cast<RTPairAudioPlayer *>(param);
    int seekCount = 0;
    while (seekCount < MAX_SEEK_COUNT) {
        RtTime::sleepMs(5 * 1000);
        RT_LOGD("now pause");
        player->pause();
        //player->seekTo(10 * 1000 * 1000);
        //player->seekTo(11 * 1000 * 1000);
        //player->seekTo(12 * 1000 * 1000);
        RtTime::sleepMs(5 * 1000);
        RT_LOGD("now start");
        player->start();
        RtTime::sleepMs(10 * 1000);
        player->seekTo(5 * 1000 * 1000);
        player->seekTo(6 * 1000 * 1000);
        player->seekTo(7 * 1000 * 1000);
        seekCount++;
    }
    return RT_NULL;
}

#define HAVE_UI_OPT 1
RT_RET unit_test_pair_audio_player_case_easy(const char* media_one, const char* media_two) {
    RtThread*            uiThread = RT_NULL;
    RTPairAudioPlayer*   aPlayer = new RTPairAudioPlayer();
    const char*          name = "audio_player_seek_test";

#if HAVE_UI_OPT
    uiThread = new RtThread(seek_test_loop, aPlayer);
    uiThread->setName(name);
    uiThread->start();
#endif
    aPlayer->init();
    for (int idx = 0; idx < 1; idx++) {
        aPlayer->playUri(media_one);
        aPlayer->wait();
    #if 0
        aPlayer->playPcm(media_two);
        aPlayer->wait();
    #endif
    }
    aPlayer->deinit();
    rt_safe_delete(aPlayer);
    rt_safe_delete(uiThread);
    return RT_OK;
}

RT_RET unit_test_pair_audio_player_case_hard(const char* media_one, const char* media_two) {
    RTPairAudioPlayer* aPlayer = new RTPairAudioPlayer();
    aPlayer->init();
    for (int idx = 0; idx < 2; idx++) {
        aPlayer->playUri(media_one);
        aPlayer->playPcm(media_two);
    }
    aPlayer->deinit();
    delete aPlayer;
    aPlayer = NULL;
    return RT_OK;
}
