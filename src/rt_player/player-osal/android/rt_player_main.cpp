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

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

// #define LOG_NDEBUG 0
#define LOG_TAG "rt_player_main"
#include <utils/Log.h>
#include <binder/IPCThreadState.h>

#include <gui/IGraphicBufferProducer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>

#include <media/mediaplayer.h>
#include <media/MediaPlayerInterface.h>
#include <media/IMediaHTTPService.h>

#include "RTMediaPlayer.h"

using namespace android;   // NOLINT

int main(int , const char * argv[]) {
    // init runtime
    android::ProcessState::self()->startThreadPool();

    // rockit mediaplayer testcase
    int msec = 0;
    sp<IMediaHTTPService> httpService = NULL;
    RTMediaPlayer* pPlayer = new RTMediaPlayer();
    pPlayer->setDataSource(httpService, argv[1]);
    pPlayer->setLooping(true);
    pPlayer->setVideoSurfaceTexture(NULL);
    if (NO_ERROR == pPlayer->prepare()) {
        pPlayer->initCheck();
        do {
            int duration, position;
            usleep(5*1000);
            pPlayer->getCurrentPosition(&position);
            pPlayer->getDuration(&duration);
            if ((position + 2000) > duration) {
                pPlayer->seekTo(0);
                pPlayer->start();
            }
            if (pPlayer->isPlaying() == false) {
                pPlayer->stop();
                pPlayer->reset();
                delete pPlayer;
                pPlayer = NULL;
                exit(0);
            }
        } while (true);
    }

    // release runtime
    IPCThreadState::self()->stopProcess(true);
    IPCThreadState::self()->joinThreadPool();
    return 0;
}
