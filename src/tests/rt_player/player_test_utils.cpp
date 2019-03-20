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
#include "RTNDKMediaPlayer.h"   // NOLINT
#include "player_test_utils.h"  // NOLINT

enum PC_CMD {
    PC_CMD_NOP = 0,
    PC_CMD_PAUSE,
    PC_CMD_START,
    PC_CMD_SEEK,
    PC_CMD_TIME,
    PC_CMD_STOP,
    PC_CMD_PREPARE,
    PC_CMD_MAX,
};

typedef struct cmd_name_t {
    UINT32      cmd;
    const char *name;
} pc_cmd_name;

static const pc_cmd_name pc_cmd_names[] = {
        { PC_CMD_NOP,     "PC_CMD_NOP" },
        { PC_CMD_PAUSE,   "PC_CMD_PAUSE" },
        { PC_CMD_START,   "PC_CMD_START" },
        { PC_CMD_SEEK,    "PC_CMD_SEEK" },
        { PC_CMD_TIME,    "PC_CMD_TIME" },
        { PC_CMD_STOP,    "PC_CMD_STOP" },
        { PC_CMD_PREPARE, "PC_CMD_PREPARE" },
        { PC_CMD_MAX,     "PC_CMD_MAX" },
};

void player_utils_dispatch_cmd(RTNDKMediaPlayer* player) {
    int32_t cmd = RtTime::randInt()%PC_CMD_MAX;
    int64_t duration = 0;
    int64_t position = 0;
    switch (cmd) {
      case PC_CMD_NOP:
      case PC_CMD_TIME:
        player->getCurrentPosition(&position);
        player->getDuration(&duration);
        RT_LOGD("position=%lldms; duration=%lldms", position/1000, duration/1000);
        RtTime::sleepMs(500);
        break;
      case PC_CMD_PAUSE:
        player->pause();
        player->pause();
        RtTime::sleepMs(500);
        player->start();
        break;
      case PC_CMD_START:
        player->start();
        player->pause();
        RtTime::sleepMs(500);
        player->start();
        break;
      case PC_CMD_SEEK:
        player->seekTo(position + 5 * 1000 * 1000);
        player->seekTo(position + 6 * 1000 * 1000);
        player->seekTo(position + 7 * 1000 * 1000);
        break;
      case PC_CMD_STOP:
        player->stop();
        player->prepare();
        player->start();
        break;
      case PC_CMD_PREPARE:
        player->prepare();
        break;
      default:
        cmd = PC_CMD_MAX;
        break;
    }
    RT_LOGD("%s TESTCASE has done....", pc_cmd_names[cmd].name);
    RtTime::sleepMs(RtTime::randInt()%2000);
}

void player_utils_help(const char* bin_name) {
    RT_LOGE("%s [-test] uri", bin_name);
    RT_LOGE("available 163 cloud music");
    RT_LOGE("Tears of Venice ### uri: http://music.163.com/song/media/outer/url?id=176499.mp3 ");
    RT_LOGE("Only Time-Enya  ### uri: http://music.163.com/song/media/outer/url?id=2530213.mp3");
}
