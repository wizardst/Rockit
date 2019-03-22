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
 * author: rimon.xu@rock-chips.com
 *   date: 2019/03/20
 */

#include "rt_header.h"          // NOLINT
#include "rt_time.h"            // NOLINT
#include "rt_thread.h"          // NOLINT

#include "player_test_utils.h"  // NOLINT
#include "RTNDKMediaPlayer.h"   // NOLINT

#include "rt_check.h"           // NOLINT
#include "RTObject.h"           // NOLINT

#define TEST_AUDIO_FILE "start.mp3"

static int cmd_count = 0;

#define MAX_TEST_COUNT 1000
void *control_stream_loop(void* param) {
    RTNDKMediaPlayer *player = reinterpret_cast<RTNDKMediaPlayer *>(param);
    while (cmd_count < MAX_TEST_COUNT) {
        player_utils_dispatch_cmd(player);
        cmd_count++;
    }
    return RT_NULL;
}

RT_RET unit_test_player_stable_rand(const char* uri, bool rand) {
    RtThread *cmdThread = RT_NULL;
    RTNDKMediaPlayer*  ndkPlayer = new RTNDKMediaPlayer();
    const char*        name      = "cmd_rand";

    ndkPlayer->setDataSource(uri, RT_NULL);
    ndkPlayer->prepare();
    ndkPlayer->start();

    // simulation of control distribution
    RtTime::sleepMs(1000);
    if (rand) {
        cmd_count = 0;
        cmdThread = new RtThread(control_stream_loop, ndkPlayer);
        cmdThread->setName("cmd_std");
        cmdThread->start();
    }

    // wait util playback complete of first song
    ndkPlayer->wait(20*1000*1000);    // wait for timeout(ms)
    ndkPlayer->pause();
    ndkPlayer->stop();
    ndkPlayer->reset();

    // playe next song
    ndkPlayer->setDataSource(uri, RT_NULL);
    ndkPlayer->prepare();
    ndkPlayer->start();

    // wait util playback complete of second song
    ndkPlayer->wait(10*1000*1000);    // wait for timeout(ms)
    ndkPlayer->pause();
    ndkPlayer->stop();
    ndkPlayer->reset();

    cmd_count = MAX_TEST_COUNT;

    rt_safe_delete(ndkPlayer);
    rt_safe_delete(cmdThread);
    return RT_OK;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        player_utils_help(argv[0]);
        return 0;
    }
    const char* uri = NULL;
    bool  rand = false;
    switch (argc) {
      case 2:
        uri = argv[1];
        break;
      case 3:
        rand = (0 == strcmp(argv[1], "-test"));
        uri = argv[2];
        break;
      default:
        break;
    }

    rt_mem_record_reset();
    RTObject::resetTraces();

    /* your unit test */
    int count = 0;
    if (NULL != uri) {
        do {
            unit_test_player_stable_rand(uri, rand);
            unit_test_player_stable_rand(uri, rand);
            count++;
        } while (count < 100);
    }

    rt_mem_record_dump();
    RTObject::dumpTraces();
    return 0;
}

