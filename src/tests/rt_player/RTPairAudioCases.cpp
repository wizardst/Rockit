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
void *control_stream_loop(void* param) {
    RTPairAudioPlayer *player = reinterpret_cast<RTPairAudioPlayer *>(param);
    int seekCount = 0;
    while (seekCount < MAX_SEEK_COUNT) {
        RtTime::sleepMs(5 * 1000);
        /*
         * test 1: start->pause->start
         */
        RT_LOGD("now run test 1");
        player->pause();
        RtTime::sleepMs(1 * 1000);
        player->start();
        RT_LOGD("test 1 succeed!");
        RtTime::sleepMs(1 * 1000);
        /*
         * test 2: start->seekto->start
         */
        RT_LOGD("now run test 2");
        player->seekTo(5 * 1000 * 1000);
        player->seekTo(6 * 1000 * 1000);
        player->seekTo(7 * 1000 * 1000);
        player->start();
        player->start();
        player->start();
        RT_LOGD("test 2 succeed!");
        RtTime::sleepMs(5 * 1000);
        /*
         * test 3: start->stop->prepare->start
         */
        RT_LOGD("now run test 3");
        player->stop();
        player->prepare();
        player->start();
        RT_LOGD("test 3 succeed!");
        RtTime::sleepMs(5 * 1000);
        /*
         * test 4: start->pause->stop->prepare->start
         */
        RT_LOGD("now run test 4");
        player->pause();
        RtTime::sleepMs(5 * 1000);
        player->stop();
        player->prepare();
        player->start();
        RT_LOGD("test 4 succeed!");
        RtTime::sleepMs(5 * 1000);
        /*
         * test 5: start->seek->pause->start
         */
        RT_LOGD("now run test 5");
        player->seekTo(7 * 1000 * 1000);
        player->pause();
        RtTime::sleepMs(1 * 1000);
        player->start();
        RT_LOGD("test 5 succeed!");
        RtTime::sleepMs(5 * 1000);
        /*
         * test 6: start->stop->prepare->seek->start
         */
        player->stop();
        player->prepare();
        player->seekTo(7 * 1000 * 1000);
        player->start();
        seekCount++;
    }
    return RT_NULL;
}

#define HAVE_UI_OPT 1
RT_RET unit_test_pair_audio_player_case_easy(const char* media_one, const char* media_two) {
    RtThread*            uiThread = RT_NULL;
    RTPairAudioPlayer*   aPlayer = new RTPairAudioPlayer();
    const char*          name = "audio_player_seek_test";

    RT_LOGE("available 163 cloud music");
    RT_LOGE("Tears of Venice ### uri: http://music.163.com/song/media/outer/url?id=176499.mp3 ");
    RT_LOGE("Only Time-Enya  ### uri: http://music.163.com/song/media/outer/url?id=2530213.mp3");

#if HAVE_UI_OPT
    uiThread = new RtThread(control_stream_loop, aPlayer);
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
